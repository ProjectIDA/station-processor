#pragma ident "$Id: main.c,v 1.3 2006/02/10 02:13:01 dechavez Exp $"
/*======================================================================
 *
 *  Read 1024 byte records from stdin and inject them into a running ISP
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "util.h"
#include "isp.h"

#define DEF_HOST    "localhost"
#define DEF_SERVICE ISP_INJECT
#define DEF_PORT    0
#define DEF_DEBUG   0
#define DEF_DELAY   1
#define DEF_LOG     (char *) NULL
#define MIN_TO      10
#define DEF_TO      MIN_TO

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"server=host => server hostname or IP address\n");
    fprintf(stderr,"port=val    => TCP/IP port number\n");
    fprintf(stderr,"to=secs     => network write timeout\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"delay=secs  => set inter-record delay\n");
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char *argv[])
{
int i, sd, done, count;
char buffer[IDA_BUFSIZ];
char *host    = DEF_HOST;
char *service = DEF_SERVICE;
char *log     = DEF_LOG;
int  port     = DEF_PORT;
int  debug    = DEF_DEBUG;
int  delay    = DEF_DELAY;
int  to       = DEF_TO;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "server=", strlen("server=")) == 0) {
            host = argv[i] + strlen("server=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
            service = (char *) NULL;
        } else if (strncasecmp(argv[i], "delay=", strlen("delay=")) == 0) {
            delay = atoi(argv[i] + strlen("delay="));
            if (delay < 0) delay = 0;
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
            if (to < MIN_TO) to = MIN_TO;
        } else {
            help(argv[0]);
        }
    }

    if (debug) util_logopen(log, 1, 9, debug, NULL, argv[0]);

    sd = util_connect("localhost", service, port, "tcp", 0, 0);

    if (sd <= 0) {
        util_log(1, "util_connect: %s", syserrmsg(errno));
        exit(1);
    }
    util_noblock(sd);

    util_log(2, "begin read/write loop");

    done = count = 0;
    while (!done) {
        if (fread(buffer, sizeof(char), IDA_BUFSIZ, stdin) == IDA_BUFSIZ) {
            if (count) sleep(delay);
            if (util_write(sd, buffer, IDA_BUFSIZ, to) == IDA_BUFSIZ) {
                ++count;
            } else {
                util_log(1, "util_write: %s", syserrmsg(errno));
                done = 1;
            }
        } else {
            memset(buffer, 0, IDA_BUFSIZ);
            util_write(sd, buffer, IDA_BUFSIZ, to);
            done = 1;
        }
    }

    shutdown(sd, 2);
    close(sd);

    util_log(1, "%d records injected", count);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2006/02/10 02:13:01  dechavez
 * removed uneeded includes
 *
 * Revision 1.2  2001/05/20 18:39:32  dec
 * reduce default verbosity
 *
 * Revision 1.1.1.1  2000/02/08 20:20:05  dec
 * import existing IDA/NRTS sources
 *
 */
