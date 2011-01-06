#pragma ident "$Id: pingwait.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define DEF_CMP "64 bytes from "
#define DEF_TO  60

void catch_timeout(sig)
int sig;
{
    printf("timeout\n");
    exit(0);
}

int main(int argc, char **argv)
{
char *buffer, *cmp_str = DEF_CMP;
int i, buflen, to = DEF_TO;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "string=", strlen("string=")) == 0) {
            cmp_str = argv[i] + strlen("string=");
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
        } else {
            fprintf(stderr,
                "usage: %s [string=\"compare string\" to=timeout]\n",
                argv[0]
            );
            exit(1);
        }
    }

    buflen = strlen(cmp_str) + 1;
    if ((buffer = (char *) malloc(buflen)) == (char *) NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("malloc");
        exit(1);
    }

    if (to > 0) {
        signal(SIGALRM, catch_timeout);
        alarm(to);
    }

    if (fgets(buffer, buflen, stdin) == (char *) NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("fgets");
        exit(1);
    }
    alarm(0);

    if (strncasecmp(buffer, cmp_str, strlen(cmp_str)) == 0) {
        printf("alive\n");
    } else {
        printf("unreachable\n");
    }

    exit(0);
}

/* Revision History
 *
 * $Log: pingwait.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
