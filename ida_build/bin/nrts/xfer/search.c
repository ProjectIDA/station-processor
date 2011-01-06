#pragma ident "$Id: search.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Search disk buffer for record with indicated time;
 *
 *====================================================================*/
#include "nrts_xfer.h"

static long do_search(struct old_xfer_req *req, double target, struct nrts_chn *chn, long left, long rite, char *buffer)
{
long test;
struct span *span;
static char *fid = "do_search";

    span = get_span(req, left, chn, buffer);
    if (target >= span->beg && target <= span->end) {
        return left;
    }

    span = get_span(req, rite, chn, buffer);
    if (target >= span->beg && target <= span->end) {
        return rite;
    }

    while (rite - left > 1) {
        test = left + ((rite - left)/2);
        span = get_span(req, test, chn, buffer);
        if (target >= span->beg && target <= span->end) {
            return test;
        }

        if (target > span->end) {
            left = test;
        } else {
            rite = test;
        }
    }

    return rite;
}

long search(struct old_xfer_req *req, struct nrts_chn *chn, double target, long ldef, long rdef, char *buffer)
{
struct span *span, dl;
long oldest, yngest;
static char *fid = "search";

    oldest = chn->hdr.oldest;
    yngest = chn->hdr.yngest;
    util_log(3, "`%s' search for time %s", chn->name, util_dttostr(target, 0));
    util_log(3, "`%s' oldest = %ld, youngest = %ld", chn->name, oldest, yngest);

/* First, insure the desired time falls inside the disk buffer */

    span = get_span(req, oldest, chn, buffer);
    dl.beg = span->beg;

    span = get_span(req, yngest, chn, buffer);
    dl.end = span->end;
    sprintf(buffer, "`%s' spans %s ", chn->name, util_dttostr(dl.beg, 0));
    util_log(3, "%s to %s", buffer, util_dttostr(dl.end, 0));

    if (target < dl.beg) {
        util_log(3, "`%s' target too old, return %d", chn->name, ldef);
        return ldef;
    }

    if (target > dl.end) {
        util_log(3, "`%s' target too young, return %d", chn->name, rdef);
        return rdef;
    }

/* It must be available, so search for it */
/* First case, no wraparound at end of disk buffer */

    if (oldest < yngest) {
        util_log(3, "`%s' no disk loop wrap around");
        util_log(3, "`%s' search from oldest (%ld) to youngest (%ld)",
            chn->name, oldest, yngest
        );
        return do_search(req, target, chn, oldest, yngest, buffer);
    }

/* Second case, data wrap around end of disk buffer */

    span = get_span(req, (long) 0, chn, buffer);
    span->beg -= chn->sint;  /* will allow for one sample slop */
    if (target > span->beg && target <= dl.end) {
        util_log(3, "`%s' target is in later half of disk buffer", chn->name);
        util_log(3, "`%s' search from 0 to youngest (%ld)",
            chn->name, yngest
        );
        return do_search(req, target, chn, 0, yngest, buffer);
    } else {
        span = get_span(req, (long) chn->hdr.nrec - 1, chn, buffer);
        if (target > dl.beg && target <= span->end) {
            util_log(3, "`%s' search from oldest (%ld) to eof (%ld)",
                chn->name, oldest, chn->hdr.nrec - 1
            );
            return do_search(req,target,chn,oldest,chn->hdr.nrec - 1,buffer);
        } else {
            util_log(1, "HELP! I don't know what to do!");
            util_log(1, "target = %s", util_dttostr(target, 0));
            util_log(1, "dl beg = %s", util_dttostr(dl.beg, 0));
            util_log(1, "dl end = %s", util_dttostr(dl.end, 0));
            util_log(1, "end of dl file time = %s", util_dttostr(span->end, 0));
            ack(0);
            die(1);
        }
    }

    util_log(1, "%s: FATAL PROGRAM LOGIC ERROR!", fid);
    util_log(1, "%s: target = %s", fid, util_dttostr(target, 0));
    util_log(1, "%s: dl beg = %s", fid, util_dttostr(dl.beg, 0));
    util_log(1, "%s: dl end = %s", fid, util_dttostr(dl.end, 0));
}

/* Revision History
 *
 * $Log: search.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
