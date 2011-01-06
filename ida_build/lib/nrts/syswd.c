#pragma ident "$Id: syswd.c,v 1.10 2006/06/26 23:47:38 dechavez Exp $"
/*======================================================================
 *
 *  Return all the wfdiscs for a single NRTS system.
 *
 *====================================================================*/
#include "nrts.h"
#include "cssio.h"
#include "isi/dl.h"

typedef struct {
    int count;
    int hlen;
    int dlen;
    struct wfdisc *wfdisc;
} WORKSPACE;

static struct wfdisc *GetEmptyRecord(char *fid, WORKSPACE *work)
{
int newlen;
struct wfdisc *newbuf;

    ++work->count;
    newlen = work->count * sizeof(struct wfdisc);
    newbuf = (struct wfdisc *) realloc(work->wfdisc, newlen);
    if (newbuf == NULL) {
        work->count = 0;
        if (work->wfdisc != NULL) free(work->wfdisc);
           util_log(1, "%s: realloc: %s", fid, strerror(errno));
           return NULL;
    }
    work->wfdisc = newbuf;

    return &work->wfdisc[work->count - 1];
}

static BOOL ReadOneHeader(
    char *fid, int fd, long foff,
    struct nrts_header *hdr, struct nrts_header *(*decode)(),
    int hlen, int dlen, char *buf
){
int RecordNo, unused;
off_t offset;
struct nrts_header *result;

    RecordNo = foff / dlen;
    if (RecordNo * dlen != foff) {
        util_log(2, "%s: ReadOneHeader: foff %d is not a multiple of %d",
            fid, foff, dlen
        );
        return FALSE;
    }
    offset   = RecordNo * hlen;

    if (lseek(fd, offset, SEEK_SET) != offset) {
        util_log(1, "%s: fseek(%d): %s", fid, offset, strerror(errno));
        return FALSE;
    }

    if (read(fd, buf, hlen) != hlen) {
        util_log(1, "%s: read: %s", fid, strerror(errno));
        return FALSE;
    }

    if ((result = (*decode)(buf, hlen, &unused)) == NULL) {
        util_log(1, "%s: decode: %s", fid, strerror(errno));
        return FALSE;
    }

    *hdr = *result;

    return TRUE;
}

/* Determine maximum number of samples, subject to DLEN boundaries */

static int GetMaxsamp(int maxdur, double srate, int dlen)
{
int maxsamp, rem;

    maxsamp = (int) ((double) maxdur * srate);
    if ((rem = maxsamp % dlen) != 0) maxsamp += (dlen - rem);

    return maxsamp;
}

/* Clean error return */

static int cleanup(IDA *ida, char *buf, struct wfdisc *wfdisc, int nrec)
{
    idaDestroyHandle(ida);
    if (buf != NULL) free(buf);
    if (wfdisc != NULL) free(wfdisc);
    return nrec;
}

int nrts_syswd(char *home, char *syscode, struct wfdisc **output, int flush, int maxdur, char *dbspec)
{
int i, j, fd, hlen, dlen, rev;
size_t nrec, len;
struct nrts_files *file;
struct nrts_sys *sys;
struct nrts_mmap mmap;
struct wfdisc *LongRecs, *out;
long maxsamp, foff, remain;
WORKSPACE work = {0, 0, 0, NULL};
struct nrts_header beg;
struct nrts_header end;
struct nrts_header *(*decode)();
char *buf = NULL, *mapname;
DBIO *db;
IDA *ida = NULL;
static char *fid = "nrts_syswd";

    if (syscode == NULL) {
        errno = EINVAL;
        return -1;
    }

    file = nrts_files(&home, syscode);

    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &mmap) != 0) return -1;
    sys = (struct nrts_sys *) mmap.ptr;
    close(mmap.fd);
    if (flush != 0 && sys->pid != 0) {
        util_log(2, "flushing wfdiscs");
#ifndef _WIN32
        if (kill(sys->pid, SIGHUP) == 0) sleep(1);
#else
/* WIN32 code will be here */
#endif
    }

    if ((fd = open(file->bwd, O_RDONLY | O_BINARY )) < 0) return -2;

/* Lock the binary wfdisc file */

    if (util_rlockw(fd, 0, SEEK_SET, 0) != 0) {
        close(fd);
        return -2;
    }

/* Seek to the end to determine length */

    if ((long) (len = lseek(fd, 0, SEEK_END)) < 0) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return -3;
    } else if (len == 0) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return 0;
    }

/* Rewind to start of file */

    if (lseek(fd, 0, SEEK_SET) != 0) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return -4;
    }

/* Determine number of records to read and prepare space */

    nrec = len / sizeof(struct wfdisc);
    if (nrec * sizeof(struct wfdisc) != len) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return -5;
    }

    if ((LongRecs = (struct wfdisc *) malloc(len)) == NULL) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return -6;
    }

/* Read in the file */

    if (read(fd, LongRecs, len) != (int) len) {
        util_unlock(fd, 0, SEEK_SET, 0);
        close(fd);
        return -7;
    }

    for (i = 0; i < (int) nrec; i++) {
        util_log(3, "%s: %d: '%s'", fid, i, wdtoa(&LongRecs[i]));
    }

    util_unlock(fd, 0, SEEK_SET, 0);
    close(fd);

