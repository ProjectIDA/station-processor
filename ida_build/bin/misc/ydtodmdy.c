#pragma ident "$Id: ydtodmdy.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Given yyyyddd print Day mm/dd/yy
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

int main(int argc, char **argv)
{
long yyyyddd;
int  yyyy, ddd;
double dtime;

    if (argc != 2) {
        printf("            \n");
        exit(1);
    }

    yyyyddd = atol(argv[1]);
    yyyy    = yyyyddd/1000;
    ddd     = yyyyddd - (yyyy*1000);
    dtime   = util_ydhmsmtod(yyyy, ddd, 0, 0, 0, 0);
    printf("%s\n", util_dttostr(dtime, 5));

    exit(0);
}

/* Revision History
 *
 * $Log: ydtodmdy.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
