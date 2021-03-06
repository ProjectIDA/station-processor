#pragma ident "$Id: drec3.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 3 data record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define DATA_OFFSET 64

int drec_rev3(IDA *ida, IDA_DREC *dest, UINT8 *src, int swab)
{
int retval;

/*  Load the header  */

    if ((retval = dhead_rev3(ida, &dest->head, src)) != 0) return retval;

/*  Load the data  */

    if (dest->head.nbytes > IDA_MAXDLEN) return -201;
    memcpy(dest->data, src + DATA_OFFSET, dest->head.nbytes);

    if (swab) {
        if (dest->head.form == S_UNCOMP) {
            SSWAB((short *) dest->data, dest->head.nsamp);
        } else if (dest->head.form == L_UNCOMP) {
            LSWAB((long *) dest->data, dest->head.nsamp);
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: drec3.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
