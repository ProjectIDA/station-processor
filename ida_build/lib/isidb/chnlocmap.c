#pragma ident "$Id: chnlocmap.c,v 1.6 2007/10/31 17:00:30 dechavez Exp $"
/*======================================================================
 *
 * Retrieve an MK7 channel map from the database
 *
 *====================================================================*/
#include "isi/db.h"

static LNKLST *FlatFileLookup(DBIO *db, char *name, LNKLST *list)
{
int i, index;
IDAFF_CHANMAP *entry;
IDA_CHNLOCMAP map;
static char *fid = "isidbChnLocMapList:FlatFileLookup";

    for (i = 0, index = -1; index == -1 && i < db->ff->nmap; i++) {
        if (strcmp(name, db->ff->chanmap[i].name) == 0) index = i;
    }
    if (index < 0) return NULL;

    entry = &db->ff->chanmap[index];
    for (i = 0; i < entry->nstream; i++) {
        map.ccode = entry->stream[i].chan;
        map.fcode = entry->stream[i].filt;
        map.tcode = entry->stream[i].trig;
        if (strlen(entry->stream[i].chname) <= 3) {
            strlcpy(map.chn, entry->stream[i].chname, ISI_CHNLEN+1);
            map.loc[0] = 0;
        } else {
            memcpy(map.chn, entry->stream[i].chname, 3);
            map.chn[3] = 0;
            memcpy(map.loc, entry->stream[i].chname + 3, 2);
            map.loc[2] = 0;
        }
        if (!listAppend(list, &map, sizeof(IDA_CHNLOCMAP))) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            return NULL;
        }
    }

    return list;
}

static LNKLST *MysqlLookup(DBIO *db, char *name, LNKLST *list)
{
#ifdef HAVE_MYSQL
MYSQL_RES *result;
MYSQL_ROW row;
char query[1024];
IDA_CHNLOCMAP map;
static char *fid = "isidbChnLocMapList:MysqlLookup";

    query[0] = 0;
    strcat(query, "SELECT DISTINCT ccode,fcode,tcode,chan,loc FROM chanlocmap WHERE ");
    sprintf(query+strlen(query), "name=\"%s\"", name);

    if (!dbioExecuteQuery(db, query)) return FALSE;
    result = mysql_store_result(&db->mysql);

    while ((row = mysql_fetch_row(result)) != NULL) {
        map.ccode = atoi(row[0]);
        map.fcode = atoi(row[1]);
        map.tcode = atoi(row[2]);
        strlcpy(map.chn,  row[3], ISI_CHNLEN+1);
        strlcpy(map.loc,  row[4], ISI_LOCLEN+1);
        if (!listAppend(list, &map, sizeof(IDA_CHNLOCMAP))) {
            logioMsg(db->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
            mysql_free_result(result);
            return NULL;
        }
    }
    mysql_free_result(result);

    return list;
#else
    return NULL;
#endif
}

LNKLST *isidbChnLocMapList(DBIO *db, char *name)
{
LOGIO *lp;
LNKLST *list, *result;
static char *fid = "isidbChnLocMapList";

    if (db == NULL || name == NULL) {
        lp = (db == NULL) ? NULL : db->lp;
        logioMsg(lp, LOG_DEBUG, "%s: NULL input(s)", fid);
        errno = EINVAL;
        return NULL;
    }

    if ((list = listCreate()) == NULL) {
        logioMsg(db->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return NULL;
    }

    if (db->engine != DBIO_MYSQL) {
        result = FlatFileLookup(db, name, list);
    } else {
        result = MysqlLookup(db, name, list);
    }

    if (result != list) {
        listDestroy(list);
        return NULL;
    }

    if (!listSetArrayView(list)) {
        logioMsg(db->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        listDestroy(list);
        return NULL;
    }

    return list;
}

/* Revision History
 *
 * $Log: chnlocmap.c,v $
 * Revision 1.6  2007/10/31 17:00:30  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.5  2007/01/11 21:59:25  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.4  2007/01/08 16:00:51  dechavez
 * switch to size-bounded string operations
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
