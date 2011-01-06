#pragma ident "$Id: dodmp.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is a tapedmp record...
 *
 *====================================================================*/
#include "idalst.h"

void dodmp(char *buffer, struct counters *count, int print)
{
static long tagno = DMPREC_TAG;
int   len;
short *ptr;
short version, exit, numfil, max_retry, nrderr, maxfil;
short crnt_fno, next_fno, crnt_len, next_len;
short from, to;

    if (!(print & P_DCCREC)) return;

    tag(tagno++); printf("\n"); tag(tagno++);
    printf("Record %ld was added by program \"tapedmp\":\n",count->rec);
    ptr = (short *) (buffer + strlen("tapedmp "));
    len = IDA_BUFSIZ - strlen("tapedmp ");
    SSWAB(ptr, (long) len/sizeof(short));
    version = *ptr++;
    if (version == 1 || version == 2) {
        exit      = *ptr++; numfil = *ptr++;
        max_retry = *ptr++; nrderr = *ptr++;
        tag(tagno++); printf("No. physical files  = %hd\n", numfil);
        if (version == 2) {
            if ((maxfil = *ptr++) != 0) {
                tag(tagno++);
                printf("Max. files to spool = %hd\n", maxfil);
            }
        }
        tag(tagno++); printf("No. read errors     = %hd\n", nrderr);
        if (nrderr > 0) {
            tag(tagno++); printf("Retry limit         = %hd\n", max_retry);
        }

        crnt_fno = *ptr++; crnt_len = *ptr++;
        next_fno = *ptr++; next_len = *ptr++;
        while (crnt_len > 0) {
            from = crnt_fno; 
            to = (next_len == 0) ? next_fno : next_fno - 1;
            tag(tagno++); 
            printf("Files %3hd thru %3hd contained ", from, to);
            printf("%3hd records.\n", crnt_len);
            crnt_fno = next_fno; crnt_len = next_len;
            next_fno = *ptr++; next_len = *ptr++;
        }
        if (exit != 0) {
            if (version == 2 && numfil == maxfil) {
                ;
            } else {
                tag(tagno++); 
                printf("Program \"tapedmp\" exited ");
                printf("abnormally, due to error code %hd.\n", exit);
            }
        }
    } else {
        tag(tagno++);
        printf("Summary version %hd is not supported.\n", version);
    }
    return;
}

/* Revision History
 *
 * $Log: dodmp.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
