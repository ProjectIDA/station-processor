#pragma ident "$Id: open.c,v 1.6 2007/01/08 15:48:03 dechavez Exp $"
/*======================================================================
 *
 *  Connect to DBIO database
 *
 *====================================================================*/
#include "dbio.h"
#include "util.h"

#ifdef HAVE_MYSQL

static BOOL OpenMySQL(DBIO *db)
{
#define NUMTOKEN 5 /* host:user:passwd:dbname:port */
char *token[NUMTOKEN];
int ntoken;
char copy[MAXPATHLEN+1];
MYSQL *unused;
static char *fid = "dbioOpen:OpenMySQL";

    strlcpy(copy, db->dbid, MAXPATHLEN+1);

    if ((ntoken = utilParse(copy, token, ":", NUMTOKEN, 0)) != NUMTOKEN) {
        logioMsg(db->lp, LOG_DEBUG, "%s: dbid parse error (got %d tokens, expected %d)", fid, ntoken, NUMTOKEN);
        logioMsg(db->lp, LOG_DEBUG, "%s: dbid = '%s'", fid, db->dbid);
        errno = EINVAL;
        return FALSE;
    }

    db->dbname = db->host = db->user = db->passwd = NULL;

    if (mysql_init(&db->mysql) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: mysql_init: %s", fid, strerror(errno));
        free(db);
        return FALSE;
    }

    if (strcasecmp(token[0], "NULL") == 0) {
        db->host = NULL;
    } else if ((db->host = strdup(token[0])) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: strdup: %s", fid, strerror(errno));
        dbioClose(db);
        return FALSE;
    }

    if (strcasecmp(token[1], "NULL") == 0) {
        db->user = NULL;
    } else if ((db->user = strdup(token[1])) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: strdup: %s", fid, strerror(errno));
        dbioClose(db);
        return FALSE;
    }

    if (strcasecmp(token[2], "NULL") == 0) {
        db->passwd = NULL;
    } else if ((db->passwd = strdup(token[2])) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: strdup: %s", fid, strerror(errno));
        dbioClose(db);
        return FALSE;
    }

    if (strcasecmp(token[3], "NULL") == 0) {
        db->dbname = NULL;
    } else if ((db->dbname = strdup(token[3])) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: strdup: %s", fid, strerror(errno));
        dbioClose(db);
        return FALSE;
    }

    db->port = (unsigned int) atoi(token[4]);

    if ( (unused=mysql_real_connect(
        &db->mysql,
        db->host,
        db->user,
        db->passwd,
        db->dbname,
        db->port,
        NULL,
        0
    ))!=NULL) return TRUE;

    logioMsg(db->lp, LOG_ERR, "%s: mysql_real_connect: error %d: %s", fid, mysql_errno(&db->mysql), mysql_error(&db->mysql));
    dbioClose(db);

    return FALSE;

}

#endif /* HAVE_MYSQL */

DBIO *dbioOpen(char *spec, LOGIO *lp)
{
DBIO *db;
static char *fid = "dbioOpen";

    if (spec == NULL && (spec = getenv(DBIO_ENV_STRING)) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL spec", fid);
        errno = EINVAL;
        return NULL;
    }

    if ((db = calloc(1, sizeof(DBIO))) == NULL) return NULL;
    db->dbid = NULL;
    db->ff = NULL;
    db->engine = DBIO_UNDEFINED;
    if ((db->dbid = strdup(spec)) == NULL) return dbioClose(db);
    db->lp = lp;

#ifdef HAVE_MYSQL
    if (OpenMySQL(db)) {
        db->engine = DBIO_MYSQL;
        return db;
    }
#endif /* HAVE_MYSQL */

/* Default to flatfile lookups if MySQL fails */

    db->engine = DBIO_FLATFILE;
    if ((db->ff = idaffInit(db->dbid)) == NULL) return dbioClose(db);

    return db;
}

/* Revision History
 *
 * $Log: open.c,v $
 * Revision 1.6  2007/01/08 15:48:03  dechavez
 * strlcpy() instead of strncpy()
 *
 * Revision 1.5  2006/12/21 19:16:09  dechavez
 * log the reason for mysql_real_connect() failures
 *
 * Revision 1.4  2006/06/17 00:21:38  dechavez
 * aap mysql_real_connect changes, presumably to calm Win32 builds
 *
 * Revision 1.3  2006/02/08 23:55:26  dechavez
 * added LOGIO parameter, switched to idaff flat file functions
 *
 * Revision 1.2  2005/07/26 00:34:00  dechavez
 * support improved handle
 *
 * Revision 1.1  2004/04/23 00:43:12  dechavez
 * initial release
 *
 */
