#pragma ident "$Id: netread.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 * 
 *  Read from a protocol free socket and write to stdout.
 *
 *====================================================================*/
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include "util.h"

static void help(char *myname)
{
    fprintf(stderr, "%s: host=name port=number bs=count\n", myname);
    exit(1);
}

#define DEFAULT_HOST "san"
#define DEFAULT_PORT 10010
#define DEFAULT_BS   1024
int main(int argc, char **argv)
{
static char *default_host = DEFAULT_HOST;
char *host = default_host;
int port   = DEFAULT_PORT;
int bs     = DEFAULT_BS;
int i, sd, n;
char *buf;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "host=", strlen("host=")) == 0) {
            host = argv[i] + strlen("host=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i]+strlen("port="));
        } else if (strncasecmp(argv[i], "bs=", strlen("bs=")) == 0) {
            bs = atoi(argv[i]+strlen("bs="));
        } else {
            help(argv[0]);
        }
    }

    if (host == (char *) NULL || port < 0 || bs < 0) help(argv[0]);

    if ((buf = (char *) malloc(bs)) == (char *) NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("malloc");
        exit(1);
    }

    sd = util_connect(host, NULL, port, "tcp", 0, 0);
    if (sd <= 0) {
        fprintf(stderr, "%s: can't connect to %s:%d\n", argv[0],host,port);
        exit(1);
    }

    while ((n = read(sd, buf, bs)) > 0) write(fileno(stdout), buf, n);
}

/* Revision History
 *
 * $Log: netread.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
