#pragma ident "$Id: substring.c,v 1.3 2008/02/01 21:30:28 dechavez Exp $"
/*======================================================================
 *
 *  does the same as "expr substing arg beg count".  It turns out that
 *  not all implementations of expr support the substr command,
 *  hence this program.
 *
 *====================================================================*/
#include "platform.h"

int main(int argc, char **argv)
{
int start, count, extra;
char *string, *substr;

    if (argc != 4) {
        fprintf(stderr, "usage: %s string start_index count\n", argv[0]);
        exit(1);
    }
    string = argv[1];
    start = atoi(argv[2]);
    count = atoi(argv[3]);

    if (start < 1 || count == 0) {
        printf("");
        exit(2);
    }

    if (strlen(string) < start) {
        printf("");
        exit(3);
    }

    extra = count - (strlen(string) - (start-1));
    if (extra > 0) count -= extra;

    substr = &string[start-1];
    substr[count] = 0;

    printf("%s\n", substr);
    exit(0);
}

/* Revision History
 *
 * $Log: substring.c,v $
 * Revision 1.3  2008/02/01 21:30:28  dechavez
 * fixed bug with long strings
 *
 * Revision 1.2  2008/02/01 19:05:36  dechavez
 * added missing argument to help message printf
 *
 * Revision 1.1  2008/02/01 17:37:10  dechavez
 * initial release
 *
 */
