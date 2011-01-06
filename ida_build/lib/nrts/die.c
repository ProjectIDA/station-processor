#pragma ident "$Id: die.c,v 1.1.1.1 2000/02/08 20:20:29 dec Exp $"
/*======================================================================
 *
 *  Verbose exit.
 *
 *====================================================================*/
#include "nrts.h"

void nrts_die(status)
int status;
{
    util_log(1, "exit %d", status);
    exit(status);
}

/* Revision History
 *
 * $Log: die.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
