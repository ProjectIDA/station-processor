#pragma ident "$Id: tosac.c,v 1.2 2003/12/10 06:27:25 dechavez Exp $"
/*======================================================================
 *
 *  Filter to take xfer_packet format data and convert to SAC format.
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"
#include "null_sac.h"
#include "xfer.h"

void help(myname)
char *myname;
{
    fprintf(stderr, "usage: %s [options]\n", myname);
    fprintf(stderr,"\nOptions:\n");
    fprintf(stderr,"if=name     => input file name\n");
    fprintf(stderr,"evla=float  => event latitude\n");
    fprintf(stderr,"evlo=float  => event longitude\n");
    fprintf(stderr,"evel=float  => event elevation\n");
    fprintf(stderr,"evdp=float  => event depth\n");
    fprintf(stderr,"stla=float  => station latitude\n");
    fprintf(stderr,"stlo=float  => station longitude\n");
    fprintf(stderr,"stel=float  => station elevation\n");
    fprintf(stderr,"stdp=float  => station depth\n");
    fprintf(stderr,"dist=float  => event to station distance\n");
    fprintf(stderr,"az=float    => event to station azimuth\n");
    fprintf(stderr,"baz=float   => event to station back-azimuth\n");
    fprintf(stderr,"gcarc=float => great circle distance\n");
    fprintf(stderr,"userX=float => user defined, X=[0-9]\n");
    fprintf(stderr,"istreg=int  => station geographic region code\n");
    fprintf(stderr,"ievreg=int  => event geographic region code\n");
    fprintf(stderr,"ievtyp=int  => event type code\n");
    fprintf(stderr,"-ascii      => output ascii files\n");
    fprintf(stderr,"-binary     => output binary files\n");
    fprintf(stderr,"\ndefault: if=stdin -binary\n");
    exit(1);
}

int main(int argc, char **argv)
{
char *log = NULL;
int debug = 0;
int ascii = 0;
int i, status, nrec;
FILE *fp = stdin;
char *iname = NULL;
struct sac_header defaults;

    defaults = null_sac_header;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "evla=", strlen("evla=")) == 0) {
            defaults.evla = atof(argv[i] + strlen("evla="));
        } else if (strncmp(argv[i], "evlo=", strlen("evlo=")) == 0) {
            defaults.evlo = atof(argv[i] + strlen("evlo="));
        } else if (strncmp(argv[i], "evel=", strlen("evel=")) == 0) {
            defaults.evel = atof(argv[i] + strlen("evel="));
        } else if (strncmp(argv[i], "evdp=", strlen("evdp=")) == 0) {
            defaults.evdp = atof(argv[i] + strlen("evdp="));
        } else if (strncmp(argv[i], "stla=", strlen("stla=")) == 0) {
            defaults.stla = atof(argv[i] + strlen("stla="));
        } else if (strncmp(argv[i], "stlo=", strlen("stlo=")) == 0) {
            defaults.stlo = atof(argv[i] + strlen("stlo="));
        } else if (strncmp(argv[i], "stel=", strlen("stel=")) == 0) {
            defaults.stel = atof(argv[i] + strlen("stel="));
        } else if (strncmp(argv[i], "stdp=", strlen("stdp=")) == 0) {
            defaults.stdp = atof(argv[i] + strlen("stdp="));
        } else if (strncmp(argv[i], "dist=", strlen("dist=")) == 0) {
            defaults.dist = atof(argv[i] + strlen("dist="));
        } else if (strncmp(argv[i], "user0=", strlen("user0=")) == 0) {
            defaults.user0 = atof(argv[i] + strlen("user0="));
        } else if (strncmp(argv[i], "user1=", strlen("user1=")) == 0) {
            defaults.user1 = atof(argv[i] + strlen("user1="));
        } else if (strncmp(argv[i], "user2=", strlen("user2=")) == 0) {
            defaults.user2 = atof(argv[i] + strlen("user2="));
        } else if (strncmp(argv[i], "user3=", strlen("user3=")) == 0) {
            defaults.user3 = atof(argv[i] + strlen("user3="));
        } else if (strncmp(argv[i], "user4=", strlen("user4=")) == 0) {
            defaults.user4 = atof(argv[i] + strlen("user4="));
        } else if (strncmp(argv[i], "user5=", strlen("user5=")) == 0) {
            defaults.user5 = atof(argv[i] + strlen("user5="));
        } else if (strncmp(argv[i], "user6=", strlen("user6=")) == 0) {
            defaults.user6 = atof(argv[i] + strlen("user6="));
        } else if (strncmp(argv[i], "user7=", strlen("user7=")) == 0) {
            defaults.user7 = atof(argv[i] + strlen("user7="));
        } else if (strncmp(argv[i], "user8=", strlen("user8=")) == 0) {
            defaults.user8 = atof(argv[i] + strlen("user8="));
        } else if (strncmp(argv[i], "user9=", strlen("user9=")) == 0) {
            defaults.user9 = atof(argv[i] + strlen("user9="));
        } else if (strncmp(argv[i], "az=", strlen("az=")) == 0) {
            defaults.az = atof(argv[i] + strlen("az="));
        } else if (strncmp(argv[i], "baz=", strlen("baz=")) == 0) {
            defaults.baz = atof(argv[i] + strlen("baz="));
        } else if (strncmp(argv[i], "gcarc=", strlen("gcarc=")) == 0) {
            defaults.gcarc = atof(argv[i] + strlen("gcarc="));
        } else if (strncmp(argv[i], "istreg=", strlen("istreg=")) == 0) {
            defaults.istreg = atoi(argv[i] + strlen("istreg="));
        } else if (strncmp(argv[i], "ievreg=", strlen("ievreg=")) == 0) {
            defaults.ievreg = atoi(argv[i] + strlen("ievreg="));
        } else if (strncmp(argv[i], "ievtyp=", strlen("ievtyp=")) == 0) {
            defaults.ievtyp = atoi(argv[i] + strlen("ievtyp="));
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strcmp(argv[i], "-ascii") == 0) {
            ascii = 1;
        } else if (strcmp(argv[i], "-binary") == 0) {
            ascii = 0;
        } else {
            help(argv[0]);
        }
    }

    if (iname != NULL && (fp = fopen(iname, "rb")) == NULL) {
        perror(iname);
        exit(1);
    }

    if (debug > 0) util_logopen(log, 1, 9, 1, NULL, argv[0]);

/*  Convert to SAC  */

    if ((status = Xfer_ToSAC(fp, ascii, &defaults, 0)) != 0) {
        fprintf(stderr,"Xfer_ToSAC failed with status %d\n", status);
    }

    exit(status);

}

/* Revision History
 *
 * $Log: tosac.c,v $
 * Revision 1.2  2003/12/10 06:27:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
