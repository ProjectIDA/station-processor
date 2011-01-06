#pragma ident "$Id: pttrip.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Print time_tag elements.
 *
 *====================================================================*/
#include "idalst.h"

void pttrip(IDA_DHDR *dhead, struct counters  *count, int print)
{
static long tagno = TTRIP_TAG;

    if (tagno == TTRIP_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("DATA RECORD TIME TAG CONTENTS\n");
        tag(tagno++); printf("Record Str     Ext. time      Q     ");
                      printf("1-Hz time    Datum (sys) time   ");
                      printf("Corrected time");
                      printf("\n");
    }

    tag(tagno++);
    printf("%6ld",   count->rec);
    printf("  %02d", dhead->dl_stream);
    if (dhead->beg.year == 9999) {
        printf(" %s ",   util_lttostr(dhead->beg.ext, 0));
    } else {
        printf(" yyyy:%s ",   ext_timstr(dhead->beg.ext, NULL));
    }
    printf("%2d",    dhead->beg.qual);
    printf(" %s",    sys_timstr(dhead->beg.hz,  dhead->beg.mult, NULL));
    printf(" %s",    sys_timstr(dhead->beg.sys, dhead->beg.mult, NULL));
    printf(" %s",    tru_timstr(&dhead->beg, NULL));
    printf("\n");

    tag(tagno++);
    printf("          ");
    if (dhead->end.year == 9999) {
        printf(" %s ",   util_lttostr(dhead->end.ext, 0));
    } else {
        printf(" yyyy:%s ",   ext_timstr(dhead->end.ext, NULL));
    }
    printf("%2d",    dhead->end.qual);
    printf(" %s",    sys_timstr(dhead->end.hz,  dhead->end.mult, NULL));
    printf(" %s",    sys_timstr(dhead->end.sys, dhead->end.mult, NULL));
    printf(" %s",    tru_timstr(&dhead->end, NULL));
    printf("\n");
}

/* Revision History
 *
 * $Log: pttrip.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:22  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
