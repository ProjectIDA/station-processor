#pragma ident "$Id: fixsys.c,v 1.2 2004/04/26 21:12:35 dechavez Exp $"
/*======================================================================
 *
 *  Force NRTS sys file to have expected length.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nrts.h"
#include "util.h"

void help(char *myname)
{
    fprintf(stderr,"usage: %s [options] sys_name\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"home=name   => set NRTS_HOME\n");
    exit(1);
}

int main (int argc, char **argv)
{
int debug = 1;
char *log = (char *) NULL;
int i, fd;
long expected, diff;
char *home = NULL;
char *sysname  = NULL;
struct nrts_files *file;
struct stat stat;

    expected = (size_t) sizeof(struct nrts_sys);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else {
            if (sysname != NULL) help(argv[0]);
            sysname = argv[i];
        }
    }

/*  Must specify system name  */

    if (sysname == NULL) help(argv[0]);

/* Start logging facility if selected */

    if (debug > 0) util_logopen(log, 1, 9, debug, log, argv[0]);

/*  Get file names  */

    file = nrts_files(&home, sysname);

/* Do it */

    printf("checking `%s'\n", file->sys);

    if ((fd = open(file->sys, O_RDWR, 0644)) < 0) {
        fprintf(stderr, "open(%s, 0x%x, 0644): %s\n",
            file->sys, O_RDWR, syserrmsg(errno)
        );
        exit(1);
    }

    if (fstat(fd, &stat) != 0) {
        fprintf(stderr, "fstat: %s: %$s\n", file->sys, syserrmsg(errno));
        exit(1);
    }
    diff = (long) expected - (long) stat.st_size;

    if (diff != 0) {
        if (diff % sizeof(struct nrts_chn) == 0) {
            printf("NRTS_MAXCHN evidently changed from %d to %d\n",
                NRTS_MAXCHN - diff / sizeof(struct nrts_chn), NRTS_MAXCHN
            );
            if (lseek(fd, expected-1, SEEK_SET) == -1) {
                fprintf(stderr, "lseek: %s: %s\n",
                    file->sys, syserrmsg(errno)
                );
                exit(1);
            }
            if (write(fd, "", 1) != 1) {
                fprintf(stderr, "write: %s: %s\n",
                    file->sys, syserrmsg(errno)
                );
                exit(1);
            }
            printf("`%s' file size adjusted to %d\n", file->sys, expected);
        }
    } else {
        printf("`%s' has expected length (%d)\n", file->sys, expected);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: fixsys.c,v $
 * Revision 1.2  2004/04/26 21:12:35  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:13  dec
 * import existing IDA/NRTS sources
 *
 */
