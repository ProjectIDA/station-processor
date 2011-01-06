#pragma ident "$Id: handle.c,v 1.2 2007/01/11 21:59:21 dechavez Exp $"
/*======================================================================
 * 
 * create and initialize an IDA handle
 *
 *====================================================================*/
#include "ida.h"
#include "isi/db.h"

#define IDA_DEFAULT_MAP_STRING "-"

void idaInitHandle(IDA *ida)
{
    if (ida == NULL) return;
    ida->site = NULL;
    ida->mapname[0] = 0;
    ida->chnlocmap = NULL;
    ida->tqual = NULL;
    ida->rev.value = 0;
    ida->rev.description[0] = 0;
    ida->db = NULL;
    ida->lp = NULL;
    ida->cfs = NULL;
    ida->flags = 0;
}

void idaClearHandle(IDA *ida)
{
    if (ida == NULL) return;
    if (ida->site != NULL) free(ida->site);
    listDestroy(ida->chnlocmap);
    listDestroy(ida->tqual);
    listDestroy(ida->cfs);
    idaInitHandle(ida);
}

IDA *idaDestroyHandle(IDA *ida)
{
    if (ida == NULL) return NULL;
    idaClearHandle(ida);
    free(ida);
    return NULL;
}

IDA *idaCreateHandle(char *site, int rev, char *mapname, DBIO *db, LOGIO *lp, UINT32 flags)
{
IDA *ida;
static char *fid = "idaCreateHandle";

    if (db == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL database not allowed", fid);
        errno = EINVAL;
        return NULL;
    }

    if ((ida = (IDA *) malloc(sizeof(IDA))) == NULL) return NULL;
    idaInitHandle(ida);

    if ((ida->cfs = listCreate()) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return idaDestroyHandle(ida);
    }

    if (site != NULL && (ida->site = strdup(site)) == NULL) return idaDestroyHandle(ida);
    if (!isidbLookupRev(db, rev, &ida->rev)) {
        logioMsg(lp, LOG_ERR, "%s: unsupported packet rev '%d'", fid, rev);
        errno = EINVAL;
        return idaDestroyHandle(ida);
    }
    ida->flags = flags;
    ida->lp = lp;
    ida->db = db;
    if ((ida->tqual = isidbTqualList(db)) == NULL) {
        logioMsg(lp, LOG_ERR, "ERROR: can't load time code quality map, db=%s", db->dbid);
        return idaDestroyHandle(ida);
    }
    if (mapname != NULL && strcmp(mapname, IDA_DEFAULT_MAP_STRING) == 0) mapname = NULL;
    if (!idaSetChnLocMap(ida, mapname, time(NULL))) return idaDestroyHandle(ida);

    return ida;
}

/* Revision History
 *
 * $Log: handle.c,v $
 * Revision 1.2  2007/01/11 21:59:21  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.1  2006/02/08 22:58:02  dechavez
 * initial release
 *
 */
