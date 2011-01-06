#pragma ident "$Id: ckdupadj.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Look for adjacent duplicate records.
 *
 *====================================================================*/
#include "idalst.h"

int ckdupadj(char *buffer, struct counters *count, int print)
{
static int first = 1;
static char prev[IDA_BUFSIZ];
static long tagno = DUPADJ_TAG;

    if (first) {
        memcpy(prev, buffer, IDA_BUFSIZ);
        first = 0;
        return 0;
    }

    if (memcmp(prev, buffer, IDA_BUFSIZ) == 0) {
        ++count->dupadj;
        if (tagno == DUPADJ_TAG) {
            tag(tagno++); printf("\n"); tag(tagno++);
            printf("DUPLICATE ADJACENT RECORDS\n");
        }
        tag(tagno++);
        printf("Record %ld ", count->rec);
        printf("is identical to %ld ", count->rec-1);
        printf("type = ");
        switch (ida_rtype(buffer, ida->rev.value)) {
            case IDA_DATA:    printf("data");           break;
            case IDA_CALIB:   printf("calibration");    break;
            case IDA_LOG:     printf("log");            break;
            case IDA_CONFIG:  printf("configuration");  break;
            case IDA_EVENT:   printf("event");          break;
            case IDA_IDENT:   printf("identification"); break;
            case IDA_POSTHDR: printf("identification"); break;
            case IDA_OLDHDR:  printf("identification"); break;
            case IDA_DMPREC:  printf("tapedmp");        break;
            default:           printf("unknown");        break;
        }
        printf("\n");
        return 1;
    }

    return 0;
}

/* Revision History
 *
 * $Log: ckdupadj.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2003/06/11 20:26:18  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.2  2002/03/28 02:51:51  dec
 * fixed bug in reporting duplicate record indices
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
