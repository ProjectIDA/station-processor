#pragma ident "$Id: css2sac.c,v 1.2 2003/12/10 06:15:37 dechavez Exp $"
/*======================================================================
 *
 *  Convert all data pointed to in the provided wfdisc file to SAC.  The
 *  data for a particular channel are collapsed into a single record as
 *  long as the gap or overlap between sequential records is within a
 *  user specified tolerance (default is 1 sample).
 *
 *  Important!  We *ignore* the sample rate field and just calculate
 *  it for the output wfdisc using the start/end times and new number
 *  of samples.  We *assume* that data for a given station/channel have
 *  a constant nominal sample rate between records.  What it is does
 *  not matter, as it is not used.
 *
 *====================================================================*/
#define INCLUDE_SACIO
#include <stdio.h>
#include <sys/param.h>
#include <math.h>
#include "cssio.h"
#include "null_sac.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

char path[MAXPATHLEN+1];

static void help()
{
    fprintf(stderr, "usage: css2sac [options]\n");
    fprintf(stderr,"\nOptions:\n");
    fprintf(stderr,"if=name     => input file name\n");
    fprintf(stderr,"tol=int     => end of record tolerance in samples\n");
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
    fprintf(stderr,"\ndefault: if=stdin tol=1 -binary\n");
    exit(1);
}

int main(int argc, char **argv)
{
int ascii = 0;
int i, status, nrec, tolerance = 1;
FILE *fp = stdin;
char *iname = NULL;
char *oname = NULL;
struct wfdisc *wfdisc;
struct sac_header sach;

    sach = null_sac_header;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "tol=", strlen("tol=")) == 0) {
            if ((tolerance = atoi(argv[i]+strlen("tol="))) < 0.0) {
                fprintf(stderr,"css2sac: tolerance must be positive\n");
                exit(1);
            }
        } else if (strncmp(argv[i], "evla=", strlen("evla=")) == 0) {
            sach.evla = atof(argv[i] + strlen("evla="));
        } else if (strncmp(argv[i], "evlo=", strlen("evlo=")) == 0) {
            sach.evlo = atof(argv[i] + strlen("evlo="));
        } else if (strncmp(argv[i], "evel=", strlen("evel=")) == 0) {
            sach.evel = atof(argv[i] + strlen("evel="));
        } else if (strncmp(argv[i], "evdp=", strlen("evdp=")) == 0) {
            sach.evdp = atof(argv[i] + strlen("evdp="));
        } else if (strncmp(argv[i], "stla=", strlen("stla=")) == 0) {
            sach.stla = atof(argv[i] + strlen("stla="));
        } else if (strncmp(argv[i], "stlo=", strlen("stlo=")) == 0) {
            sach.stlo = atof(argv[i] + strlen("stlo="));
        } else if (strncmp(argv[i], "stel=", strlen("stel=")) == 0) {
            sach.stel = atof(argv[i] + strlen("stel="));
        } else if (strncmp(argv[i], "stdp=", strlen("stdp=")) == 0) {
            sach.stdp = atof(argv[i] + strlen("stdp="));
        } else if (strncmp(argv[i], "dist=", strlen("dist=")) == 0) {
            sach.dist = atof(argv[i] + strlen("dist="));
        } else if (strncmp(argv[i], "user0=", strlen("user0=")) == 0) {
            sach.user0 = atof(argv[i] + strlen("user0="));
        } else if (strncmp(argv[i], "user1=", strlen("user1=")) == 0) {
            sach.user1 = atof(argv[i] + strlen("user1="));
        } else if (strncmp(argv[i], "user2=", strlen("user2=")) == 0) {
            sach.user2 = atof(argv[i] + strlen("user2="));
        } else if (strncmp(argv[i], "user3=", strlen("user3=")) == 0) {
            sach.user3 = atof(argv[i] + strlen("user3="));
        } else if (strncmp(argv[i], "user4=", strlen("user4=")) == 0) {
            sach.user4 = atof(argv[i] + strlen("user4="));
        } else if (strncmp(argv[i], "user5=", strlen("user5=")) == 0) {
            sach.user5 = atof(argv[i] + strlen("user5="));
        } else if (strncmp(argv[i], "user6=", strlen("user6=")) == 0) {
            sach.user6 = atof(argv[i] + strlen("user6="));
        } else if (strncmp(argv[i], "user7=", strlen("user7=")) == 0) {
            sach.user7 = atof(argv[i] + strlen("user7="));
        } else if (strncmp(argv[i], "user8=", strlen("user8=")) == 0) {
            sach.user8 = atof(argv[i] + strlen("user8="));
        } else if (strncmp(argv[i], "user9=", strlen("user9=")) == 0) {
            sach.user9 = atof(argv[i] + strlen("user9="));
        } else if (strncmp(argv[i], "az=", strlen("az=")) == 0) {
            sach.az = atof(argv[i] + strlen("az="));
        } else if (strncmp(argv[i], "baz=", strlen("baz=")) == 0) {
            sach.baz = atof(argv[i] + strlen("baz="));
        } else if (strncmp(argv[i], "gcarc=", strlen("gcarc=")) == 0) {
            sach.gcarc = atof(argv[i] + strlen("gcarc="));
        } else if (strncmp(argv[i], "istreg=", strlen("istreg=")) == 0) {
            sach.istreg = atoi(argv[i] + strlen("istreg="));
        } else if (strncmp(argv[i], "ievreg=", strlen("ievreg=")) == 0) {
            sach.ievreg = atoi(argv[i] + strlen("ievreg="));
        } else if (strncmp(argv[i], "ievtyp=", strlen("ievtyp=")) == 0) {
            sach.ievtyp = atoi(argv[i] + strlen("ievtyp="));
        } else if (strcmp(argv[i], "-ascii") == 0) {
            ascii = 1;
        } else if (strcmp(argv[i], "-binary") == 0) {
            ascii = 0;
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else {
            help();
        }
    }

    if (iname != NULL && (fp = fopen(iname, "rb")) == NULL) {
        perror(iname);
        exit(1);
    }

/*  Read input wfdisc file  */

    if ((nrec = rwfdisc(fp, &wfdisc)) <= 0) {
        fprintf(stderr,"css2sac: bad or empty wfdisc file\n");
        exit(2);
    }
    fclose(fp);

/*  Convert to SAC  */

    if ((status = css2sac(wfdisc, nrec, tolerance, &sach, ascii)) != 0) {
        fprintf(stderr,"css2sac failed with status %d\n", status);
    }

    exit(status);

}

/* Revision History
 *
 * $Log: css2sac.c,v $
 * Revision 1.2  2003/12/10 06:15:37  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
