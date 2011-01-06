#pragma ident "$Id: span.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Get time span for data record with the given index.
 *
 *====================================================================*/
#include <errno.h>
#include <unistd.h>
#include "util.h"
#include "nrts_xfer.h"

struct span *get_span(struct old_xfer_req *req, int index, struct nrts_chn *chn, char *buffer)
{
int dummy;
static struct span span;
struct nrts_header *hd;
off_t off;
static char *fid = "span";

/* Read the header */

    off = chn->hdr.off + (index * chn->hdr.len);

    if (lseek(req->hfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(req->hfd, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        ack(errno);
        die(1);
    }

    if (read(req->hfd, buffer, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "%s: read(req->hfd, buffer, %d): %s",
                 fid, chn->hdr.len, syserrmsg(errno)
        );
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        ack(errno);
        die(1);
    }

/* Decode the header */

    if ((hd = (*req->decode)(buffer, chn->hdr.len, &dummy)) == NULL) {
        util_log(1, "%s: can't decode header", fid);
        ack(errno);
        die(1);
    }

    span.beg = hd->wfdisc.time;
    span.end = hd->wfdisc.endtime;
    
    return &span;
}

/* Revision History
 *
 * $Log: span.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
