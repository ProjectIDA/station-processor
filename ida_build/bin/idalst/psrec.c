#pragma ident "$Id: psrec.c,v 1.3 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Print summary of das status records
 *
 *====================================================================*/
#include "idalst.h"

void psrec(IDA_DAS_STATS *srec, struct counters *count, int print)
{
static long tagno  = DASSTAT_TAG;

    if (tagno == DASSTAT_TAG) {
        tag(tagno++); printf("\n"); 
        tag(tagno++); printf("DAS STATUS RECORD SUMMARY\n");
        tag(tagno++); printf("Record     NAKs     ACKs  Dropped ");
                      printf("Timeouts Triggers  Sequence #\n");
    }

    tag(tagno++);
    printf("%6ld",     count->rec);
    printf(" %8ld",    srec->naks);
    printf(" %8ld",    srec->acks);
    printf(" %8ld",    srec->dropped);
    printf(" %8ld",    srec->timeouts);
    printf(" %8ld",    srec->triggers);
    printf("  0x%08x", srec->seqno);
    printf("\n");
}

/* Revision History
 *
 * $Log: psrec.c,v $
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
