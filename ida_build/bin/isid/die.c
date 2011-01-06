#pragma ident "$Id: die.c,v 1.4 2007/01/11 17:59:04 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "isid.h"

static MUTEX mutex;

VOID BlockOnShutdown(VOID)
{
    MUTEX_LOCK(&mutex);

/* if the system is going down, we'll never get here */

    MUTEX_UNLOCK(&mutex);
}

BOOL ShutdownInProgress(VOID)
{
    if (MUTEX_TRYLOCK(&mutex)) {
        MUTEX_UNLOCK(&mutex);
        return FALSE;
    } else {
        return TRUE;
    }
}

VOID GracefulExit(INT32 status)
{
int nclients;
static char *fid  = "GracefulExit";

    MUTEX_LOCK(&mutex);
    DisableNewConnections();

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - ISID_MOD_SIGNALS);
    }
    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

VOID InitGracefulExit()
{
    MUTEX_INIT(&mutex);
}

/* Revision History
 *
 * $Log: die.c,v $
 * Revision 1.4  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.3  2004/04/26 21:19:09  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.2  2003/11/04 20:14:08  dechavez
 * replace sleep for hardcoded seconds with a utilDelayMsec of ISI_SHUTDOWN_DELAY
 * msecs
 *
 * Revision 1.1  2003/10/16 18:07:23  dechavez
 * initial release
 *
 */
