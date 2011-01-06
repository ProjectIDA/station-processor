#pragma ident "$Id: cnf.c,v 1.6 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  Snapshot of the current (NRTS) disk loop streams
 *
 *====================================================================*/
#include "isi/dl.h"
#include "isi/db.h"

/* Get a snapshot of the current configuration */

static BOOL AppendSysStreamCnf(DBIO *db, LNKLST *list, NRTS_SYS *sys)
{
int i, j;
UINT32 when = 0; // date based selection not implemented
NRTS_STA *sta;
NRTS_CHN *chn;
ISI_STREAM_CNF cnf;
static char *fid = "isiBuildStreamCnfList:AppendSysStreamCnf";

    for (i = 0; i < sys->nsta; i++) {
        sta = &sys->sta[i];
        isidbLookupCoords(db, sta->name, when, &cnf.coords);
        for (j = 0; j < sta->nchn; j++) {
            chn = &sta->chn[j];
            isiStaChnToStreamName(sta->name, chn->name, &cnf.name);
            isidbLookupSrate(db, &cnf.name, &cnf.srate);
            isidbLookupSitechan(db, &cnf.name, (UINT32) time(NULL), &cnf.inst);
            if (!listAppend(list, (VOID *) &cnf, (UINT32) sizeof(ISI_STREAM_CNF))) return FALSE;
        }
    }

    return TRUE;
}

BOOL isidlBuildStreamCnfList(ISI_DL_MASTER *master, LNKLST *list)
{
int i;
static char *fid = "isidlBuildStreamCnfList";

    if (master == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    
    for (i = 0; i < master->ndl; i++) {
        if (master->dl[i]->nrts != 0) {
            if (!AppendSysStreamCnf(master->glob->db, list, master->dl[i]->nrts->sys)) {
                logioMsg(master->lp, LOG_ERR, "%s: AppendSysStreamCnf: %s", fid, strerror(errno));
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
 * $Log: cnf.c,v $
 * Revision 1.6  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.5  2006/06/26 23:07:58  dechavez
 * use dummy "when" for isiLookupCoords
 *
 * Revision 1.4  2006/06/02 20:47:18  dechavez
 * allow for missing NRTS disk loop
 *
 * Revision 1.3  2006/02/14 17:05:26  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.2  2006/02/09 00:32:41  dechavez
 * isiLookupSitechan() instead of isiLookupInst()
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
