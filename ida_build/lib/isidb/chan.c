#pragma ident "$Id: chan.c,v 1.4 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Generate a list of all channels for a particular station, based on
 * the calibration table.
 *
 *====================================================================*/
#include "isi/db.h"

#ifdef HAVE_MYSQL

LNKLST *isidbCalibrationChanList(DBIO *db, char *sta, UINT32 when)
{
static char *fid = "isidbChanList";
LNKLST *list;
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return NULL;
    }

    query[0] = 0;
    strcat(query, "SELECT DISTINCT chan FROM calibration WHERE ");
    sprintf(query+strlen(query), "sta=\"%s\" AND ", sta);
    sprintf(query+strlen(query), "time<=SEC_TO_TIME(%lu) AND ", when);
    sprintf(query+strlen(query), "endtime>=SEC_TO_TIME(%lu)", when);

    if (!dbioExecuteQuery(db, query)) return NULL;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL && !listAppend(list, row[0], strlen(row[0])+1)) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            mysql_free_result(result);
            listDestroy(list);
            return NULL;
        }
    }
    mysql_free_result(result);

    if (!listSetArrayView(list)) {
        logioMsg(db->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        listDestroy(list);
        return NULL;
    }

    return list;

}

#else

LNKLST *isidbCalibrationChanList(DBIO *db, char *sta, UINT32 when)
{
    errno = ENOTSUP;
    return NULL;
}

#endif

/* Revision History
 *
 * $Log: chan.c,v $
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
