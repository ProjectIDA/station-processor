#pragma ident "$Id: wdedit.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Read a wfdisc from stdin and send to stdout a new wfdisc which 
 *  has some/all fields modifed as per the command line.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"
#define MINARGC 2

int main(int argc, char **argv)
{
int i;
struct wfdisc wd;

    if (argc < MINARGC) {
        fprintf(stderr,"usage: wdedit name=value... <>\n");
        exit(1);
    }

    while (rwfdrec(stdin, &wd) == 0) {
        for (i = 1; i < argc; i++) {

            if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
                strcpy(wd.sta, argv[i]+strlen("sta="));

            } else if (strncmp(argv[i], "chan=", strlen("chan=")) == 0) {
                strcpy(wd.chan, argv[i]+strlen("chan="));

            } else if (strncmp(argv[i], "time=", strlen("time=")) == 0) {
                wd.time = atof(argv[i]+strlen("time="));

            } else if (strncmp(argv[i], "wfid=", strlen("wfid=")) == 0) {
                wd.wfid = atol(argv[i]+strlen("wfid="));

            } else if (strncmp(argv[i], "chanid=", strlen("chanid=")) == 0) {
                wd.chanid = atol(argv[i]+strlen("chanid="));

            } else if (strncmp(argv[i], "jdate=", strlen("jdate=")) == 0) {
                wd.jdate = atol(argv[i]+strlen("jdate="));

            } else if (strncmp(argv[i], "endtime=", strlen("endtime=")) == 0) {
                wd.endtime = atof(argv[i]+strlen("endtime="));

            } else if (strncmp(argv[i], "nsamp=", strlen("nsamp=")) == 0) {
                wd.nsamp = atol(argv[i]+strlen("nsamp="));

            } else if (strncmp(argv[i], "smprate=", strlen("smprate=")) == 0) {
                wd.smprate = atof(argv[i]+strlen("smprate="));

            } else if (strncmp(argv[i], "calib=", strlen("calib=")) == 0) {
                wd.calib = atof(argv[i]+strlen("calib="));

            } else if (strncmp(argv[i], "calper=", strlen("calper=")) == 0) {
                wd.calper = atof(argv[i]+strlen("calper="));

            } else if (strncmp(argv[i], "instype=", strlen("instype=")) == 0) {
                strcpy(wd.instype, argv[i]+strlen("instype="));

            } else if (strncmp(argv[i], "segtype=", strlen("segtype=")) == 0) {
                wd.segtype = (argv[i]+strlen("segtype="))[0];

            } else if (strncmp(argv[i], "datatype=",strlen("datatype=")) == 0) {
                strcpy(wd.datatype, argv[i]+strlen("datatype="));

            } else if (strncmp(argv[i], "clip=", strlen("clip=")) == 0) {
                wd.clip = (argv[i]+strlen("clip="))[0];

            } else if (strncmp(argv[i], "dir=", strlen("dir=")) == 0) {
                strcpy(wd.dir, argv[i]+strlen("dir="));

            } else if (strncmp(argv[i], "dfile=", strlen("dfile=")) == 0) {
                strcpy(wd.dfile, argv[i]+strlen("dfile="));

            } else if (strncmp(argv[i], "foff=", strlen("foff=")) == 0) {
                wd.foff = atol(argv[i]+strlen("foff="));

            } else if (strncmp(argv[i], "commid=", strlen("commid=")) == 0) {
                wd.commid = atol(argv[i]+strlen("commid="));

            } else if (strncmp(argv[i], "lddate=", strlen("lddate=")) == 0) {
                strcpy(wd.lddate, argv[i]+strlen("lddate="));

            } else {
                fprintf(stderr,"wdedit: unrecognized argument ");
                fprintf(stderr,"'%s' ignored\n",argv[i]);
            }
        }
        wwfdisc(stdout, &wd);
    }
    exit(0);
}

/* Revision History
 *
 * $Log: wdedit.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
