#pragma ident "$Id: attol.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Given the time in ascii (yy:ddd-hh:mm:ss.msc) file format, return
 *  the  equivalent Unix (long) time (msecs truncated to zero).
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

#define MAXSTRLEN 19

int main(int argc, char **argv)
{
long ltime;
char *ptr;
char string[MAXSTRLEN+1];

    if (argc == 1) {
        scanf("%s", string);
        ptr = string;
    } else {
        ptr = argv[1];
    }

    printf("%ld\n", (long) util_attodt(ptr));

    exit(0);
}

/* Revision History
 *
 * $Log: attol.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:08  dec
 * import existing IDA/NRTS sources
 *
 */
