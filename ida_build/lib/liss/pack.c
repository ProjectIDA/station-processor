#pragma ident "$Id: pack.c,v 1.4 2008/02/03 21:13:36 dechavez Exp $"
/*======================================================================
 *
 *  Unpack various SEED things
 *
 *====================================================================*/
#include <math.h>
#include "liss.h"
#include "util.h"

BOOL lissUnpackB1000(LISS_B1000 *dest, UINT8 *src)
{
BOOL swap;
INT16 type;

    memcpy(&type, src, 2);
    if (type == 1000) {
        swap = 0;
    } else {
        utilSwapINT16((UINT16 *) &type, 1);
        if (type == 1000) {
            swap = 1;
        } else {
            errno = EINVAL;
            return FALSE;
        }
    }

    memcpy(&dest->next, src + 2, 2);
    if (swap) utilSwapINT16((UINT16 *) &dest->next, 1);

    memcpy(&dest->format, src + 4, 1);
    memcpy(&dest->order,  src + 5, 1);
    memcpy(&dest->length, src + 6, 1);

    return TRUE;
}

void lissUnpackFSDH(LISS_FSDH *dest, UINT8 *src)
{
    sscanf(src, "%06dD             ", &dest->seqno);

    strlcpy(dest->staid, src+ 8, LISS_SNAMLEN+1);
    strlcpy(dest->locid, src+13, LISS_LNAMLEN+1);
    strlcpy(dest->chnid, src+15, LISS_CNAMLEN+1);
    strlcpy(dest->netid, src+18, LISS_NNAMLEN+1);

    utilTrimString(dest->staid);
    utilTrimString(dest->chnid);
    utilTrimString(dest->netid);

    dest->start = lissSeedTimeToEpoch(src+20, &dest->order);
    memcpy(&dest->nsamp,  src+30, 2);
    memcpy(&dest->srfact, src+32, 2);
    memcpy(&dest->srmult, src+34, 2);
    memcpy(&dest->active, src+36, 1);
    memcpy(&dest->ioclck, src+37, 1);
    memcpy(&dest->qual,   src+38, 1);
    memcpy(&dest->more,   src+39, 1);
    memcpy(&dest->tcorr,  src+40, 4);
    memcpy(&dest->bod,    src+44, 2);
    memcpy(&dest->first,  src+46, 2);

    if (dest->swap = (dest->order != NATIVE_BYTE_ORDER)) {
        utilSwapINT16((UINT16 *) &dest->nsamp,  1);
        utilSwapINT16((UINT16 *) &dest->srfact, 1);
        utilSwapINT16((UINT16 *) &dest->srmult, 1);
        utilSwapINT32((UINT32 *) &dest->tcorr,  1);
        utilSwapINT16((UINT16 *) &dest->bod,    1);
        utilSwapINT16((UINT16 *) &dest->first,  1);
    }
}

int lissUnpackMiniSeed(LISS_PKT *pkt, UINT8 *src, UINT32 flags)
{
int i;
UINT8 *ptr;
INT32 next;
INT16 type, *sdata;

    pkt->srclen = 0;
    pkt->bod = NULL;

/* Decode it and detect non-data packets */

    lissUnpackFSDH(&pkt->fsdh, src);

    if (strcasecmp(pkt->fsdh.chnid, "LOG") == 0) return pkt->status = LISS_NOT_DATA;

    if (pkt->fsdh.srfact == 0) return pkt->status = LISS_NOT_DATA;
    if (pkt->fsdh.srmult == 0) return pkt->status = LISS_NOT_DATA;
    if (pkt->fsdh.nsamp   < 1) return pkt->status = LISS_NOT_DATA;

/* Look for blockette 1000 */

    next = pkt->fsdh.first;
    do {
        ptr  = src + next;
        type = lissTypeAndOffset(ptr, &next, pkt->fsdh.swap);
    } while (next > 0 && type != 1000);

    if (type != 1000) return pkt->status = LISS_NOT_MINISEED;

/* Decode blockette 1000 */

    if (!lissUnpackB1000(&pkt->b1000, ptr)) return pkt->status = LISS_NONSENSE_PKT;

/* Fill in the derived fields */

    pkt->srclen = (INT32) (pow(2.0, (double) pkt->b1000.length)) - pkt->fsdh.bod;
    pkt->order = pkt->b1000.order ? BIG_ENDIAN_BYTE_ORDER : LTL_ENDIAN_BYTE_ORDER;
    pkt->bod = src + pkt->fsdh.bod;
    pkt->flags = flags;

/* Ignore nonsense packets */

    if (pkt->b1000.format == 0) return pkt->status = LISS_NONSENSE_PKT;
    
/* Decompress/reorder if asked */

    if (pkt->flags & LISS_OPTION_DECODE) {

        switch (pkt->b1000.format) {

          case LISS_STEIM1:
            pkt->status = lissDecompressSteim1(pkt->data, LISS_MAX_BLKSIZ, pkt->bod, pkt->srclen, pkt->order, pkt->fsdh.nsamp);
            break;

          case LISS_STEIM2:
            pkt->status = lissDecompressSteim2(pkt->data, LISS_MAX_BLKSIZ, pkt->bod, pkt->srclen, pkt->order, pkt->fsdh.nsamp);
            break;

          case LISS_INT_32:
            memcpy(pkt->data, (INT32 *) pkt->bod, pkt->fsdh.nsamp);
            if (pkt->fsdh.swap) utilSwapINT32((UINT32 *) pkt->data, pkt->fsdh.nsamp);
            pkt->status = LISS_PKT_OK;
            break;

          case LISS_INT_16:
            sdata = (INT16 *) pkt->bod;
            for (i = 0; i < pkt->fsdh.nsamp; i++) pkt->data[i] = (INT32) sdata[i];
            if (pkt->fsdh.swap) utilSwapINT32((UINT32 *) pkt->data, pkt->fsdh.nsamp);
            pkt->status = LISS_PKT_OK;
            break;

          default:
            pkt->status = LISS_UNSUPPORTED;
        }
    } else {
        pkt->status = LISS_PKT_OK;
    }

    return pkt->status;
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.4  2008/02/03 21:13:36  dechavez
 * lissUnpackMiniSeed() made smarter about decoding non-data packets, return
 * value switched from BOOL to decode status
 *
 * Revision 1.3  2008/01/07 21:52:31  dechavez
 * added selection tests and optional data decoding (brought over from lissRead())
 *
 * Revision 1.2  2007/10/31 17:08:28  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
