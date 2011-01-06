#pragma ident "$Id: dos2unix.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Remove all <CR>'s from stdin
 *
 *====================================================================*/
#include <stdio.h>
#include <ctype.h>
#include "util.h"

int main(int argc, char **argv)
{
char c;

    if (SETMODE(fileno(stdin), O_BINARY) == -1) {
        perror("dos2unix: setmode");
        exit(1);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("dos2unix: setmode");
        exit(1);
    }

    while (fread(&c, sizeof(char), 1, stdin) == 1) {
        if (c != 0x0d && fwrite(&c, sizeof(char), 1L, stdout) != 1) {
            perror("dos2unix: fwrite");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: dos2unix.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
