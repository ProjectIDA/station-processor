#pragma ident "$Id: time.c,v 1.2 2008/01/07 21:48:12 dechavez Exp $"
/*======================================================================
 *
 *  lissEpochTimeToSeed()
 *  Given a epoch (double) time and a 10 byte character array, load the
 *  array with the split time.
 *
 *  lissSeedTimeToEpoch()
 *  Given an array with data in seed time format, convert to double.
 *  This routine also examines the raw data in order to guess the
 *  native byte order of the rest of the header (something which the
 *  SEED format fails to take into account... duh).  It does this by
 *  looking at the year, and if it is negative or greater than 2038
 *  (the year all this code falls apart due to Unix time overflow!)
 *  then it is assumed that the header is ordered opposite of this
 *  host (there is the further assumption that there are only two
 *  types of word order, little-endian and big-endian).  There are
 *  a few years where this test will not work, in which case we further
 *  do a test on the day of year field, and that narrows down the
 *  possibility of failure to a few days in the late 18th century for
 *  which there is likely to be little data.
 *
 *  lissSint()
 *  Given a LISS_FSDH, return the sample interval in seconds
 *
 *====================================================================*/
#include "liss.h"
#include "util.h"

void lissEpochTimeToSeed(UINT8 *output, REAL64 dtime, UINT32 order)
{
BOOL swap;
int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
INT16 yr, da, ms;
u_char hr, mn, sc, un = 0;

    swap = (order != NATIVE_BYTE_ORDER) ? TRUE : FALSE;

    utilTsplit(dtime, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    yr = (INT16)  tmp1;      if (swap) utilSwapINT16((UINT16 *) &yr, 1);
    da = (INT16)  tmp2;      if (swap) utilSwapINT16((UINT16 *) &da, 1);
    hr = (u_char) tmp3;
    mn = (u_char) tmp4;
    sc = (u_char) tmp5;
    ms = (INT16)  tmp6 * 10; if (swap) utilSwapINT16((UINT16 *) &ms, 1);

    memcpy(output+0, (void *) &yr, 2);
    memcpy(output+2, (void *) &da, 2);
    memcpy(output+4, (void *) &hr, 1);
    memcpy(output+5, (void *) &mn, 1);
    memcpy(output+6, (void *) &sc, 1);
    memcpy(output+7, (void *) &un, 1);
    memcpy(output+8, (void *) &ms, 2);
}

REAL64 lissSeedTimeToEpoch(UINT8 *src, UINT32 *order)
{
BOOL swap;
UINT32 native_order = NATIVE_BYTE_ORDER, other_order;
INT16 yr, da, ms;
UINT8 hr, mn, sc;
REAL64 result;

    other_order = (native_order == LTL_ENDIAN_BYTE_ORDER) ? BIG_ENDIAN_BYTE_ORDER : LTL_ENDIAN_BYTE_ORDER;

    memcpy((void *) &yr, src+0, 2);
    memcpy((void *) &da, src+2, 2);
    memcpy((void *) &hr, src+4, 1);
    memcpy((void *) &mn, src+5, 1);
    memcpy((void *) &sc, src+6, 1);
    memcpy((void *) &ms, src+8, 2);

/* Try to guess the byte ordering used */

    if (yr < 0 || yr > 2038 || da < 0 || da > 366) {
        *order = other_order;
        swap = TRUE;
    } else {
         *order = native_order;
        swap = FALSE;
    }

/* Re-order if necessary, and then convert */

    if (swap) {
        utilSwapINT16((UINT16 *) &yr, 1);
        utilSwapINT16((UINT16 *) &da, 1);
        utilSwapINT16((UINT16 *) &ms, 1);
    }
    ms /= 10;

    result = utilYdhmsmtod( (int)yr, (int)da, (int)hr, (int)mn, (int)sc, (int)ms);

    return result;
}

REAL64 lissSint(LISS_FSDH *fsdh)
{
REAL64 sint;

    if (fsdh->srfact > 0 && fsdh->srmult > 0) {
        sint  = 1.0 / (REAL64) fsdh->srfact * (REAL64) fsdh->srmult;
    } else if (fsdh->srfact > 0 && fsdh->srmult < 0) {
        sint = (REAL64) -fsdh->srmult / (REAL64) fsdh->srfact;
    } else if (fsdh->srfact < 0 && fsdh->srmult > 0) {
        sint = (REAL64) -fsdh->srfact / (REAL64) fsdh->srmult;
    } else {
        sint = (REAL64) fsdh->srfact / (REAL64) fsdh->srmult;
    }

    return sint;
}

/* Revision History
 *
 * $Log: time.c,v $
 * Revision 1.2  2008/01/07 21:48:12  dechavez
 * added lissSint()
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
