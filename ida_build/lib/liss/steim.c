#pragma ident "$Id: steim.c,v 1.4 2009/02/03 22:34:37 dechavez Exp $"
/*======================================================================
 *
 *  Steim 1 and Steim 2 decompressors
 *
 *  Both decompressors are meant to handle data from both big-endian and
 *  little endian systems, without the assumption that the data are in
 *  big endian byte order.
 *
 *====================================================================*/
#include "liss.h"
#include "util.h"

#define FLEN 64           /* number of bytes per frame     */
#define NSEQ 16           /* number of sequences per frame */

int lissDecompressSteim1(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count)
{
int i, j, k, nsamp, nfrm;
INT32  ltmp, val, beg, end, key, code[NSEQ];
INT16 stmp;
INT8  ctmp;
UINT8 *frm;
union {
    UINT8 *c;
    INT16 *s;
    INT32 *l;
} ptr;
BOOL swap;

    swap = (order == NATIVE_BYTE_ORDER) ? FALSE : TRUE;
    nfrm = srclen / FLEN;

/* Get the block start/stop values */

    ptr.c = src;

    memcpy(&beg, ptr.c + 4, 4);
    if (swap) utilSwapINT32((UINT32 *) &beg, 1);

    memcpy(&end, ptr.c + 8, 4);
    if (swap) utilSwapINT32((UINT32 *) &end, 1);

/* Loop over each frame */
/* We do not verify that the 0x00 codes are where they should be */

    val   = dest[0] = beg;
    nsamp = 1;

    for (i = 0; i < nfrm; i++) {

        frm = src + (i * FLEN);  /* point to start of current frame */
        memcpy(&key, frm, sizeof(INT32)); /* codes are in first 4 bytes */
        if (swap) utilSwapINT32((UINT32 *) &key, 1);
        for (j = NSEQ - 1; j >= 0; j--) {
            code[j] = key & 0x03;
            key >>= 2;
        }

        for (j = 1; j < NSEQ; j++) {

            if (nsamp >= destlen) {
                return LISS_DECOMP_ERROR_A;
            }

            ptr.c = frm + (j * 4);  /* point to current 4 byte sequence */

            switch (code[j]) {
              case 0:
                break;

              case 1:
                for (k = (nsamp == 1) ? 1 : 0; k < 4; k++) {
                    ctmp = *(INT8 *) &ptr.c[k];
                    dest[nsamp++] = (val += (INT32) ctmp);
                }
                break;

              case 2:
                for (k = (nsamp == 1) ? 1 : 0; k < 2; k++) {
                    stmp = ptr.s[k];
                    if (swap) utilSwapINT16((UINT16 *) &stmp, 1);
                    dest[nsamp++] = (val += (INT32) stmp);
                }
                break;

              case 3:
                if (nsamp > 1) {
                    ltmp = ptr.l[0];
                    if (swap) utilSwapINT32((UINT32 *) &ltmp, 1);
                    dest[nsamp++] = (val += ltmp);
                }
                break;

              default:
                return LISS_DECOMP_ERROR_B;
            }
        }
    }

    return LISS_PKT_OK;

}

