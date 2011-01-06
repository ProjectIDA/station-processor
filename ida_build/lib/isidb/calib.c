#pragma ident "$Id: calib.c,v 1.3 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup calib and calper from calibration table.
 *
 *====================================================================*/
#include "isi/db.h"

#define UNDEFINED_CALIB 0.0
#define UNDEFINED_CALPER -1.0

static BOOL MysqlLookup(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, double *calib, double *calper)
{
static char *fid = "isidbLookupCalib:MysqlLookup";

#ifdef HAVE_MYSQL
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;

    query[0] = 0;
    strcat(query, "SELECT calib,calper FROM calibration ");
    sprintf(query + strlen(query), "WHERE sta=\"%s\" ", name->sta);
    sprintf(query + strlen(query), "AND chan=\"%s\" ", name->chnloc);
    sprintf(query + strlen(query), "AND time<=SEC_TO_TIME(%lu) ", when);
    sprintf(query + strlen(query), "AND endtime>=SEC_TO_TIME(%lu)", when);

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

/* There should be at most 1 row.  We don't check for duplicates, leaving that
 * to some exernal database integrity checker and just take the first entry.
 */

    if ((row = mysql_fetch_row(result)) != NULL) {
        *calib = atof(row[0]);
        *calper = atof(row[1]);
    }

    mysql_free_result(result);
    return TRUE;

#else
    return FALSE;
#endif
}


BOOL isidbLookupCalib(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, double *calib, double *calper)
{
LOGIO *lp;
static char *fid = "isidbLookupCalib";

    if (db == NULL || name == NULL || calib == NULL || calper == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    *calib = UNDEFINED_CALIB;
    *calib = UNDEFINED_CALPER;
    if (db->engine != DBIO_MYSQL) return TRUE;

    return MysqlLookup(db, name, when, calib, calper);
}

/* Revision History
 *
 * $Log: calib.c,v $
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
