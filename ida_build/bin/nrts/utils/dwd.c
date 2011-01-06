#pragma ident "$Id: dwd.c,v 1.7 2004/04/26 21:12:35 dechavez Exp $"
/*======================================================================
 *
 *  Dump NRTS wfdiscs for specified station.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "nrts.h"
#include "cssio.h"
#include "util.h"

long count  = 0;

static void help(char *myname)
{
    fprintf(stderr,"usage: %s sta [maxdur=int flush=int debug=int]\n", myname);
    exit(1);
}

static int GetWfdiscFromBwdFile(char *home, char *sta, int flush, int maxdur, char *dbspec)
{
int i;
struct wfdisc *wfdisc;

    if ((count = nrts_stawd(home, sta, &wfdisc, flush, maxdur, dbspec)) < 0) {
        fprintf(stderr, "nrts_stawd(%s, %s, %x, %d) failed\n",
            home, sta, &wfdisc, flush
        );
        count = 0;
        return 6;
    }

    for (i = 0; i < count; i++) wwfdisc(stdout, wfdisc + i);

    return 0;
}

int main (int argc, char **argv)
{
int i, sd, status;
int flush = 0;
int debug = 0;
int maxdur = 0;
char *sta   = NULL;
char *home  = NULL;
char *dbspec = NULL;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "flush=", strlen("flush=")) == 0) {
            flush = atoi(argv[i] + strlen("flush="));
        } else if (strncmp(argv[i], "maxdur=", strlen("maxdur=")) == 0) {
            maxdur = atoi(argv[i] + strlen("maxdur="));
        } else if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "dbdir=", strlen("dbdir=")) == 0) {
            dbspec = argv[i] + strlen("dbdir=");
        } else if (strncmp(argv[i], "dbspec=", strlen("dbspec=")) == 0) {
            dbspec = argv[i] + strlen("dbspec=");
        } else if (sta == NULL) {
            sta = argv[i];
        } else {
            help(argv[0]);
        }
    }

    if (sta == NULL) help(argv[0]);
    if (home == NULL) home = getenv(NRTS_HOME);
    if (home == NULL) home = NRTS_DEFHOME;

    if (debug) util_logopen(NULL, 1, NRTS_MAXLOG, debug, NULL, argv[0]);

    status = GetWfdiscFromBwdFile(home, sta, flush, maxdur, dbspec);

    util_log(1, "%ld records written\n", count);
    exit(status);
}

/* Revision History
 *
 * $Log: dwd.c,v $
 * Revision 1.7  2004/04/26 21:12:35  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.6  2003/12/22 22:53:54  dechavez
 * change default flush to false
 *
 * Revision 1.5  2003/12/10 06:24:52  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.4  2003/06/24 21:31:06  dechavez
 * removed obsoleted network support (use nrtsGetwd for network wfdiscs)
 *
 * Revision 1.3  2002/11/20 01:07:00  dechavez
 * added dbdir parameter
 *
 * Revision 1.2  2002/11/19 18:37:48  dechavez
 * added maxdur parameter
 *
 * Revision 1.1.1.1  2000/02/08 20:20:13  dec
 * import existing IDA/NRTS sources
 *
 */
