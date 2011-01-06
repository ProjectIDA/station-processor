#pragma ident "$Id: signals.c,v 1.11 2007/11/01 21:32:14 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
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
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_SIGNALS

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

        /* process signals */

        if (sig == SIGHUP) {
            util_log(2, "%s", util_sigtoa(sig));
#ifdef DEBUG_MSGQS
            ShowMessageQueues();
#endif
            if (OutputMediaType() != ISP_OUTPUT_TAPE) UpdateCdStats(FALSE);
        } else if (sig == SIGUSR1) {
            LogMsg(LOG_INFO, "%s", util_sigtoa(sig));
            LogMsgLevel(LOG_DEBUG);
        } else if (sig == SIGUSR2) {
            LogMsg(LOG_INFO, "%s", util_sigtoa(sig));
            LogMsgLevel(LOG_INFO);
            nrtsToggleCheckTstampLoggingFlag();
        } else if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
            LogMsg(LOG_INFO, "%s", util_sigtoa(sig));
            ispd_die(MY_MOD_ID + 0);
        } else {
            LogMsg(LOG_INFO, "%s", util_sigtoa(sig));
            LogMsg(LOG_INFO, "signal ignored");
        }
    }
}

void signals_init()
{
THREAD tid;
sigset_t set;
static char *fid = "signals_init";

/* Block all signals */

    sigfillset(&set);
    THREAD_SIGSETMASK(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, NULL)) {
        LogMsg(LOG_INFO, "%s: failed to create SignalHandlerThread", fid);
        ispd_die(MY_MOD_ID + 1);
    }
}

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.11  2007/11/01 21:32:14  dechavez
 * use POSIX sigwait
 *
 * Revision 1.10  2007/01/05 00:47:54  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.9  2006/04/07 17:03:02  dechavez
 * don't log iso stats update in response to SIGHUP (useless noise)
 *
 * Revision 1.8  2005/10/11 18:22:33  dechavez
 * UpdateCdStats for all but tape systems on SIGHUP
 *
 * Revision 1.7  2005/08/26 20:14:49  dechavez
 * include verbose argument to UpdateCdStats() call
 *
 * Revision 1.6  2005/07/06 15:41:52  dechavez
 * switch from util_log to LogMsg
 *
 * Revision 1.5  2005/02/08 17:58:56  dechavez
 * added nrtsToggleCheckTstampLoggingFlag() to SIGUSR2 handler
 *
 * Revision 1.4  2001/10/24 23:09:37  dec
 * removed ForcedRestart SIGINT response, added SIGHUP handler to update
 * CD stats if appropriate
 *
 * Revision 1.3  2001/10/08 18:56:09  dec
 * ForcedRestart SIGINT handler
 *
 * Revision 1.2  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
