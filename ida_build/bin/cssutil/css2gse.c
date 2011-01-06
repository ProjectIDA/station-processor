#pragma ident "$Id: css2gse.c,v 1.2 2003/12/10 06:15:37 dechavez Exp $"
/*======================================================================
 *
 *  Convert all data pointed to in the provided wfdisc file to GSE 2.0.  
 *
 *====================================================================*/
#include <stdio.h>
#include <sys/param.h>
#include <math.h>
#include "cssio.h"

static int help()
{
    fprintf(stderr, "usage: css2gse < CSS_wfdisc_file > GSE_data_file\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, nrec, count;
FILE *ifp = stdin, *ofp = stdout;
struct wfdisc *wfdisc;

/*  Read input wfdisc file  */

    if ((nrec = rwfdisc(ifp, &wfdisc)) <= 0) {
        fprintf(stderr,"%s: bad or empty wfdisc file\n", argv[0]);
        exit(2);
    }

/*  Convert to GSE */

    count = 0;
    for (i = 0; i < nrec; i++) {
        if (CssToGse(ofp, &wfdisc[i])) {
            ++count;
        } else {
            fprintf(stderr, "%s: record %d skipped due to errors\n",
                argv[0], i+1
            );
        }
        fflush(ofp);
    }

    fprintf(stderr, "%d CSS 3.0 records converted to GSE 2.0.\n", count);

    exit(0);

}

/* Revision History
 *
 * $Log: css2gse.c,v $
 * Revision 1.2  2003/12/10 06:15:37  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1  2001/12/10 20:44:32  dec
 * created
 *
 */
