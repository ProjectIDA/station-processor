#pragma ident "$Id: dhead1.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 1 data record header.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int dhead_rev1(IDA *db, IDA_DHDR *dest, UINT8 *src)
{

/*  rev 1 not implemented  */

    errno = ENOTSUP;
    return -101;

}

/* Revision History
 *
 * $Log: dhead1.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
