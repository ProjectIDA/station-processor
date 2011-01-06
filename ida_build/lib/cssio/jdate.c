#pragma ident "$Id: jdate.c,v 1.2 2007/01/04 23:33:39 dechavez Exp $"
/*======================================================================
 *
 *  Given an epoch time, return the equivalent jdate.
 * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "cssio.h"

#ifdef HAVE_GMTIME_R

long cssio_jdate(double dtime)
{
time_t ltime;
struct tm tm;

    ltime = (time_t) dtime;
    gmtime_(&ltime, &tm);
    return ((1900 + tm.tm_year) * 1000) + tm.tm_yday + 1;
}

#else

long cssio_jdate(double dtime)
{
long  ltime;
struct tm *tiempo;

    ltime = (long) dtime;
    tiempo = gmtime(&ltime);
    return ((1900 + tiempo->tm_year) * 1000) + tiempo->tm_yday + 1;
}

#endif /* !HAVE_GMTIME_R */

/* Revision History
 *
 * $Log: jdate.c,v $
 * Revision 1.2  2007/01/04 23:33:39  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
