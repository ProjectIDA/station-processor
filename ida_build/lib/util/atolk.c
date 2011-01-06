#pragma ident "$Id: atolk.c,v 1.2 2001/05/07 22:40:12 dec Exp $"
/*======================================================================
 *
 *  Like regular atol(), expect strings of the form xk or xK are
 *  interpreted to mean x * 1024.
 *
 *====================================================================*/
#include <string.h>
#include "util.h"

long util_atolk(char *string)
{
int len;
long mult;

    len = strlen(string);
    if (string[len-1] == 'k' || string[len-1] == 'K') {
        string[len-1] = 0;
        mult = 1024L;
    } else {
        mult = 1L;
    }

    return atol(string) * mult;
}

/* Revision History
 *
 * $Log: atolk.c,v $
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:40  dec
 * import existing IDA/NRTS sources
 *
 */
