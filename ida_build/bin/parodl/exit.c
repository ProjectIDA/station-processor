#pragma ident "$Id: exit.c,v 1.1 2009/03/24 21:11:40 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 *====================================================================*/
#include "parodl.h"

static MUTEX mutex;
static ISI_DL *dl = NULL;

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
static char *fid = "Exit";

    BlockShutdown(fid);

    if (status < 0) {
        status = -status;
        LogMsg(LOG_INFO, "going down on signal %ld", status - PARODL_MOD_SIGNALS);
    }

    if (dl != NULL) isidlCloseDiskLoop(dl);

    LogMsg(LOG_INFO, "exit %ld", status);

/* WIN32_SERVICEs and WIN32_DLLs don't want to exit() */

#if !defined(WIN32_SERVICE) && !defined(WIN32_DLL)
    exit((int) status);
#endif

}

VOID InitExit(ISI_DL *dlptr)
{
    MUTEX_INIT(&mutex);
    dl = dlptr;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * Revision 1.1  2009/03/24 21:11:40  dechavez
 * initial release
 *
 */
