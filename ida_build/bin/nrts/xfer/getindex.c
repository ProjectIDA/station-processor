#pragma ident "$Id: getindex.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Get dl indices corresponding to requested times.
 *
 *====================================================================*/
#include "nrts_xfer.h"

void getindex(struct old_xfer_req *req, struct nrts_chn *chn, struct old_xfer_chn *rchn, char *buf)
{
unsigned long count, newrec;
long oldest, yngest;
static char *fid = "getindex";

    count = chn->count;

/* Update indices if we have already xferd data for this channel */

    if (rchn->count > 0) {

        if (count < rchn->count) {
            util_log(1, "unexpected error, %s counter decremented!", chn->name);
            util_log(1, "was %u, is %u", count, rchn->count);
            die(1);
        }

        if ((newrec = count - rchn->count) > chn->hdr.nrec) {
            util_log(1,
                "Oops! Disk loop was overrun while prior xfer was in effect!"
            );
            util_log(1,
                "Since I never expected this to happen no code was developed"
            );
            util_log(1,
                "to deal with the problem.  Sorry about that, and good luck!"
            );
        }

        if (newrec == 0) {
            rchn->done = 1;
            return;
        }

        rchn->begndx = (rchn->endndx + 1) % chn->hdr.nrec;
        rchn->endndx = (rchn->begndx + newrec - 1) % chn->hdr.nrec;

/* Otherwise search the disk loop if there are any data in it */

    } else if (count > 0) {

        oldest = chn->hdr.oldest;
        yngest = chn->hdr.yngest;

    /* Get first index */

        if (rchn->begtim == NRTS_OLDEST) {
            rchn->begndx = oldest;
            util_log(3, "%s beg index set to %d (NRTS_OLDEST)",
                chn->name, oldest
            );
        } else if (rchn->begtim == NRTS_YNGEST) {
            rchn->begndx = yngest;
            util_log(3, "%s beg index set to %d (NRTS_YNGEST)",
                chn->name, yngest
            );
        } else {
            rchn->begndx = search(req, chn, rchn->begtim, oldest, -1, buf);
            if (rchn->begndx < 0) {
                rchn->done = 1;
                return;
            }
            util_log(3, "%s beg index set to %d", chn->name, rchn->begndx);
        }

    /* Get the last index */

        if (rchn->endtim == NRTS_OLDEST) {
            rchn->endndx = oldest;
            util_log(3, "%s end index set to %d (NRTS_OLDEST)",
                chn->name, oldest
            );
        } else if (rchn->endtim == NRTS_YNGEST) {
            rchn->endndx = yngest;
            util_log(3, "%s end index set to %d (NRTS_YNGEST)",
                chn->name, yngest
            );
        } else {
            rchn->endndx = search(req, chn, rchn->endtim, -1, yngest, buf);
            util_log(3, "%s end index set to %d", chn->name, rchn->endndx);
        }

/* or ignore this channel if the disk loop is empty */

    } else {
        util_log(3, "ignore channel %s", chn->name);
        rchn->done = 1;
        return;
    }

/* Initialize next index to read to the begining */

    rchn->nxtndx = rchn->begndx;

/* Save the current dl counter and mark the channel ready for xfer */

    rchn->count = count;
    rchn->done  = 0;

    return;
}

/* Revision History
 *
 * $Log: getindex.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
