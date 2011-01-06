#pragma ident "$Id: util.c,v 1.3 2007/01/11 20:19:27 dechavez Exp $"
/*======================================================================
 *
 *  Various convenience functions
 *
 *====================================================================*/
#include "dbio.h"

BOOL dbioExecuteQuery(DBIO *db, char *query)
{
static char *fid = "dbioExecuteQuery";

#ifdef HAVE_MYSQL
    if (db->engine != DBIO_MYSQL) {
        errno = EINVAL;
        return FALSE;
    }

    if (mysql_query(&db->mysql, query) != 0) {
        logioMsg(db->lp, LOG_ERR, "%s: mysql_query failed", fid);
        logioMsg(db->lp, LOG_ERR, "%s: query = '%s'", fid, query);
        logioMsg(db->lp, LOG_ERR, "%s: error code %d: %s", fid, mysql_errno(&db->mysql), mysql_error(&db->mysql));
        return FALSE;
    }

    return TRUE;
#else
    return FALSE;
#endif
}

BOOL dbioDeleteTable(DBIO *db, char *name)
{
#define QBUFLEN 1024
char query[QBUFLEN];

    snprintf(query, QBUFLEN, "DROP TABLE IF EXISTS %s", name);
    return dbioExecuteQuery(db, query);
}

/* Revision History
 *
 * $Log: util.c,v $
 * Revision 1.3  2007/01/11 20:19:27  dechavez
 * snprintf instead of sprintf
 *
 * Revision 1.2  2006/04/20 19:18:53  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.1  2006/02/08 23:57:14  dechavez
 * initial release
 *
 */
