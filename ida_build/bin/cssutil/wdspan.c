#pragma ident "$Id: wdspan.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  List the earliest and latest times which are spanned by the given
 *  wfdisc file.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"

static long beg, end;
static int first = 1;

static int wdspan(fp)
FILE *fp;
{
long beg_time, end_time;
struct wfdisc wfdisc;

    while (rwfdrec(fp, &wfdisc) == 0) {
        beg_time = (long) wfdisc.time;
        end_time = (long) wfdisc.endtime;
        if (first) {
            beg = beg_time;
            end = end_time;
            first = 0;
        } else {
            beg = (beg_time < beg) ? beg_time : beg;
            end = (end_time > end) ? end_time : end;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
int i;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "r")) == NULL) {
                fprintf(stderr, "wdspan: fopen: ");
                perror(fname);
                exit(1);
            } else {
                wdspan(fp);
                fclose(fp);
            }
        }
    } else {
        wdspan(stdin);
    }

    printf("%ld %ld\n", beg, end);

    exit(0);
}

/* Revision History
 *
 * $Log: wdspan.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
