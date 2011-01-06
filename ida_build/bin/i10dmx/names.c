#pragma ident "$Id: names.c,v 1.4 2005/05/25 23:57:23 dechavez Exp $"
/*======================================================================
 *
 *  Functions to store/generate misc names.
 *
 *====================================================================*/
#include "i10dmx.h"
#include "util.h"

#define MAXTOK 64
#define PERMISSION 0775

extern char *Buffer;

static char *staname = NULL;
static char base[MAXPATHLEN+2];
static char name[MAXPATHLEN+2];
static char dname[] = "data";
static char fname[] = "xxxxxx-xxxxxx.w";

void nameinit(char *outdir, char *sta)
{
int i, ntok;
char *token[MAXTOK];
char rootdir[3];
#ifndef WIN32
char *cwd;
#endif /* !WIN32 */

    sprintf(rootdir,"%c", PATH_DELIMITER);

/* Make local copy of station name, if provided */

    if (sta != NULL) {
         if ((staname = strdup(sta)) == NULL) {
            perror("strdup");
            exit(1);
        }
    } else {
        staname = NULL;
    }

/*  Get absolute path name of base directory  */
#ifdef WIN32
    {
    LPTSTR lpFilePart;
    char cFullPath[MAX_PATH];
    char rootDrive[4];
    GetFullPathName(outdir,
    sizeof(cFullPath),
    cFullPath,
    &lpFilePart
    );
    strcpy(base, cFullPath);
    _splitpath( cFullPath, rootDrive, NULL, NULL, NULL );
    i= _chdrive(rootDrive[0]-'A'+1);
    strcat(rootDrive, "\\");
    }
#else
    if (outdir[0] != PATH_DELIMITER) {
        if ((cwd = getcwd(NULL, MAXPATHLEN+2)) == NULL) {
            perror("getcwd");
            exit(1);
        }
        sprintf(base, "%s/%s", cwd, outdir);
    } else {
        strcpy(base, outdir);
    }
#endif
/*  Make sure base directory does not already exist  */

    if (chdir(base) == 0) {
        fprintf(stderr,"%s already exists.\n", outdir);
        exit(1);
    }

/*  Make the base directory (tree if we have to)  */

    chdir(rootdir);
    strcpy(name, base);
    if ((ntok = util_sparse(name, token, rootdir, MAXTOK)) < 1) {
        fprintf(stderr, "Can't parse outdir '%s' string.\n", base);
        exit(1);
    }
 
    for (i = 0; i < ntok; i++) {
        MKDIR(token[i], PERMISSION);
        chdir(token[i]);
    }


/*  Make base directory the current directory for the remainder of the run  */

    if (chdir(base) != 0) {
        fprintf(stderr, "chdir: ");
        perror(base);
        exit(1);
    }

    MKDIR(dname, PERMISSION);

}

char *pathname(char *defsname, char *channel)
{
char *station;

    station = (staname == NULL) ? defsname : staname;

/*  Make the file name and combine to get absolute path name  */

    sprintf(fname, "%s-%s.w", station, channel);
    sprintf(name, "%s%c%s%c%s", base, PATH_DELIMITER, dname, PATH_DELIMITER, fname);

    return name;
}

char *logname(char *defsname)
{
static char lname[MAXPATHLEN+2];
char *station;

    station = (staname == NULL) ? defsname : staname;
    sprintf(lname, "%s%c%s%c%s.log", base, PATH_DELIMITER, dname, PATH_DELIMITER, station);

    return lname;
}

char *ttagname()
{
static char ttagname[MAXPATHLEN+2];

    sprintf(ttagname, "%s%cttags", base, PATH_DELIMITER);
    return ttagname;
}

char *crnt_dname()
{
    return dname;
}

char *crnt_fname()
{
    return fname;
}

char *basedir()
{
    return base;
}

char *sname(char *defsname)
{
    return (staname == NULL) ? defsname : staname;
}

/* Revision History
 *
 * $Log: names.c,v $
 * Revision 1.4  2005/05/25 23:57:23  dechavez
 * reconcile Win32/Unix builds
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
