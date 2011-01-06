#pragma ident "$Id: ext1hzoff.c,v 1.3 2006/02/08 23:05:51 dechavez Exp $"
/*======================================================================
 *
 *  Given a time tag, determine the external - 1Hz offset, in seconds.
 *
 *====================================================================*/
#include "ida.h"
#include "util.h"

double ida_ext1hzoff(IDA_TIME_TAG *tag)
{
double dext, d1hz;

/*  Convert 1Hz time to double seconds */

    d1hz  = (double) tag->hz;
    d1hz *= (double) tag->mult;
    d1hz /= (double) 1000;

/* Convert external time to double seconds */

    dext  = (double) tag->ext;
    if (tag->year != IDA_UNKNOWN && tag->year != IDA_EMBEDDED_YEAR) dext = (double) year_secs(tag->year);

/* Determine the offset */

    return dext - d1hz;
}

/* Revision History
 *
 * $Log: ext1hzoff.c,v $
 * Revision 1.3  2006/02/08 23:05:51  dechavez
 * replaced explicit contants with macro from ida.h
 *
 * Revision 1.2  2004/06/24 18:34:36  dechavez
 * fixed ancient bug in determining external time in seconds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
