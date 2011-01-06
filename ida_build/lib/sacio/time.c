#pragma ident "$Id: time.c,v 1.3 2007/01/04 23:34:27 dechavez Exp $"
/*======================================================================
 *
 *  Convert SAC reference time into double, and vice versa.
 *
 *====================================================================*/
#include <time.h>
#include "sacio.h"
 
#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif
 
#ifndef daysize
#define daysize(i) (365 + leap_year(i))
#endif

#define SPM (      60L) /* seconds per minute */
#define SPH (SPM * 60L) /* seconds per hour   */
#define SPD (SPH * 24L) /* seconds per day    */

static double sacio_ydhmsmtod(int yr,int da,int hr,int mn,int sc,int ms)
{
int i, days_in_year_part;
long   secs;

    days_in_year_part = 0;
    for (i = 1970; i < yr; i++) days_in_year_part += daysize(i);
    secs = (long) days_in_year_part * SPD;

    secs += (long)(da-1)*SPD + (long)hr*SPH + (long)mn*SPM + (long)sc;

    return (double) secs + ((double) (ms)/1000.0);

}

static void sacio_tsplit(double dtime, int *yr, int *da, int *hr, int *mn, int *sc, int *ms)
{
time_t ltime;
int   imsc;
double dmsc;
struct tm *tiempo;

    ltime = (long) dtime;
    dmsc = ((dtime - (double) ltime)) * (double) 1000.0;
    imsc = (int) dmsc;
    if (dmsc - (double) imsc >= (double) 0.5) imsc++;
    if (imsc == 1000) {
        ++ltime;
        imsc = 0;
    }

    tiempo = gmtime(&ltime);
    *yr = 1900 + tiempo->tm_year;
    *da = ++tiempo->tm_yday;
    *hr = tiempo->tm_hour;
    *mn = tiempo->tm_min;
    *sc = tiempo->tm_sec;
    *ms = imsc;

}

/* Take SAC header and return time of first sample */

double sacio_sttodt(hdr)
struct sac_header *hdr;
{
int i, yr, da, hr, mn, sc, ms;
double dtmp;

    if (hdr == NULL) return (double) -12345;

    if (hdr->nzyear < 0) return (double) -12345;
    if (hdr->nzjday < 0) return (double) -12345;
    if (hdr->nzhour < 0) return (double) -12345;
    if (hdr->nzmin  < 0) return (double) -12345;
    if (hdr->nzsec  < 0) return (double) -12345;
    if (hdr->nzmsec < 0) return (double) -12345;

    yr = (int) hdr->nzyear;
    da = (int) hdr->nzjday;
    hr = (int) hdr->nzhour;
    mn = (int) hdr->nzmin;
    sc = (int) hdr->nzsec;
    ms = (int) hdr->nzmsec;

    if (
        yr < 1970 ||
        da <    1 || da >  366 ||
        hr <    0 || hr >   23 ||
        mn <    0 || mn >   59 ||
        sc <    0 || sc >   59 ||
        ms <    0 || ms >  999
    ) return (double) -12345;

    return sacio_ydhmsmtod(yr,da,hr,mn,sc,ms);

}

/* Load time of first sample into SAC header */

void sacio_settime(struct sac_header *hdr, double tofs)
{
int i, yr, da, hr, mn, sc, ms;

	sacio_tsplit(tofs, &yr, &da, &hr, &mn, &sc, &ms);
    hdr->nzyear = yr;
    hdr->nzjday = da;
    hdr->nzhour = hr;
    hdr->nzmin  = mn;
    hdr->nzsec  = sc;
    hdr->nzmsec = ms;

	hdr->b = 0.0;
}

/* Revision History
 *
 * $Log: time.c,v $
 * Revision 1.3  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.2  2001/03/16 22:20:31  dec
 * fix y2k bug in sacio_sttodt
 *
 * Revision 1.1.1.1  2000/02/08 20:20:37  dec
 * import existing IDA/NRTS sources
 *
 */
