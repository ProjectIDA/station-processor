#pragma ident "$Id: search.c,v 1.3 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Search disk buffer for record with indicated time;
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "edes.h"

#define STA (edes->sys[i].sta + j)
#define CHN (edes->sys[i].sta[j].chn + k)

static long do_search(struct edes_params *edes, int i, int j, int k, double target, long left, long rite, char *buffer)
{
long test;
struct edes_span *span;
static char *fid = "do_search";

    span = get_span(edes, i, j, k, left, buffer);
    if (target >= span->beg && target <= span->end) {
        return left;
    }

    span = get_span(edes, i, j, k, rite, buffer);
    if (target >= span->beg && target <= span->end) {
        return rite;
    }

    while (rite - left > 1) {
        test = left + ((rite - left)/2);
        span = get_span(edes, i, j, k, test, buffer);
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

long search(struct edes_params *edes, int i, int j, int k, double target, long ldef, long rdef, char *buffer)
{
struct edes_span *span, dl;
long oldest, yngest;
static char *fid = "search";

    oldest = CHN->hdr.oldest;
    yngest = CHN->hdr.yngest;

    util_log(3, "`%s:%s' search for time %s",
        STA->name, CHN->name, util_dttostr(target, 0)
    );
    util_log(3, "`%s:%s' oldest = %ld, youngest = %ld",
        STA->name, CHN->name, oldest, yngest
    );

/* First, insure the desired time falls inside the disk buffer */

    span   = get_span(edes, i, j, k, oldest, buffer);
    dl.beg = span->beg;
    if (target == dl.beg) {
        util_log(3, "`%s:%s' target is oldest avail, return %d",
            STA->name, CHN->name, oldest
        );
        return oldest;
    }

    span   = get_span(edes, i, j, k, yngest, buffer);
    dl.end = span->end;
    if (target == dl.end) {
        util_log(3, "`%s:%s' target is yngest avail, return %d",
            STA->name, CHN->name, yngest
        );
        return yngest;
    }

    sprintf(buffer, "`%s:%s' spans %s ",
        STA->name, CHN->name, util_dttostr(dl.beg, 0)
    );
    util_log(3, "%s to %s", buffer, util_dttostr(dl.end, 0));

    if (target < dl.beg) {
        util_log(3, "`%s' target too old, return %d", CHN->name, ldef);
        return ldef;
    }

    if (target > dl.end) {
        util_log(3, "`%s' target too young, return %d", CHN->name, rdef);
        return rdef;
    }

/* It must be available, so search for it */
/* First case, no wraparound at end of disk buffer */

    if (oldest < yngest) {
        util_log(3, "`%s:%s' no disk loop wrap around",
            STA->name, CHN->name
        );
        util_log(3, "`%s:%s' search from oldest (%ld) to youngest (%ld)",
            STA->name, CHN->name, oldest, yngest
        );
        return do_search(edes, i, j, k, target, oldest, yngest, buffer);
    }

/* Second case, data wrap around end of disk buffer */

    span = get_span(edes, i, j, k, (long) 0, buffer);
    span->beg -= CHN->sint;  /* will allow for one sample slop */

    util_log(3, "`%s:%s'    target=%s", STA->name, CHN->name, util_dttostr(target, 0));
    util_log(3, "`%s:%s' span->beg=%s", STA->name, CHN->name, util_dttostr(span->beg, 0));
    util_log(3, "`%s:%s'    dl.end=%s", STA->name, CHN->name, util_dttostr(dl.end, 0));

    if (target > span->beg && target <= dl.end) {
        util_log(3, "`%s:%s' target is in later half of disk buffer",
            STA->name, CHN->name
        );
        util_log(3, "`%s:%s' search from 0 to youngest (%ld)",
            STA->name, CHN->name, yngest
        );
        return do_search(edes, i, j, k, target, 0, yngest, buffer);
    } else {
        span = get_span(edes, i, j, k, (long) CHN->hdr.nrec - 1, buffer);
        if (target > dl.beg && target <= span->end) {
            util_log(3, "`%s:%s' search from oldest (%ld) to eof (%ld)",
                STA->name, CHN->name, oldest, CHN->hdr.nrec - 1
            );
            return do_search(edes, i, j, k, 
                target, oldest, CHN->hdr.nrec - 1, buffer
            );
        } else {
            util_log(1, "HELP! I don't know what to do!");
            util_log(1, "target = %s", util_dttostr(target, 0));
            util_log(1, "dl beg = %s", util_dttostr(dl.beg, 0));
            util_log(1, "dl end = %s", util_dttostr(dl.end, 0));
            util_log(1, "end of dl file time = ");
            util_log(1, "%s", util_dttostr(span->end, 0));
            xfer_errno = XFER_EFAULT;
            Xfer_Ack(edes->osd, xfer_errno);
            Xfer_Exit(edes->osd, xfer_errno);
        }
    }

    util_log(1, "%s: FATAL PROGRAM LOGIC ERROR!", fid);
    util_log(1, "%s: target = %s", fid, util_dttostr(target, 0));
    util_log(1, "%s: dl beg = %s", fid, util_dttostr(dl.beg, 0));
    util_log(1, "%s: dl end = %s", fid, util_dttostr(dl.end, 0));
    xfer_errno = XFER_EFAULT;
    Xfer_Ack(edes->osd, xfer_errno);
    Xfer_Exit(edes->osd, xfer_errno);
}

/* Revision History
 *
 * $Log: search.c,v $
 * Revision 1.3  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.2  2001/02/09 00:49:22  nobody
 * 2.11.2 - fixed search bug when target lies on exact end of disk loop
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
