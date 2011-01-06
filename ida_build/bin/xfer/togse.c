#pragma ident "$Id: togse.c,v 1.2 2003/12/10 06:27:25 dechavez Exp $"
/*======================================================================
 *
 *  Demultiplex a xfer_packet format data stream into GSE2.0 CM6 or
 *  CM8 format.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "util.h"
#include "xfer.h"

void catch_signal(sig)
int sig;
{
    util_log(1, "%s", util_sigtoa(sig));
    exit(1);
}

void help(myname)
char *myname;
{
    fprintf(stderr, "usage: %s [options]\n", myname);
    fprintf(stderr,"\nOptions:\n");
    fprintf(stderr,"if=name     => input file name\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"linlen=len  => set maximum output line length\n");
    fprintf(stderr,"diff=[0|1|2]=> desired differencing\n");
    fprintf(stderr,"clip=flag   => if set, clip data at CM6 max\n");
    fprintf(stderr,"format=cm6  => output GSE2.0 CM6 format\n");
    fprintf(stderr,"format=cm8  => output GSE2.0 CM8 format\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "default: if=stdin debug=0 linlen=80 diff=2 clip=0 format=cm6\n");
    exit(1);
}

int main(int argc, char **argv)
{
int format = 6;
int diff   = 2;
int clip   = 0;
int debug  = -1;
int linlen = 80;
int i, status;
FILE *fp = stdin;
char *log   = NULL;
char *iname = NULL;
char *tmp;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            if (debug < 0) debug = 1;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "linlen=", strlen("linlen=")) == 0) {
            linlen = atoi(argv[i] + strlen("linlen="));
        } else if (strncmp(argv[i], "diff=", strlen("diff=")) == 0) {
            diff = atoi(argv[i] + strlen("diff="));
        } else if (strncmp(argv[i], "clip=", strlen("clip=")) == 0) {
            clip = atoi(argv[i] + strlen("clip="));
        } else if (strncmp(argv[i], "format=", strlen("format=")) == 0) {
            tmp = util_lcase(argv[i]+strlen("format="));
            if (strcmp(tmp, "cm6") == 0) {
                format = 6;
            } else if (strcmp(tmp, "cm8") == 0) {
                format = 8;
            } else {
                help(argv[0]);
            }
        } else {
            help(argv[0]);
        }
    }

    if (iname != NULL && (fp = fopen(iname, "rb")) == NULL) {
        perror(iname);
        exit(1);
    }

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);

/*  Convert to desired format  */

    if ((status = Xfer_ToGSE(fp, format, linlen, diff, clip, 0)) != 0) {
        fprintf(stderr,"Xfer_ToGSE failed with status %d\n", status);
    }

    exit(status);

}

/* Revision History
 *
 * $Log: togse.c,v $
 * Revision 1.2  2003/12/10 06:27:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
