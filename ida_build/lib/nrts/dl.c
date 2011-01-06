#pragma ident "$Id: dl.c,v 1.19 2008/01/11 00:53:56 dechavez Exp $"
/*======================================================================
 *
 *  NRTS disk loop I/O
 *
 *====================================================================*/
#include "platform.h"
#include "nrts/dl.h"
#ifdef WIN32

/* suppress certain scary warnings that are actually harmless */

#pragma warning( disable : 4550)

#endif /* WIN32 */

/* IDA10 conversion buffer */

static BOOL CreateIda10ConversionBuffer(NRTS_DL *dl)
{
static char *fid = "nrtsOpenDiskLoop:InitPacketDecoder:CreateIda10ConversionBuffer";

    if ((dl->ida10 = listCreate()) == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

/* Determine the packet decoder function */

static BOOL InitPacketDecoder(NRTS_DL *dl, char *site)
{
int rev;
char *map;

    if (dl->sys->type == NRTS_IDA) {
        rev = ((struct nrts_ida *) dl->sys->info)->rev;
        map = ((struct nrts_ida *) dl->sys->info)->map;
        dl->unpack = nrtsIdaDecoder;
    } else if (dl->sys->type == NRTS_IDA10) {
        rev = 10;
        map = NULL;
        dl->unpack = nrtsIda10Decoder;
        if (!CreateIda10ConversionBuffer(dl)) return FALSE;
    } else {
        errno = UNSUPPORTED;
        return FALSE;
    }

    if (map != NULL && strcmp(map, "-") == 0) map = NULL;

    dl->ida = idaCreateHandle(site, rev, map, dl->db, dl->lp, 0);
    return dl->ida != NULL ? TRUE : FALSE;
}

/* Memory map and unmap the sys file */

static NRTS_SYS *MapSys(char *fname, int perm, NRTS_MAP *buf)
{
char *rdwr = "rw";
char *rdonly = "r";

    if (fname == NULL || buf == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (nrts_mmap(fname, perm == ISI_RDWR ? rdwr : rdonly, NRTS_SYSTEM, buf) != 0) return NULL;

    return (NRTS_SYS *) buf->ptr;
}

static VOID UnmapSys(NRTS_MAP *map)
{
    if (map == NULL) return;
    nrts_unmap(map);
}

/* close a disk loop for one site */

VOID nrtsCloseDiskLoop(NRTS_DL *dl)
{
static char *fid = "nrtsCloseDiskLoop";

    if (dl == NULL) return;
    if (dl->perm == ISI_RDWR) {
        dl->sys->pid = 0;
        nrtsFlushBwd(dl);
    }
    if (dl->handle.hdr > 0) close(dl->handle.hdr);
    if (dl->handle.dat > 0) close(dl->handle.dat);
    nrtsFreeFileNames(&dl->fname);
    UnmapSys(&dl->map);
    dl->sys = NULL;
    dl->ida = idaDestroyHandle(dl->ida);
}

static BOOL SavePacket(NRTS_DL *dl, NRTS_PKT *pkt)
{
static char *fid = "SavePacket";

    if (pkt->chndx < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: unxpected chndx=%d", fid, pkt->chndx);
        return FALSE;
    }

    if (dl->save[pkt->chndx] == NULL) {
        if ((dl->save[pkt->chndx] = malloc(pkt->work.len)) == NULL) {
            logioMsg(dl->lp, LOG_ERR, "%s: malloc(%d): %s", fid, pkt->work.len, strerror(errno));
            return FALSE;
        }
    }

    memcpy(dl->save[pkt->chndx], &pkt->work.buf, pkt->work.len);

    return TRUE;
}

/* open disk loop for one site */

NRTS_DL *nrtsOpenDiskLoop(ISI_GLOB *glob, char *site, LOGIO *lp, int perm)
{
pid_t mypid;
NRTS_DL *dl;
int i, error;
NRTS_PKT pkt;
NRTS_STREAM stream;
static NRTS_TOLERANCE DEFAULT_TOL = { NRTS_DEFAULT_CLOCK_TOLERANCE, NRTS_DEFAULT_TIC_TOLERANCE, NRTS_DEFAULT_SAMP_TOLERANCE };
static char *fid = "nrtsOpenDiskLoop";

    pkt.indx = -1;

    if (glob == NULL || site == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL glob or site not allowed!", fid);
        errno = EINVAL;
        return NULL;
    }

    if (glob->db == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL glob db not allowed!", fid);
        errno = EINVAL;
        return NULL;
    }

    if ((dl = (NRTS_DL *) malloc(sizeof(NRTS_DL))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", strerror(errno));
        return NULL;
    }

    dl->lp = lp;
    dl->db = glob->db;
    dl->debug = &glob->debug;
    dl->home = glob->root;
    dl->perm = perm;
    dl->map.fd = 0;
    dl->map.ptr = NULL;
    memset(&dl->fname, 0, sizeof(NRTS_DLNAMES));
    for (i = 0; i < NRTS_MAXCHN; i++) dl->save[i] = NULL;
    dl->flags = NRTS_DL_FLAGS_DEFAULT;
    dl->tol = DEFAULT_TOL;
    dl->buf = NULL;
    dl->buflen = 0;
    dl->pkt = NULL;
    dl->sys = NULL;
    dl->ida = NULL;
    dl->ida10 = NULL;
    dl->handle.hdr = dl->handle.dat = -1;

    if (!nrtsBuildFileNames(glob->root, site, &dl->fname)) {
        logioMsg(lp, LOG_ERR, "%s: nrtsBuildFileNames: %s: %s", fid, site, strerror(errno));
        free(dl);
        return NULL;
    }

    if (!utilFileExists(dl->fname.sys)) {
        logioMsg(dl->lp, LOG_DEBUG, "%s: missing required dl file %s", fid, dl->fname.sys);
        errno = ENOENT;
        return NULL;
    }

    if (!utilFileExists(dl->fname.hdr)) {
        logioMsg(dl->lp, LOG_DEBUG, "%s: missing required dl file %s", fid, dl->fname.hdr);
        errno = ENOENT;
        return NULL;
    }
    if ((dl->handle.hdr = open(dl->fname.hdr, O_RDONLY|O_BINARY)) == -1) {
        logioMsg(dl->lp, LOG_DEBUG, "%s: can't open dl file %s", fid, dl->fname.hdr);
        return NULL;
    }

    if (!utilFileExists(dl->fname.dat)) {
        logioMsg(dl->lp, LOG_DEBUG, "%s: missing required dl file %s", fid, dl->fname.dat);
        errno = ENOENT;
        return NULL;
    }
    if ((dl->handle.dat = open(dl->fname.dat, O_RDONLY|O_BINARY)) == -1) {
        logioMsg(dl->lp, LOG_DEBUG, "%s: can't open dl file %s", fid, dl->fname.dat);
        return NULL;
    }

    if ((dl->sys = MapSys(dl->fname.sys, dl->perm, &dl->map)) == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: MapSys: %s: %s", fid, dl->fname.sys, strerror(errno));
        free(dl);
        return NULL;
    }

    if (!InitPacketDecoder(dl, site)) {
        logioMsg(dl->lp, LOG_ERR, "%s: InitPacketDecoder: %s", fid, strerror(errno));
        return NULL;
        free(dl);
    }

/* all done if we won't want to be the parent (ie, writer) */

    if (dl->perm != ISI_RDWR) return dl;

/* Othewise, make sure there already isn't a parent.
 * We ignore the lock if it matches our current pid. Process ID's for NRTS
 * processes will usually be constant across reboots (assuming they haven't
 * been manually stopped and restarted) and so the kill() test for our own
 * pid will make it look the the disk loop is already locked when in fact
 * it is just dirty because the system had previously crashed leaving the
 * pid in the memory mapped sys file non-zero.
 */

    mypid = getpid();
    if (dl->sys->pid == mypid) dl->sys->pid = 0;
    if (dl->sys->pid != 0 && kill(dl->sys->pid, 0) == 0) {
        errno = EACCES;
        logioMsg(dl->lp, LOG_ERR, "%s: disk loop locked by %d", fid, dl->sys->pid);
        dl->perm = ISI_RDONLY; /* so we don't clobber pid when closing */
        nrtsCloseDiskLoop(dl);
        free(dl);
        return NULL;
    }

/* mark ourselves as parent */

    dl->sys->pid = mypid;
    dl->sys->start = time(NULL);
    dl->sys->nread = 0;

/* allocate temp buffer for reading youngest packets */

    stream.sys = dl->sys;
    stream.sta = &dl->sys->sta[0];
    stream.chn = &dl->sys->sta[0].chn[0];
    dl->buflen = stream.chn->hdr.len + stream.chn->dat.len;
    for (i = 1; i < dl->sys->sta[0].nchn; i++) {
        stream.chn = &dl->sys->sta[0].chn[i];
        if (stream.chn->hdr.len + stream.chn->dat.len != dl->buflen) {
            logioMsg(dl->lp, LOG_ERR, "%s: %s disk loop contains non-constant buflen!", fid, site);
            nrtsCloseDiskLoop(dl);
            return NULL;
        }
    }

    if ((dl->buf = (UINT8 *) malloc(dl->buflen)) == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        nrtsCloseDiskLoop(dl);
        return NULL;
    }

/* Initialize the binary wfdisc file */

    if (!nrtsBuildBwdFromDisk(dl)) {
        logioMsg(dl->lp, LOG_ERR, "%s: nrtsBuildBwdFromDisk failed", fid);
        nrtsCloseDiskLoop(dl);
        return NULL;
    }

/* read most recent packet for each stream so that we can check for gaps */

    error = 0;
    for (i = 0; error == 0 && i < dl->sys->sta[0].nchn; i++) {
        stream.chn = &dl->sys->sta[0].chn[i];
        if (stream.chn->count > 0) {
            if (nrtsRead(dl, &stream, stream.chn->hdr.yngest, dl->buf, dl->buflen, &pkt, NULL) < 0) {
                ++error;
            } else if (!SavePacket(dl, &pkt)) {
                return NULL;
            }
            stream.chn->nread = 0; /* flag this session as just starting */
        }
    }

    if (error != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: error reading %d disk loop!", fid, site);
        nrtsCloseDiskLoop(dl);
        return NULL;
    }

    return dl;
}

BOOL nrtsWriteToDiskLoop(NRTS_DL *dl, UINT8 *buf)
{
NRTS_PKT *pkt, localpkt;
NRTS_CHN *chn;
off_t off = 0;
long oldest, yngest, lend;
int hfd, dfd, action, TestOrExtend;
char tmp[256];
BOOL FirstPacket;
static char *fid = "nrtsWriteToDiskLoop";

    if (dl == NULL || buf == NULL) {
        if (dl != NULL) logioMsg(dl->lp, LOG_ERR, "%s: NULL inputs not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

/*  If not already decoded, decode it to find its place in the sys table.
 *  Corrupt or non-data packets are silently ignored.
 */

    if (dl->pkt == NULL) {
        pkt = &localpkt;
        if (!(dl->unpack)(dl, NULL, buf, pkt)) return TRUE;
    } else {
        pkt = dl->pkt;
    }
    if (pkt->status != NRTS_ACCEPT) return TRUE;

    chn = &dl->sys->sta[0].chn[pkt->chndx];
    FirstPacket = (chn->nread == 0) ? TRUE : FALSE;

/*  Determine the offsets to write to */

    if ((yngest = chn->hdr.yngest + 1) == chn->hdr.nrec) {
        yngest = 0;
        pkt->tear = TRUE; /* since we are wrapping around the end of file */
    }

    if (yngest == chn->hdr.lend) {
        if ((oldest = chn->hdr.oldest + 1) == chn->hdr.nrec) oldest = 0;
        if ((lend   = chn->hdr.lend   + 1) == chn->hdr.nrec) lend   = 0;
        /* overwriting existing data, modify old wfdisc to show this */
        if (!nrtsUpdateBwd(dl, pkt, NRTS_REDUCE)) {
            logioMsg(dl->lp, LOG_ERR, "%s: nrtsUpdateBwd failed", fid);
            return FALSE;
        }
    } else {
        oldest = chn->hdr.oldest;
        lend   = chn->hdr.lend;
    }

    chn->hdr.backup.oldest = oldest;
    chn->hdr.backup.yngest = yngest;
    chn->hdr.backup.lend   = lend;

    pkt->foff.hdr = chn->hdr.off + (yngest * chn->hdr.len);
    pkt->foff.dat = chn->dat.off + (yngest * chn->dat.len);

/* seek to proper offset in the header file */

    if ((hfd = open(dl->fname.hdr, O_RDWR  | O_BINARY | O_SYNC)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: open: %s: %s", fid, dl->fname.hdr, syserrmsg(errno));
        return FALSE;
    }

    if (lseek(hfd, pkt->foff.hdr, SEEK_SET) != pkt->foff.hdr) {
        logioMsg(dl->lp, LOG_ERR, "%s: lseek: %s", dl->fname.hdr, fid, syserrmsg(errno));
        return FALSE;
    }

/* seek to proper offset in the data file */

    if ((dfd = open(dl->fname.dat, O_RDWR  | O_BINARY | O_SYNC)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: open: %s: %s", fid, dl->fname.dat, syserrmsg(errno));
        return FALSE;
    }

    if (lseek(dfd, pkt->foff.dat, SEEK_SET) != pkt->foff.dat) {
        logioMsg(dl->lp, LOG_ERR, "%s: lseek: %s: %s", fid, dl->fname.dat, syserrmsg(errno));
        return FALSE;
    }

/* write the data */

    chn->status = NRTS_BUSY1;

    if (write(hfd, buf, chn->hdr.len) != chn->hdr.len) {
        logioMsg(dl->lp, LOG_ERR, "%s: write: %s", fid, syserrmsg(errno));
        return FALSE;
    }
    close(hfd);

    if (write(dfd, buf + chn->hdr.len, chn->dat.len) != chn->dat.len) {
        logioMsg(dl->lp, LOG_ERR, "%s: write: %s", fid, syserrmsg(errno));
        return FALSE;
    }
    close(dfd);

/* save the sample interval if this is the first packet we've seen for this stream */

    if (FirstPacket) chn->sint = (float) isiSrateToSint(&pkt->isi.hdr.srate);
    
/*  Update the pointers  */

    chn->status = NRTS_BUSY2;

    chn->hdr.oldest = oldest;
    chn->hdr.yngest = yngest;
    chn->hdr.lend   = lend;

    chn->status = NRTS_IDLE;

/*  Update the counters  */

    ++chn->nread;
    ++chn->count;
    chn->tread = time(NULL);

    ++dl->sys->nread;
    ++dl->sys->count;

/*  Update the list of segments */

    if (!FirstPacket) {
        TestOrExtend = dl->flags & NRTS_DL_FLAGS_ALWAYS_TESTWD ? NRTS_TESTWD : NRTS_EXTEND;
        action = pkt->tear ? NRTS_CREATE : TestOrExtend;
    } else {
        action = NRTS_TESTWD;
    }

    if (!nrtsUpdateBwd(dl, pkt, action)) {
        logioMsg(dl->lp, LOG_ERR, "%s: nrtsUpdateBwd failed", fid);
        return FALSE;
    }

/* Save the packet for later comparisons */

    if (!SavePacket(dl, pkt)) return FALSE;
    return TRUE;
}

/* Revision History
 *
 * $Log: dl.c,v $
 * Revision 1.19  2008/01/11 00:53:56  dechavez
 * added missing default return value to CreateIda10ConversionBuffer()
 *
 * Revision 1.18  2008/01/07 21:44:12  dechavez
 * added support for IDA10 conversion buffer,
 * fixed nrtsWriteToDiskLoop() to ignore everything but explicity accepted packets
 *
 * Revision 1.17  2007/06/01 19:22:21  dechavez
 * added new (NULL) chn args to nrtsRead
 *
 * Revision 1.16  2007/04/18 22:53:34  dechavez
 * support NRTS_DL_FLAGS_ALWAYS_TESTWD option in bwd determining action argument
 * for nrtsUpdateBwd() in nrtsWriteToDiskLoop()
 *
 * Revision 1.15  2007/03/26 21:38:18  dechavez
 * renamed NRTS_DEFAULT_FLAGS constant to NRTS_DL_FLAGS_DEFAULT
 *
 * Revision 1.14  2007/02/01 01:54:25  dechavez
 * call nrtsUpdateBwd() with NRTS_TESTWD in the case of a fresh start
 * (eliminates extra wfdisc records that don't represent a true time tear)
 *
 * Revision 1.13  2006/07/19 22:18:47  dechavez
 * additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes
 *
 * Revision 1.12  2006/06/19 18:08:39  dechavez
 * open/close new read-only handles in nrtsOpenDiskLoop and nrtsCloseDiskLoop (aap)
 *
 * Revision 1.11  2006/04/20 22:57:02  dechavez
 * don't clopper pid when encountering a locked disk loop
 *
 * Revision 1.10  2006/04/17 17:23:32  dechavez
 * zero pid in sys before lock test if it matches current pid (this prevents
 * failing to init after the system crashed)
 *
 * Revision 1.9  2006/03/13 22:35:30  dechavez
 * support changed field names in updated ISI_GLOB
 *
 * Revision 1.8  2006/02/09 19:46:04  dechavez
 * IDA handle support
 *
 * Revision 1.7  2005/11/04 00:02:27  dechavez
 * suppress microsoft 4550 compiler warnings, removed unreferenced local variables
 *
 * Revision 1.6  2005/10/06 22:03:00  dechavez
 * included value for new samp field in default NRTS_TOLERANCE
 *
 * Revision 1.5  2005/09/30 18:12:18  dechavez
 * added platform.h (09-30-2005 aap)
 *
 * Revision 1.4  2005/09/15 00:02:01  dechavez
 * check for and quietly fail (ENOENT, LOG_DEBUG message) if not all the required
 * NRTS files are present
 *
 * Revision 1.3  2005/07/27 18:35:11  dechavez
 * save hdr and dat offsets in pkt header (which is _essential_ to good bwds!)
 *
 * Revision 1.2  2005/07/26 17:13:34  dechavez
 * flush disk loop on close, fixed SavePacket() bug with new disk loops
 *
 * Revision 1.1  2005/07/25 23:58:53  dechavez
 * initial release
 *
 */
