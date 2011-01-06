#pragma ident "$Id: files.c,v 1.6 2005/11/03 23:59:05 dechavez Exp $"
/*======================================================================
 *
 * Determine full path names to all NRTS files for the given syscode.
 *
 * If home is non-NULL, then it is used as the top of the directory
 * tree.  If it is NULL, then the environment variable NRTS_HOME is
 * used as the home or /usr/nrts if the environment variable is not
 * defined.
 *
 * Returns a pointer to a nrts_files structure with the appropriate
 * fields filled in.  It makes no attempt to determine if the files
 * actually exist or not.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"
#ifdef WIN32

/* suppress certain scary warnings that are actually harmless */

#pragma warning( disable : 4273)

#endif /* WIN32 */

extern char *getenv();

struct nrts_files *nrts_files(home, syscode)
char **home;
char *syscode;
{
static struct nrts_files file;
static char *fid = "nrts_files";
static char *prefix;

    prefix = *home;

    if (prefix == NULL) {
        prefix = getenv(NRTS_HOME);
    }

    if (prefix == NULL) {
        prefix = NRTS_DEFHOME;
    }

    if (syscode != NULL) {
        sprintf(file.cnf, "%s/%s/cnf", prefix, syscode);
        sprintf(file.log, "%s/log/%s", prefix, syscode);
        sprintf(file.hdr, "%s/%s/hdr", prefix, syscode);
        sprintf(file.dat, "%s/%s/%s",  prefix, syscode, NRTS_DATNAME);
        sprintf(file.bwd, "%s/%s/bwd", prefix, syscode);
        sprintf(file.sys, "%s/%s/sys", prefix, syscode);
    }

    *home = prefix;

    return &file;
}

BOOL nrtsBuildFileNames(char *home, char *syscode, NRTS_DLNAMES *out)
{
char tmp[MAXPATHLEN+1];

    if (home == NULL || syscode == NULL || out == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    sprintf(tmp, "%s/%s/hdr", home, syscode);
    if ((out->hdr = strdup(tmp)) == NULL) return FALSE;
    sprintf(tmp, "%s/%s/dat", home, syscode);
    if ((out->dat = strdup(tmp)) == NULL) return FALSE;
    sprintf(tmp, "%s/%s/bwd", home, syscode);
    if ((out->bwd = strdup(tmp)) == NULL) return FALSE;
    sprintf(tmp, "%s/%s/sys", home, syscode);
    if ((out->sys = strdup(tmp)) == NULL) return FALSE;

    return TRUE;
}

VOID nrtsFreeFileNames(NRTS_DLNAMES *fname)
{
    if (fname == NULL) return;
    if (fname->hdr != NULL) free(fname->hdr);
    if (fname->dat != NULL) free(fname->dat);
    if (fname->bwd != NULL) free(fname->bwd);
    if (fname->sys != NULL) free(fname->sys);
}

/* Revision History
 *
 * $Log: files.c,v $
 * Revision 1.6  2005/11/03 23:59:05  dechavez
 * suppress microsoft 4273 compiler warnings
 *
 * Revision 1.5  2005/07/25 23:59:29  dechavez
 * removed nrtsFiles()
 *
 * Revision 1.4  2005/05/06 22:17:45  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.3  2004/06/11 17:08:08  dechavez
 * Use (new) home dir field of handle to locate disk loop files
 *
 * Revision 1.2  2003/10/16 17:23:35  dechavez
 * added nrtsFiles(), nrtsBuildFileNames(), nrtsFreeFileNames()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
