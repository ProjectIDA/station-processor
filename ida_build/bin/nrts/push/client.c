#pragma ident "$Id: client.c,v 1.4 2003/12/10 06:24:50 dechavez Exp $"
/*======================================================================
 *
 *  Announce oneself to a nrts_push daemon
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "nrts_push.h"

#define DEF_SERVICE ((char *) 0)
#define DEF_PORT    NRTS_PUSHD_PORT
#define DEF_DEBUG   0
#define DEF_LOG     (char *) NULL
#define MIN_TO      10
#define DEF_TO      MIN_TO

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s server=host [options] syscode\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Required arguments:\n");
    fprintf(stderr,"server=host => server hostname or IP address\n");
    fprintf(stderr,"syscode     => NRTS system code to advertise\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"port=val    => TCP/IP port number\n");
    fprintf(stderr,"to=secs     => network write timeout\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, sd;
char *host    = NULL;
char *service = DEF_SERVICE;
char *log     = DEF_LOG;
char *syscode = NULL;
int  port     = DEF_PORT;
int  debug    = DEF_DEBUG;
int  to       = DEF_TO;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "server=", strlen("server=")) == 0) {
            host = argv[i] + strlen("server=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
            service = (char *) NULL;
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
            if (to < MIN_TO) to = MIN_TO;
        } else if (strncasecmp(argv[i], "-v", strlen("-v")) == 0) {
            fprintf(stderr, "%s Version %s\n", argv[0], VERSION);
        } else if (syscode == NULL) {
            syscode = argv[i];
            if (strlen(syscode) > PUSHD_SYSCODE_LEN) {
                fprintf(stderr, "%s: syscode `%s' too long (%d char max)\n",
                    argv[0], syscode, PUSHD_SYSCODE_LEN
                );
                exit(0);
            }
        } else {
            help(argv[0]);
        }
    }

    if (host == NULL || syscode == NULL) help(argv[0]);

    if (debug) util_logopen(log, 1, 9, debug, NULL, argv[0]);

    sd = util_connect(host, service, port, "tcp", 0, 0);

    if (sd <= 0) {
        util_log(1, "util_connect: %s", syserrmsg(errno));
        exit(1);
    }
    util_noblock(sd);

    if (util_write(sd, syscode, PUSHD_SYSCODE_BUFLEN, to) != PUSHD_SYSCODE_BUFLEN) {
        util_log(1, "util_write(%s) failed", syscode);
    }
    util_log(1, "advertise %s", syscode);

    shutdown(sd, 2);
    close(sd);

    exit(0);
}

/* Revision History
 *
 * $Log: client.c,v $
 * Revision 1.4  2003/12/10 06:24:50  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2000/11/30 23:34:35  nobody
 * fixed off by one bug in util_write
 *
 * Revision 1.2  2000/10/12 16:04:35  dec
 * added version number support
 *
 * Revision 1.1  2000/10/12 15:50:28  dec
 * release 0.9.0
 *
 */
