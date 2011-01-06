#pragma ident "$Id: ckdupdat.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Look for duplicate data records.
 *
 *====================================================================*/
#include "idalst.h"

int ckdupdat(IDA_DHDR *prev_head, long prev_rec, IDA_DHDR *crnt_head, long crnt_rec, int print, struct counters *count)
{
static long tagno = DUPDAT_TAG;

    if (memcmp(prev_head, crnt_head, sizeof(IDA_DHDR)) != 0) return 0;

    ++count->dupdat; 

/*  Print out duplicate record headers if requested  */

    if (!(print & P_DUPDAT)) return 1;

    if (tagno == DUPDAT_TAG) {
        tag(tagno++); printf("\n"); 
        tag(tagno++); printf("DUPLICATE DATA RECORD HEADERS\n");
        tag(tagno++);
        printf("Prev #  Crnt #  Id DAC Pts   Sint   ");
        printf("Sys Time-Tag    Q  Corrected Time-Tag\n");
    }
    tag(tagno++);
    printf("%6ld  ", prev_rec);
    printf("%6ld  ", crnt_rec);
    printf("%02hd", crnt_head->dl_stream);
    if (crnt_head->atod == IDA_DAS) {
        printf(" DAS");
    } else if (crnt_head->atod == IDA_ARS) {
        printf(" ARS");
    } else {
        printf(" ???");
    }
    printf("%4d",   crnt_head->nsamp);
    printf("%7.3f", (float) crnt_head->decim/(float) crnt_head->srate);
    printf(" %s",   sys_timstr(crnt_head->beg.sys,crnt_head->beg.mult, NULL));
    printf("  %d",  crnt_head->beg.qual);
    printf(" %s",   tru_timstr(&crnt_head->beg, NULL));
    printf("\n");

    return 1;
}

/* Revision History
 *
 * $Log: ckdupdat.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:18  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
