#pragma ident "$Id: srate.c,v 1.4 2007/01/11 21:59:25 dechavez Exp $"
/*======================================================================
 *
 * Lookup sample rate from sint table.
 *
 *====================================================================*/
#include "isi/db.h"

BOOL isidbLookupSrate(DBIO *db, ISI_STREAM_NAME *name, ISI_SRATE *srate)
{
int sint;
LOGIO *lp;
static char *fid = "isidbLookupSrate";

    if (db == NULL || name == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (!isidbLookupSint(db, name, &sint)) {
        logioMsg(db->lp, LOG_ERR, "%s: isidbLookupSint failed", fid);
        srate->factor = (INT16) 0xffff;
        srate->multiplier = (INT16) 0xffff;
        return FALSE;
    }

    isiSintToSrate((REAL64) sint / 1000.0, srate);

    return TRUE;
}

/* Revision History
 *
 * $Log: srate.c,v $
 * Revision 1.4  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.3  2006/06/26 22:33:57  dechavez
 * fixed uninitialized log handle error
 *
 * Revision 1.2  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
