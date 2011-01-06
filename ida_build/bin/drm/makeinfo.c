#pragma ident "$Id: makeinfo.c,v 1.4 2003/12/10 06:30:31 dechavez Exp $"
/*======================================================================
 *
 *  Tap into an active NRTS to get the current configuration and print
 *  it a DRM portable (ie, ascii) form.  Not all the information which
 *  is provided is actually printed, just that which is necessary for
 *  the DRM to do its job.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "xfer.h"
#include "drm.h"

#define VERSION "drm_makeinfo Version 2.10.2"

void help(myname)
char *myname;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s server=host_name\n", myname);
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
char *home     = NULL;
char *log      = NULL;
char *server   = NULL;
char *service  = NULL;
char port      = -1;
int debug      = -1;
int i, sd;
struct xfer_req req;
struct xfer_cnf cnf;
struct xfer_cnfnrts *nrts;

    req.protocol = 1;
    req.type     = XFER_CNFREQ;
    req.timeout  = 60;
    req.preamble.ver01.format    = XFER_CNFNRTS;
    req.preamble.ver01.client_id = getpid();
    port = -1;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            req.timeout = atoi(argv[i] + strlen("to="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            if (server != NULL) help(argv[0]);
            server = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            if (server != NULL) help(argv[0]);
            server = argv[i] + strlen("server=");
        } else if (server == NULL) {
            server = argv[i];
        } else {
            help(argv[0]);
        }
    }

/* Must specify remote server name */

    if (server == NULL) help(argv[0]);

/* If the remote port was not given, then use the system defined service */
 
    service = (port < 0) ? XFER_SERVICE : (char *) NULL;

/*  Start logging */

    util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
    util_log(1, "%s", VERSION);

/* Connect to remote server and issue configuration request */

    if (port < 0) {
        util_log(1, "connecting to %s@%s", service, server);
    } else {
        util_log(1, "connecting to %s:%d", server, port);
    }

    if ((sd = Xfer_Connect(server, service, port, "tcp", &req, &cnf, 0)) < 0) {
        util_log(1, "%s", Xfer_ErrStr());
        exit(2);
    } else {
        shutdown(sd, 2);
        close(sd);
    }

/* Print it */

    if (drm_prtinfo(&cnf.type.nrts) != 0) {
        util_log(1, "drm_prtinfo: %s", syserrmsg(errno));
        exit(3);
    }

    exit(0);
}

void drm_exit(status)
int status;
{
    util_log(1, "exit %d", status);
    exit(status);
}

void drm_ack_init(dummy)
int dummy;
{
    return;
}

/* Revision History
 *
 * $Log: makeinfo.c,v $
 * Revision 1.4  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.3  2003/10/16 18:11:35  dechavez
 * ansi function declaration
 *
 * Revision 1.2  2000/02/18 06:36:27  dec
 * Added ReleaseNotes and consistent version numbering
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