static int do_steim2(
INT32 code,    /* key code                         */
INT32 word,    /* 4-byte sequence to decode        */
INT32 *output, /* ptr to output array              */
INT32 nsamp,   /* current sample count             */
INT32 maxsamp  /* maximum number of words to store */
){
INT32 dnib, rest, shift, diff;
struct decode_info {
    INT32 ndif;
    INT32 bits;
} decode;
static struct decode_info decode_table[4][4] = {
            /*                    dnib                  */
            /*       0         1         2         3    */
/* code = 0 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = 1 */ { {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },
/* code = 2 */ { {-1, -1}, { 1, 30}, { 2, 15}, { 3, 10} },
/* code = 3 */ { { 5,  6}, { 6,  5}, { 7,  4}, {-1, -1} }
};
struct mask_info {
    INT32 sign;
    INT32 diff;
} mask;
static struct mask_info mask_table[4][4] = {
/* code = 0 */ { {         0,          0}, {         0,          0},
                 {         0,          0}, {         0,          0} },

/* code = 1 */ { {         0,          0}, {         0,          0},
                 {         0,          0}, {         0,          0} },

/* code = 2 */ { {         0,          0}, {0x20000000, 0x3fffffff},
                 {0x00004000, 0x00007fff}, {0x00000200, 0x000003ff} },

/* code = 3 */ { {0x00000020, 0x0000003f}, {0x00000010, 0x0000001f},
                 {0x00000008, 0x0000000f}, {         0,          0} }
};
static char *fid = "do_steim2";

    if (code == 0) return nsamp;
    dnib = (word >> 30) & 0x03;
    rest =  word & 0x3fffffff;

    if (code < 0 || code > 3) {
        return LISS_DECOMP_ERROR_C;
    }
    if (dnib < 0 || dnib > 3) {
        return LISS_DECOMP_ERROR_D;
    }

    decode = decode_table[code][dnib];
    if (decode.ndif < 0 || decode.bits < 0) {
        return LISS_DECOMP_ERROR_E;
    }

    mask = mask_table[code][dnib];

    for (shift = decode.bits * (decode.ndif - 1); shift >= 0 && nsamp < maxsamp; shift -= decode.bits) {
        diff = (rest >> shift) & mask.diff;
        if (diff & mask.sign) diff |= ~mask.diff;
        output[nsamp++] = diff;
    }

    return nsamp;
}

int lissDecompressSteim2(INT32 *dest, INT32 destlen, UINT8 *src, INT32 srclen, UINT32 order, INT32 count)
{
int i, j, k, nsamp, nfrm;
INT32  beg, end, key, code[NSEQ], word;
UINT8 *frm;
union {
    UINT8 *c;
    INT8  *b;
    INT16 *s;
    INT32 *l;
} ptr;
BOOL swap;
static char *fid = "lissDecompressSteim2";

    swap = (order == NATIVE_BYTE_ORDER) ? FALSE : TRUE;
    nfrm = srclen / FLEN;

/* Get the block start/stop values */

    ptr.c = src;

    memcpy(&beg, ptr.c + 4, 4);
    if (swap) utilSwapINT32((UINT32 *) &beg, 1);

    memcpy(&end, ptr.c + 8, 4);
    if (swap) utilSwapINT32((UINT32 *) &end, 1);

/* Loop over each frame */
/* We do not verify that the 0x00 codes are where they should be */

    for (nsamp = 0, i = 0; i < nfrm; i++) {

        frm = src + (i * FLEN);  /* point to start of current frame */
        memcpy(&key, frm, sizeof(INT32)); /* codes are in first 4 bytes */
        if (swap) utilSwapINT32((UINT32 *) &key, 1);
        for (j = NSEQ - 1; j >= 0; j--) {
            code[j] = key & 0x03;
            key >>= 2;
        }

        for (j = 0; j < NSEQ; j++) {

            if (nsamp >= destlen) {
                return LISS_DECOMP_ERROR_F;
            }

            ptr.c = frm + (j * 4);  /* point to current 4 byte sequence */

            if (code[j] == 1) {
                for (k = 0; k < 4; k++) dest[nsamp++] = (INT32) ptr.b[k];
            } else if (code[j] != 0) {
                memcpy(&word, ptr.c, 4);
                if (swap) utilSwapINT32((UINT32 *) &word, 1);
                nsamp = do_steim2(code[j], word, dest, nsamp, destlen);
                if (nsamp < 0) return nsamp;
            }
        }
    }

    for (dest[0]=beg, i=1, j = 0; i < nsamp; i++, j++) dest[i] += dest[j];

    if (count != nsamp) {
        return LISS_DECOMP_ERROR_G;
    } else if (dest[nsamp-1] != end) {
        return LISS_DECOMP_ERROR_H;
    }

    return LISS_PKT_OK;
}

/* Revision History
 *
 * $Log: steim.c,v $
 * Revision 1.4  2009/02/03 22:34:37  dechavez
 * fixed usigned to signed int conversion bug in lissDecompressSteim1 (aap)
 *
 * Revision 1.3  2008/01/11 00:40:46  dechavez
 * deal with bus errors associated with pointer casts directly on input byte stream
 *
 * Revision 1.2  2008/01/07 21:49:20  dechavez
 * replaced hardcoded error return codes with liss.h constants
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
