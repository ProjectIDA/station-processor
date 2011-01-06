#pragma ident "$Id: makesac.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Generate standard test data files in SAC format.
 *
 *====================================================================*/
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "util.h"
#include "sacio.h"
#include "null_sac.h"

#define UNDEFINED 0
#define BOX       1
#define SPIKE     2
#define LINE      3
#define SINE      4
#define MAXFREQ   100

#define pi 3.141592654

static int help()
{
    fprintf(stderr,"usage: makesac ");
    fprintf(stderr,"{box=npts | spike=npts} [sint=sint]\n");
    exit(1);
}

int main(int argc, char **argv)
{
FILE *ofp = stdout;
int type, nfreq=0;
long i, j, npts, center;
float factor = 1.0, sint = 1.0, slope = 1.0;
float intercept = 0.0;
float data;
float freq[MAXFREQ], rinc[MAXFREQ], radians[MAXFREQ];
struct sac_header hdr;

    if (argc < 2) help();
    
    freq[0] = 1.0; nfreq = 1;

    for (type = UNDEFINED, i = 1; i < argc; i++) {
        if (strncmp(argv[i], "box=", strlen("box=")) == 0) {
            type = BOX;
            npts = atol(argv[i]+strlen("box="));
        } else if (strncmp(argv[i], "spike=", strlen("spike=")) == 0) {
            type = SPIKE;
            npts = atol(argv[i]+strlen("spike="));
        } else if (strncmp(argv[i], "line=", strlen("line=")) == 0) {
            type = LINE;
            npts = atol(argv[i]+strlen("line="));
        } else if (strncmp(argv[i], "sine=", strlen("sine=")) == 0) {
            type = SINE;
            npts = atol(argv[i]+strlen("sine="));
        } else if (strncmp(argv[i], "slope=", strlen("slope=")) == 0) {
            slope = atof(argv[i]+strlen("slope="));
        } else if (strncmp(argv[i], "freq=", strlen("freq=")) == 0) {
            assert(nfreq <= MAXFREQ);
            freq[nfreq++] = atof(argv[i]+strlen("freq="));
        } else if (strncmp(argv[i], "intercept=", strlen("intercept=")) == 0) {
            intercept = atof(argv[i]+strlen("intercept="));
        } else if (strncmp(argv[i], "sint=", strlen("sint=")) == 0) {
            sint = atof(argv[i]+strlen("sint="));
        } else if (strncmp(argv[i], "factor=", strlen("factor=")) == 0) {
            factor = atof(argv[i]+strlen("factor="));
        } else {
            help();
        }
    }

    if (SETMODE(fileno(ofp), O_BINARY) == -1) {
        perror("setmode");
        exit(1);
    }

    if (type == UNDEFINED) help();
    if (sint <= 0.0) {
        fprintf(stderr,"illegal sample interval\n");
        help();
    }

    hdr = null_sac_header;
    hdr.npts   = npts;
    hdr.delta  = sint;
    hdr.iftype = ITIME;
    hdr.leven  = 1;

    if (sacio_wbh(ofp, &hdr) != 0) {
        perror("can't write SAC header");
        exit(1);
    }

    center = npts / 2;
    switch (type) {
      case BOX:
        for (data = 0.0, i = 0; i < center/2; i++) {
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        for (data = sint*factor, i = center/2; i < center + center/2; i++) {
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        for (data = 0.0, i = center/2 + center/2; i < npts; i++) {
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        break;

      case SPIKE:
        for (data = 0.0, i = 0; i < center; i++) {
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        data = sint*factor;
        if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        for (data = 0.0, i = center+1; i < npts; i++) {
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        break;

      case LINE:
        for (i = 0; i < npts; i++) {
            data = factor * (((float) i * slope) + intercept);
            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        break;

      case SINE:
        for (j = 0; j < nfreq; j++) {
            rinc[j] = 2.0 * pi * freq[j] * sint;
            radians[j] = 0.0;
        }
        for (i = 0; i < npts; i++) {
            for (data = 0.0, j = 0; j < nfreq; radians[j] += rinc[j], j++) {
                data += factor * sin(radians[j]);
            }

            if (fwrite(&data, sizeof(float), 1, ofp) != 1) {
                perror("fwrite");
                exit(1);
            }
        }
        break;

      default: fprintf(stderr, "unsupported type!\n"); help();
    }

    exit(0);

}

/* Revision History
 *
 * $Log: makesac.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:18  dec
 * import existing IDA/NRTS sources
 *
 */
