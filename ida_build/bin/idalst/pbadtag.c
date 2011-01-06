#pragma ident "$Id: pbadtag.c,v 1.4 2008/03/05 23:10:21 dechavez Exp $"
/*======================================================================
 *
 *  Print error message about illegal time tag.
 *
 *====================================================================*/
#include "idalst.h"

void pbadtag(IDA_TIME_TAG *ttag, struct counters *count, int type, int print)
{
char *rectyp, *problem;
static long tagno = BADTAG_TAG;

    if (ttag->error == TTAG_LEAP_YEAR_PATCH) return;

    if (tagno == BADTAG_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("SUMMARY OF ILLEGAL TIME TAGS\n");
        tag(tagno++); printf("Record   Type  External     1-Hz");
                      printf("        Sys     Mult  Qual Problem\n");
    }

    switch (type) {
      case IDA_DATA:   rectyp = "  data"; break;
      case IDA_IDENT:  rectyp = " ident"; break;
      case IDA_CONFIG: rectyp = "config"; break;
      case IDA_LOG:    rectyp = "   log"; break;
      case IDA_CALIB:  rectyp = " calib"; break;
      case IDA_EVENT:  rectyp = " event"; break;
      default:          rectyp = "NOTDEF"; break;
    }

    if (!ttag->error) {
        problem = "no problem, why am i here"; 
    } else if (ttag->error & TTAG_BAD_CLKRD) {
        problem = "corrupt clock data?";
    } else if (ttag->error & TTAG_BAD_OFFSET) {;
        problem = "bad offset (hz > sys)";
    } else if (ttag->error & TTAG_LOW_EXTTIM) {
        problem = "ext time too small";
    } else if (ttag->error & TTAG_BIG_EXTTIM) {
        problem = "ext time too large";
    } else if (ttag->error & TTAG_BAD_SYSTIM) {
        problem = "beg sys time >= end sys time";
    } else if (ttag->error & TTAG_LEAP_YEAR_PATCH) {
        problem = "leap year bug present";
    } else {
        problem = "UNKNOWN ERROR";
    }

    tag(tagno++);
    printf("%6ld", count->rec);
    printf(" %s",     rectyp);
    printf(" 0x%08x", ttag->ext);
    printf(" 0x%08x", ttag->hz);
    printf(" 0x%08x", ttag->sys);
    printf(" 0x%04x", ttag->mult);
    printf(" %c %2d", ttag->code, ttag->qual);
    printf(" %s",     problem);
    printf("\n");
}

/* Revision History
 *
 * $Log: pbadtag.c,v $
 * Revision 1.4  2008/03/05 23:10:21  dechavez
 * added TTAG_LEAP_YEAR_PATCH support
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:20  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
