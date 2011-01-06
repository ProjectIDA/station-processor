#pragma ident "$Id: timerr.c,v 1.3 2006/02/08 23:12:51 dechavez Exp $"
/*======================================================================
 *
 *  Check for intra- or inter-record time jumps, return the size of
 *  the discrepancy in clock tics.
 *
 *  A negative return value indicates problems with input.
 *
 *====================================================================*/
#include "ida.h"

int ida_timerr(IDA_DHDR *crnt, IDA_DHDR *prev, int *sign, unsigned long *errptr)
{
unsigned long nsamp, beg, end, err, expinc, actinc;

    if (sign == NULL) return -1;

    if (prev == NULL) {         /* Check for intra-record errors */
        beg   = crnt->beg.sys;
        end   = crnt->end.sys;
        nsamp = (unsigned long) crnt->nsamp;
        if ( end < beg ) return -2;
    } else {                    /* Check for inter-record errors */
        if (prev->dl_stream != crnt->dl_stream) return -3;
        if (prev->srate     != crnt->srate)     return -4;
        if (prev->end.mult  != crnt->beg.mult)  return -5;
        beg   = prev->end.sys;
        end   = crnt->beg.sys;
        nsamp = (unsigned long) 2;
    }

/*  Expected time increment in tics and expected time stamp  */

    expinc = (nsamp - 1) * crnt->decim *
             (1000 / (crnt->srate * crnt->beg.mult));

/*  Actual time increment in tics  */

    if (end > beg) {
        actinc = end - beg;
        if (actinc == expinc) {
            err  = 0;
           *sign = 0;
        } else if (actinc > expinc) {
            err  = actinc - expinc;
           *sign = 1;
        } else {
            err  = expinc - actinc;
           *sign = -1;
        }
    } else {
        actinc = beg - end;
        err    = actinc + expinc;
       *sign   = -1;
    }

/*  Return size of error in tics  */

    *errptr = err;
    return 0;

}

unsigned long ida_offerr(IDA_DHDR *dhead, int *sign, unsigned long *expected)
{
unsigned long ext_inc, diff = 0;

    if (dhead->end.ext >= dhead->beg.ext) {
        ext_inc = (dhead->end.ext - dhead->beg.ext) * 1000 / dhead->beg.mult;
        *expected = dhead->beg.hz + ext_inc;
        if (dhead->end.hz > *expected) {
            diff = dhead->end.hz - *expected;
           *sign = 1;
        } else if (dhead->end.hz < *expected) {
            diff = *expected - dhead->end.hz;
           *sign = -1;
        } else {
            diff = 0;
           *sign = 0;
        }
    }

    return diff;
}

/* Revision History
 *
 * $Log: timerr.c,v $
 * Revision 1.3  2006/02/08 23:12:51  dechavez
 * ansi-style declarations
 *
 * Revision 1.2  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
