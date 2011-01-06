#pragma ident "$Id: saclh.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  List the contents of a binary SAC header.
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"
#include "util.h"

int main(int argc, char **argv)
{
struct sac_header hdr;
FILE *fp;

    if (argc == 1) {
        fp = stdin;
    } else if (argc == 2) {
        if ((fp = fopen(argv[1], "rb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: %s sac_binary_file\n", argv[0]);
        exit(1);
    }

    if (SETMODE(fileno(fp), O_BINARY) == -1) {
        perror("saclh: setmode");
        exit(1);
    }

    if (sacio_rbh(fp, &hdr) != 0) {
        perror("saclh: sacio_rbh");
        exit(1);
    }

    if (sacio_wah(stdout, &hdr) != 0) {
        perror("saclh: sacio_wah");
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: saclh.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
