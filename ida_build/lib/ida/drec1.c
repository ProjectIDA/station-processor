#pragma ident "$Id: drec1.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 1 data record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int drec_rev1(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab)
{

/*  rev 1 not implemented  */

    errno = ENOTSUP;
    return -201;

}

/* Revision History
 *
 * $Log: drec1.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
