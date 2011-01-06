#pragma ident "$Id: site.c,v 1.4 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Generate a list of all sites known to this system (these may or may
 * not be "systems", ie have an ISI disk loop assocaited with them).
 *
 *====================================================================*/
#include "isi/db.h"

static LNKLST *MysqlLookup(DBIO *db)
{
#ifdef HAVE_MYSQL
LNKLST *list;
MYSQL_RES *result;
MYSQL_ROW row;
static char *query = "SELECT sta FROM site";
static char *fid = "isidbSiteList:MysqlLookup";

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return FALSE;
    }

    if (mysql_query(&db->mysql, query) != 0) {
        logioMsg(db->lp, LOG_ERR, "%s: mysql_query failed", fid);
        logioMsg(db->lp, LOG_ERR, "%s: query = '%s'", fid, query);
        logioMsg(db->lp, LOG_ERR, "%s: error code %d: %s", fid, mysql_errno(&db->mysql), mysql_error(&db->mysql));
        listDestroy(list);
        return FALSE;
    }
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL && !listAppend(list, row[0], strlen(row[0])+1)) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            mysql_free_result(result);
            listDestroy(list);
            return FALSE;
        }
    }
    mysql_free_result(result);

    if (!listSetArrayView(list)) {
        logioMsg(db->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        listDestroy(list);
        return FALSE;
    }
    return list;
#else
    return NULL;
#endif
}

LNKLST *isidbSiteList(DBIO *db)
{
static char *fid = "isidbSiteList";

    if (db == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return &db->ff->systems;
    } else {
        return MysqlLookup(db);
    }
}

/* Revision History
 *
 * $Log: site.c,v $
 * Revision 1.4  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.3  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.2  2006/02/14 17:05:25  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
