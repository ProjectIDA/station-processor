#pragma ident "$Id: ttag.c,v 1.1 2000/11/02 20:27:33 dec Exp $"
/*======================================================================
 *
 * System time tag operations.  These convoluted arithmetic operations
 * are for the benefit of systems which lack 64 bit integers (VxWorks).
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "sanio.h"

static int minimum_code = 0;
static int maximum_code = 6;
static char *bad_code        = "<invalid format code>";
static char *bad_input       = "<illegal input (neg)>";
static char *undefined_input = "<**undefined time!**>";
static char *cd2_undefined   = "0000000 00:00:00.000";

#define SPM (       60L)
#define SPH (SPM *  60L)
#define SPD (SPH *  24L)
#define SPY (SPD * 365L)

#define IsLeap(year) (((year)%4 == 0 && (year)%100 != 0) || (year)%400 == 0)
#define DaysPer(year) (365 + IsLeap(year))
#define SecsPer(year) (86400 * (DaysPer(year)))

static BOOL CheckArgs(SAN_STIME *a, SAN_STIME *b, SAN_STIME *c)
{
    if (
        (a == (SAN_STIME *) NULL) ||
        (b == (SAN_STIME *) NULL) ||
        (c == (SAN_STIME *) NULL)
    ) {
        errno = EINVAL;
        return FALSE;
    }

    return TRUE;
}

/* Take a date and turn it into an SAN_STIME epoch time */

VOID ttagStimeBuild(
    int year, int day, int hr, int mn, int sec, int ms, SAN_STIME *out
){
int i;

    if (year < SAN_EPOCH_YEAR) return;

    out->sec = 0;
    out->msc = (UINT16) ms;
    out->sign = 1;

    for (i = SAN_EPOCH_YEAR; i < year; i++) out->sec += SecsPer(i);

    out->sec += day * 86400;
    out->sec += hr * 3600;
    out->sec += mn * 60;
    out->sec += sec;
}

/* Decompose an SAN_STIME epoch time into pieces...
 * Requires positive times (ie, always later than epoch start.
 * We DON'T check!
 */

VOID ttagStimeSplit(SAN_STIME *tag, int start, int *pyr, int *psec, int *pmsc)
{
int year;
UINT32 sec, ysec;

    year = start;
    sec  = tag->sec;
    while (sec > (ysec = SecsPer(year))) {
        sec -= ysec;
        ++year;
    }
    
    *pyr = year;
    *psec = sec;
    *pmsc = tag->msc;
}

/*  Given a SAN_STIME value and format code, make a string of one of
 *  the following formats:
 *
 *  Format code   Output string
 *       0        yyyy:ddd-hh:mm:ss.msc, where time is a date
 *       1        yyyy:ddd-hh:mm:ss,     where time is a date
 *       2        yyyy:ddd-hh:mm:ss.msc, where input time is an interval
 *       3        yyyy:ddd-hh:mm:ss,     where time is an interval
 *       4        ddd-hh:mm:ss.msc, where input time is an interval
 *       5        ddd-hh:mm:ss, where input time is an interval
 *       6        yyyyddd hh:mm:ss.msc, where time is a date
 *       7        as 6, but {0, 0, -2} => 0000000 00:00:00;msc
 */

char *ttagStimeString(SAN_STIME *stime, int code, char *buf)
{
UINT32 sec;
int    yr, da, hr, mn, sc, ms, sign;

    if (code < 0) {
        code = -code;
        sign = -1;
    } else {
        sign = 1;
    }

    if (code < minimum_code || code > maximum_code) {
        strcpy(buf, bad_code);
        return buf;
    }

    if (ttagStimeUndefined(stime)) {
        strcpy(buf, undefined_input);
        return buf;
    }

/* Deal with the intervals */

    if (code != 0 && code != 1 && code != 6 && code != 7) {

        sec = stime->sec;
        ms  = (int) stime->msc;

        yr = sec / SPY; sec -= yr * SPY;
        da = sec / SPD; sec -= da * SPD;
        hr = sec / SPH; sec -= hr * SPH;
        mn = sec / SPM; sec -= mn * SPM;
        sc = sec;
        buf[0] = 0;
        if (stime->sign < 0 && sign < 0) sprintf(buf, "-");
        if (code == 2) {
            sprintf(buf+strlen(buf),"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                yr, da, hr, mn, sc, ms
            );
        } else if (code == 3) {
            sprintf(buf+strlen(buf),"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d",
                yr, da, hr, mn, sc
            );
        } else if (code == 4) {
            sprintf(buf+strlen(buf),"%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
                da, hr, mn, sc, ms
            );
        } else {
            sprintf(buf+strlen(buf),"%3.3d-%2.2d:%2.2d:%2.2d",
                da, hr, mn, sc
            );
        }
        return buf;
    }

/* Must be a time stamp */

    if (code == 7 && stime->sec == 0) return cd2_undefined;
    if (stime->sign < 0) return bad_input;
    ttagStimeSplit(stime, SAN_EPOCH_YEAR, &yr, &sc, &ms);
    da = sc / SPD; sc -= da * SPD; da += 1;
    hr = sc / SPH; sc -= hr * SPH;
    mn = sc / SPM; sc -= mn * SPM;

    if (code == 0) {
        sprintf(buf,"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d.%3.3d",
            yr, da, hr, mn, sc, ms
        );
    } else if (code == 6 || code == 7) {
        sprintf(buf,"%4.4d%3.3d %2.2d:%2.2d:%2.2d.%3.3d",
            yr, da, hr, mn, sc, ms
        );
    } else {
        sprintf(buf,"%4.4d:%3.3d-%2.2d:%2.2d:%2.2d",
            yr, da, hr, mn, sc
        );
    }

    return buf;
}

