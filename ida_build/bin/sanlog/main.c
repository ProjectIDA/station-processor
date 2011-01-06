#pragma ident "$Id: main.c,v 1.2 2001/09/18 23:24:51 dec Exp $"
/*======================================================================
 *
 *  Archive log messages from several SAN digitizers
 *
 *====================================================================*/
#include "sanlog.h"

#define VERSION       "1.0.1"
#define DEFAULT_LOG   "-"
#define DEFAULT_DEBUG 1
#define MY_MOD_ID SANLOG_MOD_MAIN

SANLOG_LIST *List;

static void help(char *myname)
{
    fprintf(stderr, "usage: "
        "%s pf=SanListFile [-bd to=Timeout log=path debug=level]\n"
    );
    exit(1);
}

main(int argc, char **argv)
{
int i;
static SANLOG_LIST list;
static char *log = DEFAULT_LOG;
int to = SANLOG_MIN_TIMEOUT;
int debug = DEFAULT_DEBUG;
BOOL daemon = FALSE;
char *pf = NULL;

    List = &list;

/* Read command line */

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "pf=", strlen("pf=")) == 0) {
            pf = argv[i] + strlen("pf=");
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strcasecmp, argv[i], "-bd") {
            daemon = TRUE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

/* Jump into the backround if running as a daemon */

    if (daemon && util_bground(0, 0) < 0) {
        fprintf(stderr, "%s: failed to backround: %s\n", argv[0], strerror(errno));
        exit(1);
    }

/* Start logging */

    util_logopen(log, 1, 9, debug, NULL, argv[0]);
    util_log(1, "Version %s", VERSION);

/* Read the list of SANs to monitor */

    if (pf == NULL) {
        util_log(1, "missing SanListFile");
        GracefulExit(MY_MOD_ID + 1);
    } else if (!ReadSanList(pf, &list)) {
        util_log(1, "error reading/processing %s", pf);
        GracefulExit(MY_MOD_ID + 2);
    }

/* Start the signal handling thread */

    if (!InitSignalHandler()) {
        util_log(1, "failed to start signal handler");
        GracefulExit(MY_MOD_ID + 3);
    }

/* Force timeout to be reasonable */

    if (to < SANLOG_MIN_TIMEOUT) {
        util_log(1, "bad timeout (%d), forced to %d", to, SANLOG_MIN_TIMEOUT);
        to = SANLOG_MIN_TIMEOUT;
    }

/* Start up the individual I/O threads */

    util_log(1, "Launching %d SAN connections", list.count);
    for (i = 0; i < list.count; i++) StartSanioThread(&list.san[i], to);

/* Done, wait for termination signal */

    util_log(1, "Initialization complete");
    while (1) pause();
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2001/09/18 23:24:51  dec
 * release 1.1.2
 *
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
