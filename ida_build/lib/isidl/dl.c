#pragma ident "$Id: dl.c,v 1.33 2009/03/17 17:21:20 dechavez Exp $"
/*======================================================================
 *
 * ISI disk loop support
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isi/dl.h"
#include "nrts/dl.h"

static void DeleteHandle(ISI_DL *handle)
{
    if (handle == NULL) return;
    if (handle->lock.fp != NULL) fclose(handle->lock.fp);
    if (handle->fd.hdr > 0) close(handle->fd.hdr);
    if (handle->fd.raw > 0) close(handle->fd.raw);
    free(handle);
}

static ISI_DL *DeleteHandleAndFail(ISI_DL *dl)
{
    if (dl != NULL) DeleteHandle(dl);
    return NULL;
}

static void InitTeeHandle(ISI_TEE *tee)
{
    tee->disabled = FALSE;
    tee->fp = NULL;
    memset(tee->path, 0, MAXPATHLEN+1);
}

static void InitPath(char *base, ISI_DL_PATH *path)
{
    snprintf(path->hdr, MAXPATHLEN+1, "%s/%s", base, ISI_DL_HDR_NAME);
    snprintf(path->raw, MAXPATHLEN+1, "%s/%s", base, ISI_DL_RAW_NAME);
}

static void LockDiskLoop(char *pfid, ISI_DL *dl)
{
static char *fid = "LockDiskLoop";

    if (dl->flags & ISI_DL_FLAGS_IGNORE_LOCKS) {
        logioMsg(dl->lp, dl->glob->debug.lock, "%s: %s: ignored (locks disabled)", pfid, fid);
        return;
    }

    if (dl->lock.perm == ISI_RDWR) {
        logioMsg(dl->lp, dl->glob->debug.lock, "%s: %s: utilWriteLockWait", pfid, fid);
        if (!utilWriteLockWait(dl->lock.fp)) {
            logioMsg(dl->lp, LOG_ERR, "%s: %s: utilWriteLockWait: %s", pfid, fid, strerror(errno));
            return;
        }
    } else {
        logioMsg(dl->lp, dl->glob->debug.lock, "%s: %s: utilReadLockWait", pfid, fid);
        if (!utilReadLockWait(dl->lock.fp)) {
            logioMsg(dl->lp, LOG_ERR, "%s: %s: utilReadLockWait: %s", pfid, fid, strerror(errno));
            return;
        }
    }
}

static void UnlockDiskLoop(char *pfid, ISI_DL *dl)
{
static char *fid = "UnlockDiskLoop";

    if (dl->flags & ISI_DL_FLAGS_IGNORE_LOCKS) {
        logioMsg(dl->lp, dl->glob->debug.lock, "%s: %s: ignored (locks disabled)", pfid, fid);
        return;
    }

    logioMsg(dl->lp, dl->glob->debug.lock, "%s: %s: utilReleaseLock", pfid, fid);
    utilReleaseLock(dl->lock.fp);
}

static ISI_DL *CreateHandle(ISI_GLOB *glob, char *sta, LOGIO *lp)
{
ISI_DL *handle;
static char *fid = "CreateHandle";

    if ((handle = (ISI_DL *) malloc(sizeof(ISI_DL))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    handle->glob = glob;
    handle->sys = NULL;
    snprintf(handle->base, MAXPATHLEN+1, "%s/%s/%s", handle->glob->root, sta, ISI_DL_DIR_NAME);
    handle->lp = lp;
    handle->nrts = NULL;
    InitTeeHandle(&handle->tee);
    InitPath(handle->base, &handle->path);

    handle->flags = glob->flags;
    handle->fd.hdr = handle->fd.raw = -1;
    handle->lock.fp = NULL;

    return handle;
}

static BOOL MapSysFile(ISI_DL *dl)
{
char path[MAXPATHLEN+1];
static char *fid = "MapSysFile";

    snprintf(path, MAXPATHLEN+1, "%s/%s", dl->base, ISI_DL_SYS_NAME);
    if (!utilMmap(path, 0, sizeof(ISI_DL_SYS), &dl->sysmap)) {
        logioMsg(dl->lp, LOG_ERR, "%s: utilMmap: %s: %s", fid, path, strerror(errno));
        return FALSE;
    }
    dl->sys = (ISI_DL_SYS *) dl->sysmap.ptr;

    return TRUE;
}

static void InitNewSys(ISI_DL_SYS *sys, char *site, UINT32 numpkt, UINT32 nhide, UINT32 maxlen)
{
    strlcpy(sys->site, site, ISI_SITELEN+1);
    sys->seqno.signature = time(NULL);
    sys->seqno.counter = 0;
    sys->numpkt = numpkt;
    sys->nhide  = nhide;
    sys->maxlen = maxlen;
    sys->index.active = 0;
    sys->index.lend   = sys->numpkt - sys->nhide;
    sys->index.oldest = ISI_UNDEFINED_INDEX;
    sys->index.yngest = ISI_UNDEFINED_INDEX;
    sys->parent = 0;
    sys->state = ISI_INACTIVE;
    sys->count = 0;
    sys->tstamp.start = UTIL_UNDEFINED_TIMESTAMP;
    sys->tstamp.write = UTIL_UNDEFINED_TIMESTAMP;
}

static BOOL CreateDLFile(ISI_DL *dl, char *fname, UINT32 len, BOOL fill)
{
UINT8 byte = 0xee;
char path[MAXPATHLEN+1];
static char *fid = "CreateDLFile";

    snprintf(path, MAXPATHLEN+1, "%s/%s", dl->base, fname);
    logioMsg(dl->lp, LOG_INFO, "Creating %s (%d bytes)\n", path, len);
    return utilCreateFile(path, len, fill, byte);
}

ISI_DL *isidlCreateDiskLoop(ISI_GLOB *glob, char *site, UINT32 numpkt, UINT32 nhide, UINT32 maxlen, LOGIO *lp)
{
UINT32 len;
ISI_DL *dl;
static char *fid = "isidlCreateDiskLoop";

    if (glob == NULL || site == NULL) {
        errno = EINVAL;
        return NULL;
    }

    numpkt += nhide;

    if (strlen(site) > ISI_STALEN) {
        logioMsg(lp, LOG_ERR, "%s: site too long!\n", fid);
        errno = EINVAL;
        return NULL;
    }

    if ((dl = CreateHandle(glob, site, lp)) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: failed to create handle", fid);
        return NULL;
    }

/* Create the requisite files */

    if (util_mkpath(dl->base, 0755) != 0) {
        logioMsg(lp, LOG_ERR, "%s: util_mkpath: %s: %s", fid, dl->base, strerror(errno));
        return NULL;
    }

    len = sizeof(ISI_DL_SYS);
    if (!CreateDLFile(dl, ISI_DL_SYS_NAME, len, FALSE)) {
        logioMsg(lp, LOG_ERR, "%s: failed to create sys file", fid);
        return DeleteHandleAndFail(dl);
    }

    len = sizeof(ISI_RAW_HEADER) * numpkt;
    if (!CreateDLFile(dl, ISI_DL_HDR_NAME, len, FALSE)) {
        logioMsg(lp, LOG_ERR, "%s: failed to create hdr file", fid);
        return DeleteHandleAndFail(dl);
    }

    len = maxlen * numpkt;
    if (!CreateDLFile(dl, ISI_DL_RAW_NAME, len, FALSE)) {
        logioMsg(lp, LOG_ERR, "%s: failed to create heap file", fid);
        return DeleteHandleAndFail(dl);
    }

    len = 1;
    if (!CreateDLFile(dl, ISI_DL_LOCK_NAME, len, FALSE)) {
        logioMsg(lp, LOG_ERR, "%s: failed to create lock file", fid);
        return DeleteHandleAndFail(dl);
    }

