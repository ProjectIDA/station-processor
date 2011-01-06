#pragma ident "$Id: utils.c,v 1.5 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  NRTS convenience functions
 *
 *====================================================================*/
#include "isi/dl.h"

/* Find the NRTS stuff for given stream */

NRTS_DL *isidlLocateNrtsDiskloop(ISI_DL_MASTER *master, ISI_STREAM_NAME *target, NRTS_STREAM *result)
{
NRTS_DL *nrts;
int index, i, j;
ISI_STREAM_NAME test;
NRTS_SYS *sys;
NRTS_STA *sta;
NRTS_CHN *chn;

    if (master == NULL || target == NULL || result == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (index = 0; index < master->ndl; index++) {
        if ((nrts = master->dl[index]->nrts) != NULL) {
            sys = nrts->sys;
            for (i=0; i < nrts->sys->nsta; i++) {
                sta = &nrts->sys->sta[i];
                for (j = 0; j < sta->nchn; j++) {
                    chn = &sta->chn[j];
                    isiStaChnToStreamName(sta->name, chn->name, &test);
                    if (isiStreamNameMatch(&test, target)) {
                        result->sys = sys;
                        result->sta = sta;
                        result->chn = chn;
                        sprintf(result->ident, "%s:%s", sta->name, chn->name);
                        return nrts;
                    }
                }
            }
        }
    }

    return NULL;
}

/* Test to see if a particular stream exists is available in any of the supported disk loops */

BOOL isidlStreamExists(ISI_DL_MASTER *master, ISI_STREAM_NAME *target)
{
static NRTS_STREAM unused;

    return isidlLocateNrtsDiskloop(master, target, &unused) != NULL ? TRUE : FALSE;
}

/* Revision History
 *
 * $Log: utils.c,v $
 * Revision 1.5  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.4  2006/09/29 19:43:08  dechavez
 * cleared tabs
 *
 * Revision 1.3  2006/06/21 18:33:11  dechavez
 * allow for ISI systems w/o NRTS disk loops in isiLocateNrtsDiskloop()
 *
 * Revision 1.2  2005/10/19 23:31:14  dechavez
 * fixed uninitialized variable bug in isiLocateNrtsDiskloop()
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
