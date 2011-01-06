#pragma ident "$Id: filelock.c,v 1.4 2009/03/20 21:05:29 dechavez Exp $"
/*======================================================================
 *
 *  Intended for use in scripts, to keep more than one instance of a
 #  script from running at any time.  It works by creating a file with
 #  and advisory lock and writing the calling scripts process ID (given
 #  as the second command line argument) to the file.
 #
 #  If the lock file already exists, it first checks to see that the
 #  process whose ID is in the file does not exist.  If it does, then
 #  the program exits with status 1.
 #
 #  Otherwise it exits with status 0 and the script can continue with
 #  confidence that it is all alone.
 *
 *====================================================================*/
#include "util.h"

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ -v ] path pid\n", myname);
    exit(1);
}

static BOOL ProcessExists(int pid)
{
    if (pid == 0) return FALSE;
    if (kill(pid, 0) == 0 || errno == EPERM) return TRUE;
    return FALSE;
}

int main(int argc, char **argv)
{
int i;
FILE *fp;
char *path = NULL;
BOOL verbose = FALSE;
struct {
    int new;
    int old;
} pid;

    pid.new = pid.old = 0;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (path == NULL) {
            path = argv[i];
        } else if (pid.new == 0) {
            pid.new = atoi(argv[i]);
        } else {
            help(argv[0]);
        }
    }

    if (path == NULL || pid.new == 0) help(argv[0]);

    if (!ProcessExists(pid.new)) {
        fprintf(stderr, "%s: no process with pid=%d found\n", argv[0], pid.new);
        exit(1);
    }

    if ((fp = fopen(path, "a+")) == NULL) {
        fprintf(stderr, "%s: fopen: ", argv[0]);
        perror(path);
        exit(1);
    }

    if (!utilWriteLockWait(fp)) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("utilWriteLockWait");
        exit(1);
    }

    rewind(fp);
    fscanf(fp, "%d", &pid.old);
    if (ferror(fp) || feof(fp)) pid.old = 0;

    if (ProcessExists(pid.old)) {
        if (verbose) fprintf(stderr, "%s already locked by process %d\n", path, pid.old);
        exit(1);
    }

    rewind(fp);
    ftruncate(fileno(fp), 0);
    fprintf(fp, "%d\n", pid.new);
    if (verbose) fprintf(stderr, "%s lock granted to process %d\n", path, pid.new);

    exit(0);
}

/* Revision History
 *
 * $Log: filelock.c,v $
 * Revision 1.4  2009/03/20 21:05:29  dechavez
 * use utilWriteLockWait
 *
 * Revision 1.3  2008/12/16 11:44:18  dechavez
 * fixed missing endquote in pragma
 *
 * Revision 1.2  2008/10/09 20:43:20  dechavez
 * fixed missing close quote in pragma
 *
 * Revision 1.1  2008/03/13 21:51:44  dechavez
 * initial release
 *
 */
