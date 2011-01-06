#pragma ident "$Id: coords.c,v 1.4 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup ISI_COORDS given station name and optional time stamp (the time
 * stamp is just a place holder, we have any code here that refers to it
 * since none of the stations in the database have moved (yet).
 *
 *====================================================================*/
#include "isi/db.h"

static ISI_COORDS UndefinedCoords = { (REAL32) -12.3456, (REAL32) -12.3456, (REAL32) -12.3456, (REAL32) -12.3456 };

static BOOL MysqlLookup(DBIO *db, char *site, UINT32 unused, ISI_COORDS *coords)
{
#ifdef HAVE_MYSQL
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;
static char *fid = "isidbLookupCoords:MysqlLookup";

    if (strlen(site) > ISI_STALEN) {
        logioMsg(db->lp, LOG_ERR, "%s: illegal site name (strlen > %d)", fid, ISI_STALEN);
        errno = EINVAL;
        return FALSE;
    }

    query[0] = 0;
    strcat(query, "SELECT lat,lon,elev FROM site ");
    sprintf(query+strlen(query), "WHERE sta=\"%s\"", site);

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

/* There should be at most 1 row.  We don't check for duplicates, leaving that
 * to some exernal database integrity checker and just take the first entry.
 */

    if ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] != NULL) coords->lat  = (REAL32) atof(row[0]);
        if (row[1] != NULL) coords->lon  = (REAL32) atof(row[1]);
        if (row[2] != NULL) coords->elev = (REAL32) atof(row[2]);
        coords->depth = 0.0;
    }

    mysql_free_result(result);
    return TRUE;
#else
    return FALSE;
#endif
}

BOOL isidbLookupCoords(DBIO *db, char *site, UINT32 when, ISI_COORDS *coords)
{
LOGIO *lp;
static char *fid = "isidbLookupCoords";

    if (db == NULL || site == NULL || coords == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    *coords = UndefinedCoords;

    if (db->engine != DBIO_MYSQL) return TRUE;
    return MysqlLookup(db, site, when, coords);
}

/* Revision History
 *
 * $Log: coords.c,v $
 * Revision 1.4  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.3  2006/06/26 22:34:22  dechavez
 * cast UndefinedCoords members to REAL32
 *
 * Revision 1.2  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
