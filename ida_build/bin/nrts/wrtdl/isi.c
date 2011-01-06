#pragma ident "$Id: isi.c,v 1.5 2007/01/11 18:07:17 dechavez Exp $"
/*======================================================================
 *
 *  IDA System Interface support
 *
 *====================================================================*/
#include "wrtdl.h"

VOID LogIsiMessage(char *message)
{
    util_log(1, message);
}

REAL64 ConvertToIsiBegTime(REAL64 beg)
{
    if (beg > 0.0) {
        return beg;
    } else if (beg == NRTS_OLDEST) {
        return ISI_OLDEST;
    } else if (beg == NRTS_YNGEST) {
        return ISI_NEWEST;
    } else {
        return ISI_UNDEFINED_TIMESTAMP;
    }
}

REAL64 ConvertToIsiEndTime(REAL64 end, BOOL keepup)
{
    if (end > 0.0) {
        return end;
    } else if (end == NRTS_OLDEST) {
        return ISI_OLDEST;
    } else if (end == NRTS_YNGEST) {
        return keepup ? ISI_KEEPUP : ISI_NEWEST;
    } else {
        return ISI_UNDEFINED_TIMESTAMP;
    }
}

ISI_DATA_REQUEST *BuildIsiDataRequest(NRTS_SYS *sys, REAL64 beg, REAL64 end, ISI_PARAM *param)
{
int i;
ISI_DATA_REQUEST *dreq;
ISI_TWIND_REQUEST *twind;
NRTS_STA *sta;
NRTS_CHN *chn;

    sta = &sys->sta[0];
    chn = sta->chn;

    if ((dreq = (ISI_DATA_REQUEST *) malloc(sizeof(ISI_DATA_REQUEST))) == NULL) return NULL;
    isiInitDataRequest(dreq);
    isiSetDatreqPolicy(dreq, ISI_RECONNECT_POLICY_MIN_GAP);
    isiSetDatreqFormat(dreq, ISI_FORMAT_NATIVE);
    isiSetDatreqCompress(dreq, ISI_COMP_IDA);

    dreq->nreq = sta->nchn;
    dreq->req.twind = (ISI_TWIND_REQUEST *) malloc(sizeof(ISI_TWIND_REQUEST) * dreq->nreq);
    if (dreq->req.twind == 0) {
        free(dreq);
        return NULL;
    }

    for (i = 0; i < (int) dreq->nreq; i++) {
        chn = &sta->chn[i];
        twind = &dreq->req.twind[i];
        isiStaChnLocToStreamName(sta->name, chn->name, NULL, &twind->name);
        if (chn->beg < 0 || chn->end < beg) {
            twind->beg = beg;
        } else {
            twind->beg = chn->end + (REAL64) chn->sint;
        }
        twind->end = end;
    }

    return dreq;
}

/* Revision History
 *
 * $Log: isi.c,v $
 * Revision 1.5  2007/01/11 18:07:17  dechavez
 * renamed "stream" stuff to more accurate "twind" (time window)
 *
 * Revision 1.4  2005/06/24 21:46:40  dechavez
 * accomodate new design of ISI_DATA_REQUEST structure
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/12/10 06:20:51  dechavez
 * fixed error setting request start time when explicity given on command line
 *
 * Revision 1.1  2003/11/25 20:38:56  dechavez
 * created
 *
 */
