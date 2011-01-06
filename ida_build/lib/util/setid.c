#pragma ident "$Id: setid.c,v 1.1 2005/07/06 15:33:47 dechavez Exp $"
/*======================================================================
 *
 *  (attempt to) set user and group id
 *
 *  This lets us avoid having setuid programs.  We use the non-reentrant
 *  version of getpwnam.  This should not be a problem as since this is
 *  intended to be called during program initialization when things are
 *  still single threaded.
 *
 *====================================================================*/
#include "util.h"

BOOL utilSetIdentity(char *user)
{
#ifndef unix
    return TRUE;
#else
BOOL status;
struct passwd *passwd;

    if (user == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if ((passwd = getpwnam(user)) == NULL) return FALSE;
    status = TRUE;
    if (setgid(passwd->pw_gid) != 0) status = FALSE;
    if (setuid(passwd->pw_uid) != 0) status = FALSE;

    return status;

#endif
}

/* Revision History
 *
 * $Log: setid.c,v $
 * Revision 1.1  2005/07/06 15:33:47  dechavez
 * initial release
 *
 */
