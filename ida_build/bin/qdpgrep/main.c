#pragma ident "$Id: main.c,v 1.1 2006/12/06 22:43:59 dechavez Exp $"
/*======================================================================
 *
 *  Extract QDP packets from one particular instrument
 *
 *====================================================================*/
#include "qdplus.h"

int main (int argc, char **argv)
{
UINT32 in=0, out=0;
QDPLUS_PKT pkt;
UINT64 keep;

    if (argc != 2) {
        fprintf(stderr, "usage: %s serialno\n", argv[0]);
        exit(1);
    }

    keep = (UINT64) strtoll((char *) argv[1], NULL, 16);

    while (qdplusRead(stdin, &pkt)) {
        ++in;
        if (pkt.serialno == keep) {
            if (!qdplusWrite(stdout, &pkt)) {
                fprintf(stderr, "%s: ", argv[0]);
                perror("fwrite");
                exit(1);
            }
            ++out;
        }
    }
    fprintf(stderr, "%lu packets in\n", in);
    fprintf(stderr, "%lu packets out\n", out);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2006/12/06 22:43:59  dechavez
 * initial release
 *
 */
