#pragma ident "$Id: exit.c,v 1.3 2009/01/26 21:16:26 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "qhlp.h"

static QHLP_PAR *par = NULL;
static BOOL ShutdownFlag = FALSE;
static MUTEX mutex;

void SetShutdownFlag()
{
BOOL FlagAlreadySet;
static char *fid = "SetShutdownFlag";

    logioUpdateWatchdog(Watch, fid);

    MUTEX_LOCK(&mutex);
        FlagAlreadySet = ShutdownFlag;
        ShutdownFlag = TRUE;
    MUTEX_UNLOCK(&mutex);

    if (!FlagAlreadySet) LogMsg("shutdown initiated");
}

void QuitOnShutdown(INT32 status)
{
BOOL done;
static char *fid = "QuitOnShutdown";

    logioUpdateWatchdog(Watch, fid);

    MUTEX_LOCK(&mutex);
        done = ShutdownFlag;
    MUTEX_UNLOCK(&mutex);

    if (done) Exit(status);
}

void Exit(INT32 status)
{
static char *fid = "Exit";

    logioUpdateWatchdog(Watch, fid);

    MUTEX_LOCK(&mutex);

    if (par != NULL) {
        isidlCloseDiskLoop(par->output.dl);
        SaveState(par->qdplus);
    }

    LogMsg("exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

void InitExit(QHLP_PAR *ptr)
{
    par = ptr;
    MUTEX_INIT(&mutex);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.3  2009/01/26 21:16:26  dechavez
 * added watchdog tracers
 *
 * Revision 1.2  2007/05/15 20:43:02  dechavez
 * use QuitOnShutdown() instead of ShutdownInProgress() test
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
