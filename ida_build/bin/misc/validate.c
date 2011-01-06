#pragma ident "$Id: validate.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Given a date of the form yyyyddd, determine if if is valid, ie
 *  does year yyyy have a day dddd?
 *
 *  Exit status of 0 if yes, 1 if not (reflects status of last arg).
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int main(int argc, char **argv)
{
long yyyyddd;
int i, year, day, status;

    if (argc < 2) {
        fprintf(stderr,"usage: validate yyyyddd [yyyyddd ...]\n");
        exit(2);
    }

    for (i = 1; i < argc; i++) {
        yyyyddd = atol(argv[i]);
        year = yyyyddd / 1000;
        day  = yyyyddd - (year * 1000);
        if (day > 0 && day <= daysize(year)) {
            status = 0;
            printf("%s: OK\n", argv[i]);
        } else {
            status = 1;
            printf("%s: invalid\n", argv[i]);
        }
    }

    exit(status);
}

/* Revision History
 *
 * $Log: validate.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
