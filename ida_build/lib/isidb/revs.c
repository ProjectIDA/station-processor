#pragma ident "$Id: revs.c,v 1.5 2007/10/31 17:00:30 dechavez Exp $"
/*======================================================================
 *
 * Lookup format revision from revs table.  If the user provides a
 * IDA_REV structure the table contents are loaded, otherwise this
 * function just returns TRUE or FALSE depending on if there is an
 * entry for the given revsion value.
 *
 *====================================================================*/
#include "isi/db.h"

static BOOL FlatFileLookup(DBIO *db, int value, IDA_REV *buf)
{
int i;

    for (i = 0; i < db->ff->nrev; i++) {
        if (db->ff->rev[i].code == value) {
            if (buf != NULL) {
                buf->value = value;
                strlcpy(buf->description, db->ff->rev[i].text, IDA_REV_DESC_LEN+1);
            }
            return TRUE;
        }
    }
            
    return FALSE;
}

static BOOL MysqlLookup(DBIO *db, int value, IDA_REV *buf)
{
#ifdef HAVE_MYSQL
BOOL retval;
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;
static char *fid = "isidbLookupRev:MysqlLookup";

/* Look first for a station specific entry */

    sprintf(query, "SELECT descrip FROM revs WHERE code=%d", value);

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

/* There should be at most 1 row.  We don't check for duplicates, leaving that
 * to some exernal database integrity checker and just take the first entry.
 */

    if ((row = mysql_fetch_row(result)) != NULL) {
        if (buf != NULL) {
            buf->value = value;
            strlcpy(buf->description, row[0], IDA_REV_DESC_LEN+1);
        }
        retval = TRUE;
    } else {
        retval = FALSE;
    }

    mysql_free_result(result);
    return retval;
#else
    return FALSE;
#endif
}

BOOL isidbLookupRev(DBIO *db, int value, IDA_REV *result)
{
LOGIO *lp;
static char *fid = "isidbLookupRev";

    if (db == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return FlatFileLookup(db, value, result);
    } else {
        return MysqlLookup(db, value, result);
    }
}

/* Return the whole table in a linked list */

static LNKLST *FlatFileRevList(DBIO *db, LNKLST *list)
{
int i;
IDA_REV rev;
static char *fid = "isidbRevList:FlatFileRevList";

    for (i = 0; i < db->ff->nrev; i++) {
        rev.value = db->ff->rev[i].code;
        strlcpy(rev.description, db->ff->rev[i].text, IDA_REV_DESC_LEN+1);
        if (!listAppend(list, &rev, sizeof(IDA_REV))) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            listDestroy(list);
            return NULL;
        }
    }

    return list;
}

static LNKLST *MysqlRevList(DBIO *db, LNKLST *list)
{
#ifdef HAVE_MYSQL
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];
IDA_REV rev;
static char *fid = "isidbRevList:MysqlRevList";

    sprintf(query, "SELECT DISTINCT code,descrip FROM revs");
    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        rev.value = atoi(row[0]);
        strlcpy(rev.description, row[1], IDA_REV_DESC_LEN+1);
        if (!listAppend(list, &rev, sizeof(IDA_REV))) {
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

LNKLST *isidbRevList(DBIO *db)
{
LOGIO *lp;
LNKLST *list, *result;
static char *fid = "isidbRevList";

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
        result = FlatFileRevList(db, list);
    } else {
        result = MysqlRevList(db, list);
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
 * $Log: revs.c,v $
 * Revision 1.5  2007/10/31 17:00:30  dechavez
 * replaced string memcpy with strlcpy
 *
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
