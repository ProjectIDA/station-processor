#pragma ident "$Id: modtime.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Print file modification time (Unix time).
 *
 *====================================================================*/
#include "platform.h"
#define PATHNAME argv[1]

int main(int argc, char **argv)
{
struct stat buf;
struct tm *mtime;

    if (argc != 2) {
        fprintf(stderr, "usage: modtime file_name\n");
        exit(1);
    }

    if (stat(PATHNAME, &buf) != 0) {
        perror(PATHNAME);
        exit(1);
    }

    printf("%ld\n", buf.st_mtime);
    exit(0);

}

/* Revision History
 *
 * $Log: modtime.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
