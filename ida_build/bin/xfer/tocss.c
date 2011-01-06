#pragma ident "$Id: tocss.c,v 1.2 2003/12/10 06:27:25 dechavez Exp $"
/*======================================================================
 *
 *  Demultiplex a xfer_packet format data stream into CSS 3.0.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "util.h"
#include "xfer.h"

void help(myname)
char *myname;
{
    fprintf(stderr, "usage: %s [options]\n", myname);
    fprintf(stderr,"\nOptions:\n");
    fprintf(stderr,"if=name     => input file name\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"+swap       => swap data\n");
    exit(1);
}

int main(int argc, char **argv)
{
int swap = 0;
int debug = -1;
int i, status;
FILE *fp = stdin;
char *log   = NULL;
char *iname = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            if (debug < 0) debug = 1;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strcmp(argv[i], "+swap") == 0) {
            swap = 1;
        } else {
            help(argv[0]);
        }
    }

    if (iname != NULL && (fp = fopen(iname, "rb")) == NULL) {
        perror(iname);
        exit(1);
    }

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);

/*  Convert to CSS  */

    if ((status = Xfer_ToCSS(fp, 0, swap)) != 0) {
        fprintf(stderr,"Xfer_ToCSS failed with status %d\n", status);
    }

    exit(status);

}

/* Revision History
 *
 * $Log: tocss.c,v $
 * Revision 1.2  2003/12/10 06:27:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
