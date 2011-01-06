#pragma ident "$Id: close.c,v 1.3 2006/02/08 23:56:16 dechavez Exp $"
/*======================================================================
 *
 *  Close connection to database and free resources
 *
 *====================================================================*/
#include "dbio.h"

DBIO *dbioClose(DBIO *db)
{
    if (db == NULL) return NULL;
#ifdef HAVE_MYSQL
    if (db->engine == DBIO_MYSQL) {
        mysql_close(&db->mysql);
        if (    db->host != NULL) free(db->host);
        if (    db->user != NULL) free(db->user);
        if (  db->passwd != NULL) free(db->passwd);
        if (  db->dbname != NULL) free(db->dbname);
    }
#endif /* HAVE_MYSQL */

    if (db->ff != NULL) idaffDestroy(db->ff);

    if (db->dbid != NULL) free(db->dbid);
    free(db);

    return NULL;
}

/* Revision History
 *
 * $Log: close.c,v $
 * Revision 1.3  2006/02/08 23:56:16  dechavez
 * idaff support
 *
 * Revision 1.2  2005/07/26 00:34:00  dechavez
 * support improved handle
 *
 * Revision 1.1  2004/04/23 00:43:12  dechavez
 * initial release
 *
 */
