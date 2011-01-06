#pragma ident "$Id: main.c,v 1.16 2009/01/28 05:35:40 dechavez Exp $"
/*======================================================================
 *
 *  Build a new raw ISI disk loop
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"
#include "qdplus.h"
#include "liss.h"

extern char *VersionIdentString;

#define TYPE_UNKNOWN -1
#define TYPE_IDA      0
#define TYPE_QDP      1
#define TYPE_LISS     2

#define DEFAULT_LOG "-"

#define DEFAULT_IDA_NREC   489384 /* 7 days of default MK8 config */
#define DEFAULT_IDA_NHIDE  17568 /* 6 hours of default MK8 config */
#define DEFAULT_IDA_RECLEN IDA_BUFSIZ

#define DEFAULT_QDP_NREC   489384 /* need to figure out good default value */
#define DEFAULT_QDP_NHIDE  17568 /* need to figure out good default value */
#define DEFAULT_QDP_RECLEN QDPLUS_PKTLEN

#define DEFAULT_LISS_NREC   489384 /* need to figure out good default value */
#define DEFAULT_LISS_NHIDE  17568 /* need to figure out good default value */
#define DEFAULT_LISS_RECLEN LISS_DEFAULT_BLKSIZ

static void help(char *myname)
{
    printf("%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s type={ida | qdp | liss} [ options ] name\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "db=spec    => database specifier\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "For fine control over the allocation use the following:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "nrec=int   => number of records to allocate\n");
    fprintf(stderr, "hide=int   => number of records to \"hide\"\n");
    fprintf(stderr, "reclen=int => record length\n");
    fprintf(stderr, "\n");
    exit(1);
}

int main (int argc, char **argv)
{
int i;
int type = TYPE_UNKNOWN;
int nrec = 0, nhide = 0, reclen = 0;
static char *dbspec = NULL;
static char *log = DEFAULT_LOG;
static char *sta = NULL;
char *string;
ISI_GLOB glob;
ISI_DL *dl;
LOGIO lp;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "type=", strlen("type=")) == 0) {
            string = argv[i] + strlen("type=");
            if (strcmp(string, "ida") == 0) {
                type = TYPE_IDA;
                nrec = DEFAULT_IDA_NREC;
                nhide = DEFAULT_IDA_NHIDE;
                reclen = DEFAULT_IDA_RECLEN;
            } else if (strcmp(string, "qdp") == 0) {
                type = TYPE_QDP;
                nrec = DEFAULT_QDP_NREC;
                nhide = DEFAULT_QDP_NHIDE;
                reclen = DEFAULT_QDP_RECLEN;
            } else if (strcmp(string, "liss") == 0) {
                type = TYPE_LISS;
                nrec = DEFAULT_LISS_NREC;
                nhide = DEFAULT_LISS_NHIDE;
                reclen = DEFAULT_LISS_RECLEN;
            } else {
                fprintf(stderr, "%s: unrecognize type '%s'\n", argv[0], string);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "nrec=", strlen("nrec=")) == 0) {
            if ((nrec = atoi(argv[i] + strlen("nrec="))) < 1) {
                fprintf(stderr, "%s: illegal %s\n", argv[0], argv[i]);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "hide=", strlen("hide=")) == 0) {
            if ((nhide = atoi(argv[i] + strlen("hide="))) < 1) {
                fprintf(stderr, "%s: illegal %s\n", argv[0], argv[i]);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "reclen=", strlen("reclen=")) == 0) {
            if ((reclen = atoi(argv[i] + strlen("reclen="))) < 1) {
                fprintf(stderr, "%s: illegal %s\n", argv[0], argv[i]);
                help(argv[0]);
            }
        } else if (sta == NULL) {
            sta = argv[i];
        } else {
            fprintf(stderr, "\n%s: too many arguments!\n", argv[0]);
            help(argv[0]);
        }
    }

    if (nrec == 0 || reclen == 0 || nhide == 0 || sta == NULL) help(argv[0]);

    if (!isidlSetGlobalParameters(dbspec, argv[0], &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    logioInit(&lp, log, NULL, argv[0]);
    logioMsg(&lp, LOG_INFO, "%s %s\n", argv[0], VersionIdentString);

/* Check to see if there already is a disk loop, by trying to open it */

    if ((dl = isidlOpenDiskLoop(&glob, sta, &lp, ISI_RDONLY)) != NULL) {
        isidlLogDL(dl, LOG_INFO);
        printf("\n ** ERROR: A valid ISI disk loop already exists in %s **\n", dl->base);
        exit(1);
    }

/* Create the disk loop */

    if ((dl = isidlCreateDiskLoop(&glob, sta, (UINT32) nrec, (UINT32) nhide, (UINT32) reclen, &lp)) == NULL) {
        perror("isidlCreateDiskLoop");
        exit(1);
    }

/* Now make sure we can open it to verify it is OK */

    isidlCloseDiskLoop(dl);

    if ((dl = isidlOpenDiskLoop(&glob, sta, &lp, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "Can't open newly created disk loop!!!");
        exit(1);
    }

/* QDP disk loops need a subdirectory for meta-data */

    if (type == TYPE_QDP) {
        isidlSetMetaDir(dl);
        if (util_mkpath(dl->path.meta, 0755) != 0) {
            fprintf(stderr, "ERROR: can't create meta-data directory: ");
            perror(dl->path.meta);
            exit(1);
        }
        isidlCloseDiskLoop(dl);
        isidlOpenDiskLoop(&glob, sta, &lp, ISI_RDONLY);
    }

    isidlLogDL(dl, LOG_INFO);

    logioMsg(dl->lp, LOG_INFO, "%s disk loop created OK\n", dl->base);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.16  2009/01/28 05:35:40  dechavez
 * Don't create QDP state file, fixed annoying "No such file" message
 * when creating new QDP loop w/o pre-existing metadir
 *
 * Revision 1.15  2008/02/21 09:15:54  dechavez
 * Fixed bug where type was required even if explicit sizes were given
 *
 * Revision 1.14  2007/09/07 20:08:17  dechavez
 * Create state file for type QDP disk loops
 *
 * Revision 1.13  2007/05/03 21:00:36  dechavez
 * Added LISS defaults (type=liss)
 *
 * Revision 1.12  2007/01/11 22:02:28  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.11  2006/12/12 23:10:55  dechavez
 * Added generation of metadata subdirectory for QDP disk loops
 *
 * Revision 1.10  2006/06/27 00:18:47  akimov
 * misc WIN32 warning removals
 *
 * Revision 1.9  2006/06/02 21:29:48  dechavez
 * added support for QDP(LUS) packets, require user to specify type on command line
 *
 * Revision 1.8  2006/03/13 23:12:31  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.7  2006/02/10 01:12:23  dechavez
 * Made disk loop already exists message to look like an error, message, not merely informational
 *
 * Revision 1.6  2006/02/09 22:36:55  dechavez
 * removed unecessary includes
 *
 * Revision 1.5  2005/07/26 00:53:46  dechavez
 * added ISI_GLOB support, open after to create to check NRTS status
 *
 * Revision 1.4  2005/06/24 21:49:03  dechavez
 * added hide parameter
 *
 * Revision 1.3  2005/06/02 17:32:24  dechavez
 * added perm argument to isiOpenDiskLoop call
 *
 * Revision 1.2  2005/05/25 00:35:29  dechavez
 * initial production release
 *
 * Revision 1.1  2005/05/13 18:51:29  dechavez
 * checkpoint build before 05-11 akimov update
 *
 */
