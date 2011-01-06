#pragma ident "$Id: idadd.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Dump an IDA data disk to stdout.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "util.h"

#define BLKSIZ  512

void help(myname)
char *myname;
{
    fprintf(stderr,"usage: %s [-v] raw_disk_device\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int ifd, verbose = 0;
long i, csp, rno = 0;
long nsec, nread, hr, mn, sc, bufsiz = BLKSIZ;
char buffer[BLKSIZ];
char *idev = NULL;
time_t beg, end, elapsed;
char *myname;
char *version = "idadd (4/23/96 version)";

    myname = argv[0];

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (idev == NULL) {
            idev = argv[i];
        } else {
            help(myname);
        }
    }

    if (idev == NULL) help(myname);

    if ((ifd = open(idev, O_RDONLY)) < 0) {
        fprintf(stderr, "%s: open: ", myname);
        perror(idev);
        exit(2);
    }

    for (i = 0; i < 3; i++) {
        if ((nread = read(ifd, buffer, bufsiz)) != bufsiz) {
            fprintf(stderr, "%s: read: ", myname);
            perror(idev);
            exit(3);
        }
    }

    memcpy(&csp, buffer + 4, 4);
    if (util_order() != BIG_ENDIAN_ORDER) util_lswap(&csp, 1);
    nsec = csp - 4;

    if ((nread = read(ifd, buffer, bufsiz)) != bufsiz) {
        fprintf(stderr, "%s: read: ", myname);
        perror(idev);
        exit(4);
    }

    if (verbose) {
        fprintf(stderr, "Reading %ld sectors (%ld records)...\n",
            nsec, nsec*BLKSIZ / 1024
        );
        time(&beg);
    }

    for (i = 1; i <= nsec; i++) {
        if ((nread = read(ifd, buffer, bufsiz)) != bufsiz) {
            fprintf(stderr, "%s: read: ", myname);
            perror(idev);
            exit(5);
        }
        if (fwrite(buffer, sizeof(char), bufsiz, stdout) != bufsiz) {
            fprintf(stderr, "%s: fwrite: ", myname);
            perror("stdout");
            exit(6);
        }
    }
    if (verbose) {
        time(&end);
        sc = end - beg;
        hr  = (sc - (sc % 3600)) / 3600;
        sc -= hr * 3600;
        mn = (sc - (sc % 60)) / 60;
        sc -= mn * 60;
        fprintf(stderr, "Began: %s", ctime(&beg));
        fprintf(stderr, "Ended: %s", ctime(&end));
        fprintf(stderr, "Elapsed time: %2.2d:%2.2d:%2.2d\n", hr, mn, sc);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: idadd.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
