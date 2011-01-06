#pragma ident "$Id: wdconvert.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Convert wfdiscs to 3.0 or 2.8 format.
 *
 *====================================================================*/
#include <stdio.h>
#include <time.h>
#include "util.h"
#include "cssio.h"

static int help()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: wdconvert [format] < input > output\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "format: 3.0 => output 3.0 wfdisc records (default)\n");
    fprintf(stderr, "format: 2.8 => output 2.8 wfdisc records\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
struct wfdisc   wfdisc;
struct wfdisc28 wfdisc28;

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "3.0")) == 0) {
        while (rwfdrec(stdin, &wfdisc) == 0) {
            if (wwfdisc(stdout, &wfdisc) != 0) exit(1);
        }
    } else if (argc == 2 && strcmp(argv[1], "2.8") == 0) {
        while (rwfdrec(stdin, &wfdisc) == 0) {
            wf30to28(&wfdisc28, &wfdisc);
            if (wwfdisc28(stdout, &wfdisc28) != 0) exit(1);
        }
    } else {
        help();
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdconvert.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
