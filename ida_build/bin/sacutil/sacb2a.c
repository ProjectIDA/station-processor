#pragma ident "$Id: sacb2a.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Convert a binary SAC file into ascii.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"
#include "sacio.h"

int main(int argc, char **argv)
{
long i;
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
        if ((ofp = fopen(argv[2], "w")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: sacb2a binary_name ascii_name\n");
        exit(1);
    }

    if (SETMODE(fileno(ifp), O_BINARY) == -1) {
        perror("sacb2a: setmode");
        exit(1);
    }

    if (sacio_rbh(ifp, &hdr) != 0) {
        perror("sacb2a: sacio_rbh");
        exit(1);
    }
    if (sacio_wah(ofp, &hdr) != 0) {
        perror("sacb2a: sacio_wah");
        exit(1);
    }

    for (i = 0; i < hdr.npts; i++) {
        if (fread(&sample, sizeof(float), 1, ifp) != 1) {
            if (feof(ifp)) {
                fprintf(stderr,"sacb2a: unexpected EOF encountered ");
                fprintf(stderr,"after %ld samples.\n", i);
                exit(1);
            } else {
                perror("sacb2a: fread");
                exit(1);
            }
        }
        if (sacio_wad(ofp, &sample, 1L, &count) != 1) {
            perror("sacb2a: sacio_wad");
            exit(1);
        }
    }
    if (hdr.npts % 5 != 0) fprintf(ofp, "\n");

    exit(0);
}

/* Revision History
 *
 * $Log: sacb2a.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
