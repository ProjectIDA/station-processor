#pragma ident "$Id: dhead8.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 8 data record header.
 *
 *  Rev 8 is identical to rev 7 except that all "required" bit shifts
 *  are done in the data logger, so we the ffu_shift field is zeroed.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int dhead_rev8(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
int retval;

    if ((retval = dhead_rev7(ida, dest, src)) != 0) return retval;

    dest->ffu_shift = 0;

    return 0;
}

/* Revision History
 *
 * $Log: dhead8.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
