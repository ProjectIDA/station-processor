#pragma ident "$Id: prt.c,v 1.2 2003/12/10 06:27:25 dechavez Exp $"
/*======================================================================
 *
 *  Filter to take arbitrary EDEP data and print summary to standard
 *  out.  Requires that the first record be a configuration, and all
 *  subsequent records be waveforms.  This will work for all server
 *  provided data formats.  It will not work for any derived formats
 *  assembled on the client side, in particular it will not work for
 *  the xfer_packet that eztap produces.
 *
 *====================================================================*/
#include <stdio.h>
#include "xfer.h"

int main(int argc, char **argv)
{
struct xfer_cnf cnf;
struct xfer_wav wav;
long count = 0;

    if (Xfer_ReadCnf(stdin, &cnf) != 0) {
        perror(argv[0]);
        exit(1);
    } else {
        Xfer_PrintCnf(stdout, &cnf);
    }

    while (Xfer_ReadWav(stdin, &wav) == 0) Xfer_PrintWav(stdout, &wav);

    exit(0);
}

/* Revision History
 *
 * $Log: prt.c,v $
 * Revision 1.2  2003/12/10 06:27:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
