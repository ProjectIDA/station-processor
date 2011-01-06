#pragma ident "$Id: init.c,v 1.11 2009/05/14 18:12:44 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#define INCLUDE_QHLP_STATIC_DEFAULTS
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_INIT

typedef struct {
    BOOL pkt;
    BOOL ttag;
    BOOL bwd;
    BOOL dl;
    BOOL lock;
} DEBUG_FLAGS;

/* Open input and output disk loops */

static ISI_DL *OpenDiskLoop(char *dbspec, char *myname, char *site, LOGIO *lp, int perm, DEBUG_FLAGS *dbg)
{
ISI_DL *dl;
static ISI_GLOB glob;
static char *fid = "OpenDiskLoop";

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }

    if (dbg != NULL) {
        if (dbg->pkt) glob.debug.pkt = LOG_INFO;
        if (dbg->ttag) glob.debug.ttag = LOG_INFO;
        if (dbg->bwd) glob.debug.bwd = LOG_INFO;
        if (dbg->dl) glob.debug.dl = LOG_INFO;
        if (dbg->lock) glob.debug.lock = LOG_INFO;
    }

    if ((dl = isidlOpenDiskLoop(&glob, site, lp, perm)) == NULL) {
        fprintf(stderr, "%s: isidlOpenDiskLoop failed: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 2);
    }

    if (dl->nrts != NULL) dl->nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM;

    return dl;
}

static void help(char *myname)
{
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ options ] qdp_site hlp_site\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "db=spec  => set database to `spec'\n");
    fprintf(stderr, "log=name => set log file name\n");
#ifdef unix
    fprintf(stderr, "-bd      => run in the background\n");
#endif /* unix */
    fprintf(stderr, "-ida10   => generate IDA10 packets\n");
    fprintf(stderr, "-lcase   => use lower case channel names\n");
    fprintf(stderr, "-strict  => require LCQ token entries for all streams\n");
    fprintf(stderr, "-notrig  => suppress 100 Hz event trigger\n");
    fprintf(stderr, "\n");
    exit(MY_MOD_ID + 3);
}

