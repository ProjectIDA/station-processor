#pragma ident "$Id: sactimes.c,v 1.2 2003/12/10 06:31:25 dechavez Exp $"
/*======================================================================
 *
 *  List time spanned by SAC binary files.
 *
 *====================================================================*/
#include <stdio.h>
#include "sacio.h"
#include "util.h"

char *fname;
int  status = 0;

static int list_times(fp, print_name)
FILE *fp;
int  print_name;
{
struct sac_header hdr;
double beg, end;

    if (print_name) printf("%s: ", fname);
    if (sacio_rbh(fp, &hdr) != 0) {
        perror("sactimes: sacio_rbh");
        return status = -1;
    }

    if ((beg = sacio_sttodt(&hdr)) == (double) -12345) {
        puts("undefined time");
        return status = -1;
    }
    if (hdr.b == -12345.0) {
        puts("undefined b");
        return status = -1;
    }
    if (hdr.npts <= 0) {
        puts("undefined npts");
        return status = -1;
    }

    beg += (double) hdr.b;
    end = beg + ( (double) (hdr.npts - 1) * (double) hdr.delta );

    printf("%s ", util_dttostr(beg,0));
    printf("%s\n", util_dttostr(end,0));

    return status = 0;
}

int main(int argc, char **argv)
{
int   i, nfiles;
FILE *fp;

    nfiles = argc - 1;

    if (!nfiles) {
        if (SETMODE(fileno(stdin), O_BINARY) == -1) {
            perror("sactimes: setmode");
            exit(1);
        }
        list_times(stdin, nfiles);
    } else {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "rb")) == NULL) {
                perror(fname);
            } else {
                list_times(fp, nfiles-1);
            }
        }
    }
    exit(status);
}


/* Revision History
 *
 * $Log: sactimes.c,v $
 * Revision 1.2  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:19  dec
 * import existing IDA/NRTS sources
 *
 */
