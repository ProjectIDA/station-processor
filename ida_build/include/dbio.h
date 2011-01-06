#pragma ident "$Id: dbio.h,v 1.3 2006/02/08 23:52:56 dechavez Exp $"
/*======================================================================
 *
 *  MySQL database frontend
 *
 *====================================================================*/
#ifndef dbio_h_included
#define dbio_h_included

#include "platform.h"
#include "logio.h"
#include "ffdb.h"

#define DBIO_UNDEFINED 1
#define DBIO_FLATFILE  2
#define DBIO_MYSQL     3
#define DBIO_ENV_STRING "DBIO_DATABASE"

#ifdef HAVE_MYSQL
#include "mysql.h"
#endif /* HAVE_MYSQL */

typedef struct {
    int engine; /* DBIO_MYSQL or DBIO_FLATFILE */
    IDAFF *ff; /* used for flat file lookups */
    char *dbid; /* string used to specify database source */
    LOGIO *lp;  /* logging facility handle */
#ifdef HAVE_MYSQL
    MYSQL mysql;
    char *host;
    char *user;
    char *passwd;
    char *dbname;
    unsigned int port;
#endif /* HAVE_MYSQL */
} DBIO;

/* close.c */
DBIO *dbioClose(DBIO *dbio);

/* open.c */
DBIO *dbioOpen(char *spec, LOGIO *lp);

/* util.c */
BOOL dbioExecuteQuery(DBIO *db, char *query);
BOOL dbioDeleteTable(DBIO *db, char *name);

#endif /* dbio_h_included */

/* Revision History
 *
 * $Log: dbio.h,v $
 * Revision 1.3  2006/02/08 23:52:56  dechavez
 * switched to ffdb instead of ida for flatfile stuff, updated prototypes,
 *
 * Revision 1.2  2005/07/26 00:32:25  dechavez
 * improved handle to make lookup stuff more "natural"
 *
 * Revision 1.1  2004/06/04 23:05:42  dechavez
 * initial release
 *
 */
