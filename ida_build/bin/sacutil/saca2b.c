#pragma ident "$Id: saca2b.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Convert an ascii SAC file into binary.
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
        if ((ifp = fopen(argv[1], "r")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        ofp = stdout;
    } else if (argc == 3) {
        if ((ifp = fopen(argv[1], "r")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        if ((ofp = fopen(argv[2], "wb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: saca2b: binary_name ascii_name\n");
        exit(1);
    }

    if (SETMODE(fileno(ofp), O_BINARY) == -1) {
        perror("saca2b: setmode");
        exit(1);
    }

    if (sacio_rah(ifp, &hdr) != 0) {
        perror("saca2b: sacio_rah");
        exit(1);
    }
    if (sacio_wbh(ofp, &hdr) != 0) {
        perror("saca2b: sacio_wbh");
        exit(1);
    }

    for (i = 0; i < hdr.npts; i++) {
        if (fscanf(ifp, "%f", &sample) != 1) {
            if (feof(ifp)) {
                fprintf(stderr,"saca2b: unexpected EOF encountered ");
                fprintf(stderr,"after %ld samples.\n", i);
                exit(1);
            } else {
                perror("saca2b: fscanf");
                exit(1);
            }
        }
        if (fwrite(&sample, sizeof(float), 1, ofp) != 1) {
            perror("saca2b: fwrite");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: saca2b.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
