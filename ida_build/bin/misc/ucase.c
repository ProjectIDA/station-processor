#pragma ident "$Id: ucase.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  ucase stdin
 *
 *====================================================================*/
#include <stdio.h>
#include <ctype.h>
#include "util.h"

int main(int argc, char **argv)
{
char c;

    if (SETMODE(fileno(stdin), O_BINARY) == -1) {
        perror("ucase: setmode");
        exit(1);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("ucase: setmode");
        exit(1);
    }

    while ((c = getchar()) != EOF) {
        if (islower(c)) c = toupper(c);
        if (fwrite(&c, sizeof(char), 1L, stdout) != 1) {
            perror("ucase: fwrite");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: ucase.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