/* qsort() compatible compare function */

int ttagStimeCompare(void *aptr, void *bptr)
{
int factor;
SAN_STIME *a, *b;

    a = (SAN_STIME *) aptr;
    b = (SAN_STIME *) bptr;

    if (a->sign >= 0 && b->sign <  0) return  1;
    if (a->sign <  0 && b->sign >= 0) return -1;

    factor = (a->sign >= 0) ? 1 : -1;

    if (a->sec > b->sec) return  1 * factor;
    if (a->sec < b->sec) return -1 * factor;
    if (a->msc > b->msc) return  1 * factor;
    if (b->msc < b->msc) return -1 * factor;

    return 0;
}

/* Add two time tags */

BOOL ttagStimeAdd(SAN_STIME *a, SAN_STIME *b, SAN_STIME *c)
{
SAN_STIME atmp, btmp;
int compare;

    if (!CheckArgs(a, b, c)) return FALSE;

/* Case 1: both arguments are positive */

    if (a->sign >= 0 && b->sign >= 0) {
        c->sec = a->sec + b->sec;
        c->msc = a->msc + b->msc;
        if (c->msc > 999) {
            c->msc -= 1000;
            c->sec += 1;
        }
        c->sign = a->sign;
        return TRUE;
    }

/* Case 2: both arguments are negative */

    if (a->sign < 0 && b->sign < 0) {
        atmp = *a;
        btmp = *b;
        atmp.sign = 1;
        btmp.sign = 1;
        ttagStimeAdd(&atmp, &btmp, c);
        c->sign = -1;
        return TRUE;
    }

/* Case 3: a is negative, b is positive */

    if (a->sign < 0) {
        ttagStimeAdd(b, a, c);
        return TRUE;
    }

/* Case 4a: a is positive, b is negative, abs(a) > abs(b) */

    btmp = *b;
    btmp.sign = 1;
    compare = ttagStimeCompare(a, &btmp);
    if (compare == 1) {
        if (a->msc < b->msc) {
            c->sec = a->sec - b->sec - 1;
            c->msc = 1000 + a->msc - b->msc;
        } else if (a->msc > b->msc) {
            c->sec = a->sec - b->sec;
            c->msc = a->msc - b->msc;
        } else {
            c->sec = a->sec - b->sec;
            c->msc = 0;
        }
        c->sign = a->sign;
        return TRUE;
    }

/* Case 4b: a is positive, b is negative, abs(a) < abs(b) */

    if (compare == -1) {
        atmp = *a;
        btmp = *b;
        atmp.sign *= -1;
        btmp.sign *= -1;
        ttagStimeAdd(&btmp, &atmp, c);
        c->sign *= -1;
        return TRUE;
    }

/* Case 4c: a is positive, b is negative, abs(a) == abs(b) */

    c->sec = 0;
    c->msc = 0;
    c->sign = 0;

    return TRUE;
}

/* Subtract two time tags */

BOOL ttagStimeSub(SAN_STIME *a, SAN_STIME *b, SAN_STIME *c)
{
SAN_STIME btmp;

    if (!CheckArgs(a, b, c)) return FALSE;

    btmp = *b;
    btmp.sign *= -1;
    ttagStimeAdd(a, &btmp, c);

    return TRUE;
}

/* Revision History
 *
 * $Log: ttag.c,v $
 * Revision 1.1  2000/11/02 20:27:33  dec
 * Initial working release
 *
 */
