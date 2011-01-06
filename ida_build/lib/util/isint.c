#pragma ident "$Id: isint.c,v 1.3 2005/05/25 22:41:46 dechavez Exp $"
/*======================================================================
 *
 *  Test to see if a string represents a legal decimal integer.
 *
 *====================================================================*/
#include <ctype.h>
#include "util.h"

int util_isinteger(char *string)
{
int i, ok;

    for (i = 0; i < (int) strlen(string); i++) {
        ok = (isdigit(string[i]) || string[i] == '+' || string[i] == '-');
        if (!ok) return 0;
    }

    return 1;

}

/* Revision History
 *
 * $Log: isint.c,v $
 * Revision 1.3  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
