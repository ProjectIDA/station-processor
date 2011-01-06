#pragma ident "$Id: cktoff.c,v 1.5 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Check for time offset errors (within a single record)
 *
 *====================================================================*/
#include "idalst.h"

int cktoff(IDA_DHDR *dhead, long record, int print)
{
unsigned long err, expected;
int sign;
static long tagno = TOFFERR_TAG;
static int first = 1;

    err = ida_offerr(dhead, &sign, &expected);
    if (err <= 1) return 0;

    if (first) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("TIME OFFSET ERROR SUMMARY\n");
        tag(tagno++); printf("St Record     Ext. Time      Q    1-Hz Time");
                      printf("         Expected         Difference\n");
        first = 0;
    }

    tag(tagno++);
    printf("%02d",  dhead->dl_stream);
    printf(" %6ld", record);
    if (dhead->beg.year == 9999) {
        printf(" %s",   util_lttostr(dhead->beg.year, 0));
    } else {
        printf(" yyyy:%s", ext_timstr(dhead->beg.ext, NULL));
    }
    printf(" %2d",  dhead->beg.qual);
    printf(" %s",   sys_timstr(dhead->beg.hz,  dhead->beg.mult, NULL));
    printf("\n");
    tag(tagno++);
    printf("         ");
    if (dhead->end.year == 9999) {
        printf(" %s",   util_lttostr(dhead->end.year, 0));
    } else {
        printf(" yyyy:%s",   ext_timstr(dhead->end.ext, NULL));
    }
    printf(" %2d",  dhead->end.qual);
    printf(" %s",   sys_timstr(dhead->end.hz,  dhead->end.mult, NULL));
    printf(" %s",   sys_timstr(expected, dhead->end.mult, NULL));
    printf(" %c",   sign > 0 ? '+' : '-');
    printf("%s",    sys_timstr(err,  dhead->end.mult, NULL));
    printf("\n");

    return 1;
}

/* Revision History
 *
 * $Log: cktoff.c,v $
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
