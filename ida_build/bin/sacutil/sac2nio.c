#pragma ident "$Id: sac2nio.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Convert a binary SAC file into NIO.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"
#include "sacio.h"

int main(int argc, char **argv)
{
int   i;
FILE *ifp, *ofp;
float sample;
long  count = 0;
struct sac_header hdr;

    if (argc == 1) {
        ifp = stdin;
        ofp = stdout;
    } else if (argc == 2) {
        if ((ifp = fopen(argv[1], "rb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        ofp = stdout;
    } else if (argc == 3) {
        if ((ifp = fopen(argv[1], "rb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        if ((ofp = fopen(argv[2], "wb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: sac2nio sac_in nio_out\n");
        exit(1);
    }

    if (SETMODE(fileno(ifp), O_BINARY) == -1) {
        perror("sac2nio: setmode");
        exit(1);
    }

    if (SETMODE(fileno(ofp), O_BINARY) == -1) {
        perror("sac2nio: setmode");
        exit(1);
    }

    if (sacio_rbh(ifp, &hdr) != 0) {
        perror("sac2nio: sacio_rbh");
        exit(1);
    }

    for (i = 0; i < hdr.npts; i++) {
        if (fread(&sample, sizeof(float), 1, ifp) != 1) {
            if (feof(ifp)) {
                fprintf(stderr,"sac2nio: warning, unexpected EOF encountered ");
                fprintf(stderr,"after %ld samples.\n", i);
                exit(0);
            } else {
                perror("sac2nio: fread");
                exit(1);
            }
        }
        if (fwrite(&sample, sizeof(float), 1, ofp) != 1) {
            perror("sac2nio: fwrite");
            exit(1);
        }
    }

    fprintf(stderr,"sac2nio: %ld samples written.\n", i);

    exit(0);
}

/* Revision History
 *
 * $Log: sac2nio.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
