#pragma ident "$Id: signals.c,v 1.7 2007/01/05 00:47:54 dechavez Exp $"
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
#include "isp_console.h"

static BOOL SuccessFlag = FALSE;

BOOL SuccessfulBurn()
{
    return SuccessFlag;
}

void ClearBurnFlag()
{
    SuccessFlag = FALSE;
}

static void *SignalHandler(void *dummy)
{
sigset_t set;
int sig;

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

#if defined (BSD) || defined (LINUX)
        sigwait(&set, &sig);
#else
        sig = sigwait(&set);
#endif

        /* process signals */

        if (sig == SIGTERM || sig == SIGQUIT || sig == SIGINT) {
            util_log(1, "%s", util_sigtoa(sig));
            Quit(0);
        } else if (sig == SIGHUP) {
            SuccessFlag = TRUE;
        } else if (sig == SIGSEGV) {
            util_log(1, "%s", util_sigtoa(sig));
            util_log(1, "abort()");
            abort();
        } else {
            ;
        }
    }
}

void InitSignals()
{
THREAD tid;
sigset_t set;
static char *fid = "InitSignals";

/* Block all signals */

    sigfillset(&set);
    THREAD_SIGSETMASK(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandler, NULL)) {
        fprintf(stderr, "%s: failed to create SignalHandler", fid);
        exit(1);
    }
}

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.7  2007/01/05 00:47:54  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.6  2003/12/10 06:25:41  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.5  2003/03/13 23:21:00  dechavez
 * Linux portability ifdefs
 *
 * Revision 1.4  2001/10/26 23:39:12  dec
 * added SIGHUP handler to set successful burn flag, and functions to return
 * its value and to clear the flag
 *
 * Revision 1.3  2001/05/20 17:44:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
