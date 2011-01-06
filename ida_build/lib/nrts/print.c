#pragma ident "$Id: print.c,v 1.1 2005/07/26 00:02:11 dechavez Exp $"
/*======================================================================
 *
 * Print/log stuff
 *
 *====================================================================*/
#include "nrts/dl.h"

void nrtsPrintDL(FILE *fp, NRTS_DL *dl)
{
    if (dl == NULL) return;
    nrts_prtsta(fp, &dl->sys->sta[0], NULL, NRTS_TIMES);
}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.1  2005/07/26 00:02:11  dechavez
 * initial release
 *
 */
