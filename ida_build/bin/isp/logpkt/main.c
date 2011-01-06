#pragma ident "$Id: main.c,v 1.3 2006/02/10 02:12:42 dechavez Exp $"
/*======================================================================
 *
 *  Standard in filter to packetize data into IDA I (ISP Log) format.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "ida.h"

int main(int argc, char *argv[])
{
int i, nread;
int maxlen = IDA_BUFSIZ - 2;
int fullen = IDA_BUFSIZ;
char buffer[IDA_BUFSIZ], *ptr;

    buffer[0] = 'I';
    buffer[1] = ~buffer[0];
    ptr       = buffer + 2;

    while ((nread = fread(ptr, sizeof(char), maxlen, stdin)) > 0) {
        if (nread < maxlen) memset(ptr + nread, 0, maxlen - nread);
        if (fwrite(buffer, sizeof(char), fullen, stdout) != fullen) {
            fprintf(stderr, "%s: ", argv[0]);
            perror("fwrite");
            exit(1);
        }
    }

    if (ferror(stdin)) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("fread");
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2006/02/10 02:12:42  dechavez
 * removed uneeded includes
 *
 * Revision 1.2  2003/12/10 06:25:50  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:08  dec
 * import existing IDA/NRTS sources
 *
 */
