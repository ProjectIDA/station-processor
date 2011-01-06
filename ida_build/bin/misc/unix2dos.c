#pragma ident "$Id: unix2dos.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Convert all <LF>'s to <CR<LF>'s
 *
 *====================================================================*/
#include <stdio.h>
#include <ctype.h>
#include "util.h"

int main(int argc, char **argv)
{
char c, CR = 0x0d;

    if (SETMODE(fileno(stdin), O_BINARY) == -1) {
        perror("unix2dos: setmode");
        exit(1);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("unix2dos: setmode");
        exit(1);
    }

    while (fread(&c, sizeof(char), 1L, stdin) == 1) {
        if (c == 0x0a && fwrite(&CR, sizeof(char), 1L, stdout) != 1) {
            perror("unix2dos: fwrite");
            exit(1);
        }
        if (fwrite(&c, sizeof(char), 1L, stdout) != 1) {
            perror("unix2dos: fwrite");
            exit(1);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: unix2dos.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