/* Map the sys into the handle */

    if (!MapSysFile(dl)) {
        logioMsg(dl->lp, LOG_ERR, "%s: failed to map sys file", fid);
        return DeleteHandleAndFail(dl);
    }
    InitNewSys(dl->sys, site, numpkt, nhide, maxlen);

    return dl;
}

void isidlUpdateParentID(ISI_DL *dl, int pid)
{
static char *fid = "isidlUpdateParentID";

    if (dl == NULL) return;

    LockDiskLoop(fid, dl);
        isidlSysSetParent(dl, pid, FALSE);
        if (dl->nrts != NULL) dl->nrts->sys->pid = pid;
    UnlockDiskLoop(fid, dl);
}

void isidlSetMetaDir(ISI_DL *dl)
{
    if (dl == NULL) return;
    snprintf(dl->path.meta, MAXPATHLEN+1, "%s/%s/%s", dl->glob->root, dl->sys->site, ISI_DL_META_DIR);
}

ISI_DL *isidlOpenDiskLoop(ISI_GLOB *glob, char *site, LOGIO *lp, int perm)
{
FILE *fp = NULL;
ISI_DL *dl;
struct stat statinfo;
pid_t mypid;
char buf[MAXPATHLEN+1];
static char *fid = "isidlOpenDiskLoop";

    if (glob == NULL || site == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((dl = CreateHandle(glob, site, lp)) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: failed to create handle", fid);
        return NULL;
    }

    if (stat(dl->base, &statinfo) != 0) {
        logioMsg(lp, LOG_DEBUG, "%s: can't stat %s: %s", fid, dl->base, strerror(errno));
        return DeleteHandleAndFail(dl);
    }
    if (!MapSysFile(dl)) {
        logioMsg(lp, LOG_DEBUG, "%s: can't map sys file", fid);
        return DeleteHandleAndFail(dl);
    }
    snprintf(buf, MAXPATHLEN+1, "%s/%s", dl->base, ISI_DL_LOCK_NAME);
    if ((dl->lock.fp = fopen(buf, perm == ISI_RDONLY ? "rb" : "ab")) == NULL) {
        logioMsg(lp, LOG_DEBUG, "%s: can't fopen %s: %s", fid, buf, strerror(errno));
        return DeleteHandleAndFail(dl);
    }
    dl->lock.perm = perm;

/* see if we've got meta-data attached to this disk loop */

    isidlSetMetaDir(dl);
    if (utilDirectoryExists(dl->path.meta)) {
        dl->flags |= ISI_DL_FLAGS_HAVE_META;
        sprintf(dl->path.qdpstate, "%s/%s", dl->path.meta, ISI_DL_QDP_STATE_FILE);
        if (utilFileExists(dl->path.qdpstate)) dl->flags |= ISI_DL_FLAGS_HAVE_QDP_STATE;
    }

/* attach to the NRTS disk loop, if it is available */

    dl->nrts = nrtsOpenDiskLoop(dl->glob, site, dl->lp, perm);

/* all done if were don't want to be the parent */

    if (perm == ISI_RDONLY) return dl;

/* Othewise, make sure there already isn't a parent.
 * We ignore the lock if it matches our current pid. Process ID's for ISI
 * processes will usually be constant across reboots (assuming they haven't
 * been manually stopped and restarted) and so the kill() test for our own
 * pid will make it look like the disk loop is already locked when in fact
 * it is just dirty because the system had previously crashed leaving the
 * pid in the memory mapped sys file non-zero.
 */

    LockDiskLoop(fid, dl);
        mypid = getpid();
        if (dl->sys->parent == mypid) isidlSysSetParent(dl, 0, FALSE);
        if (dl->sys->parent != 0 && kill(dl->sys->parent, 0) == 0) {
            errno = EACCES;
            logioMsg(lp, LOG_ERR, "%s: disk loop locked by %d", fid, dl->sys->parent);
            return DeleteHandleAndFail(dl);
        } else {
            isidlSysSetParent(dl, mypid, TRUE);
            isidlSysSetTstampStart(dl, utilTimeStamp(), TRUE);
        }

    /* clean up inconsistent state */

        if (dl->sys->state == ISI_INACTIVE) isidlSysSetState(dl, ISI_IDLE, TRUE);
        if (dl->sys->state != ISI_IDLE) {
            logioMsg(dl->lp, LOG_WARN, "warning: %s DL state is %s", dl->sys->site, isidlStateString(dl->sys->state));
            logioMsg(dl->lp, LOG_WARN, " index : %s", isidlIndexString(&dl->sys->index, buf));
            logioMsg(dl->lp, LOG_WARN, "backup : %s", isidlIndexString(&dl->sys->backup, buf));
            isidlSysSetIndex(dl, &dl->sys->backup, FALSE);
            isidlSysSetState(dl, ISI_IDLE, TRUE);
            logioMsg(dl->lp, LOG_WARN, "%s backup indices restored, state set to %s", dl->sys->site, isidlStateString(dl->sys->state));
        }
    UnlockDiskLoop(fid, dl);

    if ((dl->fd.hdr = open(dl->path.hdr, O_WRONLY | O_BINARY | O_SYNC)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: open: %s: %s:", fid, dl->path.hdr, strerror(errno));
        return DeleteHandleAndFail(dl);
    }

    if ((dl->fd.raw = open(dl->path.raw, O_WRONLY | O_BINARY | O_SYNC)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: open: %s: %s:", fid, dl->path.raw, strerror(errno));
        return DeleteHandleAndFail(dl);
    }

    return dl;
}

VOID isidlCloseDiskLoop(ISI_DL *dl)
{
static char *fid = "isidlCloseDiskLoop";

    if (dl == NULL) return;
    LockDiskLoop(fid, dl);
        if (dl->nrts != NULL) nrtsCloseDiskLoop(dl->nrts);
        isidlCloseTee(dl);
        if (dl->sys->parent == getpid()) {
            isidlSysSetParent(dl, 0, FALSE);
            isidlSysSetState(dl, ISI_INACTIVE, FALSE);
        }
        utilUnmap(&dl->sysmap);
    UnlockDiskLoop(fid, dl);
    DeleteHandle(dl);
}

static void IncrementIndicies(ISI_INDEX *index, UINT32 numpkt)
{

/* If we've never written anything to this disk loop, then both the oldest
 * and youngest indices are the active index (which ought to be 0).  If
 * we've already been here, then just the youngest index is updated to
 * match the active.
 */

    if (index->oldest == ISI_UNDEFINED_INDEX) index->oldest = index->active;
    index->yngest = index->active;

/* The disk loop is "full" once we have reached the logical end.  Once that
 * happens, then all indices will march forward, incrementing by one each
 * time (and resetting to 0 when they get to the end of the file).  
 */

    if (index->yngest == index->lend) {
        index->lend   = ++index->lend   % numpkt;
        index->oldest = ++index->oldest % numpkt;
    }

/* We will always push the active index forward by one.  Eventually it will
 * reach the logical end, and triggering the above block that increments
 * the oldest and logical end.
 */

    index->active = ++index->active % numpkt;
}

#define ISI_HDR 1
#define ISI_RAW 2

static int OpenAndSeek(char *fid, int which, ISI_DL *dl, UINT32 index, int oflag)
{
int fd;
off_t offset, itemlen;
char *path;

    if (which == ISI_HDR) {
        itemlen = (off_t) sizeof(ISI_RAW_HEADER);
        path = dl->path.hdr;
    } else {
        itemlen = (off_t) dl->sys->maxlen;
        path = dl->path.raw;
    }

    if ((fd = open(path, oflag)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: open: %s: %s:", fid, path, strerror(errno));
        return -1;
    }

    offset = itemlen * (off_t) index;
    if (lseek(fd, offset, SEEK_SET) != offset) {
        logioMsg(dl->lp, LOG_ERR, "%s: lseek (offset %lu): %s: %s:", fid, offset, path, strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

static BOOL UnlockAndFail(char *fid, ISI_DL *dl)
{
    UnlockDiskLoop(fid, dl);
    return FALSE;
}

static BOOL WriteRawPacket(char *fid, ISI_DL *dl, UINT32 index, ISI_RAW_PACKET *raw)
{
off_t offset;

    offset = sizeof(ISI_RAW_HEADER) * (off_t) index;
    if (lseek(dl->fd.hdr, offset, SEEK_SET) != offset) {
        logioMsg(dl->lp, LOG_ERR, "%s: lseek (offset %lu): %s: %s:", fid, offset, dl->path.hdr, strerror(errno));
        return FALSE;
    }
    if (write(dl->fd.hdr, &raw->hdr, sizeof(ISI_RAW_HEADER)) != sizeof(ISI_RAW_HEADER)) {
        logioMsg(dl->lp, LOG_ERR, "%s: hdr write: %s: %s", fid, dl->path.hdr, strerror(errno));
        return FALSE;
    }

    offset = (off_t) dl->sys->maxlen * (off_t) index;
    if (lseek(dl->fd.raw, offset, SEEK_SET) != offset) {
        logioMsg(dl->lp, LOG_ERR, "%s: lseek (offset %lu): %s: %s:", fid, offset, dl->path.raw, strerror(errno));
        return FALSE;
    }
    if (write(dl->fd.raw, raw->payload, raw->hdr.len.used) != (ssize_t) raw->hdr.len.used) {
        logioMsg(dl->lp, LOG_ERR, "%s: payload write: %s: %s", fid, dl->path.raw, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

static BOOL NRTSCompatiblePacket(NRTS_DL *dl, ISI_RAW_PACKET *raw)
{

/* Only IDA packets are really compatible with NRTS */

    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA5:
      case ISI_TYPE_IDA6:
      case ISI_TYPE_IDA7:
      case ISI_TYPE_IDA8:
      case ISI_TYPE_IDA9:
      case ISI_TYPE_IDA10:
        return TRUE;
    }

/* But, MiniSEED can be shoehorned in */

    if (raw->hdr.desc.type == ISI_TYPE_MSEED) {
        nrtsMSEEDToIDA10DiskLoop(dl, raw->payload);
        return FALSE; /* since we won't be writing here in any case */
    }

/* Anything else is not NRTS compatible */

    return FALSE;
}

BOOL isidlWriteToDiskLoop(ISI_DL *dl, ISI_RAW_PACKET *raw, int options)
{
NRTS_PKT pkt;
char *tofsString, tmp[64];
static char *fid = "isidlWriteToDiskLoop";

/* Sanity check */

    if (strcmp(raw->hdr.site, dl->sys->site) != 0) {
        errno = EFAULT;
        return FALSE;
    }

/* Do old NRTS first, since that will cause the raw packet to
/* get any (gag) time stamp patches needed by downstream systems.
 */

    if (dl->nrts != NULL && NRTSCompatiblePacket(dl->nrts, raw) && raw->hdr.status == ISI_RAW_STATUS_OK) {
        if (!(dl->nrts->unpack)(dl->nrts, NULL, raw->payload, &pkt)) {
            logioMsg(dl->lp, LOG_INFO, "%s: dl->unpack failed, set NRTS_REJECT", fid);
            pkt.status = NRTS_REJECT;
        }
        switch (pkt.status) {

        /* Good data packets go into the NRTS disk loop */

          case NRTS_ACCEPT:
            dl->nrts->pkt = &pkt;
            if (!nrtsWriteToDiskLoop(dl->nrts, raw->payload)) {
                logioMsg(dl->lp, LOG_INFO, "%s: site '%s' nrtsWriteToDiskLoop failed", fid, dl->sys->site);
                logioMsg(dl->lp, LOG_INFO, "NRTS disk loop updates for site '%s' suspended", dl->sys->site);
                dl->nrts = NULL; /* memory leak but don't care since this is catastrophic anyway */
            } else {
                dl->nrts->pkt = NULL;
            }
            break;

        /* Rejected packets don't go into NRTS and are marked as suspect for downstream systems */

          case NRTS_REJECT:
            raw->hdr.status = ISI_RAW_STATUS_SUSPECT;
            break;

        /* Partial overlaps don't go into NRTS disk loops */

          case NRTS_PARTIAL_OVERLAP:
            tofsString = isiTstampString(&pkt.isi.hdr.tofs, tmp);
            logioMsg(dl->lp, dl->glob->debug.dl, "partially overlapping %s %s packet dropped from NRTS", pkt.ident, tofsString);
            break;

        /* Duplicates and overlaps may be cause for early return (permanently disappeared) */

          case NRTS_DUPLICATE:
            if (options & ISI_OPTION_REJECT_DUPLICATES) {
                tofsString = isiTstampString(&pkt.isi.hdr.tofs, tmp);
                logioMsg(dl->lp, dl->glob->debug.dl, "duplicate %s %s packet dropped from NRTS and ISI", pkt.ident, tofsString);
                return TRUE;
            }
            break;

          case NRTS_COMPLETE_OVERLAP:
            if (options & ISI_OPTION_REJECT_COMPLETE_OVERLAPS) {
                tofsString = isiTstampString(&pkt.isi.hdr.tofs, tmp);
                logioMsg(dl->lp, dl->glob->debug.dl, "completely overlapping %s %s packet dropped from NRTS and ISI", pkt.ident, tofsString);
                return TRUE;
            }
            break;

        /* Everything else is benign, and ignored */

          case NRTS_NOTDATA:
          case NRTS_IGNORE:
            break;

          default:
            logioMsg(dl->lp, LOG_INFO, "%s: ***** WARNING ***** UNEXPECTED pkt.status = %d", fid, pkt.status);
            break;
        }
    }

/* Write (possibly patched) raw packet */

    LockDiskLoop(fid, dl);
        isidlSysSetBackup(dl, &dl->sys->index, TRUE);
        isidlSysSetState(dl, ISI_ACTIVE, TRUE);

            if (options & ISI_OPTION_GENERATE_SEQNO) {
                raw->hdr.seqno = isidlSysIncrSeqno(dl, TRUE);
            } else {
                isidlSysSetSeqno(dl, &raw->hdr.seqno, TRUE);
            }

            if (!WriteRawPacket(fid, dl, dl->sys->index.active,  raw)) return UnlockAndFail(fid, dl);

        isidlSysSetState(dl, ISI_UPDATE, TRUE);
        IncrementIndicies(&dl->sys->index, dl->sys->numpkt);
        isidlSysSetState(dl, ISI_IDLE, TRUE);
        isidlSysIncrCount(dl, TRUE);
        isidlSysSetTstampWrite(dl, utilTimeStamp(), TRUE);
    UnlockDiskLoop(fid, dl);

/* Tee, if enabled */

    if (!dl->tee.disabled) isidlWriteToTeeFile(dl, raw);

/* Clear the raw header status that may have gotten set due to NRTS_REJECT */

    if (raw->hdr.status == ISI_RAW_STATUS_SUSPECT) {
        if (dl->nrts != NULL) logioMsg(dl->lp, dl->glob->debug.dl, "suspect %s packet dropped from NRTS", pkt.ident);
        raw->hdr.status = ISI_RAW_STATUS_OK;
    }

    return TRUE;
}

static BOOL ReadHdr(char *fid, ISI_DL *dl, ISI_RAW_HEADER *dest, UINT32 index)
{
int fd;
UINT32 allocated;

    allocated = dest->len.payload; /* note how much space is available for payload */

    if ((fd = OpenAndSeek(fid, ISI_HDR, dl, index, O_RDONLY | O_BINARY)) < 0) return FALSE;
    if (read(fd, dest, sizeof(ISI_RAW_HEADER)) != sizeof(ISI_RAW_HEADER)) {
        logioMsg(dl->lp, LOG_ERR, "%s: hdr read: index=0x%08x: %s", fid, index, strerror(errno));
        close(fd);
        return FALSE;
    }
    close(fd);
    dest->len.payload = allocated; /* restore what got overwritten in the read */

    return TRUE;
}

static BOOL ReadRaw_(char *fid, ISI_DL *dl, ISI_RAW_PACKET *dest, UINT32 index)
{
int fd;

    if ((fd = OpenAndSeek(fid, ISI_RAW, dl, index, O_RDONLY | O_BINARY)) < 0) return FALSE;
    if (read(fd, dest->payload, dest->hdr.len.used) != (ssize_t) dest->hdr.len.used) {
        logioMsg(dl->lp, LOG_ERR, "%s: payload read: %s", fid, strerror(errno));
        close(fd);
        return FALSE;
    }
    close(fd);

    return TRUE;
}

BOOL isidlReadDiskLoop(ISI_DL *dl, ISI_RAW_PACKET *dest, UINT32 index)
{
static char *fid = "isidlReadDiskLoop";

    if (dl == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (index == ISI_UNDEFINED_INDEX) {
        logioMsg(dl->lp, dl->glob->debug.dl, "%s: isiInitRawHeader", fid);
        isiInitRawHeader(&dest->hdr);
        return TRUE;
    }
    logioMsg(dl->lp, dl->glob->debug.dl, "%s: index = %lu", fid, index);

    LockDiskLoop(fid, dl);

        /* read the header */

        if (!ReadHdr(fid, dl, &dest->hdr, index)) return UnlockAndFail(fid, dl);

        /* make sure we've enough space to receive the payload */

        if (dest->hdr.len.payload < dest->hdr.len.used) {
            if ((dest->payload = realloc(dest->payload, dest->hdr.len.used)) == NULL) {
                dest->hdr.len.payload = 0;
                logioMsg(dl->lp, LOG_ERR, "%s: realloc (%d): %s", fid, dest->hdr.len.used, strerror(errno));
                return UnlockAndFail(fid, dl);
            }
        }

        /* read the payload */

        if (!ReadRaw_(fid, dl, dest, index)) return UnlockAndFail(fid, dl);

    UnlockDiskLoop(fid, dl);

    return TRUE;
}

BOOL isidlSnapshot(ISI_DL *dl, ISI_DL *snapshot, ISI_DL_SYS *sys)
{
static char *fid = "isidlSnapshot";

    if (dl == NULL || snapshot == NULL || sys == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    LockDiskLoop(fid, dl);
        *snapshot = *dl;
        *sys = *(dl->sys);
    UnlockDiskLoop(fid, dl);

    snapshot->sys = sys;

    return TRUE;
}

ISI_SEQNO isidlPacketSeqno(ISI_DL *dl, UINT32 index)
{
ISI_SEQNO retval;
ISI_RAW_HEADER hdr;
static char *fid = "isidlPacketSeqno";

    if (dl == NULL || index >= dl->sys->numpkt) {
        errno = EINVAL;
        return ISI_UNDEFINED_SEQNO;
    }

    LockDiskLoop(fid, dl);
        retval = ReadHdr(fid, dl, &hdr, index) ? hdr.seqno : ISI_UNDEFINED_SEQNO;
    UnlockDiskLoop(fid, dl);

    return retval;
}

ISI_SEQNO isidlCrntSeqno(ISI_DL *dl)
{
ISI_SEQNO retval;
static char *fid = "isidlPacketSeqno";

    if (dl == NULL) {
        errno = EINVAL;
        return ISI_UNDEFINED_SEQNO;
    }
    LockDiskLoop(fid, dl);
        retval = dl->sys->seqno;
    UnlockDiskLoop(fid, dl);

    return retval;
}

/* Revision History
 *
 * $Log: dl.c,v $
 * Revision 1.33  2009/03/17 17:21:20  dechavez
 * reworked disk loop locks to use libutil utilWriteLockWait() and
 * utilReadLockWait(), removed mutex in ISI_DL handle
 *
 * Revision 1.32  2009/02/23 22:00:12  dechavez
 * restored MUTEX_INIT to isidlSnapshot() (dealing with memory at the application level)
 *
 * Revision 1.31  2009/02/03 22:54:35  dechavez
 * added debug messages in isidlReadDiskLoop()
 *
 * Revision 1.30  2009/01/26 21:04:56  dechavez
 * removed unwise MUTEX_INIT from isidlSnapshot()
 *
 * Revision 1.29  2008/12/15 23:33:22  dechavez
 * restored O_SYNC flag when opening NRTS files for writing (don't recall when it got removed)
 * set sync flag in all isidlSysSetX calls
 *
 * Revision 1.28  2008/01/07 21:58:30  dechavez
 * automatically convert MiniSEED to IDA10/NRTS, if possible
 *
 * Revision 1.27  2007/12/21 22:18:36  dechavez
 * added NRTSCompatiblePacket() before attempting to update NRTS disk loop
 *
 * Revision 1.26  2007/09/14 19:35:25  dechavez
 * removed ISI_DL_FLAGS_TIME_DL_OPS_ENABLED related code, improved debug
 * and log messages, added support for both partial and complete overlaps,
 * fixed problem with plugged NRTS diskloops following first rejected packet
 *
 * Revision 1.25  2007/04/18 23:17:42  dechavez
 * note state file in dl flags, if present
 *
 * Revision 1.24  2007/02/08 23:24:40  dechavez
 * disk loop writers keep files open, added conditional support for
 * ISI_DL_FLAGS_TIME_DL_OPS (not compiled in this build)
 *
 * Revision 1.23  2007/01/23 02:59:25  dechavez
 * added support for ISI_DL_FLAGS_IGNORE_LOCKS flag and changed isiReadDiskLoop
 * to handle indicies explicitly set as undefined correctly
 *
 * Revision 1.22  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.21  2007/01/08 16:02:31  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.20  2006/12/12 22:48:17  dechavez
 * added metadata directory support, isidlCrntSeqno()
 *
 * Revision 1.19  2006/11/10 06:42:13  dechavez
 * flock locking only if ENABLE_DISKLOOP_LOCKS defined (and it isn't in this build)
 *
 * Revision 1.18  2006/09/29 19:44:08  dechavez
 * fixed typo in fcntl error reports in LockDiskLoop/UnlockDiskLoop, don't
 * attempt to unlock a non-locked file (eg, when UnlockDiskLoop is called
 * after a failed LockDiskLoop)
 *
 * Revision 1.17  2006/06/26 20:53:10  dechavez
 * fixed signed/unsigned mismatch
 *
 * Revision 1.16  2006/06/26 20:51:35  dechavez
 * removed unreferenced local variables
 *
 * Revision 1.15  2006/06/12 21:35:36  dechavez
 * Added sync arguments to isidlSysX functions such that disk syncs didn't
 * happen everytime the structure was changed, just after critical updates.
 * Added O_BINARY to OpenAndSeek flags for Win32 compatibility (aap)
 *
 * Revision 1.14  2006/06/02 20:50:03  dechavez
 * wrappped all changes to mapped ISI sys structure with calls to to isidlSysXXX functions
 *
 * Revision 1.13  2006/04/20 22:54:19  dechavez
 * added isidlUpdateParentID(), check for matching locks
 *
 * Revision 1.12  2006/04/07 17:04:32  dechavez
 * removed references to obsolete fields in tee handle
 *
 * Revision 1.11  2006/03/13 22:32:17  dechavez
 * O_SYNC on write (this was a major oversight(!))
 *
 * Revision 1.10  2006/02/10 02:26:28  dechavez
 * include index in failed ReadHdr
 *
 * Revision 1.9  2006/02/09 00:26:00  dechavez
 * fixed race condition in LockDiskLoop()
 *
 * Revision 1.8  2005/12/09 21:17:21  dechavez
 * recognize NRTS_NOTDATA status from decoder, changed isiWriteDiskLoop()
 * to take flags argument and added support for new ISI_REJECT_DUPLICATES option
 *
 * Revision 1.7  2005/10/18 19:01:29  dechavez
 * log failed fcntl in LockDiskloop closer to event
 *
 * Revision 1.6  2005/09/30 22:06:43  dechavez
 * changes to tee handle (for gzip), changed name of ReadRaw for Win32 sanity
 *
 * Revision 1.5  2005/09/10 21:45:32  dechavez
 * major rework to use explicit hdr and raw file I/O instead of mapped memory operations
 *
 * Revision 1.4  2005/08/26 18:16:18  dechavez
 * added tee support
 *
 * Revision 1.3  2005/07/27 18:43:04  dechavez
 * only write NRTS_ACCEPTed packets to NRTS disk loop
 *
 * Revision 1.2  2005/07/26 17:08:45  dechavez
 * include mutex lock in disk loop lock, lock diskloop while closing, removed deadlock in isiOpenDiskLoop()
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
