#pragma ident "$Id: names.c,v 1.7 2006/06/27 00:06:40 dechavez Exp $"
/*======================================================================
 *
 *  Functions to store/generate misc names.
 *
 *====================================================================*/
#include "idadmx.h"
#include "util.h"

#define MAXTOK 64
#define PERMISSION 0775

extern char *Buffer;
extern IDA *ida;

static char *ident;
static char base[MAXPATHLEN+2];
static char name[MAXPATHLEN+2];
static char dname[] = "data";
static char fname[] = "sta-1ffccm.w";

void nameinit(char *outdir, char *id, int dump_head, FILE **hp)
{
char rootdir[3];
int i, ntok;
char *token[MAXTOK];
#ifdef WIN32
char cFullPath[MAX_PATH];
LPTSTR lpFilePart;
char rootDrive[4];
#else
char *cwd;
#endif

    sprintf(rootdir,"%c", PATH_DELIMITER);

/*  Make local copy of volume ident  */

    if (id != NULL) {
        if ((ident = strdup(id)) == NULL) {
            perror("strdup");
            exit(1);
        }
    } else {
        ident = NULL;
    }

/*  Get absolute path name of base directory  */

#ifdef WIN32
    GetFullPathName(outdir, sizeof(cFullPath), cFullPath, &lpFilePart);
    strcpy(base, cFullPath);
    _splitpath( cFullPath, rootDrive, NULL, NULL, NULL );
    i= _chdrive(rootDrive[0]-'A'+1);
    strcat(rootDrive, "\\");
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

    if (chdir(base) != 0) {
        perror(outdir);
        exit(1);
    }

/*  Open file for headers if needed  */

    if (dump_head && (*hp = fopen("headers", "wb")) == NULL) {
        fprintf(stderr, "Can't open %s/", base);
        perror("headers");
        exit(1);
    }

/*  Make base directory the current directory for the remainder of the run  */

    if (chdir(base) != 0) {
        fprintf(stderr, "chdir: ");
        perror(base);
        exit(1);
    }

    MKDIR(dname, PERMISSION);

}

char *pathname(int stream)
{

/*  Make the file name and combine to get absolute path name  */

    sprintf(fname, "%s-%.2d.w", ida->site, stream);
    sprintf(name, "%s%c%s%c%s", base, PATH_DELIMITER,dname, PATH_DELIMITER,fname);

    return name;
}

char *crnt_dname(void)
{
    return dname;
}

char *crnt_fname(void)
{
    return fname;
}

char *basedir(void)
{
    return base;
}

char *vol_id(void)
{
    return ident;
}

/* Revision History
 *
 * $Log: names.c,v $
 * Revision 1.7  2006/06/27 00:06:40  dechavez
 * restored cwd declaration for unix builds
 *
 * Revision 1.6  2006/06/27 00:04:27  akimov
 * removed unreferenced local variables
 *
 * Revision 1.5  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.4  2005/11/10 00:12:58  dechavez
 * respect case of sname as specified by user on command line
 *
 * Revision 1.3  2005/09/30 16:48:08  dechavez
 * 09-30-2005 aap win32 mods
 *
 * Revision 1.2  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