/* If maxdur option not selected, then we are done */

    *output = LongRecs;
    if (maxdur <= 0) return nrec;

/* Otherwise, break down records into segments of no more than
 * maxdur seconds
 */

    work.count  = 0;
    work.wfdisc = NULL;

    if ((fd = open(file->hdr, O_RDONLY|O_BINARY)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->hdr, strerror(errno));
        return cleanup(ida, buf, work.wfdisc, nrec);
    }

    if (sys->type == NRTS_IDA) {
        rev = ((struct nrts_ida *) sys->info)->rev;
        mapname = ((struct nrts_ida *) sys->info)->map;
    } else if (sys->type == NRTS_IDA10) {
        rev = 10;
        mapname = NULL;
    } else {
        util_log(1, "%s: unsupported system type = %d", fid, sys->type);
        return cleanup(ida, buf, work.wfdisc, nrec);
    }

    if ((ida = idaCreateHandle(sys->sta[0].name, rev, mapname, db, NULL, 0)) == NULL) {
        util_log(1, "%s: idaCreateHandle failed", fid);
        return cleanup(ida, buf, work.wfdisc, nrec);
    }
    decode = (struct nrts_header *(*)())
             nrts_inidecode(home, syscode, sys, 0, ida, (void *) 0, fd);

    if (decode == NULL) {
        util_log(1, "%s: nrts_inidecode: %s", fid, strerror(errno));
        return cleanup(ida, buf, work.wfdisc, nrec);
    }

/* Make sure we have constant header and data record lengths */

    hlen = sys->sta[0].chn[0].hdr.len;
    dlen = sys->sta[0].chn[0].dat.len;
    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            if (
                (sys->sta[i].chn[j].hdr.len != hlen) ||
                (sys->sta[i].chn[j].dat.len != dlen)
            ) return cleanup(ida, buf, work.wfdisc, nrec);
        }
    }

    if ((buf = (char *) malloc(dlen)) == NULL) {
        util_log(1, "%s: malloc: %s", fid, strerror(errno));
        return cleanup(ida, buf, work.wfdisc, nrec);
    }

/* Split each input wfdisc into smaller segments, using actual
 * header values to determine start and end times.
 */

    for (i = 0; i < (int) nrec; i++) {
        foff   = LongRecs[i].foff;
        remain = LongRecs[i].nsamp;
        while (remain > 0) {
            if ((out = GetEmptyRecord(fid, &work)) == NULL) {
                return cleanup(ida, buf, work.wfdisc, nrec);
            }
            *out = LongRecs[i];

#ifdef DEBUG
            fprintf(stderr, "count=%d, remain=%d, foff=%d\n",
                work.count, remain, foff
            );
#endif /* DEBUG */
            if (!ReadOneHeader(
                fid, fd, foff, &beg, decode, hlen, dlen, buf)
            ) return cleanup(ida, buf, work.wfdisc, nrec);
            maxsamp = GetMaxsamp(maxdur, beg.wfdisc.smprate, dlen);
            out->foff  = foff;
            out->nsamp = (remain > maxsamp) ? maxsamp : remain;
            out->time  = beg.wfdisc.time;
            foff += (beg.wrdsiz * out->nsamp) - dlen;

#ifdef DEBUG
            fprintf(stderr, "count=%d, maxsamp=%d, nsamp=%d, foff=%d\n",
                work.count, maxsamp, out->nsamp, foff
            );
#endif /* DEBUG */
            if (!ReadOneHeader(
                fid, fd, foff, &end, decode, hlen, dlen, buf)
            ) return cleanup(ida, buf, work.wfdisc, nrec);
            out->endtime = end.wfdisc.endtime;

            remain -= out->nsamp;
            foff   += dlen;
        }
    }

    idaDestroyHandle(ida);
    free(buf);
    free(LongRecs);
    close(fd);
    *output = work.wfdisc;
    return work.count;
}

/* Revision History
 *
 * $Log: syswd.c,v $
 * Revision 1.10  2006/06/26 23:47:38  dechavez
 * revert back to 1.8
 *
 * Revision 1.8  2006/02/09 19:42:26  dechavez
 * IDA handle support
 *
 * Revision 1.7  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.6  2004/07/26 22:53:14  dechavez
 * open files with O_BINARY attribute
 *
 * Revision 1.5  2004/06/24 17:33:58  dechavez
 * removed unnecessary includes, WIN32 "port" (aap)
 *
 * Revision 1.4  2004/04/23 00:46:04  dechavez
 * changed dbdir to dbspec
 *
 * Revision 1.3  2002/11/20 01:06:13  dechavez
 * added dbdir support so that ida and asl decoders would work and removed
 * assumptions about data and header record lengths
 *
 * Revision 1.2  2002/11/19 18:34:42  dechavez
 * added support for maxdur parameter
 *
 * Revision 1.1.1.1  2000/02/08 20:20:31  dec
 * import existing IDA/NRTS sources
 *
 */
