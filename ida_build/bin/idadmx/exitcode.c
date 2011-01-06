#pragma ident "$Id: exitcode.c,v 1.5 2006/02/09 20:14:38 dechavez Exp $"
/*======================================================================
 *
 *  Exit handlers.
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;

static void catch_signal(int sig)
{
    sprintf(Buffer, "going down on signal %d", sig);
    logmsg(Buffer);
    die(ON_SIGNAL);
}

int exitcode(void)
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
        perror("idadmx: signal");
        die(INIT_ERROR);
    }
#endif

#ifdef SIGINT
    if (signal(SIGINT, catch_signal) == SIG_ERR) {
        perror("idadmx: signal");
        die(INIT_ERROR);
    }
#endif
    return 0;
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
 * Revision 1.5  2006/02/09 20:14:38  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.4  2005/09/30 16:48:07  dechavez
 * 09-30-2005 aap win32 mods
 *
 * Revision 1.3  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.2  2003/11/21 20:12:04  dechavez
 * remove Sigfunc casts
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
