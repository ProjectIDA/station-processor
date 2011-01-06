#pragma ident "$Id: stamap.c,v 1.4 2007/10/31 16:56:39 dechavez Exp $"
/*======================================================================
 *
 * Find the name of the default MK7 channel map for a particular station
 *
 *====================================================================*/
#include "isi/db.h"

static char *FlatFileLookup(DBIO *db, char *sta, UINT32 when, char *dest, int len)
{
char *result;

    if ((result = idaffLookupStamap(db->ff, sta)) != NULL) {
        strlcpy(dest, result, len);
        result = dest;
    } else {
        result = NULL;
    }

    return result;
}

static char *MysqlLookup(DBIO *db, char *sta, UINT32 when, char *dest, int len)
{
#ifdef HAVE_MYSQL
char *retval;
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];
static char *fid = "isidbLookupStamap:MysqlLookup";

    query[0] = 0;
    strcat(query, "SELECT DISTINCT chanmap FROM stamap WHERE ");
    sprintf(query+strlen(query), "sta=\"%s\"", sta);
    sprintf(query + strlen(query), "AND bvt<=SEC_TO_TIME(%lu) ", when);
    sprintf(query + strlen(query), "AND evt>=SEC_TO_TIME(%lu)", when);

    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    if ((row = mysql_fetch_row(result)) != NULL) {
        strlcpy(dest, row[0], len);
        retval = dest;
    } else {
        retval = NULL;
    }

    mysql_free_result(result);

    return retval;
#else
    return FALSE;
#endif
}

char *isidbLookupStamap(DBIO *db, char *sta, UINT32 when, char *dest, int len)
{
LOGIO *lp;
static char *fid = "isidbLookupStamap";

    if (db == NULL || sta == NULL || dest == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if (when == 0) when = time(NULL);

    if (db->engine != DBIO_MYSQL) {
        return FlatFileLookup(db, sta, when, dest, len);
    } else {
        return MysqlLookup(db, sta, when, dest, len);
    }
}

/* Revision History
 *
 * $Log: stamap.c,v $
 * Revision 1.4  2007/10/31 16:56:39  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.3  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
