#pragma ident "$Id: debug.c,v 1.3 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup debug flags for a particular app
 *
 *====================================================================*/
#include "isi/db.h"
#include "util.h"

static BOOL FlatFileLookup(DBIO *db, char *app, ISI_DEBUG *debug)
{
IDAFF_GLOB ffglob;

    idaffLookupGlob(db->ff, &ffglob);
    debug->pkt  = ffglob.debug.pkt;
    debug->ttag = ffglob.debug.ttag;
    debug->bwd  = ffglob.debug.bwd;
    debug->dl   = ffglob.debug.dl;
    debug->lock = ffglob.debug.lock;

    return TRUE;
}

static BOOL MysqlLookup(DBIO *db, char *app, ISI_DEBUG *debug)
{
static char *path = "/usr/nrts/etc/isi.cfg";
IDAFF_GLOB ffglob;

    if (utilFileExists(path)) {
        idaffReadGlobalInitFile(path, &ffglob);
    } else {
        idaffDefaultGlob(&ffglob);
    }
    debug->pkt  = ffglob.debug.pkt;
    debug->ttag = ffglob.debug.ttag;
    debug->bwd  = ffglob.debug.bwd;
    debug->dl   = ffglob.debug.dl;
    debug->lock = ffglob.debug.lock;

    return TRUE;
}

BOOL isidbLookupDebug(DBIO *db, char *app, ISI_DEBUG *debug)
{
LOGIO *lp;
static char *fid = "isidbLookupDebug";

    if (db == NULL || app == NULL || debug == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return FlatFileLookup(db, app, debug);
    } else {
        return MysqlLookup(db, app, debug);
    }
}

/* Revision History
 *
 * $Log: debug.c,v $
 * Revision 1.3  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2006/11/10 06:50:33  dechavez
 * added ttag to ISI_DEBUG
 *
 * Revision 1.1  2006/03/13 22:26:47  dechavez
 * initial release (faked MySQL support)
 *
 */
