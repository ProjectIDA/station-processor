#pragma ident "$Id: dcmp.c,v 1.2 2006/02/08 23:02:26 dechavez Exp $"
/*======================================================================
 *
 *  Decompress IDA data.  Returns the value of the last word.
 *
 *====================================================================*/
#include "ida.h"

INT32 ida_dcmp(INT32 *dest, UINT8 *src, int nsamp)
{
int i, src_ndx;
int nb;
int negval;
INT32 dd;

    src_ndx = 0;

/*  Get value of first word  */

    negval = src[src_ndx] & 0x01;
    nb = 0; while (src[src_ndx+nb] & 0x80) nb++;
    dest[0] = (src[src_ndx] >> 1) & 0x0000003f;
    if (nb >= 1) dest[0] |= (src[++src_ndx] & 0x0000007f) <<  6;
    if (nb == 2) dest[0] |= (src[++src_ndx] & 0x0000007f) << 13;
    if (negval)  dest[0] = -dest[0];
    src_ndx++;

/*  Get remaining values  */

    for (i = 1; i < nsamp; i++) {
        negval = src[src_ndx] & 0x01;
        nb = 0; while (src[src_ndx+nb] & 0x80) nb++;
        dd = (src[src_ndx] >> 1) & 0x0000003f;
        if (nb >= 1) dd |= (src[++src_ndx] & 0x0000007f) <<  6;
        if (nb == 2) dd |= (src[++src_ndx] & 0x0000007f) << 13;
        if (negval) dd = -dd;
        dest[i] = dest[i-1] + dd;
        src_ndx++;
    }

    return dest[nsamp-1];
}

/* Revision History
 *
 * $Log: dcmp.c,v $
 * Revision 1.2  2006/02/08 23:02:26  dechavez
 * protable datatypes
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
