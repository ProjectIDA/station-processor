#pragma ident "$Id: steim.c,v 1.5 2008/01/07 21:21:37 dechavez Exp $"
/*======================================================================
 *
 *  Steim2A decompression
 *
 *====================================================================*/
#include "qdp.h"

#define B00   0
#define B01   1
#define B10   2
#define B11   3
#define NDNIB 4 /* number of possible nibbles (00, 01, 10, 11) */

#define B0011 B11

/* Decode a single 32-bit sequence into a set of differences */

static int DecodeSequence(UINT8 code, UINT32 sequence, INT32 *out, UINT32 maxsamp, UINT32 *total)
{
typedef struct { UINT32 ndif; UINT32 nbit; } DECODE_VALUES;
DECODE_VALUES decode;
static DECODE_VALUES decode_table[NDNIB][NDNIB] = {
             /*                   dnib                    */
             /*       B00       B01       B10       B11   */
/* code = B00 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = B01 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = B10 */ { {-1, -1}, { 1, 30}, { 2, 15}, { 3, 10} },
/* code = B11 */ { { 5,  6}, { 6,  5}, { 7,  4}, {-1, -1} }
};

typedef struct { long sign; long diff; } MASK_VALUES;
MASK_VALUES mask;
static MASK_VALUES mask_table[NDNIB][NDNIB] = {
             /*                                                     dnib                                                 */
             /*              B00                       B01                       B10                       B11           */
/* code = B00 */ { {         0,          0}, {         0,          0}, {         0,          0}, {         0,          0} },
/* code = B01 */ { {         0,          0}, {         0,          0}, {         0,          0}, {         0,          0} },
/* code = B10 */ { {         0,          0}, {0x20000000, 0x3fffffff}, {0x00004000, 0x00007fff}, {0x00000200, 0x000003ff} },
/* code = B11 */ { {0x00000020, 0x0000003f}, {0x00000010, 0x0000001f}, {0x00000008, 0x0000000f}, {         0,          0} }
};
int shift;
UINT32 dnib, data, diff, nsamp, i;
INT8 *ptr;

    nsamp = *total;

    switch (code) {

    /* 00 = same as Steim1, special: sequence contains non-data information */

      case B00: return QDP_OK;

    /* 01 = same as Steim1, four 1-byte (8-bit) differences contained in sequence */

      case B01:
        for (ptr = (UINT8 *) &sequence, i = 0; i < 4; i++) {
            if (nsamp > maxsamp) return QDP_OVERFLOW;
            out[nsamp++] = (INT32) ptr[i];
        }
        break;

    /* 10, 11 = high order 2 bits of sequence is dnib which describes encoding */

      case B10:
      case B11:
        dnib = sequence >> 30;
        data = sequence & 0x3fffffff;
        decode = decode_table[code][dnib];
        if (decode.ndif < 0 || decode.nbit < 0) return QDP_INVALID;
        mask = mask_table[code][dnib];
        for ( shift = decode.nbit * (decode.ndif - 1); shift >= 0; shift -= decode.nbit) {
            if (nsamp > maxsamp) return QDP_OVERFLOW;
            diff = (data >> shift) & mask.diff;
            if (diff & mask.sign) diff |= ~mask.diff;
            out[nsamp++] = (INT32) diff;
        }
        break;

    /* anything else is a mistake */

      default:
        return QDP_INVALID;
    }

    *total = nsamp;
    return QDP_OK;
}

/* Split a QDP compression map into an array of nibbles (mallocs memory) */

static UINT8 *BuildCodeArray(UINT8 *map, UINT32 nseq)
{
UINT32 seqno,index;
UINT8 *code;

    if ((code = (UINT8 *) malloc(nseq)) == NULL) return NULL;

    seqno = index = 0;
    while (seqno < nseq) {
        code[seqno++] = (map[index] >> 6) & B0011;
        if (seqno < nseq) code[seqno++] = (map[index] >> 4) & B0011;
        if (seqno < nseq) code[seqno++] = (map[index] >> 2) & B0011;
        if (seqno < nseq) code[seqno++] = (map[index] >> 0) & B0011;
        ++index;
    }

    return code;
}

#ifdef I_DONT_KNOW_WHY_THIS_CODE_IS_HERE_BUT_AM_AFRAID_TO_DELETE_IT_IN_CASE_I_REMEMBER_LATER

/* Decode a collection of 32-bit sequences into a user supplied array of 32-bit differences */

static int DecodeSteim2AList(UINT8 *map, LNKLST *list, INT32 *out, UINT32 maxsamp, UINT32 *nsamp)
{
UINT32 i, j, nseq, seqno;
int result;
UINT8 *code;
QDP_COMP_DATA *comp;

    if (map == NULL || list == NULL || out == NULL || nsamp == NULL) return QDP_INVALID;

/* Figure out how many sequences we have */

    for (nseq = 0, i = 0; i < list->count; i++) {
        comp = (QDP_COMP_DATA *) list->array[i];
        nseq += comp->nseq;
    }

/* split the compression map into nibbles */

    if ((code = BuildCodeArray(map, nseq)) == NULL) return QDP_ERROR;

/* decode each set of 32-bit sequences */

    seqno = 0;
    *nsamp = 0;
    result = QDP_OK;
    for (i = 0; result == QDP_OK, i < list->count; i++) {
        comp = (QDP_COMP_DATA *) list->array[i];
        for (j = 0; result == QDP_OK, j < comp->nseq; j++) {
            result = DecodeSequence(code[seqno++], comp->seq[j], out, maxsamp, nsamp);
        }
    }

    free(code);
    return result;
}

#endif

/* Decode a single set of 32-bit sequences into a user supplied array of 32-bit differences */

static int DecodeSteim2A(UINT8 *map, QDP_COMP_DATA *comp, INT32 *out, UINT32 maxsamp, UINT32 *nsamp)
{
UINT32 i;
int result;
UINT8 *code;

    if (map == NULL || comp == NULL || out == NULL || nsamp == NULL) return QDP_INVALID;

/* split the compression map into nibbles */

    if ((code = BuildCodeArray(map, comp->nseq)) == NULL) return QDP_ERROR;

/* decode each 32-bit sequence in the set */

    for (i = 0, result = QDP_OK; result == QDP_OK && i < comp->nseq; i++) {
        result = DecodeSequence(code[i], comp->seq[i], out, maxsamp, nsamp);
    }

    free(code);
    return result;
}

/* Decompress the data in one compressed blockette */

int qdpDecompressCOMP(QDP_BLOCKETTE_COMP *blk, INT32 *out, UINT32 maxsamp, UINT32 *nsamp)
{
int result;
UINT32 crnt, prev;

    if (blk == NULL || out == NULL || nsamp == NULL) return QDP_INVALID;

/* Decode the data to get an array of differences */

    *nsamp = 0;
    if ((result = DecodeSteim2A(blk->map, &blk->comp, out, maxsamp, nsamp)) != QDP_OK) return result;

/* Apply the differences to reconstitute the original series */

    for (out[0] += blk->prev, crnt=1, prev=0; crnt < *nsamp; crnt++) out[crnt] += out[prev++];

    return QDP_OK;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: steim.c,v $
 * Revision 1.5  2008/01/07 21:21:37  dechavez
 * #ifdef'd out some dead code
 *
 * Revision 1.4  2007/06/27 05:55:57  dechavez
 * fixed dnib 01 (4 8-bit differences) bug
 *
 * Revision 1.3  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
