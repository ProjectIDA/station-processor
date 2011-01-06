#pragma ident "$Id: gmt.c,v 1.3 2005/09/13 15:33:18 dechavez Exp $"
/*======================================================================
 *
 *  Print current system time (GMT) in seconds and in ascii form.
 *
 *====================================================================*/
#include <stdio.h>
#include <time.h>
#include "util.h"

int main(int argc, char **argv)
{
long ltime;
int i;
int raw = 0;
int print = 0;
int yyyyddd = 0;
int yyyydddhhmmss1 = 0;
int yyyydddhhmmss2 = 0;

    ltime = (long) time(NULL);

    if (argc == 1) printf("%ld\n", ltime);

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "raw") == 0) {
            raw = 1;
        } else if (strcmp(argv[i], "yyyyddd") == 0) {
            yyyyddd = 1;
        } else if (strcmp(argv[i], "yyyydddhhmmss") == 0) {
            yyyydddhhmmss1 = 1;
        } else if (strcmp(argv[i], "yyyy:ddd-hh:mm:ss") == 0) {
            yyyydddhhmmss2 = 1;
        }
    }
    if (raw) {
        printf("%ld", ltime);
        ++print;
    }
    if (yyyydddhhmmss2) {
        if (print) printf(" ");
        printf("%s",  util_lttostr(ltime, 0));
        ++print;
    }
    if (yyyydddhhmmss1) {
        if (print) printf(" ");
        printf("%s",  util_lttostr(ltime, 3));
        ++print;
    }
    if (yyyyddd)      {
        if (print) printf(" ");
        printf("%s",  util_lttostr(ltime, 4));
        ++print;
    }
    if (print) printf("\n");

    exit(0);
}

/* Revision History
 *
 * $Log: gmt.c,v $
 * Revision 1.3  2005/09/13 15:33:18  dechavez
 * assume raw if no options given
 *
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
