#pragma ident "$Id: trim.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Trim the first 'n' characters off the front of each line.
 *  If 'n' is not specified (only command line argument) then n=7 is
 *  assumed.
 *
 *  Useful for cutting tags off of idalst output.
 *  For example: idalst [...] +tag < input | sort | trim > output
 *
 *====================================================================*/
#include "platform.h"

int main(int argc, char **argv)
{
char line[256];
int  num_trim;

    if (argc != 2) {
        num_trim = 7;
    } else {
        num_trim = atoi(argv[1]);
    }

    while (fgets(line, 255, stdin) != NULL) {
        printf("%s",line+num_trim);
    }

    if (ferror(stdin) || ferror(stdout)) exit(1);

    exit(0);
}

/* Revision History
 *
 * $Log: trim.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
