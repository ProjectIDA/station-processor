#pragma ident "$Id: crec.c,v 1.2 2006/06/26 23:50:16 dechavez Exp $"
/*======================================================================
 *
 *  Copy from raw buffer to calibration record structure
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int ida_crec(IDA *ida, IDA_CALIB_REC *dest, UINT8 *src)
{

    dest->rev = ida->rev.value;

    if (ida->rev.value >= 5) {
        return crec_rev5(ida, dest, src);
    } else {
        errno = ENOTSUP;
        return -1;
    }

}

/* Revision History
 *
 * $Log: crec.c,v $
 * Revision 1.2  2006/06/26 23:50:16  dechavez
 * fixed name error
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
