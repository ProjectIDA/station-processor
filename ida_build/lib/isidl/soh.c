#pragma ident "$Id: soh.c,v 1.4 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  Snapshot of the current (NRTS) disk loop streams
 *
 *====================================================================*/
#include "isi/dl.h"

static BOOL AppendSysStreamSoh(DBIO *db, LNKLST *list, NRTS_SYS *sys)
{
int i, j;
NRTS_STA *sta;
NRTS_CHN *chn;
ISI_STREAM_SOH soh;
static char *fid = "isidlBuildStreamSohList:AppendSysStreamSoh";

    for (i = 0; i < sys->nsta; i++) {
        sta = &sys->sta[i];
        for (j = 0; j < sta->nchn; j++) {
            chn = &sta->chn[j];
            isiStaChnToStreamName(sta->name, chn->name, &soh.name);
            soh.tofs.value = chn->beg;
            soh.tofs.status = ISI_TSTAMP_STATUS_UNKNOWN;
            soh.tols.value = chn->end;
            soh.tols.status = ISI_TSTAMP_STATUS_UNKNOWN;
            soh.tslw = (REAL64) (time(NULL) - chn->tread);
            soh.nseg = (UINT32) chn->nseg;
            soh.nrec = (UINT32) chn->nread;
            if (!listAppend(list, (VOID *) &soh, (UINT32) sizeof(ISI_STREAM_SOH))) return FALSE;
        }
    }

    return TRUE;
}

BOOL isidlBuildStreamSohList(ISI_DL_MASTER *master, LNKLST *list)
{
int i;
static char *fid = "isidlBuildStreamSohList";

    if (master == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    
    for (i = 0; i < master->ndl; i++) {
        if (master->dl[i]->nrts != NULL) {
            if (!AppendSysStreamSoh(master->glob->db, list, master->dl[i]->nrts->sys)) {
                logioMsg(master->lp, LOG_ERR, "%s: AppendSysStreamSoh: %s", fid, strerror(errno));
                return FALSE;
            }
        }
    }

    if (!listSetArrayView(list)) {
        logioMsg(master->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        return FALSE;
    }
    return TRUE;
}

/* Revision History
 *
 * $Log: soh.c,v $
 * Revision 1.4  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.3  2006/06/02 20:47:08  dechavez
 * allow for missing NRTS disk loop
 *
 * Revision 1.2  2006/02/14 17:05:26  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
