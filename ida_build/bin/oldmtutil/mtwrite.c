#pragma ident "$Id: mtwrite.c,v 1.3 2004/06/24 18:56:28 dechavez Exp $"
/*======================================================================
 *
 *  Write to tape device supported by mtio library routines.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "oldmtio.h"
#include "util.h"

#ifdef MSDOS
#define REAL_BIG unsigned char huge
#else
#define REAL_BIG char
#endif

static int help()
{
    fprintf(stderr,"usage: mtwrite device [bs=#] < input\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, target, lun;
long count = 0;
int verbose = 0;
REAL_BIG *buffer;
TAPE tp;
long bytes;
char *device;
#ifdef MSDOS
long bufsiz = 10240;
#else
long bufsiz = 32768;
#endif

    if (argc < 2) help();

    device = argv[1];
#ifdef MSDOS
    if (strcmp(device, "0") == 0) {
        device = "/dev/rst00";
    } else if (strcmp(device, "1") == 0) {
        device = "/dev/rst01";
    } else if (strcmp(device, "2") == 0) {
        device = "/dev/rst02";
    } else if (strcmp(device, "3") == 0) {
        device = "/dev/rst03";
    } else if (strcmp(device, "4") == 0) {
        device = "/dev/rst04";
    } else if (strcmp(device, "5") == 0) {
        device = "/dev/rst05";
    } else if (strcmp(device, "6") == 0) {
        device = "/dev/rst06";
    } else if (strcmp(device, "7") == 0) {
        device = "/dev/rst07";
    }
#endif /* MSDOS */

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

    if ((buffer = (REAL_BIG *) malloc(bufsiz)) == NULL) {
        perror("mtwrite: malloc");
        exit(1);
    }

    if (SETMODE(fileno(stdin), O_BINARY) == -1) {
        perror("mtwrite: setmode");
        exit(1);
    }

    if ((tp = mtopen(device, "w")) == (TAPE) NULL) {
        mterror("mtwrite: mtopen");
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
            if (mtwrite(tp, buffer, bytes) != bytes) {
                mterror("mtwrite: mtwrite");
                exit(1);
            }
            ++count;
            if (verbose) fprintf(stderr,"Wrote %ld bytes.\r", bytes);
        } else if (ferror(stdin)) {
            perror("mtwrite: fread");
            exit(1);
        } else {
            if (verbose) {
                fprintf(stderr,"mtwrite: %ld ", count);
                fprintf(stderr,"records written.\n");
            }
            if (mtweof(tp, 2) != 0) {
                mterror("mtwrite: mtweof");
                exit(1);
            }
            if (verbose) fprintf(stderr,"mtwrite: EOF*EOF written\n");
            mtclose(tp);
            exit(0);
        }
    }
}

/* Revision History
 *
 * $Log: mtwrite.c,v $
 * Revision 1.3  2004/06/24 18:56:28  dechavez
 * replace HUGE with REAL_BIG, since math.h is now included in platform.h and
 * it defines HUGE as MAXFLOAT (on at least one system)
 *
 * Revision 1.2  2003/12/10 06:31:23  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1  2000/05/01 22:55:38  dec
 * import existing sources
 *
 */
