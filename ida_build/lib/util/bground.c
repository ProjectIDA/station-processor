#pragma ident "$Id: bground.c,v 1.6 2003/11/26 18:07:56 dechavez Exp $"
/*======================================================================
 *
 *  Initialize a daemon process.
 *
 *  Modified from: W. Richard Stevens, 1990, Unix Network Programming,
 *                 Prentice Hall, pp 82-85.
 *
 *====================================================================*/
#include "util.h"

#ifdef unix

BOOL utilBackGround(VOID)
{
int null_fd;
register int childpid, fd;

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

    if ((null_fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(null_fd, fileno(stdin));
        dup2(null_fd, fileno(stdout));
        dup2(null_fd, fileno(stderr));
        close(null_fd);
    }

#ifdef BSD
    if ((fd = open("/dev/tty", O_RDWR)) != -1) {
        ioctl(fd, TIOCNOTTY, 0);
        close(fd);
    }
#endif

/* Fork and let the parent exit */

    if ((childpid = fork()) < 0) return FALSE;
    if (childpid > 0) exit(0);

    return TRUE;
}

int util_bground(int ignsigcld, int closefd)
{
    return utilBackGround() ? getpid() : -1;
}

#else

BOOL utilBackGround(VOID)
{
    errno = ENOSYS;
    return FALSE;
}

#endif /* unix */

/* Revision History
 *
 * $Log: bground.c,v $
 * Revision 1.6  2003/11/26 18:07:56  dechavez
 * do not close all open file descriptors
 *
 * Revision 1.5  2003/11/13 19:41:05  dechavez
 * recast util_bground() into call to utilBground()
 *
 * Revision 1.4  2003/10/16 15:57:15  dechavez
 * utilBackGround() to return an ENOSYS failure for non-unix builds
 *
 * Revision 1.3  2003/06/10 00:04:21  dechavez
 * added utilBackGround()
 *
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:40  dec
 * import existing IDA/NRTS sources
 *
 */
