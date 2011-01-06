#pragma ident "$Id: signals.c,v 1.4 2007/11/01 21:39:11 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2001 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "sanlog.h"

#define MY_MOD_ID SANLOG_MOD_SIGNALS

static void *SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    util_log(2, "Signal handler started, tid = %d", THREAD_SELF());

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);
        util_log(1, "%s", util_sigtoa(sig));

        /* process signals */

        switch (sig) {
          case SIGHUP:
            FlushFiles();
            break;

          case SIGUSR1:
            util_incrloglevel();
            break;

          case SIGUSR2:
            util_rsetloglevel();
            break;

          case SIGTERM:
          case SIGQUIT:
          case SIGINT:
            GracefulExit(0);
            break;

          default:
            util_log(1, "signal ignored");
        }
    }
}

BOOL InitSignalHandler()
{
THREAD tid;
sigset_t set;
static char *fid = "InitSignalHandler";

/* Block all signals */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, NULL)) {
        util_log(1, "%s: failed to create SignalHandlerThread", fid);
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.4  2007/11/01 21:39:11  dechavez
 * use POSIX sigwait()
 *
 * Revision 1.3  2007/01/05 00:47:54  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.2  2003/06/10 00:39:08  dechavez
 * replace THREAD_SIGSETMASK macro with explicit pthread_sigmask()
 *
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
