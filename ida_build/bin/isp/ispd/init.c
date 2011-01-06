#pragma ident "$Id: init.c,v 1.20 2008/08/21 21:27:50 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_INIT

#define MAXTOKEN 64

extern char *Home;
extern char *Syscode;

extern ISP_PARAMS *Params;
extern ISP_SERVER *Server;
extern struct isp_status *Status;
extern struct isp_dascnf *DasCnf;

extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;

#ifdef USE_DEBUG_DEFAULTS
static char DEBUG_STA[] = "2012";
static char DEBUG_LOG[] = "-";
#endif

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ini=path log=name -debug -bd] system\n", myname);
    exit(1);
}

void init(int argc, char **argv)
{
FILE *fp;
int daemon = FALSE;
int i, ntoken, status, problems;
BOOL debug;
THREAD tid;
char pwd[MAXPATHLEN+1], RunFile[MAXPATHLEN+1];
char *token[MAXTOKEN];
char *myname, *sta, *log, *user, *prefix, *dbspec = NULL;
struct stat statbuf;
struct isp_dascnf dascnf;
LOGIO *lp = NULL;
static char *fid = "init";

    memset(DasCnf, 0, sizeof(struct isp_dascnf));
    MUTEX_INIT(&DasCnf->mutex);

/* Get my name without path prefix (if any) */

    if ((myname = strdup(argv[0])) == NULL) {
        perror(argv[0]);
        exit(1);
    }

    ntoken = util_sparse(myname, token, "./", MAXTOKEN);
    myname = token[ntoken-1];

/* Parse command line for required input and selected overrides */

    sta   = (char *) NULL;
    user  = ISPD_DEF_USER;
    log   = NULL;
    debug = ISPD_DEF_DEBUG;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta = argv[i] + strlen("sta=");
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncasecmp(argv[i], "user=", strlen("user=")) == 0) {
            user = argv[i] + strlen("user=");
        } else if (strcasecmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcasecmp(argv[i], "-bd") == 0) {
            daemon = TRUE;
        } else if (sta == (char *) NULL) {
            sta = argv[i];
        } else {
            help(myname);
        }
    }

    if (sta == NULL) {
        fprintf(stderr, "%s: system name must be specified\n", myname);
        help(myname);
    }

    if (log == NULL) log = daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;

    if (!isidlSetGlobalParameters(dbspec, argv[0], &Params->glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    sprintf(pwd, "%s/%s/%s", Params->glob.root, sta, ISP_SUBDIR);
    if (chdir(pwd) != 0) {
        fprintf(stderr, "%s: chdir: ", myname);
        perror(pwd);
        exit (1);
    }

    Home    = strdup(Params->glob.root);
    Syscode = strdup(sta);
    prefix  = strdup(sta);

    if (Home == (char *) NULL || Syscode == (char *) NULL || prefix == NULL) {
        perror(myname);
        exit(1);
    }
    Syscode = util_lcase(Syscode);

/* Make sure we can find the various set up files now */

    problems = 0;

    if ((fp = fopen(ISP_RUN_FILE, "r")) == (FILE *) NULL) {
        fprintf(stderr, "%s: fopen: %s/", myname, pwd);
        perror(ISP_RUN_FILE);
        ++problems;
    } else {
        fclose(fp);
    }

    if (problems) exit(1);

/* Load run parameters */

    sprintf(RunFile, "%s/%s", pwd, ISP_RUN_FILE);
    if (!ispLoadRunParam(RunFile, sta, Params, Server)) {
        fprintf(stderr, "%s: problems with parameter file\n", myname);
        exit(1);
    }

/* By insuring that we can stat the output device now, we will later
 * be able to treat open failures as no-media-installed conditions.
 */

    SetOutputMediaType();
    if (OutputMediaType() == ISP_OUTPUT_TAPE && stat(Params->odev, &statbuf) != 0) {
        fprintf(stderr, "%s: can't stat: ", myname);
        perror(Params->odev);
        exit(1);
    }


/* Check for digitizer specific support files */

    if (Params->digitizer == ISP_DAS) {
        if (isp_getcnf(&dascnf) != 0) {
            fprintf(stderr, "%s: fopen: %s/", myname, pwd);
            perror(ISP_CNF_FILE);
            fprintf(stderr, "can't load DAS configuration... set default\n");
            dascnf.flag = ISP_DASCNF_DEF;
            if (isp_setcnf(&dascnf) != 0 || isp_getcnf(&dascnf) != 0) {
                perror("can't load default DAS configuration either!\n");
                ++problems;
            }
        }
    }

    if (problems) exit(1);

/* Set user and group identity */

    utilSetIdentity(user);

/* Go into the background */

    if (daemon && !utilBackGround()) {
        perror("utilBackGround");
        exit(1);
    }

/* Start logging facility */

    util_ucase(prefix);
    if ((lp = InitLogging(myname, log, prefix, debug)) == NULL) {
        perror("InitLogging");
        exit(1);
    }
    ispSetLogParameter(Params, lp);

    LogMsg(LOG_DEBUG, "uid=%d, euid=%d, gid=%d, egid=%d", getuid(), geteuid(), getgid(), getegid());

    if (Params->rt593.present) {
        LogMsg(LOG_INFO, "Configured for RT593 with %d tic delay", Params->rt593.correct);
    }

/* Initialize the message queues and massio buffers */

    InitMsgqs();

/* Initialize the status structure */

    status_init();

/* Start signal handling thread */

    signals_init();

/* Initialize the output device */

    InitMediaOps();

/* Start the ISI thread */

    InitIsi(lp);
    sleep(1);

/* Start the digitizer specific I/O thread */

    if (Params->digitizer == ISP_DAS) {
        InitDasIO();
    } else {
        InitSanIO();
    }
    sleep(1);

/* Start the DAS packet processor thread */

    InitProcess();
    sleep(1);

/* Start the massio buffering thread */

    InitMassio();
    sleep(1);

/* Start the DAS status request thread */

    InitSoh();
    sleep(1);

/* Start the digitizer configuration verification process */

    if (Params->digitizer == ISP_DAS) {
        VerifyDasConfig(0);
    } else {
        VerifySanConfig(0);
    }

/* Start up auxiliary threads, if any */

    if (Params->clock.enabled) {
        set_alarm(ISP_ALARM_AUXCLK);
        InitClock();
    }

    if (Params->baro.enabled) {
        set_alarm(ISP_ALARM_AUX);
        InitBaro();
    }

    if (Params->dpm.enabled) {
        set_alarm(ISP_ALARM_AUX);
        InitDPM();
    }

/* Start up ISP injection service */

    if (Params->inject) isp_inject();

/* Command and control server */

    server_init();

/* iboot watchdog */

    StartIbootWatchdog();

    return;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.20  2008/08/21 21:27:50  dechavez
 * added StartIbootWatchdog
 *
 * Revision 1.19  2007/01/25 20:28:23  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.18  2007/01/11 22:02:30  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.17  2006/03/13 23:18:36  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.16  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.15  2005/09/10 22:48:54  dechavez
 * removed suprious debug output
 *
 * Revision 1.14  2005/09/10 22:01:22  dechavez
 * set log according to daemon state, default to foreground and use -bd for
 * background, to be consistent with other major ISI apps
 *
 * Revision 1.13  2005/08/26 20:16:55  dechavez
 * replaced InitNrts with InitIsi and removed tee subsystem
 *
 * Revision 1.12  2005/07/26 18:38:11  dechavez
 * prefix logio log messages with site tag
 *
 * Revision 1.11  2005/07/26 00:58:14  dechavez
 * initial ISI dl support (2.4.6d)
 *
 * Revision 1.10  2005/07/06 15:45:10  dechavez
 * use utilSetIdentity instead of explicit system calls, switched to logio
 * for logging
 *
 * Revision 1.9  2005/04/04 20:21:15  dechavez
 * InitTeeSubsystem()
 *
 * Revision 1.8  2002/09/09 21:59:21  dec
 * dpm support
 *
 * Revision 1.7  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.6  2002/02/27 00:28:07  dec
 * user "user" parameter to determine uid and gid and set explicitly
 * (this works around system() function not working for setuid programs)
 *
 * Revision 1.5  2001/10/24 23:18:33  dec
 * added debug support
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
