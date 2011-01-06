#pragma ident "$Id: system.c,v 1.11 2007/10/31 16:55:53 dechavez Exp $"
/*======================================================================
 *
 * Generate a list of all ISI systems
 *
 *====================================================================*/
#include "isi/db.h"

static LNKLST *MysqlBuildSystemList(DBIO *db)
{
#ifdef HAVE_MYSQL
LNKLST *list;
MYSQL_RES *result;
MYSQL_ROW row;
ISI_SYSTEM entry;
static char *query = "SELECT sta, sf_private FROM system";
static char *fid = "isidbSystemList:MysqlLookup";

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return NULL;
    }

    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL) {
            strlcpy(entry.sta, row[0], ISI_STALEN+1);
            entry.flags.sf_private = atoi(row[1]);
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

LNKLST *isidbSystemList(DBIO *db)
{
static char *fid = "isidbSystemList";

    if (db == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if (db->engine != DBIO_MYSQL) {
        return &db->ff->systems;
    } else {
        return MysqlBuildSystemList(db);
    }
}

static BOOL FlatfileSystemLookup(DBIO *db, char *name, ISI_SYSTEM *dest)
{

    if (strcmp(db->ff->stamap->sta,name)) {
        strlcpy(dest->sta, name, ISI_STALEN+1);
        dest->flags.sf_private = 0;
        printf("FlatfileSystemLookup name = %s \n",name);
        return TRUE;
    }  else  {
        return FALSE;
    }
}

static BOOL MysqlSystemLookup(DBIO *db, char *name, ISI_SYSTEM *dest)
{
#ifdef HAVE_MYSQL
MYSQL_ROW row;
MYSQL_RES *result;
char query[1024];

    sprintf(query, "SELECT sf_private FROM system WHERE sta=\"%s\"" ,name);
    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);
    strlcpy(dest->sta, name, ISI_STALEN+1);
    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL) {
            dest->flags.sf_private = atoi(row[0]);
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

BOOL isidbLookupSystemByName(DBIO *db, char *name, ISI_SYSTEM *dest)
{
static char *fid = "isidbLookupSystemByName";

    if (db == NULL || name == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }
 
    if (db->engine != DBIO_MYSQL) {
        return FlatfileSystemLookup(db, name, dest);
    } else {
        return MysqlSystemLookup(db, name, dest);
    }
}

BOOL isidbLookupSysFlagsByName(DBIO *db, char *name, ISI_SYS_FLAGS *dest)
{
ISI_SYSTEM sys;
static char *fid = "isidbLookupSysFlagsByName";

    if (db == NULL || name == NULL || dest == NULL) {
        logioMsg(NULL, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (!isidbLookupSystemByName(db, name, &sys)) return FALSE;

    *dest = sys.flags;

    return TRUE;
}

/* Revision History
 *
 * $Log: system.c,v $
 * Revision 1.11  2007/10/31 16:55:53  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.10  2007/06/04 18:59:41  judy
 * fixed pointer problem in isidbLookupSysFlagsByName()
 *
 * Revision 1.9  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.8  2007/01/08 16:00:51  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.7  2007/01/04 21:42:58  judy
 * added isidbLookupSystemByName()
 *
 * Revision 1.6  2006/11/10 06:46:22  judy
 * added isidbLookupSysFlagsByName()
 *
 * Revision 1.5  2006/08/30 18:17:57  judy
 * added support for ISI_SYSTEM
 *
 * Revision 1.4  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.3  2006/02/14 17:05:25  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.2  2006/02/10 23:09:39  dechavez
 * return NULL instead of FALSE (benign error, but confusing)
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
