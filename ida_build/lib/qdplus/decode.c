#pragma ident "$Id: decode.c,v 1.4 2007/05/17 22:25:05 dechavez Exp $"
/*======================================================================
 *
 *  Decode various data structures
 *
 *====================================================================*/
#include "qdplus.h"

int qdplusUnpackWrappedQDP(UINT8 *start, QDPLUS_PKT *dest)
{
UINT8 *ptr;
int dlen;

    ptr = start;
    ptr += utilUnpackUINT64(ptr, &dest->serialno);
    ptr += utilUnpackUINT32(ptr, &dest->seqno.signature);
    ptr += utilUnpackUINT64(ptr, &dest->seqno.counter);
    ptr += utilUnpackBytes(ptr, dest->qdp.raw, QDP_CMNHDR_LEN);
    qdpNetToHost(&dest->qdp);
    dlen = dest->qdp.hdr.dlen < QDP_MAX_PAYLOAD ? dest->qdp.hdr.dlen : QDP_MAX_PAYLOAD;
    ptr += utilUnpackBytes(ptr, dest->qdp.raw + QDP_CMNHDR_LEN, dlen);

    return (int) (ptr - start);
}

/* Decode a QDPLUS_DT_USER_AUX payload, leaving data in place */

void qdplusDecodeUserAux(UINT8 *start, QDPLUS_AUXPKT *dest)
{
UINT8 *ptr;

    ptr = start;
    ++ptr; /* skip over identifier */
    ++ptr; /* skip over reserved */
    dest->type = *ptr++;
    ptr += utilUnpackBytes(ptr, dest->chn, ISI_CHNLEN); dest->chn[ISI_CHNLEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->loc, ISI_LOCLEN); dest->loc[ISI_LOCLEN] = 0;
    ptr += utilUnpackUINT16(ptr, &dest->sint);
    ptr += utilUnpackUINT32(ptr, &dest->tofs.sys);
    ptr += utilUnpackUINT32(ptr, &dest->tofs.pps);
    ptr += utilUnpackUINT32(ptr, &dest->tofs.ext);
    dest->tofs.code = *ptr++;
    ++ptr; /* skip over reserved */
    ptr += utilUnpackUINT32(ptr, &dest->tols.sys);
    ptr += utilUnpackUINT32(ptr, &dest->tols.pps);
    ptr += utilUnpackUINT32(ptr, &dest->tols.ext);
    dest->tols.code = *ptr++;
    ++ptr; /* skip over reserved */
    ptr += utilUnpackUINT16(ptr, &dest->nsamp);
    dest->data = ptr;
}

/* Revision History
 *
 * $Log: decode.c,v $
 * Revision 1.4  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
