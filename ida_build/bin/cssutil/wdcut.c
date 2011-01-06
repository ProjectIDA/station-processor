#pragma ident "$Id: wdcut.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  "Cut" time windows from CSS files.  Works by examining each record
 *  in the provided wfdisc file and, if the trace pointed to in the
 *  record overlaps the desired window, produces a new record with
 *  the offset and number of samples adjusted to correspond to the
 *  window.
 *
 *  Can read both 2.8 and 3.0 wfdiscs, but only writes 3.0 format.
 *
 *====================================================================*/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "cssio.h"

#define START  argv[1]
#define STOP   argv[2]
#define INPUT  argv[3]
#define OUTPUT argv[4]

int main(int argc, char **argv)
{
int i, isdouble;
FILE *ifp, *ofp;
double beg, end;
struct wfdisc wfdisc, *output;
char *begstr, *endstr;

    begstr = START;
    endstr = STOP;

/*  Set up input/output streams  */

    if (argc == 3) {
        ifp = stdin;
        ofp = stdout;
    } else if (argc == 4) {
        if ((ifp = fopen(INPUT, "r")) == NULL) {
            perror(INPUT);
            exit(1);
        }
        ofp = stdout;
    } else if (argc == 5) {
        if ((ifp = fopen(INPUT, "r")) == NULL) {
            perror(INPUT);
            exit(1);
        }
        if ((ofp = fopen(OUTPUT, "wb")) == NULL) {
            perror(OUTPUT);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: wdcut beg end [ input [ output ] ]\n");
        exit(1);
    }

    for (isdouble = 1, i = 0; i < strlen(begstr); i++) {
        if (!isdigit(begstr[i]) && begstr[i] != '.') isdouble = 0;
    }
    beg = (isdouble) ? atof(begstr) : util_attodt(begstr);

    for (isdouble = 1, i = 0; i < strlen(endstr); i++) {
        if (!isdigit(endstr[i]) && endstr[i] != '.') isdouble = 0;
    }
    end = (isdouble) ? atof(endstr) : util_attodt(endstr);

    if (beg >= end) {
        fprintf(stderr,"%s: %s >= %s\n", argv[0], begstr, endstr);
        exit(1);
    }

    while (rwfdrec(ifp, &wfdisc) == 0) {
        output = wdcut(&wfdisc, beg, end);
        if (output != NULL) wwfdisc(ofp, output);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdcut.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
