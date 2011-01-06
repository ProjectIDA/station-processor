#pragma ident "$Id: sitechan.c,v 1.6 2007/10/31 16:59:50 dechavez Exp $"
/*======================================================================
 *
 * Lookup current ISI_INST values from sitechan table
 *
 *====================================================================*/
#include "isi/db.h"

static ISI_INST undefined = ISI_UNDEFINED_INST;

static BOOL MysqlLookup(DBIO *db, ISI_STREAM_NAME *name, UINT32 unused, ISI_INST *inst)
{
#ifdef HAVE_MYSQL
char query[1024];
MYSQL_RES *result;
MYSQL_ROW row;
static char *fid = "isidbLookupSitechan:MysqlLookup";

    query[0] = 0;
    strcat(query, "SELECT calib,calper,hang,vang,type FROM isi_sitechan ");
    sprintf(query + strlen(query), "WHERE sta=\"%s\" ", name->sta);
    sprintf(query + strlen(query), "AND chan=\"%s\" ", name->chnloc);

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
    } else {
        *inst = undefined;
    }

    mysql_free_result(result);
    return TRUE;
#else
    return FALSE;
#endif
}

BOOL isidbLookupSitechan(DBIO *db, ISI_STREAM_NAME *name, UINT32 when, ISI_INST *inst)
{
LOGIO *lp;
static char *fid = "isidbLookupSitechan";

    if (db == NULL || name == NULL || inst == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return FALSE;
    }

    if (db->engine != DBIO_MYSQL) {
        return isidbLookupInst(db, name, when, inst); /* since it's flatfile routines are identical */
    } else {
        return MysqlLookup(db, name, when, inst);
    }
}

/* The following are used to generate the isi_sitechan table from other tables */

#ifdef HAVE_MYSQL

static BOOL CreateFreshIsiSitechanTable(DBIO *db)
{
static char *NewTableCommand =
"CREATE TABLE isi_sitechan ("
" sta varchar(6) NOT NULL,"
" chan varchar(6) NOT NULL,"
" calib double NOT NULL default 0.0,"
" calper double NOT NULL default -1.0,"
" hang double NOT NULL default -999.9,"
" vang double NOT NULL default -999.9,"
" edepth double NOT NULL default -9.999,"
" type varchar(6) NOT NULL"
")";
    
    printf("Deleting existing isi_sitechan table (if any)\n");
    dbioDeleteTable(db, "isi_sitechan");

    printf("Creating fresh isi_sitechan table\n");
    if (!dbioExecuteQuery(db, NewTableCommand)) {
        fprintf(stderr, "failed to create new isi_sitechan table!");
        fprintf(stderr, "FATAL error, program aborts\n");
        return FALSE;
    }
    return TRUE;
}

static BOOL InsertIsiSitechanRow(DBIO *db, char *sta, char *chan, MYSQL_ROW row)
{
char query[1024];

    query[0] = 0;
    strcat(query, "INSERT INTO isi_sitechan (");
    strcat(query, "sta, ");
    strcat(query, "chan, ");
    strcat(query, "calib, ");
    strcat(query, "calper, ");
    strcat(query, "hang, ");
    strcat(query, "vang, ");
    strcat(query, "edepth, ");
    strcat(query, "type ");
    strcat(query, ") VALUES (");
    sprintf(query+strlen(query), "\"%s\"", sta);
    sprintf(query+strlen(query), ", \"%s\"", chan);
    sprintf(query+strlen(query), ", %s", row[0]);
    sprintf(query+strlen(query), ", %s", row[1]);
    sprintf(query+strlen(query), ", %s", row[2]);
    sprintf(query+strlen(query), ", %s", row[3]);
    sprintf(query+strlen(query), ", %s", row[4]);
    sprintf(query+strlen(query), ", \"%s\"", row[5]);
    sprintf(query+strlen(query), ")");

    if (!dbioExecuteQuery(db, query)) return FALSE;
    
    return TRUE;
}

static BOOL AddStaChanEntry(DBIO *db, char *sta, char *chan)
{
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];

    printf(" %s", chan); fflush(stdout);

    query[0] = 0;
    strcat(query, "SELECT DISTINCT ");
    strcat(query, "calibration.calib, ");
    strcat(query, "calibration.calper, ");
    strcat(query, "sitechan.hang, ");
    strcat(query, "sitechan.vang, ");
    strcat(query, "sitechan.edepth, ");
    strcat(query, "instrument.instype ");
    strcat(query, "FROM calibration,sitechan,sensor,instrument ");
    sprintf(query+strlen(query), "WHERE calibration.sta=\"%s\" ", sta);
    sprintf(query+strlen(query), "AND calibration.chan=\"%s\" ", chan);
    strcat(query, "AND calibration.time<=NOW() ");
    strcat(query, "AND calibration.endtime>=NOW() ");
    strcat(query, "AND sitechan.ondate<=NOW() ");
    strcat(query, "AND sitechan.offdate>=NOW() ");
    strcat(query, "AND sitechan.sta=calibration.sta ");
    strcat(query, "AND sitechan.chan=calibration.chan ");
    strcat(query, "AND sensor.sta=calibration.sta ");
    strcat(query, "AND sensor.chan=calibration.chan ");
    strcat(query, "AND instrument.inid=sensor.inid");

    if (!dbioExecuteQuery(db, query)) return FALSE;

    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        if (!InsertIsiSitechanRow(db, sta, chan, row)) {
            mysql_free_result(result);
            return FALSE;
        }
    }
    mysql_free_result(result);

    return TRUE;
}

static BOOL AddToIsiSitechan(DBIO *db, char *sta)
{
UINT32 i;
char *chan;
LNKLST *ChanList;

    printf("%s:", sta); fflush(stdout);
    if ((ChanList = isidbCalibrationChanList(db, sta, (UINT32) time(NULL))) == NULL) {
        printf("NULL chan list, station %s ignored\n", sta);
        listDestroy(ChanList);
        return TRUE;
    }

    for (i = 0; i < ChanList->count; i++) {
        chan = (char *) ChanList->array[i];
        if (!AddStaChanEntry(db, sta, chan)) {
            printf("<error>\n");
            listDestroy(ChanList);
            return FALSE;
        }
    }
    printf("\n");

    listDestroy(ChanList);
    return TRUE;
}


BOOL isidbGenerateIsiSitechanTable(DBIO *db)
{
LNKLST *SiteList;
UINT32 i;

    if ((SiteList = isidbSiteList(db)) == NULL) return FALSE;

    if (!CreateFreshIsiSitechanTable(db)) {
        listDestroy(SiteList);
        return FALSE;
    }

    for (i = 0; i < SiteList->count; i++) {
        if (!AddToIsiSitechan(db, (char *) SiteList->array[i])) {
            listDestroy(SiteList);
            return FALSE;
        }
    }

    listDestroy(SiteList);
    return TRUE;
}
#else

BOOL isidbGenerateIsiSitechanTable(DBIO *db)
{
    errno = ENOTSUP;
    return FALSE;
}

#endif

/* Revision History
 *
 * $Log: sitechan.c,v $
 * Revision 1.6  2007/10/31 16:59:50  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.5  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.4  2006/06/26 22:34:10  dechavez
 * fixed signed/unsigned comparison warnings
 *
 * Revision 1.3  2006/04/20 18:55:11  dechavez
 * support builds witout MYSQL support
 *
 * Revision 1.2  2006/02/14 17:05:25  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/09 00:21:15  dechavez
 * initial release
 *
 */
