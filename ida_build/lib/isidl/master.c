#pragma ident "$Id: master.c,v 1.5 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  Open a set of disk loops
 *
 *====================================================================*/
#include "isi/dl.h"
#include "isi/db.h"

static ISI_DL_MASTER *FailedOpen(ISI_DL *dl, ISI_DL_MASTER *master)
{
UINT32 i;

    if (dl != NULL) isidlCloseDiskLoop(dl);
    if (master != NULL) {
        if (listSetArrayView(&master->list)) {
            for (i = 0; i < master->list.count; i++) isidlCloseDiskLoop((ISI_DL *) master->list.array[i]);
        }
        listDestroy(&master->list);
        free(master);
    }

    return NULL;
}

ISI_DL_MASTER *isidlOpenDiskLoopSet(LNKLST *sitelist, ISI_GLOB *glob, LOGIO *lp, int perm)
{
int i;
char *site;
ISI_DL *dl;
ISI_DL_MASTER *master = NULL;
static char *fid = "isidlOpenDiskLoopSet";

    if (glob == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if (sitelist == NULL) sitelist = isidbSystemList(glob->db);

    if ((master = (ISI_DL_MASTER *) malloc(sizeof(ISI_DL_MASTER))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }

    listInit(&master->list);
    master->glob = glob;
    master->lp   = lp;
    master->dl   = NULL;
    master->ndl  = 0;

    for (i=0; i < (int) sitelist->count; i++) {
        site = (char *) sitelist->array[i];
        if ((dl = isidlOpenDiskLoop(master->glob, site, lp, perm)) == NULL) {
            logioMsg(lp, LOG_ERR, "%s: WARNING: failed to open '%s` disk loop", fid, site);
        } else if (!listAppend(&master->list, dl, sizeof(ISI_DL))) {
            logioMsg(lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            return FailedOpen(dl, master);
        } else {
            free(dl);
            dl = NULL;
        }
    }

    if (!listSetArrayView(&master->list)) {
        logioMsg(lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        return FailedOpen(NULL, master);
    }

    master->dl = (ISI_DL **) master->list.array;
    master->ndl = master->list.count;

    return master;
}

/* Revision History
 *
 * $Log: master.c,v $
 * Revision 1.5  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.4  2006/06/26 23:08:07  dechavez
 * fixed signed/unsigned compares, removed unreferenced local variables
 *
 * Revision 1.3  2006/02/14 17:05:26  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.2  2006/02/09 00:23:54  dechavez
 * warn instead of fail when unable to open a master list site
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
