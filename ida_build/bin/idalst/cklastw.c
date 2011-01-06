#pragma ident "$Id: cklastw.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Check data records for last word consistency.
 *
 *====================================================================*/
#include "idalst.h"

void cklastw(IDA_DREC *drec, struct counters *count, int print)
{
static long tagno = LASTW_TAG;
static UINT32 temp[IDA_MAXDLEN];
long last_word;

    last_word = ida_dcmp(temp, drec->data, drec->head.nsamp);

    if (last_word != drec->head.last_word) {
        if (tagno == LASTW_TAG) {
            tag(tagno++); printf("\n"); tag(tagno++);
            printf("COMPRESSION/DECOMPRESSION ERRORS\n");
            tag(tagno++);
            printf("Record # Stream    Corrected Time      ");
            printf("Expected lw   Computed lw\n");
        }
        tag(tagno++);
        printf( "%8ld",  count->rec);
        printf( "  %02d", drec->head.dl_stream);
        printf( " %s", tru_timstr(&drec->head.beg, NULL));
        printf( "    0x%08x", drec->head.last_word);
        printf( "    0x%08x", last_word);
        printf( "\n");
        ++count->lastw;
    }
}

/* Revision History
 *
 * $Log: cklastw.c,v $
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
