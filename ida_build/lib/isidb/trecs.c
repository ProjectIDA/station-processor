#pragma ident "$Id: trecs.c,v 1.2 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup trecs
 *
 *====================================================================*/
#include "isi/db.h"
#include "util.h"

static BOOL FlatFileLookup(DBIO *db, UINT32 *trecs)
{
IDAFF_GLOB ffglob;

    idaffLookupGlob(db->ff, &ffglob);
    *trecs = ffglob.trecs;

    return TRUE;
}

static BOOL MysqlLookup(DBIO *db, UINT32 *trecs)
{
static char *path = "/usr/nrts/etc/isi.cfg";
IDAFF_GLOB ffglob;

    if (utilFileExists(path)) {
        idaffReadGlobalInitFile(path, &ffglob);
    } else {
        idaffDefaultGlob(&ffglob);
    }
    *trecs = ffglob.trecs;

    return TRUE;
}

BOOL isidbLookupTrecs(DBIO *db, UINT32 *trecs)
{
LOGIO *lp;
static char *fid = "isidbLookupTrecs";

    if (db == NULL || trecs == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return FlatFileLookup(db, trecs);
    } else {
        return MysqlLookup(db, trecs);
    }
}

/* Revision History
 *
 * $Log: trecs.c,v $
 * Revision 1.2  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.1  2006/03/13 22:26:47  dechavez
 * initial release (faked MySQL support)
 *
 */
