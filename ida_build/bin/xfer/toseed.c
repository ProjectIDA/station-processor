#pragma ident "$Id: toseed.c,v 1.2 2003/12/10 06:27:25 dechavez Exp $"
/*======================================================================
 *
 *  Demultiplex a xfer_packet format data stream into miniSEED.
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
    fprintf(stderr,"of=name     => output file name\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"reclen=len  => set output record length\n");
    fprintf(stderr,"netid=name  => set network ID\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "default: if=stdin of=stdout debug=0 reclen=4096 netid=II\n");
    exit(1);
}

int main(int argc, char **argv)
{
int debug  = -1;
long reclen = 4096;
int explen;
int i, status;
FILE *fp = stdin;
char *log   = NULL;
char *iname = NULL;
char *oname = NULL;
char *netid = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "of=", strlen("of=")) == 0) {
            oname = argv[i] + strlen("of=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            if (debug < 0) debug = 1;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "reclen=", strlen("reclen=")) == 0) {
            reclen = atol(argv[i] + strlen("reclen="));
        } else if (strncmp(argv[i], "netid=", strlen("netid=")) == 0) {
            netid = argv[i] + strlen("netid=");
        } else {
            help(argv[0]);
        }
    }

    if (iname != NULL && (fp = fopen(iname, "rb")) == NULL) {
        perror(iname);
        exit(1);
    }

    if ((explen = util_powerof(reclen, 2)) < 0) {
        fprintf(stderr, "%s: error: reclen must be a power of 2", argv[0]);
        help(argv[0]);
    }

    if (netid == (char *) NULL) netid = "II";

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);

/*  Convert to desired format  */

    if ((status = Xfer_ToSEED(fp, oname, explen, netid, 0)) != 0) {
        fprintf(stderr,"Xfer_ToSEED failed with status %d\n", status);
    }

    exit(status);

}

/* Revision History
 *
 * $Log: toseed.c,v $
 * Revision 1.2  2003/12/10 06:27:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
