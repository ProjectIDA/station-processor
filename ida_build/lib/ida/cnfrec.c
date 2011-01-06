#pragma ident "$Id: cnfrec.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Copy from raw buffer to configuration record structure
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int ida_cnfrec(IDA *ida, IDA_CONFIG_REC *dest, UINT8 *src)
{
    dest->rev = ida->rev.value;

    if (ida->rev.value >= 5) {
        return cnfrec_rev5(ida, dest, src);
    } else {
        errno = ENOTSUP;
        return -1;
    }
}

/* Revision History
 *
 * $Log: cnfrec.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
