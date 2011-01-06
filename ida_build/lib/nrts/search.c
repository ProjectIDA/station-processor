#pragma ident "$Id: search.c,v 1.4 2007/06/01 19:18:47 dechavez Exp $"
/*======================================================================
 *
 *  Search diskloop for the index of the record closest to a given time
 *
 *====================================================================*/
#include "nrts/dl.h"

typedef struct {
    REAL64 beg;
    REAL64 end;
} TIMESPAN;

/* Determine the begin and end times of a data packet */

static BOOL GetTimespan(NRTS_DL *dl, NRTS_STREAM *stream, INT32 index, UINT8 *buf, UINT32 buflen, TIMESPAN *span)
{
int len;
NRTS_PKT pkt;
static char *fid = "GetTimespan";

    pkt.indx = -1;

    if ((len = nrtsRead(dl, stream, index, buf, buflen, &pkt, NULL)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: nrtsRead: error code %d: %s: %s", fid, len, stream->ident, strerror(errno));
        return FALSE;
    }

    span->beg = pkt.isi.hdr.tofs.value;
    span->end = pkt.isi.hdr.tols.value;

    return TRUE;
}

static INT32 BinarySearch(NRTS_DL *dl, NRTS_STREAM *stream, REAL64 target, INT32 left, INT32 rite, UINT8 *buf, UINT32 buflen)
{
INT32 test;
TIMESPAN span;
static char *fid = "BinarySearch";

    if (!GetTimespan(dl, stream, left, buf, buflen, &span)) return -10;
    if (target >= span.beg && target <= span.end) return left;

    if (!GetTimespan(dl, stream, rite, buf, buflen, &span)) return -20;
    if (target >= span.beg && target <= span.end) return rite;

    while (rite - left > 1) {
        test = left + ((rite - left)/2);
        if (!GetTimespan(dl, stream, test, buf, buflen, &span)) return -30;
        if (target >= span.beg && target <= span.end) return test;
        if (target > span.end) {
            left = test;
        } else {
            rite = test;
        }
    }

    return rite;
}

INT32 nrtsSearchDiskloop(NRTS_DL *dl, NRTS_STREAM *stream, REAL64 target, INT32 ldef, INT32 rdef, UINT8 *buf, UINT32 buflen)
{
TIMESPAN span, disk;
INT32 oldest, yngest;
struct nrts_chn *chn;
char tbuf[256];
static char *fid = "SearchDiskloop";

    if (dl == NULL || stream == NULL || buf == NULL) {
        if (dl != NULL) logioMsg(dl->lp, LOG_ERR, "%s: NULL input not allowed!", fid);
        return -1;
    }
    if (ldef < 0 && rdef < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ldef=%d rdef=%d", fid, ldef, rdef);
        return -1;
    }

    chn = stream->chn;

/* First, insure the desired time falls inside the disk buffer */

    oldest = chn->hdr.oldest;
    yngest = chn->hdr.yngest;

    if (!GetTimespan(dl, stream, oldest, buf, buflen, &span)) return -2;
    disk.beg = span.beg;
    if (target == disk.beg) return oldest;

    if (!GetTimespan(dl, stream, yngest, buf, buflen, &span)) return -3;
    disk.end = span.end;
    if (target == disk.end) return yngest;

    if (target < disk.beg) return ldef;
    if (target > disk.end) return rdef;

/* It must be available inside the disk loop, so search for it */
/* In the first case no wraparound at end of disk buffer */

    if (oldest < yngest) return BinarySearch(dl, stream, target, oldest, yngest, buf, buflen);

/* In the second case data wrap around end of disk buffer */

    if (!GetTimespan(dl, stream, 0, buf, buflen, &span)) return -4;
    span.beg -= chn->sint;  /* will allow for one sample slop */

    if (target > span.beg && target <= disk.end) return BinarySearch(dl, stream, target, 0, yngest, buf, buflen);

    if (!GetTimespan(dl, stream, chn->hdr.nrec - 1, buf, buflen, &span)) return -5;
    if (target > disk.beg && target <= span.end) return BinarySearch(dl, stream, target, oldest, chn->hdr.nrec - 1, buf, buflen);

    logioMsg(dl->lp, LOG_ERR, "%s: %s disk loop is corrupt!", fid, stream->ident);
    logioMsg(dl->lp, LOG_ERR, "%s: %s target = %s", fid, stream->ident, utilDttostr(target, 0, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s: %s dl beg = %s", fid, stream->ident, utilDttostr(disk.beg, 0, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s: %s dl end = %s", fid, stream->ident, utilDttostr(disk.end, 0, tbuf));
    logioMsg(dl->lp, LOG_ERR, "%s: %s dl last  = %s", fid, stream->ident, utilDttostr(span.end, 0, tbuf));

    return -5;
}

/* Revision History
 *
 * $Log: search.c,v $
 * Revision 1.4  2007/06/01 19:18:47  dechavez
 * added new (NULL) chn args to nrtsRead
 *
 * Revision 1.3  2006/07/19 22:18:47  dechavez
 * additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes
 *
 * Revision 1.2  2005/07/26 00:07:48  dechavez
 * removed calls to obsolete nrtsConvert()
 *
 */