QHLP_PAR *init(char *myname, int argc, char **argv)
{
int i;
char *log = NULL;
LOGIO *lp = NULL;
char *dbspec = DEFAULT_DB;
int format = DEFAULT_FORMAT;
BOOL complete;
BOOL debug = DEFAULT_DEBUG;
BOOL daemon = DEFAULT_DAEMON;
UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
QHLP_DL_FUNC save = NULL;
char *cfgpath = NULL;
Q330_CFG *cfg = NULL;
char *watchdir = NULL;
static char *dbgdir = DEFAULT_DEBUG_DIR;
static char *user = DEFAULT_USER;
static DEBUG_FLAGS dbg = { FALSE, FALSE, FALSE, FALSE, FALSE };
static QHLP_PAR qhlp;

    qhlp.input.site = qhlp.output.site = NULL;
    qhlp.output.options = ISI_OPTION_REJECT_COMPLETE_OVERLAPS; /* this might be a command line arg later */
    qhlp.SyncInterval = DEFAULT_SYNC_INTERVAL;

    for (complete = FALSE, i = 1; i < argc; i++) {
        if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strncmp(argv[i], "dbgdir=", strlen("dbgdir=")) == 0) {
            dbgdir = argv[i] + strlen("dbgdir=");
        } else if (strncmp(argv[i], "sync=", strlen("sync=")) == 0) {
            qhlp.SyncInterval = (UINT64) atoi(argv[i] + strlen("sync="));
        } else if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
            cfgpath = argv[i] + strlen("cfg=");
        } else if (strncmp(argv[i], "watchdir=", strlen("watchdir=")) == 0) {
            watchdir = argv[i] + strlen("watchdir=");
        } else if (strcmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (strcmp(argv[i], "-ida10") == 0) {
            format = FORMAT_IDA10;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-lcase") == 0) {
            flags |= QDP_HLP_RULE_FLAG_LCASE;
        } else if (strcmp(argv[i], "-strict") == 0) {
            flags |= QDP_HLP_RULE_FLAG_STRICT;
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
        } else if (qhlp.input.site == NULL) {
            qhlp.input.site = argv[i];
        } else if (qhlp.output.site == NULL) {
            qhlp.output.site = argv[i];
            complete = TRUE;
        } else {
            fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (!complete) {
        fprintf(stderr, "%s: incomplete command line\n", argv[0]);
        help(argv[0]);
    }

    sprintf(qhlp.dbginfo, "%s/%s", dbgdir, utilBasename(argv[0]));

/* Switch to runtime user */

    utilSetIdentity(user);

/* Start logging facility */

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;
    if ((lp = InitLogging(myname, log, qhlp.output.site, debug)) == NULL) {
        perror("InitLogging");
        exit(MY_MOD_ID + 4);
    }
    par.lcq.lp = lp;

/* Initialize watchdog, if any */

    if (watchdir != NULL) Watch = logioInitWatchdog(watchdir, argv[0]);

/* Read the config file, if any */

    cfg = q330ReadCfg(cfgpath);
    if (cfg == NULL) {
        if (errno != 0) {
            fprintf(stderr, "%s: q330ReadCfg: %s\n", argv[0], strerror(errno));
            exit(MY_MOD_ID + 5);
        } else {
            LogMsg("no Q330 config file found");
        }
    } else {
        LogMsg("Q330 config file = %s", cfg->fname);
    }

/* Open disk loops */

    LogMsg("Opening %s disk loop for QDP source", qhlp.input.site);
    qhlp.input.dl = OpenDiskLoop(dbspec, argv[0], qhlp.input.site, lp, ISI_RDONLY, NULL);
    if (!isiInitRawPacket(&qhlp.input.raw, NULL, qhlp.input.dl->sys->maxlen)) {
        fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
        exit(MY_MOD_ID + 6);
    }

    LogMsg("Opening %s disk loop for HLP destination", qhlp.output.site);
    qhlp.output.dl = OpenDiskLoop(dbspec, argv[0], qhlp.output.site, lp, ISI_RDWR, &dbg);
    if (!isiInitRawPacket(&qhlp.output.raw, NULL, qhlp.output.dl->sys->maxlen)) {
        fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
        exit(MY_MOD_ID + 7);
    }

    strlcpy(qhlp.output.raw.hdr.site, qhlp.output.dl->sys->site, ISI_SITELEN+1);
    qhlp.output.raw.hdr.len.used = qhlp.output.dl->sys->maxlen;
    qhlp.output.raw.hdr.len.native = qhlp.output.dl->sys->maxlen;

/* Determine and initialize the HLP converter */

    switch (format) {
      case FORMAT_IDA10:
        if (!InitIDA10(&par.lcq.rules, &qhlp.output, flags)) {
            fprintf(stderr, "Failed to initialize IDA10 packet formatter!\n");
            exit(MY_MOD_ID + 8);
        }
        save = SaveIDA10;
        LogMsg("Packet converter configured for IDA10");
        break;
      default:
        fprintf(stderr, "%s: unsupported high level packet format\n", argv[0]);
        exit(MY_MOD_ID + 9);
    }

/* Initialize the HLP callback */

    if (!InitPacketProcessor(cfg, qhlp.output.dl, qhlp.input.dl->path.meta, save)) {
        fprintf(stderr, "Failed to initialize HLP callback: %s\n", strerror(errno));
        exit(MY_MOD_ID + 10);
    }

/* Create QDPLUS packet processor handle */

    par.site = qhlp.input.site;
    if (!(qhlp.input.dl->flags & ISI_DL_FLAGS_HAVE_META)) {
        fprintf(stderr, "%s: no meta data found for site = %s\n", argv[0]);
        exit(MY_MOD_ID + 11);
    }
    par.path.meta = qhlp.input.dl->path.meta;
    par.path.state = qhlp.input.dl->path.qdpstate;

    if ((qhlp.qdplus = qdplusCreateHandle(&par)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("qdplusCreateHandle");
        exit(MY_MOD_ID + 12);
    }

    LogMsg("QDP metadata directory = %s", par.path.meta);
    LogMsg("HLP state file = %s", par.path.state);
    switch (qhlp.qdplus->state) {
      case QDPLUS_STATE_OK:
      case QDPLUS_STATE_EMPTY:
        break;
      case QDPLUS_STATE_DIRTY:
        LogMsg("WARNING: HLP state file %s is dirty", par.path.state);
        break;
      default:
        LogMsg("ERROR: unable to load HLP state file %s (state=%d)", par.path.state, qhlp.qdplus->state);
        fprintf(stderr, "ERROR: unable to load HLP state file %s\n", par.path.state);
        exit(MY_MOD_ID + 13);
    }

/* Go into the background, if applicable */

    if (daemon && !BackGround(qhlp.output.dl)) {
        perror("BackGround");
        exit(MY_MOD_ID + 14);
    }

/* Initialize exit handler */

    InitExit(&qhlp);

/* Start signal handling thread */

    StartSignalHandler(&qhlp);

/* Note trigger data */

    LogTriggerData();

/* Recover state, if any */

    RecoverState(&qhlp);

/* All done, return handle to run time parameters */

    return &qhlp;

}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: init.c,v $
 * Revision 1.11  2009/05/14 18:12:44  dechavez
 * added debug verbosity toggling via /usr/nrts/etc/debug
 *
 * Revision 1.10  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.9  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.8  2008/12/17 02:24:39  dechavez
 * note state when aborting on bad qhlp state file
 *
 * Revision 1.7  2008/12/15 23:29:36  dechavez
 * support corrupt or empty state files
 *
 * Revision 1.6  2007/09/22 02:34:55  dechavez
 * use Q330 config file to get detector parameters
 *
 * Revision 1.5  2007/09/17 23:27:51  dechavez
 * checkpoint - reworking threshold trigger (now with specific list of triggered channels)
 *
 * Revision 1.4  2007/09/14 19:53:48  dechavez
 * added various dbg options, trigger stuff
 *
 * Revision 1.3  2007/09/07 20:29:42  dechavez
 * set runtime identity to "nrts" or that specified via user= argument
 *
 * Revision 1.2  2007/09/07 19:57:44  dechavez
 * Fixed missing state file bug.  Set NRTS disk loop "station system" flag.
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
