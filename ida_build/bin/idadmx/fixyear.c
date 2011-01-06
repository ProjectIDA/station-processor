#pragma ident "$Id: fixyear.c,v 1.4 2006/02/09 20:14:38 dechavez Exp $"
/*======================================================================
 *
 *  Make sure the year in the time stamp of the first datum is correct.
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;
extern IDA *ida;

static int begyr, endyr, begda, endda;

void init_fixyear(int beg_yr, int end_yr, int beg_da, int end_da)
{
time_t now;
int this_year, bad_da;

    now = time(NULL);
    this_year = gmtime(&now)->tm_year + 1900;

    begyr = (beg_yr < 0) ? this_year : beg_yr;
    endyr = (end_yr < 0) ? this_year : end_yr;

    if (begyr < 1900) begyr += 1900;
    if (endyr < 1900) endyr += 1900;

    if (begyr > endyr) {
        fprintf(stderr,"Beg year (%d) > End year (%d)\n", beg_yr, end_yr);
        exit(1);
    }

    begda = beg_da;
    endda = end_da;

    bad_da = 0;
    if (
        (beg_da <= 0 && end_da >  0) ||
        (beg_da >  0 && end_da <= 0) ||
        (beg_da > dysize(begyr))     ||
        (end_da > dysize(endyr))
    ) {
        fprintf(stderr,"Improper beg (%d) ", beg_da);
        fprintf(stderr,"and/or end (%d) days\n", end_da);
        exit(1);
    }
}

void fixyear(IDA_DHDR *dhead, int fixyear_flag)
{
int day;

/* Not much to do if we are not expecting a year change */

    if (fixyear_flag) {

        if (begyr == endyr) {
            dhead->beg.year = begyr;

/*  Otherwise we have to see if we have passed the year boundary  */

        } else {
            day = dhead->beg.ext / 86400; /* day of year of first datum */
            if (day >= begda) {
                dhead->beg.year = begyr;
            } else if (day <= endda) {
                dhead->beg.year = endyr;
            } else {
                sprintf(Buffer,"fixyear: problems with days: crnt/beg/end = ");
                sprintf(Buffer+strlen(Buffer), "%d/%d/%d", day, begda, endda);
                logmsg(Buffer);
                die(ABORT);
            }
        }
    }

    ida_filltime(ida, &dhead->beg);
    return;
}

/* Revision History
 *
 * $Log: fixyear.c,v $
 * Revision 1.4  2006/02/09 20:14:38  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.3  2005/02/16 18:27:52  dechavez
 * fixed typo in testing for year equality (aap)
 *
 * Revision 1.2  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
