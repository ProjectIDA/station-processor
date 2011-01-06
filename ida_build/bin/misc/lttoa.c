#pragma ident "$Id: lttoa.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Print current system time (GMT) in seconds and in ascii form.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

int main(int argc, char **argv)
{
long ltime;

    if (argc == 1) {
        scanf("%ld", &ltime);
    } else {
        ltime = atol(argv[1]);
    }

    printf("%s ",     util_lttostr(ltime, 0));
    printf("(%s)\n",  util_lttostr(ltime, 1));
    exit(0);
}

/* Revision History
 *
 * $Log: lttoa.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
