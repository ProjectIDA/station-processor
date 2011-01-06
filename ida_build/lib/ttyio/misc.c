#pragma ident "$Id: misc.c,v 1.4 2009/03/17 18:21:45 dechavez Exp $"
/*======================================================================
 *
 *  Various functions common to both Unix and Win32 
 *
 *====================================================================*/
#include "ttyio.h"
#include "util.h"

BOOL ttyioStartReadThread(TTYIO *tp)
{
BOOL ok;

    MUTEX_LOCK(&tp->mutex);
        tp->pipe.active = THREAD_CREATE(&tp->pipe.tid, ttyioReadThread, (void *) tp);
        ok = tp->pipe.active;
    MUTEX_UNLOCK(&tp->mutex);
    if (!ok) {
        return FALSE;
    } else {
        THREAD_DETACH(tp->pipe.tid);
    }

#ifdef WIN32
    SetThreadPriority(&tp->pipe.tid, tp->pipe.priority);
#endif /* WIN32 */

    SEM_WAIT(&tp->pipe.semaphore);

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: misc.c,v $
 * Revision 1.4  2009/03/17 18:21:45  dechavez
 * THREAD_DETACH in ttyioStartReadThread
 *
 * Revision 1.3  2006/05/17 23:23:10  dechavez
 * added copyright notice
 *
 * Revision 1.2  2005/11/03 23:20:25  dechavez
 * fixed SetThreadPriority parameters
 *
 * Revision 1.1  2005/05/26 23:18:05  dechavez
 * initial release of new Unix/Windows compatible version
 *
 */
