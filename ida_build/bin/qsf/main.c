#pragma ident "$Id: main.c,v 1.1 2007/05/03 21:04:48 dechavez Exp $"
/*======================================================================
 *
 *  Read/decode a QDPLUS state file
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "qdplus.h"

int main(int argc, char **argv)
{
QDPLUS *handle;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
ISI_SEQNO beg, end;

    if (argc != 2) {
        fprintf(stderr, "usage: %s path_to_state_file\n", argv[0]);
        exit(1);
    }

    par.path.state = argv[1];

    if ((handle = qdplusCreateHandle(&par)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("qdplusCreateHandle");
        exit(1);
    }

    qdplusPrintState(stdout, handle);

    if (!qdplusStateSeqnoLimits(handle, &beg, &end)) {
        perror("qdplusStateSeqnoLimits");
        exit(1);
    }

    printf("\n");
    printf("beg = %08lx%016llx\n", beg.signature, beg.counter);
    printf("end = %08lx%016llx\n", end.signature, end.counter);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2007/05/03 21:04:48  dechavez
 * initial release
 *
 */
