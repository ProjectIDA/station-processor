#pragma ident "$Id: wdecho.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Print the indicated wfdisc elements from the selected record.
 *  Exit status is the number of elements printed or negative on error.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"
#define MINARGC  2

int main(int argc, char **argv)
{
int i, count;
struct wfdisc wd;

    if (argc < MINARGC) {
        fprintf(stderr,"usage: wdecho name... <>\n");
        exit(-1);
    }

    count = 0;
    while (rwfdrec(stdin, &wd) == 0) {
        ++count;
        for (i = 1; i < argc; i++) {
            if (strncmp(argv[i], "sta", strlen("sta")) == 0) {
                printf(" %s", wd.sta);
    
            } else if (strncmp(argv[i], "chan", strlen("chan")) == 0) {
                printf(" %s", wd.chan);
    
            } else if (strncmp(argv[i], "time", strlen("time")) == 0) {
                printf(" %lf", wd.time);
    
            } else if (strncmp(argv[i], "wfid", strlen("wfid")) == 0) {
                printf(" %ld", wd.wfid);
    
            } else if (strncmp(argv[i], "chanid", strlen("chanid")) == 0) {
                printf(" %ld", wd.chanid);
    
            } else if (strncmp(argv[i], "jdate", strlen("jdate")) == 0) {
                printf(" %ld", wd.jdate);
    
            } else if (strncmp(argv[i], "endtime", strlen("endtime")) == 0) {
                printf(" %lf", wd.endtime);

            } else if (strncmp(argv[i], "nsamp", strlen("nsamp")) == 0) {
                printf(" %ld", wd.nsamp);
    
            } else if (strncmp(argv[i], "smprate", strlen("smprate")) == 0) {
                printf(" %f", wd.smprate);
    
            } else if (strncmp(argv[i], "calib", strlen("calib")) == 0) {
                printf(" %f", wd.calib);
    
            } else if (strncmp(argv[i], "calper", strlen("calper")) == 0) {
                printf(" %f", wd.calper);
    
            } else if (strncmp(argv[i], "instype", strlen("instype")) == 0) {
                printf(" %s", wd.instype);
    
            } else if (strncmp(argv[i], "segtype", strlen("segtype")) == 0) {
                printf(" %c", wd.segtype);
    
            } else if (strncmp(argv[i], "datatype", strlen("datatype")) == 0) {
                printf(" %s", wd.datatype);
    
            } else if (strncmp(argv[i], "clip", strlen("clip")) == 0) {
                printf(" %c", wd.clip);
    
            } else if (strncmp(argv[i], "dir", strlen("dir")) == 0) {
                printf(" %s", wd.dir);
    
            } else if (strncmp(argv[i], "dfile", strlen("dfile")) == 0) {
                printf(" %s", wd.dfile);
    
            } else if (strncmp(argv[i], "foff", strlen("foff")) == 0) {
                printf(" %ld", wd.foff);
    
            } else if (strncmp(argv[i], "commid", strlen("commid")) == 0) {
                printf(" %ld", wd.commid);
    
            } else if (strncmp(argv[i], "lddate", strlen("lddate")) == 0) {
                printf(" %s", wd.lddate);
    
            }
            printf("\n");
        }
    }
    exit(count);
}

/* Revision History
 *
 * $Log: wdecho.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
