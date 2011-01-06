#pragma ident "$Id: pdata.c,v 1.6 2008/03/05 23:02:16 dechavez Exp $"
/*======================================================================
 *
 *  Print data record header contents.
 *
 *====================================================================*/
#include "idalst.h"

void pdata (IDA_DREC *drec, struct counters *count, int print)
{
short *sdata;
long  *ldata;
IDA_DHDR *head;
char tbuf[64];
static long tagno = DATA_TAG;

    sdata = (short *) drec->data;
    ldata = (long  *) drec->data;
    head  = &drec->head;

    if (tagno == DATA_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("DATA RECORD HEADER SUMMARY\n");
        tag(tagno++); printf("Id Record DAC B Pts   Sint   Sys ");
                      printf("Time-Tag    Q  Corrected Time-Tag");
                      if (print & P_SEQNO) printf("     Sequence #   Creation Tstamp");
                      printf("\n");
    }

    tag(tagno++);
    printf("%02hd", head->dl_stream);
    printf("%7ld",  count->rec);
    if (head->atod == IDA_DAS) {
        printf(" DAS");
    } else if (head->atod == IDA_ARS) {
        printf(" ARS");
    } else {
        printf(" ???");
    }
    printf("%2d",   head->wrdsiz);
    printf("%4d",   head->nsamp);
    printf("%7.3f", head->sint);
    printf(" %s",   sys_timstr(head->beg.sys, head->beg.mult, NULL));
    printf("  %d",  head->beg.qual);
	printf("%s", (head->beg.error & TTAG_LEAP_YEAR_PATCH) ? "L" : " ");
    printf("%s",   tru_timstr(&head->beg, NULL));
    if (print & P_SEQNO && head->extra.valid) printf("   0x%08x  %s", head->extra.seqno, utilLttostr(head->extra.tstamp, 1000, tbuf));
    printf("\n");

}

/* Revision History
 *
 * $Log: pdata.c,v $
 * Revision 1.6  2008/03/05 23:02:16  dechavez
 * note packets with leap year bug bit set by appending and L after clock quality
 *
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/08/26 20:22:42  dechavez
 * print sequence number and creation time stamps, if applicable
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
