#pragma ident "$Id: exit.c,v 1.9 2007/01/11 22:02:21 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "isidl.h"

static MUTEX mutex;
static ISIDL_PAR *par = NULL;

void BlockShutdown(char *fid)
{
    MUTEX_LOCK(&mutex);
}

void UnblockShutdown(char *fid)
{
    MUTEX_UNLOCK(&mutex);
}

void Exit(INT32 status)
{
int i;
static char *fid = "Exit";

    BlockShutdown(fid);

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - ISIDL_MOD_SIGNALS);
    }

    if (par != NULL) {
        for (i = 0; i < par->nsite; i++) isidlCloseDiskLoop(par->dl[i]);
    }

    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

VOID InitExit(ISIDL_PAR *ptr)
{
    MUTEX_INIT(&mutex);
    par = ptr;
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.9  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.8  2006/06/23 18:31:05  dechavez
 * removed Q330 deregistration
 *
 * Revision 1.7  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.6  2006/06/07 22:41:17  dechavez
 * Deregister all Q330 connections on shutdown
 *
 * Revision 1.5  2006/03/14 20:31:48  dechavez
 * pass fid to block/unlock exit mutex calls, for debugging use
 *
 * Revision 1.4  2006/03/13 23:07:13  dechavez
 * added BlockShutdown(), UnblockShutdown()
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
