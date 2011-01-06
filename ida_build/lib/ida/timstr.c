#pragma ident "$Id: timstr.c,v 1.6 2008/01/25 18:24:02 dechavez Exp $"
/*======================================================================
 *
 *  Generate ASCII strings of different kinds of time.
 *
 *  For sys_timstr(), we assume that the time is in clock tics, each
 *  tic representing a time interval of more than one ms, and that
 *  the provided 'mult' term is the number of mss per tic.
 *
 *  For ext_timstr(), we assume that the time is in sconds.
 *
 *  For tru_timstr(), use a pointer to a time tag structure and make
 *  sure that the year is defined.
 *
 *  These all write to static memory so the are MT-unsafe.
 *
 *====================================================================*/
#include "ida.h"
#include "util.h"

#define SC_PER_MN  ((unsigned long)    60)
#define SC_PER_HR  ((unsigned long)  3600)
#define SC_PER_DA  ((unsigned long) 86400)

#define MS_PER_SC  ((unsigned long) 1000)
#define MS_PER_TIC (mult)

#define TICS_PER_SC (MS_PER_SC / MS_PER_TIC)
#define TICS_PER_MN (SC_PER_MN * TICS_PER_SC)
#define TICS_PER_HR (SC_PER_HR * TICS_PER_SC)
#define TICS_PER_DA (SC_PER_DA * TICS_PER_SC)

char *sys_timstr(unsigned long tics, unsigned long mult, char *buf)
{
static char illegal[]   = "ddd-hh:mm:ss.msc";
static char badmult[]   = "ddd-hh:mm:ss.msc + slop";
static char MT_unsafe[] = "ddd-hh:mm:ss.msc + slop";
char *systimstr;
int da = 0;
int hr = 0;
int mn = 0;
int sc = 0;
int ms = 0;

    if (mult > MS_PER_SC) {
        sprintf(badmult, "mult=0x%08x!", mult);
        return badmult;
    }

    systimstr =  buf != NULL ? buf : MT_unsafe;

    if (tics) da = tics / TICS_PER_DA; tics -= da * TICS_PER_DA;
    if (tics) hr = tics / TICS_PER_HR; tics -= hr * TICS_PER_HR;
    if (tics) mn = tics / TICS_PER_MN; tics -= mn * TICS_PER_MN;
    if (tics) sc = tics / TICS_PER_SC; tics -= sc * TICS_PER_SC;
    if (tics) ms = tics * MS_PER_TIC;

/*  Should not need this, but just make sure we don't clobber systimstr */

    if (da > 999) return illegal;
    if (da < -99) return illegal;
    if (hr >  99) return illegal;
    if (hr <  -9) return illegal;
    if (mn >  99) return illegal;
    if (mn <  -9) return illegal;
    if (sc >  99) return illegal;
    if (sc <  -9) return illegal;
    if (ms > 999) return illegal;
    if (ms < -99) return illegal;

/*  Make the string  */

    sprintf(systimstr,"%03d-%02d:%02d:%02d.%03d", da, hr, mn, sc, ms);

    return systimstr;
}

char *ext_timstr(unsigned long sec, char *buf)
{
static char illegal[]   = "ddd-hh:mm:ss";
static char MT_unsafe[] = "ddd-hh:mm:ss + slop";
char *exttimstr;
int da, hr, mn, sc;

    exttimstr =  buf != NULL ? buf : MT_unsafe;

    da = sec / SC_PER_DA; sec -= da * SC_PER_DA;
    hr = sec / SC_PER_HR; sec -= hr * SC_PER_HR;
    mn = sec / SC_PER_MN; sec -= mn * SC_PER_MN;
    sc = sec;

/*  Should not need this, but just make sure we don't clobber exttimstr */

    if (da > 999) return illegal;
    if (da < -99) return illegal;
    if (hr >  99) return illegal;
    if (hr <  -9) return illegal;
    if (mn >  99) return illegal;
    if (mn <  -9) return illegal;
    if (sc >  99) return illegal;
    if (sc <  -9) return illegal;

/*  Make the string  */

    sprintf(exttimstr,"%03d-%02d:%02d:%02d", da, hr, mn, sc);

    return exttimstr;
}

char *tru_timstr(IDA_TIME_TAG *tag, char *buf)
{
static char MT_unsafe[] = "yyyy:ddd-hh:mm:ss.msc + slop";
char *trutimstr;
unsigned long utime;
float ffrac;
int   ifrac;
char tmp[32];

    trutimstr =  buf != NULL ? buf : MT_unsafe;

    if (tag->year != IDA_UNKNOWN) return utilDttostr(tag->tru, 0, trutimstr);

    utime = (unsigned long) tag->tru;
    ffrac = (float) ((tag->tru - (double) utime) * 1000.0);
    ifrac = (int) ffrac;
    if (ffrac - (float) ifrac >= 0.5) ifrac++;
    sprintf(trutimstr, "xxxx:%s.%03d", ext_timstr(utime, tmp), ifrac);

    return trutimstr;
}

/* Revision History
 *
 * $Log: timstr.c,v $
 * Revision 1.6  2008/01/25 18:24:02  dechavez
 * fixed bug in sys_timstr() where bogus tic intervals in corrupt headers could cause division by zero
 *
 * Revision 1.5  2008/01/16 23:22:51  dechavez
 * added destination buffers for optional MT-safety
 *
 * Revision 1.4  2006/02/08 23:19:54  dechavez
 * ansi-style declarations
 *
 * Revision 1.3  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.2  2004/06/21 19:37:29  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.1.1.1  2000/02/08 20:20:26  dec
 * import existing IDA/NRTS sources
 *
 */
