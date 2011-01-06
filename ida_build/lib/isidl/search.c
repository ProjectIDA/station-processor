#pragma ident "$Id: search.c,v 1.3 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  Search diskloop for the index of the record with sequence number
 *  closest to some specific (or relative) value.  It is up to the
 *  calling application to get a static snapshot of the disk loop
 *  and pass that to the function.
 *
 *====================================================================*/
#include "isi/dl.h"

static BOOL GetSpecificSeqno(char *fid, ISI_DL *dl, UINT32 index, ISI_SEQNO *result)
{
    *result = isidlPacketSeqno(dl, index);
    if (isiIsUndefinedSeqno(result)) {
        logioMsg(dl->lp, LOG_ERR, "%s: unexpected undefined seqno, dl='%s', index=%lu", fid, dl->sys->site, index);
        return FALSE;
    }
    return TRUE;
}

static UINT32 BinarySearch(ISI_DL *dl, ISI_SEQNO *target, UINT32 left, UINT32 rite)
{
UINT32 index;
ISI_SEQNO value;
static char *fid = "isidlSearchDiskLoopForSeqno:BinarySearch";

    while (rite - left > 1) {
        index = left + ((rite - left)/2);
        if (!GetSpecificSeqno(fid, dl, index, &value)) return ISI_CORRUPT_INDEX;
        if (isiSeqnoEQ(target, &value)) return index;
        if (isiSeqnoGT(target, &value)) {
            left = index;
        } else {
            rite = index;
        }
    }

    return rite;
}

UINT32 isidlSearchDiskLoopForSeqno(ISI_DL *dl, ISI_SEQNO *target, UINT32 ldef, UINT32 rdef)
{
char tbuf[256];
ISI_SEQNO oldest, yngest, zero, eof;
static char *fid = "isidlSearchDiskLoopForSeqno";

    if (dl == NULL || target == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: NULL INPUTS!", fid);
        return ISI_UNDEFINED_INDEX;
    }

/* Nothing to do if we don't have anything in the disk loop */

    if (!isiIsValidIndex(dl->sys->index.oldest)) return dl->sys->index.oldest;

/* Check for symbolic sequence numbers */

    if (isiIsOldestSeqno(target)) return dl->sys->index.oldest;
    if (isiIsNewestSeqno(target)) return dl->sys->index.yngest;

/* Don't support relative sequence numbers */

    if (!isiIsAbsoluteSeqno(target)) return ISI_UNDEFINED_INDEX;

/* Save the sequence numbers at the ends */

    if (!GetSpecificSeqno(fid, dl, dl->sys->index.oldest, &oldest)) return ISI_CORRUPT_INDEX;
    if (isiSeqnoEQ(target, &oldest)) return dl->sys->index.oldest;

    if (!GetSpecificSeqno(fid, dl, dl->sys->index.yngest, &yngest)) return ISI_CORRUPT_INDEX;
    if (isiSeqnoEQ(target, &yngest)) return dl->sys->index.yngest;

/* If requesting something more recent that our most recent packet then no need to search */

    if (isiSeqnoGT(target, &yngest)) return ISI_UNDEFINED_INDEX;

/* Likewise, if we are requesting something older than our oldest */

    if (isiSeqnoLT(target, &oldest)) return dl->sys->index.oldest;

/* Guess we have to search. First case is no wraparound at end of disk buffer */

    if (dl->sys->index.oldest <= dl->sys->index.yngest) {
        if (isiSeqnoLT(target, &oldest)) {
            return ldef;
        } else if (isiSeqnoGT(target, &yngest)) {
            return rdef;
        } else {
            return BinarySearch(dl, target, dl->sys->index.oldest, dl->sys->index.yngest);
        }
    }

/* In the second case data wrap around end of disk buffer, find which half it is in */

    if (!GetSpecificSeqno(fid, dl, 0, &zero)) return ISI_CORRUPT_INDEX;
    if (isiSeqnoEQ(target, &zero)) {
        return 0;
    } else if (isiSeqnoGT(target, &zero) && isiSeqnoLE(target, &yngest)) {
        return BinarySearch(dl, target, 0, dl->sys->index.yngest);
    }

    if (!GetSpecificSeqno(fid, dl, dl->sys->numpkt - 1, &eof)) return ISI_CORRUPT_INDEX;
    if (isiSeqnoEQ(target, &eof)) {
        return dl->sys->numpkt - 1;
    } else if (isiSeqnoGT(target, &oldest) && isiSeqnoLE(target, &eof)) {
        return BinarySearch(dl, target, dl->sys->index.oldest, dl->sys->numpkt - 1);
    }

/* Got this far only if disk loop is hosed or above logic is faulty */

    logioMsg(dl->lp, LOG_ERR, "%s: unexpected fall through while searching '%s' disk loop!", fid, dl->sys->site);
    logioMsg(dl->lp, LOG_ERR, "%s: target = %s", fid, isiSeqnoString(target, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s: oldest = %s", fid, isiSeqnoString(&oldest, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s: yngest = %s", fid, isiSeqnoString(&yngest, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s:   zero = %s", fid, isiSeqnoString(&zero, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s:    eof = %s", fid, isiSeqnoString(&eof, tbuf));

    return ISI_CORRUPT_INDEX;
}

/* Revision History
 *
 * $Log: search.c,v $
 * Revision 1.3  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2005/09/30 21:19:18  dechavez
 * deal with requests for packets outside the bounds of the disk loop
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
