#pragma ident "$Id: main.c,v 1.5 2007/11/01 22:24:18 dechavez Exp $"
/*======================================================================
 *
 *  Test harness for exercising the ISI database access routines
 *
 *====================================================================*/
#include "dbio.h"
#include "isi/dl.h"
#include "isi/db.h"

extern char *VersionIdentString;

BOOL verbose = FALSE;

static void TestRevs(DBIO *db)
{
int i;
IDA_REV result;

    printf("Supported Rev Codes\n");
    for (i = 0; i < 10; i++) {
        if (isidbLookupRev(db, i+1, &result)) printf("%2d - %s\n", result.value, result.description);
    }
}

static void CheckCoords(DBIO *db, char *site)
{
ISI_COORDS coords;

    if (isidbLookupCoords(db, site, (UINT32) time(NULL), &coords)) {
        printf("%9.4f ", coords.lat);
        printf("%9.4f ", coords.lon);
        printf("%9.4f ", coords.elev);
        printf("%9.4f ", coords.depth);
        printf("\n");
    } else {
        printf(" <isidbLookupCoords failed>\n");
    }
}

static void CheckName(DBIO *db, ISI_STREAM_NAME *name)
{
ISI_INST inst;
int sint, wrdsiz, datlen;
ISI_SRATE srate;
char ident[ISI_STREAM_NAME_LEN+1];

    isiStreamNameString(name, ident);
    printf("%s:", ident);
    if (isidbLookupSint(db, name, &sint)) {
        printf(" sint=%d", sint);
    } else {
        printf(" sint=FAIL");
    }
    if (isidbLookupWrdsiz(db, name, &wrdsiz)) {
        printf(" wrdsiz=%d", wrdsiz);
    } else {
        printf(" wrdsiz=FAIL");
    }
    if (isidbLookupDatlen(db, name, &datlen)) {
        printf(" datlen=%d", datlen);
    } else {
        printf(" datlen=FAIL");
    }
    if (isidbLookupSitechan(db, name, (UINT32) time(NULL), &inst)) {
        printf(" calib=%11.4e calper=%11.4e", inst.calib, inst.calper);
        printf(" hang=%5.1f vang=%5.1f", inst.hang, inst.vang);
        printf(" type=%s", inst.type);
    } else {
        printf(" calib=calper=hang=vang=type=FAIL");
    }
    printf("\n");
}

static void TestStamap(DBIO *db, char *sta)
{
int i;
LNKLST *map;
char name[IDA_MNAMLEN+1];
IDA_CHNLOCMAP *entry;

    if (isidbLookupStamap(db, sta, 0, name, IDA_MNAMLEN) == NULL) {
        printf("No default channel map found for station %s\n", sta);
        return;
    }

    printf("Default channel map for station %s is '%s'\n", sta, name);
    if ((map = isidbChnLocMapList(db, name)) == NULL) {
        printf("Can't find '%s' channel map!\n", name);
        return;
    }

    printf("Channel Filter Mode Chn Loc\n");
    for (i = 0; i < map->count; i++) {
        entry = (IDA_CHNLOCMAP *) map->array[i];
        printf("%7d %6d %4d %3s %3s\n", entry->ccode, entry->fcode, entry->tcode, entry->chn, entry->loc);
    }
    listDestroy(map);
}

static void help(char *myname)
{
    exit(1);
}

static void TestStuff(DBIO *db, LNKLST *SystemList, ISI_STREAM_NAME *name)
{
int i;
char *site;

    for (i = 0; i < SystemList->count; i++) {
        site = (char *) SystemList->array[i];
        printf("%-5s ", site);
        CheckCoords(db, site);
    }

    if (name != NULL) CheckName(db, name);
}

static void UpdateAncillaryTables(DBIO *db)
{
    if (!isidbGenerateIsiSitechanTable(db)) {
        printf("isidbGenerateIsiSitechanTable failed\n");
        exit(1);
    }
}

#define DEFAULT_DBID "idahub.ucsd.edu:nrts:NULL:isi:0"

int main (int argc, char **argv)
{
int i;
BOOL HaveName = FALSE, update = FALSE;
DBIO *db;
LOGIO logio, *lp;
LNKLST *SystemList;
char *dbid = DEFAULT_DBID;
char *sta = NULL;
char *chn = NULL;
char *loc = NULL;
BOOL stamap = FALSE, revs = FALSE;
ISI_STREAM_NAME name;
static char *log = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbid = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "revs") == 0) {
            revs = TRUE;
        } else if (strcmp(argv[i], "-update") == 0) {
            update = TRUE;
        } else if (strcmp(argv[i], "stamap") == 0) {
            stamap = TRUE;
        } else if (sta == NULL) {
            sta = argv[i];
        } else if (chn == NULL) {
            chn = argv[i];
        } else if (loc == NULL) {
            loc = argv[i];
        } else {
           fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (stamap && sta == NULL) {
        fprintf(stderr, "stamap option requires station name\n");
        exit(1);
    }

    if (sta != NULL && chn != NULL) {
        isiStaChnLocToStreamName(sta, chn, loc == NULL ? "" : loc, &name);
        HaveName = TRUE;
    }

    if (log == NULL) {
        lp = NULL;
    } else {
        lp = &logio;
        logioInit(lp, log, NULL, argv[0]);
    }

    if (verbose) {
        fprintf(stderr, "%s %s\n", argv[0], VersionIdentString);
        fprintf(stderr, "dbid = %s\n", dbid);
    }

    if ((db = dbioOpen(dbid, lp)) == NULL) {
        perror("dbioOpen");
        exit(1);
    }

    if (update) {
        if (db->engine != DBIO_MYSQL) {
            fprintf(stderr, "update option only valid for MySQL databases\n");
            exit(0);
        } else {
            UpdateAncillaryTables(db);
            exit(1);
        }
    }

    if (stamap) {
        TestStamap(db, sta);
        exit(0);
    }

    if (revs) {
        TestRevs(db);
        exit(0);
    }

    if ((SystemList = isidbSystemList(db)) == NULL) {
        fprintf(stderr, "isidbSystemList failed\n");
        fprintf(stderr, "FATAL error, program aborts\n");
        exit(1);
    }

    printf("%d ISI systems found\n", SystemList->count);
    TestStuff(db, SystemList, HaveName ? &name : NULL);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.5  2007/11/01 22:24:18  dechavez
 * added revs option
 *
 * Revision 1.4  2007/01/11 22:02:20  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.3  2006/04/20 19:05:46  dechavez
 * remove stuff now present in libdbio
 *
 * Revision 1.2  2006/02/14 17:05:00  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/10 02:11:46  dechavez
 * initial release
 *
 */
