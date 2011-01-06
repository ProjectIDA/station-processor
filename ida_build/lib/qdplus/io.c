#pragma ident "$Id: io.c,v 1.5 2007/05/17 22:25:05 dechavez Exp $"
/*======================================================================
 *
 *  Read QDPLUS packets
 *
 *====================================================================*/
#include "qdplus.h"

BOOL qdplusRead(FILE *fp, QDPLUS_PKT *pkt)
{
UINT8 *ptr, hdr[QDPLUS_HDRLEN];

    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (fread(hdr, 1, QDPLUS_HDRLEN, fp) != QDPLUS_HDRLEN) {
        return FALSE;
    }
    ptr = hdr;
    ptr += utilUnpackUINT64(ptr, &pkt->serialno);
    ptr += utilUnpackUINT32(ptr, &pkt->seqno.signature);
    ptr += utilUnpackUINT64(ptr, &pkt->seqno.counter);

    return qdpReadPkt(fp, &pkt->qdp) < 0 ? FALSE : TRUE;

}

BOOL qdplusWrite(FILE *fp, QDPLUS_PKT *pkt)
{
UINT8 *ptr, hdr[QDPLUS_HDRLEN];

    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    memset(hdr, 0, QDPLUS_HDRLEN);
    ptr = hdr;
    ptr += utilPackUINT64(ptr, pkt->serialno);
    ptr += utilPackUINT32(ptr, pkt->seqno.signature);
    ptr += utilPackUINT64(ptr, pkt->seqno.counter);

    if (fwrite(hdr, 1, QDPLUS_HDRLEN, fp) != QDPLUS_HDRLEN) return FALSE;

    return qdpWritePkt(fp, &pkt->qdp) < 0 ? FALSE : TRUE;
}

/* Revision History
 *
 * $Log: io.c,v $
 * Revision 1.5  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
