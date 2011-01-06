#pragma ident "$Id: main.c,v 1.2 2007/01/04 18:24:16 dechavez Exp $"
/*======================================================================
 *
 *  Given file name and a directory name, generate a path name that
 *  in the directory that will not clobber any files which may already
 *  exist there with the given file name.
 *
 *  This is done by appending numbers of the form -nn to the end of the
 *  the file name until a uniq name is found.  The suflen argument gives
 *  the length of the suffix (if any)) that should not be altered when
 *  generating new names.  For example given a file abc.gz and suflen=3
 *  then candidate names would be abc-00.gz, abc-01.gz, etc.
 *
 *====================================================================*/
#include "util.h"

extern char *VersionIdentString;

static char *BuildCandidate(char *file, int suflen, int count)
{
static char path[MAXPATHLEN+1];

    if (count == 0) return file;
    memset(path, 0, MAXPATHLEN+1);
    if (suflen > strlen(file)) suflen = 0;
    strncpy(path, file, strlen(file) - suflen);
    sprintf(path + strlen(path), "-%02d", count);
    if (suflen > 0) strcat(path, file + (strlen(file) - suflen));

    return path;
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [suflen=count] file_name dest_dir_name\n", myname);
    exit(1);
}

int main (int argc, char **argv)
{
int i;
char *file = NULL;
char *dir = NULL;
int suflen = 0;
char *candidate;
char path[MAXPATHLEN + 1];

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "suflen=", strlen("suflen=")) == 0) {
            suflen = atoi(argv[i] + strlen("suflen="));
        } else if (file == NULL) {
            file = argv[i];
        } else if (dir == NULL) {
            dir = argv[i];
        } else {
            fprintf(stderr, "unrecognized argument '%s'\n", argv[i]);
            help(argv[0]);
        }
    }

    if (file == NULL || dir == NULL) help(argv[0]);

    i = 0;
    do {
        candidate = BuildCandidate(file, suflen, i++);
        sprintf(path, "%s/%s", dir, candidate);
    } while (utilFileExists(path));

    printf("%s\n", path);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2007/01/04 18:24:16  dechavez
 * include util.h instead of platform.h
 *
 * Revision 1.1  2005/10/07 21:31:07  dechavez
 * initial release
 *
 */
