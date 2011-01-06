#pragma ident "$Id: tqual.c,v 1.4 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup time quality mapping from tqual table.
 *
 *====================================================================*/
#include "isi/db.h"

static BOOL MysqlLookup(DBIO *db, char factory, int *internal)
{
#ifdef HAVE_MYSQL
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;
static char *fid = "isidbLookupTqual:MysqlLookup";

/* Look first for a station specific entry */

    sprintf(query, "SELECT internal FROM tqual WHERE factory=\"%c\" ", factory);

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

/* There should be at most 1 row.  We don't check for duplicates, leaving that
 * to some exernal database integrity checker and just take the first entry.
 */

    if ((row = mysql_fetch_row(result)) != NULL) {
        *internal = atoi(row[0]);
    } else {
        *internal = IDA_UNDEFINED_TQUAL;
    }

    mysql_free_result(result);
    return TRUE;
#else
    return FALSE;
#endif
}

BOOL isidbLookupTqual(DBIO *db, char factory, int *internal)
{
LOGIO *lp;
static char *fid = "isidbLookupTqual";

    if (db == NULL || internal == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return idaffLookupTqual(db->ff, factory, internal);
    } else {
        return MysqlLookup(db, factory, internal);
    }
}

/* Return the whole table in a linked list */

static LNKLST *FlatFileTqualList(DBIO *db, LNKLST *list)
{
int i;
IDA_TQUAL tqual;
static char *fid = "isidbTqualList:FlatFileTqualList";

    for (i = 0; i < db->ff->nqual; i++) {
        tqual.factory = db->ff->tqual[i].factory;
        tqual.internal = db->ff->tqual[i].internal;
        if (!listAppend(list, &tqual, sizeof(IDA_TQUAL))) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}

static LNKLST *MysqlTqualList(DBIO *db, LNKLST *list)
{
#ifdef HAVE_MYSQL
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];
IDA_TQUAL tqual;
static char *fid = "isidbTqualList:MysqlTqualList";

    sprintf(query, "SELECT DISTINCT factory,internal FROM tqual");
    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        tqual.factory = row[0][0];
        tqual.internal = atoi(row[1]);
        if (!listAppend(list, &tqual, sizeof(IDA_TQUAL))) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            mysql_free_result(result);
            listDestroy(list);
            return NULL;
        }
    }
    mysql_free_result(result);

    return list;
#else
    return NULL;
#endif
}

LNKLST *isidbTqualList(DBIO *db)
{
LOGIO *lp;
LNKLST *list, *result;
static char *fid = "isidbTqualList";

    if (db == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return NULL;
    }

    if (db->engine != DBIO_MYSQL) {
        result = FlatFileTqualList(db, list);
    } else {
        result = MysqlTqualList(db, list);
    }

    if (result != list) {
        listDestroy(list);
        return NULL;
    }

    if (!listSetArrayView(list)) {
        logioMsg(db->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        listDestroy(list);
        return NULL;
    }

    return list;
}

/* Revision History
 *
 * $Log: tqual.c,v $
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
