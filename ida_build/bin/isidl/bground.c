#pragma ident "$Id: bground.c,v 1.2 2006/06/21 18:30:57 dechavez Exp $"
/*======================================================================
 *
 *  Go into the background, preserving disk loop locks
 *
 *====================================================================*/
#include "isidl.h"

#ifdef unix

BOOL BackGround(ISIDL_PAR *par)
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

/* Parent updates all the disk loops with the new process id */

    if (child > 0) {
        for (i = 0; i < par->nsite; i++) isidlUpdateParentID(par->dl[i], child);    
        exit(0);
    }

/* Child stays behind */

    return TRUE;
}

#else

BOOL BackGround(ISIDL_PAR *par)
{
    return TRUE;
}

#endif /* unix */

/* Revision History
 *
 * $Log: bground.c,v $
 * Revision 1.2  2006/06/21 18:30:57  dechavez
 * fixed parameters for dummy win32 entry (aap)
 *
 * Revision 1.1  2006/04/20 23:02:02  dechavez
 * initial release
 *
 */
