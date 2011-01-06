/*======================================================================
 *
 *  Go into the background, preserving disk loop locks
 *
 *====================================================================*/
#include "qhlp.h"

#ifdef unix

BOOL BackGround(ISI_DL *dl)
{
int i, devnull, child;

/* Ignore the terminal stop signals */

#ifdef SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif

/* Disassociate from controlling terminal */

    if ((devnull = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(devnull, fileno(stdin));
        dup2(devnull, fileno(stdout));
        dup2(devnull, fileno(stderr));
        close(devnull);
    }

/* Fork, parent gets the child's process id */

    if ((child = fork()) < 0) return FALSE;

/* Parent updates all the output disk loop with the new process id */

    if (child > 0) {
        isidlUpdateParentID(dl, child);    
        exit(0);
    }

/* Child stays behind */

    return TRUE;
}

#else

BOOL BackGround(ISI_DL *dl)
{
    return TRUE;
}

#endif /* unix */

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
 * $Log: bground.c,v $
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
