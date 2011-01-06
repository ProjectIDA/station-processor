#pragma ident "$Id: exitcode.c,v 1.3 2005/05/13 19:46:14 dechavez Exp $"
/*======================================================================
 *
 *  Exit handlers.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "i10dmx.h"

extern char *Buffer;

static void catch_signal(int sig)
{
    sprintf(Buffer, "going down on signal %d", sig);
    logmsg(Buffer);
    die(ON_SIGNAL);
}

void exitcode()
{

#ifdef SIGHUP
    signal(SIGHUP,  SIG_IGN);
#endif

#ifdef SIGUSR1
    signal(SIGUSR1, SIG_IGN);
#endif

#ifdef SIGUSR2
    signal(SIGUSR2, SIG_IGN);
#endif

#ifdef SIGTERM
    if (signal(SIGTERM, catch_signal) == SIG_ERR) {
        perror("i10dmx: signal");
        die(INIT_ERROR);
    }
#endif

#ifdef SIGINT
    if (signal(SIGINT, catch_signal) == SIG_ERR) {
        perror("i10dmx: signal");
        die(INIT_ERROR);
    }
#endif
}

void die(int status)
{
static int dying = 0;

    if (status == INIT_ERROR) exit(status);

/*  Note we are here, in case subsequent steps result in die() calls  */

    if (dying) exit(status);
    dying = 1;

    if (status == ABORT) logmsg("*** ABORTED ***");

/*  Write the wfdisc file  */

    wrt_wfdisc();

/*  Close the error log file  */

    close_msglog();
    utilDestroyOutputStreamBuffers();
    exit(status);

}

/* Revision History
 *
 * $Log: exitcode.c,v $
 * Revision 1.3  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.2  2003/11/21 20:13:07  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
