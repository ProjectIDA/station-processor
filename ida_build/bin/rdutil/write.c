#pragma ident "$Id: write.c,v 1.2 2000/05/01 22:59:02 dec Exp $"
/*======================================================================
 *
 *  Write to raw disk device using rdio library routines.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "rdio.h"
#include "util.h"

main(argc, argv)
int argc;
char *argv[];
{
int i, target, lun;
long count = 0;
int verbose = 0;
char *buffer;
RDISK *rd;
long bytes;
char *device;
long bufsiz = 32768;

    if (argc < 2) help();

    device = argv[1];

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            bufsiz = util_atolk(argv[i] + strlen("bs="));
        } else if (strcmp(argv[i], "+v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 0;
        } else {
            help();
        }
    }

    if ((buffer = (char *) malloc(bufsiz)) == NULL) {
        perror("rdwrite: malloc");
        exit(1);
    }

    if ((rd = rdio_open(device, "w")) == (RDISK *) NULL) {
        perror("rdwrite: rdio_open");
        exit(1);
    }

    clearerr(stdin);

    if (verbose) {
        fprintf(stderr,"Writing %ld byte records ", bufsiz);
        fprintf(stderr,"to %s...\n", device);
    }

    while (1) {
        bytes = fread(buffer, sizeof(char), bufsiz, stdin);
        if (bytes > 0) {
            if (rdio_write(rd, buffer, bytes) != bytes) {
                perror("rdwrite: rdio_write");
                exit(1);
            }
            ++count;
            if (verbose) fprintf(stderr,"Wrote %ld bytes.\r", bytes);
        } else if (ferror(stdin)) {
            perror("rdwrite: fread");
            exit(1);
        } else {
            if (verbose) {
                fprintf(stderr,"rdwrite: %ld ", count);
                fprintf(stderr,"records written.\n");
            }
            rdio_close(rd);
            exit(0);
        }
    }
}

int help()
{
    fprintf(stderr,"usage: mtwrite device [bs=#] < input\n");
    exit(1);
}

/* Revision History
 *
 * $Log: write.c,v $
 * Revision 1.2  2000/05/01 22:59:02  dec
 * import existing sources
 *
 */
