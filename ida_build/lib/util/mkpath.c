#pragma ident "$Id: mkpath.c,v 1.4 2007/01/07 17:40:07 dechavez Exp $"
/*======================================================================
 *
 *  Create a directory, creating parents as required.
 *
 *====================================================================*/
#include "util.h"

#define MAXTOK 64

int util_mkpath(char *path, int mode)
{
int i, ntok, save;
char name[MAXPATHLEN+1], *token[MAXTOK], cwd[MAXPATHLEN+1];
struct stat statbuf;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }

#ifdef SUNOS
    getwd(cwd);
#else
    getcwd(cwd, MAXPATHLEN+1);
#endif

/*  Start from root if this is absolute path, else from current directory  */

    if (path[0] == '/') chdir("/");

/*  Execute a mkdir on all components of the directory path.          */
/*  If any part already exists, we will get an error which we ignore. */

    strlcpy(name, path, MAXPATHLEN+1);
    if ((ntok = util_sparse(name, token, "/", MAXTOK)) < 1) {
        save = errno;
        chdir(cwd);
        errno = save;
        return -1;
    }

    for (i = 0; i < ntok; i++) {
        MKDIR(token[i], mode);
        chdir(token[i]);
    }
    chdir(cwd);

/*  Stat it to see if it worked  */

    return stat(name, &statbuf);

}

/* Revision History
 *
 * $Log: mkpath.c,v $
 * Revision 1.4  2007/01/07 17:40:07  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.3  2005/09/30 16:50:36  dechavez
 * 09-30-2005 aap win32 port
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
