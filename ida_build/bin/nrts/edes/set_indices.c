#pragma ident "$Id: set_indices.c,v 1.3 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Set dl indices corresponding to requested times.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "edes.h"

#define SYS  (edes->sys + i)
#define STA  (SYS->sta + j)
#define CHN  (STA->chn + k)
#define ICHN (edes->inf[i].sta[j].chn + k)

void set_indices(struct edes_params *edes, int i, int j, int k, char *buffer)
{
unsigned long newrec;
long oldest, yngest;
static char *fid = "set_indices";

/* Get snapshot of current system status */

    *SYS = *(struct nrts_sys *) (edes->map[i].ptr);

/* Update indices if we have already xferd data for this channel */

    if (ICHN->count > 0) {
        util_log(3, "%s: updating %s:%s indices, CHN->count = %d, ICHN->count = %d",
            fid, STA->name, CHN->name, CHN->count, ICHN->count
        );

        if (CHN->count < ICHN->count) {
            util_log(1, "%s: unexpected error, %s:%s counter decremented!",
                fid, STA->name, CHN->name
            );
            util_log(1, "%s: was %u, is %u", fid, CHN->count, ICHN->count);
            Xfer_Ack(edes->osd, XFER_EFAULT);
            Xfer_Exit(edes->osd, XFER_EFAULT);
        }

        /* Determine how many new records were acquired */

        if ((newrec = CHN->count - ICHN->count) > CHN->hdr.nrec) {
            util_log(1,"Oops! Disk loop was overrun during xfer!");
            util_log(1,"Good luck figuring out how to deal with it.");
            Xfer_Ack(edes->osd, XFER_EFAULT);
            Xfer_Exit(edes->osd, XFER_EFAULT);
        }

        if (newrec == 0) {
            util_log(3, "%s: no new %s:%s data available",
                fid, STA->name, CHN->name
            );
            ICHN->status = (edes->keepup > 0) ? EDES_WAITING : EDES_DONE;
            return;
        } else if (newrec > 0) {
            ICHN->begndx = (ICHN->endndx + 1) % CHN->hdr.nrec;
            ICHN->endndx = (ICHN->begndx + newrec - 1) % CHN->hdr.nrec;
            util_log(3, "%s: %s:%s beg index set to %d",
                fid, STA->name, CHN->name, ICHN->begndx
            );
            util_log(3, "%s: %s:%s end index set to %d",
                fid, STA->name, CHN->name, ICHN->endndx
            );
        } else {
            util_log(1,"Oops! Got a negative newrec!");
            util_log(1,"Good luck figuring out how to deal with it.");
            Xfer_Ack(edes->osd, XFER_EFAULT);
            Xfer_Exit(edes->osd, XFER_EFAULT);
        }

/* Otherwise search the disk loop if there are any data in it */

    } else if (CHN->count > 0) {
        util_log(3, "%s: searching disk loop for %s:%s data",
            fid, STA->name, CHN->name
        );

        oldest = CHN->hdr.oldest;
        yngest = CHN->hdr.yngest;
        util_log(3, "%s: oldest = %d, yngest = %d", fid, oldest, yngest);

    /* Get first index */

        if (ICHN->beg == XFER_OLDEST) {
            ICHN->begndx = oldest;
            util_log(3, "%s: beg = XFER_OLDEST", fid);
        } else if (ICHN->beg == XFER_YNGEST) {
            ICHN->begndx = yngest;
            util_log(3, "%s: beg = XFER_YNGEST", fid);
        } else {
            util_log(3, "%s: beg = %s", fid, util_dttostr(ICHN->beg, 0));
            ICHN->begndx = search(
                edes, i, j, k, ICHN->beg, oldest, -1, buffer
            );
        }
        util_log(3, "%s: %s:%s beg index set to %d",
            fid, STA->name, CHN->name, ICHN->begndx
        );

        if (ICHN->begndx < 0) {
            ICHN->status = (edes->keepup > 0) ? EDES_WAITING : EDES_DONE;
            return;
        }

    /* Get the last index */

        if (ICHN->end == XFER_OLDEST) {
            ICHN->endndx = oldest;
            util_log(3, "%s: end = XFER_OLDEST", fid);
        } else if (ICHN->end == XFER_YNGEST) {
            ICHN->endndx = yngest;
            util_log(3, "%s: end = XFER_YNGEST", fid);
        } else {
            util_log(3, "%s: end = %s", fid, util_dttostr(ICHN->end, 0));
            ICHN->endndx = search(
                edes, i, j, k, ICHN->end, -1, yngest, buffer
            );
        }
        util_log(3, "%s: %s:%s end index set to %d",
            fid, STA->name, CHN->name, ICHN->endndx
        );

        if (ICHN->endndx < 0) {
            ICHN->status = EDES_DONE;
            return;
        }

/* or ignore this channel if the disk loop is empty */

    } else {
        ICHN->status = (edes->keepup > 0) ? EDES_WAITING : EDES_DONE;
        return;
    }

/* Initialize next index to read to the begining */

    ICHN->nxtndx = ICHN->begndx;

/* Save the current dl counter and mark the channel ready for xfer */

    ICHN->count  = CHN->count;
    ICHN->status = EDES_READY;

    return;

}

/* Revision History
 *
 * $Log: set_indices.c,v $
 * Revision 1.3  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.2  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
