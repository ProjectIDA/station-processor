#pragma ident "$Id: pdunmap.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Print data record header contents.
 *
 *====================================================================*/
#include "idalst.h"

void pdunmap(IDA_DHDR *head, struct counters *count, int print)
{
static long tagno = DUNMAP_TAG;

    if (tagno == DUNMAP_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("UNMAPPED (DL->INTERNAL) DATA STREAM SUMMARY\n");
        tag(tagno++); printf("Record  Stream    Chan    Filt  Mode\n");
    }

    tag(tagno++);
    printf("%6ld",    count->rec);
    printf("  %02hd", head->dl_stream);
    printf("  %02hd", head->dl_channel);
    printf("  %02hd", head->dl_filter);
    switch (head->mode) {
      case CONT: printf("  cont"); break;
      case TRIG: printf("  trig"); break;
      default:   printf("  ????"); break;
    }
    printf("\n");

}

/* Revision History
 *
 * $Log: pdunmap.c,v $
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
