#pragma ident "$Id: dhead.c,v 1.2 2008/03/05 22:50:14 dechavez Exp $"
/*======================================================================
 *
 *  Copy from raw buffer to data record header structure.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int ida_dhead(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
int status;
    
    dest->ffu_shift = 0; /* rev specific routines modify as required */
    dest->rev = ida->rev.value;
    dest->subformat = 0;
    dest->flags = 0;
    dest->extra.tstamp = time(NULL);

    if (ida->rev.value == 9) {
        status = dhead_rev9(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 8) {
        status = dhead_rev8(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 7) {
        status = dhead_rev7(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 6) {
        status = dhead_rev6(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 5) {
        status = dhead_rev5(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 4) {
        status = dhead_rev4(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 3) {
        status = dhead_rev3(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 2) {
        status = dhead_rev2(ida, dest, src);
        dest->order = BIG_ENDIAN_ORDER;
    } else if (ida->rev.value == 1) {
        status = dhead_rev1(ida, dest, src);
        dest->order = LTL_ENDIAN_ORDER;
    } else {
        errno = ENOTSUP;
        status = -301;
    }

    if (status != 0) return status;

    return 0;
}

/* Revision History
 *
 * $Log: dhead.c,v $
 * Revision 1.2  2008/03/05 22:50:14  dechavez
 * initialize extra.tstamp (to current time) and the new subformat and flags fields
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
