#pragma ident "$Id: main.c,v 1.1 2006/12/12 23:08:37 dechavez Exp $"
/*======================================================================
 *
 *  Read/decode a Q330 meta-data file
 *
 *====================================================================*/
#include "qdp.h"

int main(int argc, char **argv)
{
int nitems, nsets = 0;
QDP_META meta;

    while ((nitems = qdpReadMeta(stdin, &meta)) > 0) {
        printf("%d QDP metadata items read\n", nitems);
        qdpPrintTokens(stdout, &meta.token);
        qdpPrint_C1_COMBO(stdout, &meta.combo);
        ++nsets;
    }

    if (nitems < 0) {
        printf("read/content error after %d sets\n", nsets);
    } else {
        printf("%d meta-data sets read\n", nsets);
    }

}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2006/12/12 23:08:37  dechavez
 * initial release
 *
 */
