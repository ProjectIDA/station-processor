#pragma ident "$Id: root.c,v 1.4 2007/10/05 19:37:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup root directory for a particular app
 *
 *====================================================================*/
#include "isi/db.h"
#include "util.h"

static BOOL FlatFileLookup(DBIO *db, char *app, char *root)
{
IDAFF_GLOB ffglob;

    idaffLookupGlob(db->ff, &ffglob);
    strlcpy(root, ffglob.root, MAXPATHLEN+1);

    return TRUE;
}

static BOOL MysqlLookup(DBIO *db, char *app, char *root)
{
IDAFF_GLOB ffglob;
char path[MAXPATHLEN+1];
char *home;
static char *isicfg = "isi.cfg";
static char *default_home = "/usr/nrts";
static char *NRTS_HOME = "NRTS_HOME";

    if ((home = getenv(NRTS_HOME)) == NULL) home = default_home;
    sprintf(path, "%s%cetc%c%s", home, PATH_DELIMITER, PATH_DELIMITER, isicfg);

    if (utilFileExists(path)) {
        idaffReadGlobalInitFile(path, &ffglob);
    } else {
        idaffDefaultGlob(&ffglob);
    }
    strlcpy(root, ffglob.root, MAXPATHLEN);

    return TRUE;
}

BOOL isidbLookupRoot(DBIO *db, char *app, char *root)
{
LOGIO *lp;
static char *fid = "isidbLookupRoot";

    if (db == NULL || app == NULL || root == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return FlatFileLookup(db, app, root);
    } else {
        return MysqlLookup(db, app, root);
    }
}

/* Revision History
 *
 * $Log: root.c,v $
 * Revision 1.4  2007/10/05 19:37:25  dechavez
 * added support for NRTS_HOME environment variable
 *
 * Revision 1.3  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2007/01/08 16:00:51  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.1  2006/03/13 22:26:47  dechavez
 * initial release (faked MySQL support)
 *
 */
