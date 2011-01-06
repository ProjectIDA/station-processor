#pragma ident "$Id: pcorrupt.c,v 1.3 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Print error message about corrupt record.
 *
 *====================================================================*/
#include "idalst.h"

void pcorrupt(struct counters *count, int type, int status, int print)
{
char *rectyp;
static long tagno = CORRUPT_TAG;

    if (tagno == CORRUPT_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("SUMMARY OF CORRUPT (UNLOADABLE) RECORDS\n");
        tag(tagno++); printf("Record    Type  Status\n");
    }

    switch (type) {
      case IDA_UNKNOWN:  rectyp = "unknown"; break;
      case IDA_IDENT:    rectyp = "  ident"; break;
      case IDA_DATA:     rectyp = "   data"; break; 
      case IDA_CONFIG:   rectyp = " config"; break;
      case IDA_LOG:      rectyp = "    log"; break;
      case IDA_CALIB:    rectyp = "  calib"; break;
      case IDA_EVENT:    rectyp = "  event"; break;
      case IDA_OLDHDR:   rectyp = " oldhdr"; break;
      case IDA_OLDCALIB: rectyp = " oldcal"; break;
      case IDA_POSTHDR:  rectyp = "posthdr"; break;
      case IDA_RESERVED: rectyp = "reserve"; break;
      case IDA_DMPREC:   rectyp = " dmprec"; break;
      case IDA_LABEL:    rectyp = "  label"; break;
      case IDA_ARCEOF:   rectyp = " arceof"; break;
      default:            rectyp = " NOTDEF"; break;
    }

    tag(tagno++);
    printf("%6ld", count->rec);
    printf(" %s",  rectyp);
    printf(" %4d", status);
    printf("\n");
    
}

/* Revision History
 *
 * $Log: pcorrupt.c,v $
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
