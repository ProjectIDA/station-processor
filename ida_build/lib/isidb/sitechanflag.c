#pragma ident "$Id: sitechanflag.c,v 1.4 2007/10/31 16:59:32 dechavez Exp $"
/*======================================================================
 *
 * Generate a list of all ISI site channel flags
 *
 *====================================================================*/
#include "isi/db.h"

static LNKLST *MysqlBuildSitechanflagList(DBIO *db)
{
#ifdef HAVE_MYSQL
LNKLST *list;
MYSQL_RES *result;
MYSQL_ROW row;
ISI_SITECHANFLAG entry;
static char *query = "SELECT sta, chan, status FROM sitechanflag";
static char *fid = "isidbSitechanflagList:MysqlLookup";

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return NULL;
    }

    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL) {
            strlcpy(entry.sta, row[0], ISI_STALEN+1);
            strlcpy(entry.chan, row[1], ISI_CNAMLEN+1);
            if (!listAppend(list, &entry, sizeof(ISI_SYSTEM))) {
                logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
                mysql_free_result(result);
                listDestroy(list);
                return NULL;
            }
        }
    }
    mysql_free_result(result);

    if (!listSetArrayView(list)) {
        logioMsg(db->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        listDestroy(list);
        return NULL;
    }
    return list;
#else
    return NULL;
#endif
}

LNKLST *isidbSitechanflagList(DBIO *db)
{
static char *fid = "isidbSitechanflagList";

    if (db == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if (db->engine != DBIO_MYSQL) {
        fprintf(stderr, "ERROR: isidbSitechanflagList FAILED - no database specifiec \n");
    } else {
        return MysqlBuildSitechanflagList(db);
    }
}


static BOOL MysqlSitechanflagLookup(DBIO *db, char *name, char *chan, ISI_SITECHANFLAG *dest)
{
#ifdef HAVE_MYSQL
MYSQL_ROW row;
MYSQL_RES *result;
char query[1024];

    sprintf(query, "SELECT status FROM sitechanflag WHERE sta=\"%s\" and chan = \"%s\" ", name, chan);
    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);
    strlcpy(dest->sta, name, ISI_STALEN+1);
    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL) {
            dest->status = atoi(row[0]);
            mysql_free_result(result);
            return TRUE;
        }
    }
    mysql_free_result(result);

    return FALSE;
#else 
    return TRUE;
#endif
}

BOOL isidbLookupSitechanflag(DBIO *db, char *name, char *chan, ISI_SITECHANFLAG *dest)
{
static char *fid = "isidbLookuopSystemByName";
    if (db == NULL || name == NULL || dest == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }
    if (db->engine != DBIO_MYSQL) {
        fprintf(stderr, "ERROR: isidbLookupSitechanflag FAILED - no database specifiec \n");
    } else {
        return MysqlSitechanflagLookup(db, name, chan, dest);
    }
}

BOOL isidbChangeChanToUp(DBIO *db, char *name, char *chan)
/*   change status field of sitechanflag table to 0 to indicate channel is up */
{
#ifdef HAVE_MYSQL
MYSQL_ROW row;
MYSQL_RES *result;
char query[1024];

    sprintf(query, "update sitechanflag set status=\"0\" WHERE sta=\"%s\" and chan = \"%s\" ", name, chan);
    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);
    mysql_free_result(result);
    return TRUE;
#else 
    return FALSE;
#endif

}

/* Revision History
 *
 * $Log: sitechanflag.c,v $
 * Revision 1.4  2007/10/31 16:59:32  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.3  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2007/01/08 16:00:51  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.1  2007/01/04 21:38:23  judy
 * initial release
 *
 */
