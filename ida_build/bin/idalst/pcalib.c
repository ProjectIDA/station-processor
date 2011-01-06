#pragma ident "$Id: pcalib.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Print summary of calibration record.
 *
 *====================================================================*/
#include "idalst.h"

#define ON TRUE

void pcalib(IDA_CALIB_REC *crec, struct counters *count, int print)
{
static long tagno = CALIB_TAG;

    tag(tagno++); printf("\n"); tag(tagno++); 
    printf("Calibration %s:\n", crec->state ? "Start" : "Stop");

    tag(tagno++);
    printf("%7ld ", count->rec);
    printf("%s ", tru_timstr(&crec->ttag, NULL));
    switch (crec->func) {
        case IDA_IMPULSE:  printf("IMP "); break;
        case IDA_SQUARE:   printf("SQR "); break;
        case IDA_SINE:     printf("SIN "); break;
        case IDA_TRIANGLE: printf("TRI "); break;
        case IDA_RANDOM:   printf("RAN "); break;
        default:            printf("??? "); break;
    }
    printf("%hd ", crec->atten);
    printf("%hd ", crec->period);
    printf("%hd ", crec->seed);
    printf("\n");
}

/* Revision History
 *
 * $Log: pcalib.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
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
