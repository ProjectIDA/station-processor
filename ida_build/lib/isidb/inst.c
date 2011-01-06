#pragma ident "$Id: inst.c,v 1.4 2007/10/31 17:00:30 dechavez Exp $"
/*======================================================================
 *
 * Lookup ISI_INST values from database
 *
 *====================================================================*/
#include "isi/db.h"

static ISI_INST undefined = ISI_UNDEFINED_INST;

static BOOL MysqlLookup(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, ISI_INST *inst)
{
#ifdef HAVE_MYSQL
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;
static char *fid = "isidbLookupSint:MysqlLookup";

    query[0] = 0;
    strcat(query, "SELECT DISTINCT calibration.calib,calibration.calper,hang,vang,instrument.instype ");
    strcat(query, "FROM calibration,sitechan,sensor,instrument ");
    sprintf(query + strlen(query), "WHERE calibration.sta=\"%s\" ", name->sta);
    sprintf(query + strlen(query), "AND calibration.chan=\"%s\" ", name->chnloc);
    sprintf(query + strlen(query), "AND calibration.time<=SEC_TO_TIME(%lu) ", when);
    sprintf(query + strlen(query), "AND calibration.endtime>=SEC_TO_TIME(%lu)", when);
    sprintf(query + strlen(query), "AND sitechan.ondate<=SEC_TO_TIME(%lu) ", when);
    sprintf(query + strlen(query), "AND sitechan.offdate>=SEC_TO_TIME(%lu) ", when);
    strcat(query, "AND sitechan.sta=calibration.sta AND sitechan.chan=calibration.chan ");
    strcat(query, "AND sensor.sta=calibration.sta AND sensor.chan=calibration.chan ");
    strcat(query, "AND instrument.inid=sensor.inid");

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

/* There should be at most 1 row.  We don't check for duplicates, leaving that
 * to some exernal database integrity checker and just take the first entry.
 */

    if ((row = mysql_fetch_row(result)) != NULL) {
        inst->calib  = (REAL32) atof(row[0]);
        inst->calper = (REAL32) atof(row[1]);
        inst->hang   = (REAL32) atof(row[2]);
        inst->vang   = (REAL32) atof(row[3]);
        strlcpy(inst->type, row[4], ISI_INAMLEN+1);
    }

    mysql_free_result(result);
    return TRUE;
#else
    return FALSE;
#endif
}

BOOL isidbLookupInst(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, ISI_INST *inst)
{
LOGIO *lp;
static char *fid = "isidbLookupInst";

    if (db == NULL || name == NULL || inst == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    *inst = undefined;
    if (db->engine != DBIO_MYSQL) return TRUE;
    return MysqlLookup(db, name, when, inst);
}

/* Revision History
 *
 * $Log: inst.c,v $
 * Revision 1.4  2007/10/31 17:00:30  dechavez
 * replaced string memcpy with strlcpy
 *
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
