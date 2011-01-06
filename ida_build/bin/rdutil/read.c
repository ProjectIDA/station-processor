#pragma ident "$Id: read.c,v 1.2 2000/05/01 22:59:02 dec Exp $"
/*======================================================================
 *
 *  Read from raw disk device using rdio library calls.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "rdio.h"
#include "util.h"

main(argc, argv)
int argc;
char *argv[];
{
long i;
long nrec;
long count = -1;
RDISK *rd;
int debug = 0;
char *log = NULL;
long bytes;
char *device;
char *buffer;
long bufsiz = 32768;

    if (argc < 2) help();

    device = argv[1];

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            bufsiz = util_atolk(argv[i] + strlen("bs="));
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = util_atolk(argv[i] + strlen("count="));
            if (count <= 0) {
                fprintf(stderr,"rdread: illegal count=\n", argv[i]);
                exit(1);
            }
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else {
            help();
        }
    }

    if ((buffer = (char *) malloc(bufsiz)) == NULL) {
        perror("rdread: malloc");
        exit(1);
    }

    if (debug) util_logopen(log, 1, 9, debug, NULL, argv[0]);

    if ((rd = rdio_open(device, "r")) == (RDISK *) NULL) {
        fprintf(stderr,"rdread: rdio_open");
        perror(device);
        exit(1);
    }

    nrec = 0;
    while (1) {
        if (nrec == count) {
            rdio_close(rd);
            exit(0);
        }
        bytes = rdio_read(rd, buffer, bufsiz);
        if (bytes < 0) {
            perror("rdread: rdio_read");
            exit(1);
        } else if (bytes == 0) {
            exit(0);
        }
        if (fwrite(buffer, sizeof(char), bytes, stdout) != bytes) {
            perror("rdread: fwrite");
            exit(1);
        }
        ++nrec;
    }
}

int help()
{
    fprintf(stderr,"usage: rdread device [bs=# count=# log=name debug=#]\n");
    exit(1);
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.2  2000/05/01 22:59:02  dec
 * import existing sources
 *
 */
