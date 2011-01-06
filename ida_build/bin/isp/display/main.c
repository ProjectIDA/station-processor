#pragma ident "$Id: main.c,v 1.8 2006/02/10 02:23:42 dechavez Exp $"
/*======================================================================
 *
 * Operator interface to ISPD
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "isp_console.h"

int console   = 0;
int port      = ISP_DEFAULT_COMMAND_PORT;
int localhost = LOCALHOST;
char *server  = (char *) NULL;
char *home    = (char *) NULL;
char *Syscode = (char *) NULL;

static void help(char *myname)
{
    printf("\n");
    printf("usage: %s [home=NRTS_HOME] system\n", myname);
    printf("\n");
    exit(0);
}

void EndWin()
{
int status;

    util_log(1, "calling endwin()");
    status = endwin();
    util_log(1, "endwin() status = %d", status);
}

int main(int argc, char **argv)
{
int i;
int timeout  = 10;
int si       =  1;
int debug    =  0;
THREAD tid;
char *pwd, *sta = NULL;
char RunFile[MAXPATHLEN+1], *log = (char *) NULL;
static char LogFile[MAXPATHLEN+1];
static char *default_server = "localhost";
ISP_SERVER ispd;
time_t idle = 600;

    server = default_server;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncasecmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta = argv[i] + strlen("sta=");
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncasecmp(argv[i], "idle=", strlen("idle=")) == 0) {
            idle = atoi(argv[i] + strlen("idle="));
        } else if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "help") == 0) {
            help(argv[0]);
        } else if (sta == (char *) NULL) {
            sta = argv[i];
        } else {
            help(argv[0]);
        }
    }

    SetMaxIdle(idle);

    if ((pwd = isp_setup(&home, &sta)) == (char *) NULL) exit(1);
    if ((Syscode = strdup(sta)) == NULL) {
        perror(argv[0]);
        exit(1);
    }
    util_lcase(Syscode);
    sprintf(RunFile, "%s/%s", pwd, ISP_RUN_FILE);
    if (!ispLoadRunParam(RunFile, sta, NULL, &ispd)) {
        fprintf(stderr, "%s: problems with parameter file\n", argv[0]);
        exit(1);
    }

    if (debug) util_logopen("syslogd", 1, 9, debug, "CONSOLE", argv[0]);

    console = (getppid() == 1);
    if (console) util_log(1, "Running as BOOT CONSOLE");

/* Start signal handling thread */

    InitSignals();

/* Allocate space for channel map */

    InitChanMap();

/* Initialize the display */

    sprintf(LogFile, "%s/log/nrtslog", home);
    if (OpenDisplay(server, ispd.port, LogFile) != 0) {
        fprintf(stderr, "%s: ABORT: Can't open display\n", argv[0]);
        sleep(1);
        Quit(1);
    }

/* Connect to the server */

    ServerConnect(server, ispd.port, ispd.to, TRUE);

/* Start status request thread */

    util_log(1, "Start routine status requests");
    if (!THREAD_CREATE(&tid, StatusRequest, NULL)) {
        EndWin();
        fprintf(stderr, "failed to create StatusRequest thread\n");
        exit(1);
    }

/* Start interactive thread */

    util_log(1, "Start interactive display");
    FirstDisplay();
    while (1) {
        switch (Digitizer()) {
          case ISP_DAS:
            util_log(1, "ISP_DAS detected, start DasDisplay");
            if (!THREAD_CREATE(&tid, DasDisplay, NULL)) {
                EndWin();
                fprintf(stderr, "failed to create DasDisplay thread\n");
                exit(1);
            }
            THREAD_EXIT(0);
          case ISP_SAN:
            util_log(1, "ISP_San detected, start SanDisplay");
            if (!THREAD_CREATE(&tid, SanDisplay, NULL)) {
                EndWin();
                fprintf(stderr, "failed to create SanDisplay thread\n");
                exit(1);
            }
            THREAD_EXIT(0);
          default:
            sleep(1);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.8  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.7  2003/10/02 20:24:51  dechavez
 * 2.3.1 mods (fixed console hang on exit, improved dumb terminal refresh
 * a bit, added idle timeout)
 *
 * Revision 1.6  2002/03/15 22:56:45  dec
 * force localhost only connections, require explicit sys name or environment
 * setting, and use sys's run file to look up ispd port number
 *
 * Revision 1.5  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.4  2001/01/12 23:31:29  dec
 * fixed bug in abort message
 *
 * Revision 1.3  2000/11/02 20:30:43  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
