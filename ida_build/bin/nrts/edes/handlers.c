#pragma ident "$Id: handlers.c,v 1.5 2003/11/21 20:23:50 dechavez Exp $"
/*======================================================================
 *
 * signal handlers
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <signal.h>
#include <sys/param.h>
#include <unistd.h>
#include "edes.h"

static int Sd = -1;

static void catch_signal(int sig)
{
int pid;
char pname[MAXPATHLEN+1];
static char *fid = "catch_signal";

    if (sig == SIGPIPE) {
        util_log(1, "%s", util_sigtoa(sig));
        xfer_errno = XFER_EPIPE;
    } else if (sig == SIGTERM) {
        util_log(1, "%s", util_sigtoa(sig));
        Xfer_Exit(Sd, 0);
    } else if (sig == SIGHUP) {
        util_log(2, "%s", util_sigtoa(sig));
        logWhoWhat();
        signal(SIGHUP, catch_signal);
    } else {
        util_log(1, "abort on unexpected %s (pwd = %s)",
            util_sigtoa(sig), getwd(pname)
        );
        abort();
    }
}

int handlers(int sd)
{

    Sd = sd;

    if (signal(SIGTERM, catch_signal) == SIG_ERR) {
        xfer_errno = XFER_EHANDLER;
        return -1;
    }
    util_log(3, "SIGTERM handler installed");

    if (signal(SIGHUP, catch_signal) == SIG_ERR) {
        xfer_errno = XFER_EHANDLER;
        return -1;
    }
    util_log(3, "SIGHUP handler installed");

    if (signal(SIGPIPE, catch_signal) == SIG_ERR) {
        xfer_errno = XFER_EHANDLER;
        return -1;
    }
    util_log(3, "SIGPIPE handler installed");

    return 0;
}

/* Revision History
 *
 * $Log: handlers.c,v $
 * Revision 1.5  2003/11/21 20:23:50  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.4  2002/04/26 00:29:36  nobody
 * log SIGHUPs at level 2
 *
 * Revision 1.3  2002/02/23 00:03:40  dec
 * added SIGPIPE handler
 *
 * Revision 1.2  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
