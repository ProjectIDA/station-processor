#pragma ident "$Id: exitcode.c,v 1.9 2006/02/10 02:07:35 dechavez Exp $"
/*======================================================================
 *
 *  Exit handlers.
 *
 *====================================================================*/
#include "wrtdl.h"

static MUTEX mutex;
static struct nrts_sys *sys = NULL;

void LockDiskloop()
{
    MUTEX_LOCK(&mutex);
}

void UnlockDiskloop()
{
    MUTEX_UNLOCK(&mutex);
}

void SetSys(struct nrts_sys *sysptr)
{
    MUTEX_INIT(&mutex);
    sys = sysptr;
}

void die(int status)
{

    LockDiskloop();
#ifndef WIN32
    tty_close();

#endif
    util_log(1, "flushing wfdiscs");
    nrts_wrtbwd();
    if (sys != NULL) {
        util_log(1, "adjusting system pointers");
        nrts_fixsys(sys);
        sys->pid = 0;
    }

    util_log(1, "exit %d", status);
    exit(status);
}

/* Revision History
 *
 * $Log: exitcode.c,v $
 * Revision 1.9  2006/02/10 02:07:35  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.8  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.7  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.6  2004/09/28 23:27:30  dechavez
 * use disk loop lock/unlock guards to prevent corruption due to race
 * condition between main and wfdisc update threads
 *
 * Revision 1.5  2003/12/22 22:14:33  dechavez
 * greatly simplified
 *
 * Revision 1.4  2003/12/04 23:35:57  dechavez
 * removed // comments causing old Solaris compiler to complain
 *
 * Revision 1.3  2003/11/25 20:42:11  dechavez
 * ANSI function declarations
 *
 * Revision 1.2  2003/11/21 20:23:24  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
