#pragma ident "$Id: main.c,v 1.1 2009/03/24 21:11:40 dechavez Exp $"
/*======================================================================
 *
 *  Read data from one or more Paroscientific barometers and write to
 *  an ISI disk loop in IDA10.5 format, using host clock for timing.
 *
 *====================================================================*/
#include "parodl.h"

#define MY_MOD_ID PARODL_MOD_MAIN

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s site [ options ] Device:Speed:Chan:Sint [ Device:Speed:Chan:Sint ... ] [ options ]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"log=name => set log file name\n");
    fprintf(stderr,"-bd      => run in the background\n");
    fprintf(stderr,"\n");
    exit(1);
}

static void StartReadAndPacketThreads(BAROMETER *bp)
{
THREAD tid;
static char *fid = "StartReadAndPacketThreads";

/* Launch the barometer reader */

    if (!THREAD_CREATE(&tid, ReadThread, (void *) bp)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: ReadThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 6);
    }
    THREAD_DETACH(tid);

/* Launch the packet builder thread */

    if (!THREAD_CREATE(&tid, PacketThread, (void *) bp)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: PacketThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
    THREAD_DETACH(tid);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
char *myname, **argv;
int argc;
int i;
char *site = NULL, *SITE;
char *dbspec = NULL;
char *log = NULL;
ISI_GLOB glob;
char *user = DEFAULT_USER;
char *dbgpath = DEFAULT_DBGPATH;
BOOL daemon = DEFAULT_DAEMON;
ISI_DL *dl = NULL;
LOGIO *lp = NULL;
BOOL debug = DEFAULT_DEBUG;
LNKLST list;
struct {
    BOOL pkt;
    BOOL ttag;
    BOOL bwd;
    BOOL dl;
    BOOL lock;
} dbg = { FALSE, FALSE, FALSE, FALSE, FALSE };
UINT16 flags = 0;
LNKLST_NODE *crnt;
BAROMETER *bp;
static char *nname = "II";

    myname = cmdline->myname;
    argc   = cmdline->argc;
    argv   = cmdline->argv;

    listInit(&list);

/*  Get command line arguments  */

    if (argc < 3) help(myname);

    if ((site = strdup(argv[1])) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }
    if ((SITE = strdup(site)) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }
    util_ucase(SITE);

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            if ((dbgpath = strdup(argv[i] + strlen("dbgpath="))) == NULL) {
                fprintf(stderr, "%s: ", myname);
                perror("strdup");
                exit(MY_MOD_ID);
            }
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-dbgpkt") == 0) {
            dbg.pkt = TRUE;
        } else if (strcmp(argv[i], "-dbgttag") == 0) {
            dbg.ttag = TRUE;
        } else if (strcmp(argv[i], "-dbgbwd") == 0) {
            dbg.bwd = TRUE;
        } else if (strcmp(argv[i], "-dbgdl") == 0) {
            dbg.dl = TRUE;
        } else if (strcmp(argv[i], "-dbglock") == 0) {
            dbg.lock = TRUE;
        } else if (strcmp(argv[i], "-timeops") == 0) {
            flags |= ISI_DL_FLAGS_TIME_DL_OPS;
        } else if (strcmp(argv[i], "-nolock") == 0) {
            flags |= ISI_DL_FLAGS_IGNORE_LOCKS;
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (!AddBarometer(&list, argv[i], site)) {
            fprintf(stderr, "%s: failed to add barometer: %s\n", myname, strerror(errno));
            exit(MY_MOD_ID);
        }
    }

    if (list.count == 0) {
        fprintf(stderr,"%s: must specify at least one barometer\n", myname);
        help(myname);
    }

/* Must specify site name */

    if (site == NULL) {
        fprintf(stderr,"%s: missing site name\n", myname);
        help(myname);
    }

/* Load the database */

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", myname, strerror(errno));
        exit(MY_MOD_ID);
    }

    if (dbg.pkt)  glob.debug.pkt = LOG_INFO;
    if (dbg.ttag) glob.debug.ttag = LOG_INFO;
    if (dbg.bwd)  glob.debug.bwd = LOG_INFO;
    if (dbg.dl)   glob.debug.dl = LOG_INFO;
    if (dbg.lock) glob.debug.lock = LOG_INFO;
    if (flags != 0) glob.flags |= flags;

    utilSetIdentity(user);

/* Connect to the disk loop */

    if ((dl = isidlOpenDiskLoop(&glob, site, NULL, ISI_RDWR)) == NULL) {
        fprintf(stderr, "%s: unable to open '%s' disk loop\n", site, myname);
        exit(MY_MOD_ID);
    }
    isidlLogDL(dl, LOG_DEBUG);
    if (dl->sys->state != ISI_IDLE) {
        fprintf(stderr, "ERROR: disk loop is not idle!\n");
        exit(MY_MOD_ID);
    }

/* Go into the background, if applicable */

    if (daemon && !BackGround(dl)) {
        perror("BackGround");
        exit(MY_MOD_ID);
    }

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(myname, log, SITE, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID);
    }
    dl->lp = lp;
    if (dl->nrts != NULL) dl->nrts->lp = lp;
    LogMsg(LOG_INFO, "database = %s", glob.db->dbid);
    if (flags & ISI_DL_FLAGS_IGNORE_LOCKS) LogMsg(LOG_INFO, "WARNING: no ISI disk loop locks");

/* Start signal handling thread */

    StartSignalHandler();

/* Initialize the exit facility */

    InitExit(dl);

/* Initialize the disk loop writer */

    InitWriter(dl);

/* Launch a pair of read and packetizer threads for each barometer */

    crnt = listFirstNode(&list);
    while (crnt != NULL) {
        bp = (BAROMETER *) crnt->payload;
        bp->lp = lp;
        bp->debug = debug;
        ida10SintToFactMult((REAL64) bp->MsecSint / (REAL64) MSEC_PER_SEC, &bp->srfact, &bp->srmult);
        strlcpy(bp->sname, site, IDA105_SNAME_LEN+1);
        strlcpy(bp->nname, nname, IDA105_NNAME_LEN+1);
        StartReadAndPacketThreads((BAROMETER *) crnt->payload);
        crnt = listNextNode(crnt);
    }

    LogMsg(LOG_DEBUG, "initialization complete");

/* Never return */

#ifndef WIN32
    while (1) pause();  /* all the action happens in the read threads */
#endif /* !WIN32 */
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
VOID main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2009/03/24 21:11:40  dechavez
 * initial release
 *
 */
