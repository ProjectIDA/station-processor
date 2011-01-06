#pragma ident "$Id: init.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nrts.h"
#include "nrts_xfer.h"

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options] syscode\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"to=secs     => set read timeout\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"home=name   => set NRTS_HOME\n");
    fprintf(stderr,"\n");
    exit(1);
}

char *init(int argc, char **argv, char **home, int *ifd, int *ofd, int *to, char *dbdir)
{
char fname[MAXPATHLEN+1];
int i, status, addrlen;
char *log      = NULL;
int  debug     = NRTS_DEFLOG;
int  timeout   = 1;
struct sockaddr_in cli_addr, *cli_addrp;
struct hostent *hp, hostent;
static char client[1024];
char *dbtmp;
static char *defdbdir = NULL;
static char *fid = "init";

    *home = NULL;
    *ifd  = -1;
    *ofd  = -1;
    *to   = 60;
    dbtmp = (char *) NULL;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            *home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            *to = atoi(argv[i] + strlen("to="));
        } else if (strncmp(argv[i], "dbdir=", strlen("dbdir=")) == 0) {
            dbtmp = argv[i] + strlen("dbdir=");
        } else {
            help(argv[0]);
        }
    }

    if (dbtmp == (char *) NULL) dbtmp = getenv("IDA_DBDIR");
    if (dbtmp == (char *) NULL) dbtmp = defdbdir;
    strcpy(dbdir, dbtmp);

/* Determine working home  */

    if (*home == NULL) {
        *home = getenv(NRTS_HOME);
    }

    if (*home == NULL) {
        *home = NRTS_DEFHOME;
    }

/*  Open log file  */

    if (log == NULL) {
        sprintf(fname, "%s/log/xfer", *home);
        log = fname;
    }

    util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
    util_log(2, "Version %s", "2.11 (17:40:39 07/23/99)");

    if (chdir(*home) != 0) {
        util_log(1, "%s: can't chdir %s: %s",
            fid, *home, syserrmsg(errno)
        );
        return NULL;
    }

/* Signal/exit handlers  */

    if (exitcode() != 0) {
        util_log(1, "%s: exitcode: %s", fid, syserrmsg(errno));
        return NULL;
    }

/*  Get client info  */

    *ifd = fileno(stdin);
    *ofd = fileno(stdout);

    addrlen = sizeof(cli_addr);
    cli_addrp = &cli_addr;
    if (getpeername(*ifd, (struct sockaddr *)cli_addrp, &addrlen) != 0) {
        util_log(1, "%s: getpeername: %s", fid, syserrmsg(errno));
        return NULL;
    } else {
        hp = gethostbyaddr((char *)
            &cli_addrp->sin_addr, sizeof(struct in_addr), cli_addrp->sin_family
        );
        if (hp == NULL) {
            hp = &hostent;
            hp->h_name = inet_ntoa(cli_addrp->sin_addr);
        }
        sprintf(client, "%s", hp->h_name);
    }

    return client;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
