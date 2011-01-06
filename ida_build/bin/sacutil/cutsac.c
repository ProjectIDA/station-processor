#pragma ident "$Id: cutsac.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Cut time windows from SAC binary files.
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"
#include "util.h"

#define START  argv[1]
#define STOP   argv[2]
#define INPUT  argv[3]
#define OUTPUT argv[4]

int main(int argc, char **argv)
{
int   ok;
long  i, nskip, ncopy;
FILE *ifp, *ofp;
float sample;
struct sac_header hdr;
struct { double start; double stop; } want, actual;

/*  Set up input/output streams  */

    if (argc == 3) {
        ifp = stdin;
        ofp = stdout;
    } else if (argc == 4) {
        if ((ifp = fopen(INPUT, "rb")) == NULL) {
            perror(INPUT);
            exit(1);
        }
        ofp = stdout;
    } else if (argc == 5) {
        if ((ifp = fopen(INPUT, "rb")) == NULL) {
            perror(INPUT);
            exit(1);
        }
        if ((ofp = fopen(OUTPUT, "wb")) == NULL) {
            perror(OUTPUT);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: %s start stop [ in_name [ out_name ] ]\n",
            argv[0]);
        exit(1);
    }

    if (SETMODE(fileno(ifp), O_BINARY) == -1) {
        perror("setmode");
        exit(1);
    }

    if (SETMODE(fileno(ofp), O_BINARY) == -1) {
        perror("setmode");
        exit(1);
    }

/*  Get desired start/stop times  */

    want.start = util_attodt(START);
    want.stop  = util_attodt(STOP);

    if (want.start >= want.stop) {
        fprintf(stderr,"%s: %s >= %s\n", argv[0], START, STOP);
        exit(1);
    }

/*  Read header and make sure it is sufficiently complete  */

    if (sacio_rbh(ifp, &hdr) != 0) {
        perror("cutsac: can't read header");
        exit(1);
    }

    ok = TRUE;
    if (hdr.npts == -12345) {
        fprintf(stderr, "cutsac: npts is undefined!\n");
        ok = FALSE;
    }
    if ((actual.start = sacio_sttodt(&hdr)) == (double) -12345) {
        fprintf(stderr, "cutsac: time is undefined!\n");
        ok = FALSE;
    }
    if (hdr.b == (float) -12345) {
        fprintf(stderr, "cutsac: begin time (B) is undefined!\n");
        ok = FALSE;
    }
    if (hdr.iftype != ITIME || hdr.leven != TRUE) {
        fprintf(stderr, "cutsac: input is not an evenly space time series!\n");
        ok = FALSE;
    }
    if (!ok) {
        fprintf(stderr, "%s: unable to continue\n", argv[0]);
        exit(1);
    }

/*  Make sure requested window intersects data  */

    actual.start += (double) hdr.b;
    actual.stop = actual.start + ((hdr.npts - 1) / hdr.delta);

    if (want.start >= actual.stop || want.stop <= actual.start) {
        fprintf(stderr, "cutsac: error - ");
        fprintf(stderr, "requested window does not intersect data\n");
        fprintf(stderr, "requested: %s - ", util_dttostr(want.start,0));
        fprintf(stderr, "%s\n", util_dttostr(want.stop,0));
        fprintf(stderr, "actual:    %s - ", util_dttostr(actual.start,0));
        fprintf(stderr, "%s\n", util_dttostr(actual.stop,0));
        exit(2);
    }
    if (actual.start > want.start) {
        fprintf(stderr, "cutsac: warning - ");
        fprintf(stderr, "actual start time is later than requested\n");
        fprintf(stderr, "requested: %s\n", util_dttostr(want.start,0));
        fprintf(stderr, "actual:    %s\n", util_dttostr(actual.start,0));
    }
    if (actual.stop < want.stop) {
        fprintf(stderr, "cutsac: warning - ");
        fprintf(stderr, "actual stop time is earlier than requested\n");
        fprintf(stderr, "requested: %s\n", util_dttostr(want.stop,0));
        fprintf(stderr, "actual:    %s\n", util_dttostr(actual.stop,0));
    }

/*  Determine sample range to copy and update header  */

    nskip = (long) ( (want.start - actual.start) / (double) hdr.delta );
    if (nskip < 0) nskip = 0;
    hdr.npts -= nskip;

    ncopy = (long) ( (want.stop - want.start) / (double) hdr.delta );
    if (ncopy > hdr.npts) ncopy = hdr.npts;
    hdr.npts = ncopy;

    hdr.b += (float) nskip * hdr.delta;
    hdr.e  = (hdr.npts - 1) * hdr.delta;

    if (sacio_wbh(ofp, &hdr) != 0) {
        perror("cutsac: can't write header");
        exit(1);
    }

/*  Copy the neccesary data  */

    if (fseek(ifp, nskip * sizeof(float), SEEK_CUR) != 0) {
        perror("cutsac: can't fseek");
        exit(1);
    }

    for (i = 0; i < hdr.npts; i++) {
        if (fread(&sample, sizeof(float), 1, ifp) != 1) {
            perror("cutsac: can't read data");
            exit(1);
        }
        if (fwrite(&sample, sizeof(float), 1, ofp) != 1) {
            perror("cutsac: can't write data");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: cutsac.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:18  dec
 * import existing IDA/NRTS sources
 *
 */
