#pragma ident "$Id: getrec.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Get the next record for this channel.  Return a pointer to the data
 *  or NULL if no more records are available.
 *
 *====================================================================*/
#include <errno.h>
#include "nrts_xfer.h"

char *getrec(struct nrts_sys *sys, struct nrts_chn *chn, struct old_xfer_req *req, struct old_xfer_chn *rchn, char *buffer)
{
int dummy;
long index;
off_t off;
struct nrts_header *hd;
static struct nrts_packet packet;
static char *fid = "getrec";

    index = rchn->nxtndx;

/* Read the header part */

    off = chn->hdr.off + (index * chn->hdr.len);

    if (lseek(req->hfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(req->hfd, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        return NULL;
    }

    if (read(req->hfd, buffer, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "%s: read(req->hfd, buf, %d): %s",
                 fid, chn->hdr.len, syserrmsg(errno)
        );
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        return NULL;
    }

/* Read the data part */

    off = chn->dat.off + (index * chn->dat.len);

    if (lseek(req->dfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(req->dfd, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, dat off =  %ld",
            fid, chn->name, index, off
        );
        return NULL;
    }

    if (read(req->dfd, buffer+chn->hdr.len, chn->dat.len) != chn->dat.len) {
        util_log(1, "%s: read(req->dfd, buf+chn->hdr.len, %d): %s",
                 fid, chn->dat.len, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, dat off =  %ld",
            fid, chn->name, index, off
        );
        return NULL;
    }

/* Increment indices for next read */

    if (rchn->nxtndx == rchn->endndx) {
        rchn->done = 1;
    } else {
        ++rchn->nxtndx;
        if (
            rchn->endndx < rchn->begndx &&
            rchn->nxtndx == chn->hdr.nrec
        ) rchn->nxtndx = 0;
    }

    return buffer;

}

/* Revision History
 *
 * $Log: getrec.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
