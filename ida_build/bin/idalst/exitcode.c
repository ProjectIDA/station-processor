#pragma ident "$Id: exitcode.c,v 1.5 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  control-C abort handler
 *
 *====================================================================*/
#include "idalst.h"

void catch_signal(int sig)
{
    signal(sig, SIG_IGN);
    fprintf(stderr,"%s caught... jumping to summary.\n", util_sigtoa(sig));
    summary(USER_ABORT, Global->count, Global->first, Global->last, 
        Global->print, Global->check
    );
}

void exitcode(void)
{
    if (
        signal(SIGINT,  catch_signal) == SIG_ERR ||
        signal(SIGTERM, catch_signal) == SIG_ERR ||
        signal(SIGINT,  catch_signal) == SIG_ERR ||
        signal(SIGBUS,  catch_signal) == SIG_ERR ||
        signal(SIGFPE,  catch_signal) == SIG_ERR ||
        signal(SIGILL,  catch_signal) == SIG_ERR ||
        signal(SIGSEGV, catch_signal) == SIG_ERR
    ) {
        perror("idalst: signal");
        exit(1);
    }
}

/* Revision History
 *
 * $Log: exitcode.c,v $
 * Revision 1.5  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.4  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.3  2003/11/21 20:28:30  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.2  2003/06/11 20:26:20  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
