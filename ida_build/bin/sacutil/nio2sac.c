#pragma ident "$Id: nio2sac.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  Read an NIO file and write a SAC version to stdout.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"
#include "sacio.h"
#include "null_sac.h"

#define FLOAT 0
#define LONG  1
#define SHORT 2

int main(int argc, char **argv)
{
short sdata;
long  ldata, offset, nbytes;
float fdata;
int   nionum, type;
static char fname[] = "Xnnn.D";
FILE *fp;
struct sac_header hdr;

    if (argc != 2) {
        fprintf(stderr,"usage: nio2sac NIO_number > output\n");
        exit(1);
    }

    nionum = atoi(argv[1]);
    if (nionum < 0) {
        nionum *= -1;
        if (nionum < 100 || nionum > 999) {
            fprintf(stderr,"nio2sac: bad file number = -%d\n", nionum);
            exit(2);
        } else {
            sprintf(fname, "R%3.3d.D", nionum);
        }
        type = FLOAT;
        offset = 0L;
    } else if (nionum >= 1100 && nionum <= 1999) {
        nionum -= 1000;
        sprintf(fname, "L%3.3d.D", nionum);
        type = LONG;
        offset = 0L;
    } else if (nionum >= 4100 && nionum <= 4999) {
        nionum -= 4000;
        sprintf(fname, "H%3.3d.D", nionum);
        type = SHORT;
        offset = 3072L;
    } else if (nionum >= 100 && nionum <= 999) {
        sprintf(fname, "I%3.3d.D", nionum);
        type = SHORT;
        offset = 0L;
    } else {
        fprintf(stderr,"nio2sac: bad file number = %d\n", nionum);
        exit(2);
    }

    if ((fp = fopen(fname, "rb")) == NULL) {
        fprintf(stderr, "nio2sac: fopen: ");
        perror(fname);
        exit(3);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("nio2sac: setmode");
        exit(3);
    }

    if (fseek(fp, 0L, SEEK_END) != 0) {
        fprintf(stderr, "nio2sac: fseek: ");
        perror(fname);
        exit(3);
    }
    nbytes = ftell(fp);
    rewind(fp);

    hdr = null_sac_header;

    nbytes -= offset;
    if (type == FLOAT) {
        hdr.npts = nbytes / sizeof(float);
    } else if (type == LONG) {
        hdr.npts = nbytes / sizeof(long);
    } else {
        hdr.npts = nbytes / sizeof(short);
    } 

    hdr.delta = 1.0;
    hdr.b     = 0.0;
    hdr.e     = (hdr.npts - 1) * hdr.delta;
    hdr.iftype = ITIME;
    hdr.leven  = 1;
    hdr.iztype = IB;
    strcpy(hdr.kinst, "NIOLIB");

    if (sacio_wbh(stdout, &hdr) != 0) {
        perror("nio2sac: sacio_wbh");
        exit(1);
    }

    if (offset && fseek(fp, offset, SEEK_SET) != 0) {
        fprintf(stderr, "nio2sac: fseek: ");
        perror(fname);
        exit(3);
    }

    switch (type) {
        case FLOAT:
            while (fread(&fdata, sizeof(float), 1, fp) == 1) {
                if (fwrite(&fdata, sizeof(float), 1, stdout) != 1) {
                    perror("nio2sac: fwrite");
                    exit(1);
                }
            }
            break;
        case LONG:
            while (fread(&ldata, sizeof(long), 1, fp) == 1) {
                fdata = (float) ldata;
                if (fwrite(&fdata, sizeof(float), 1, stdout) != 1) {
                    perror("nio2sac: fwrite");
                    exit(1);
                }
            }
            break;
        case SHORT:
            while (fread(&sdata, sizeof(short), 1, fp) == 1) {
                fdata = (float) sdata;
                if (fwrite(&fdata, sizeof(float), 1, stdout) != 1) {
                    perror("nio2sac: fwrite");
                    exit(1);
                }
            }
            break;
        default:
            fprintf(stderr,"nio2sac: undefined type!!!\n");
            exit(4);
    }

    if (ferror(fp)) {
        fprintf(stderr,"nio2sac fread: ");
        perror(fname);
        exit(5);
    }

    fclose(fp);
    exit(0);
}

/* Revision History
 *
 * $Log: nio2sac.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:18  dec
 * import existing IDA/NRTS sources
 *
 */
