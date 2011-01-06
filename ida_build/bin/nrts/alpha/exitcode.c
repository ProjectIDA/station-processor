#pragma ident "$Id: exitcode.c,v 1.2 2003/11/21 20:24:25 dechavez Exp $"
/*======================================================================
 *
 *  Exit handlers.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "util.h"
#include "nrts_alpha.h"

static void catch_signal(sig)
int sig;
{

    if (sig == SIGHUP || sig == SIGUSR1 || sig == SIGUSR2) {
        util_log(1, "%s", util_sigtoa(sig));
        util_log(1, "signal ignored");
        return;
    } else if (sig == SIGBUS || sig == SIGSEGV) {
        util_log(1, "aborting on %s", util_sigtoa(sig));
        abort();
    } else {
        util_log(1, "going down on %s", util_sigtoa(sig));
        die(0);
    }

}

int exitcode()
{

    if (signal(SIGTERM, catch_signal) == SIG_ERR) return -1;
    if (signal(SIGINT,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGBUS,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGSEGV, catch_signal) == SIG_ERR) return -1;
    if (signal(SIGPIPE, catch_signal) == SIG_ERR) return -1;
    if (signal(SIGFPE,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGILL,  catch_signal) == SIG_ERR) return -1;

    return 0;
}

void die(status)
int status;
{
    util_log(1, "exit %d", status);
    exit(status);
}

/* Revision History
 *
 * $Log: exitcode.c,v $
 * Revision 1.2  2003/11/21 20:24:25  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
