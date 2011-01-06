#pragma ident "$Id: pident.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Print identification record contents.
 *
 *====================================================================*/
#include "idalst.h"

static long tagno = IDENT_TAG;

void pident(IDA_IDENT_REC *idrec, struct counters *count, int print)
{

    tag(tagno++); printf("\n"); tag(tagno++);
    printf("Record %ld is a", count->rec);
    if (idrec->atod == IDA_DAS) {
        printf(" DAS");
    } else if (idrec->atod == IDA_ARS) {
        printf("n ARS");
    } else {
        printf("n UNKNOWN");
    }
    printf(" identification record:\n");

    tag(tagno++);
    printf("Station %s\n", idrec->sname);
    tag(tagno++);
    printf("CPU ident             = %hd\n", idrec->atod_id);

    tag(tagno++);
    printf("Firmware revision     = %hd\n", idrec->atod_rev);

    if (idrec->atod == IDA_DAS) {
        tag(tagno++);
        printf("DSP firmware revision = %hd\n", idrec->dsp_rev);
    }

    if (idrec->ttag.year != 9999) {
        tag(tagno++);
        printf("Year                  = %hd\n", idrec->ttag.year);
        tag(tagno++);
        printf("External time stamp   = ");
        printf("%s\n",ext_timstr(idrec->ttag.ext, NULL));
    } else {
        tag(tagno++);
        printf("External time stamp   = ");
        printf("%s\n",util_lttostr(idrec->ttag.ext, 0));
    }

    if (idrec->atod == IDA_ARS) {
        tag(tagno++);
        printf("Prev tape insert      = ");
        printf("%s\n",ext_timstr(idrec->beg.ext, NULL));

        tag(tagno++);
        printf("Prev tape eject       = ");
        printf("%s\n",ext_timstr(idrec->end.ext, NULL));

        tag(tagno++);
        printf("Current tape seq no.  = %hd\n", idrec->tape_no);
    }
}

/* Revision History
 *
 * $Log: pident.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
