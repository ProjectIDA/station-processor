#pragma ident "$Id: pevent.c,v 1.5 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Print summary of event record.
 *
 *====================================================================*/
#include "idalst.h"

void pevent(IDA_EVENT_REC *evtrec, struct counters *count, int print)
{
int i;
static long ecount = 0;
static long tagno  = EVENT_TAG;
char *durstr;

    if (tagno == EVENT_TAG) {
        tag(tagno++); printf("\n"); 
        tag(tagno++); printf("EVENT RECORD SUMMARY\n");
        tag(tagno++); printf("Count      Event Start          ");
                      printf("Event Stop         Duration     ");
                      printf("LTA   STA Ratio\n");
    }

    for (i = 0; i < evtrec->nevent; i++) {
        tag(tagno++);
        printf("%5ld  ", ++ecount);
        printf("%s ",    tru_timstr(&evtrec->event[i].beg, NULL));
        printf(" %s ",   tru_timstr(&evtrec->event[i].end, NULL));
        durstr  = util_dttostr(evtrec->event[i].duration, 0);
        durstr += strlen("yyyy:ddd-");
        printf(" %s",    durstr);
        printf("%6ld",   evtrec->event[i].lta);
        printf("%6ld",   evtrec->event[i].sta);
        if (evtrec->event[i].lta) {
            printf("%6ld",   evtrec->event[i].sta / evtrec->event[i].lta);
        } else {
            printf("   NaN");
        }
        printf("\n");
    }
}

/* Revision History
 *
 * $Log: pevent.c,v $
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
