#pragma ident "$Id: ts.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/* test a server */
#include "util.h"

#define BUFLEN 1024

int main(int argc, char **argv)
{
char *server;
int port, timeout, sd;
long got, want = BUFLEN;
char buffer[BUFLEN];

    if (argc != 4) {
        fprintf(stderr, "usage: %s server port timeout\n", argv[0]);
        exit(1);
    }

    server  = argv[1];
    port    = atoi(argv[2]);
    timeout = atoi(argv[3]);

    if ((sd = util_connect(server, NULL, port, "tcp", 0, 0)) < 0) {
        perror("util_connect");
        exit(1);
    }
    util_noblock(sd);

    while (1) {
        got = util_read(sd, buffer, want, timeout);
        if (got != want) perror("util_read");
        if (got > 0) {
            if (fwrite(buffer, sizeof(char), got, stdout) != got) {
                perror("fwrite");
                exit(1);
            }
        }
        if (got != want) exit(0);
    }
}

/* Revision History
 *
 * $Log: ts.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
