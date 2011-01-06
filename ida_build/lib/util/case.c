#pragma ident "$Id: case.c,v 1.4 2005/05/25 22:41:46 dechavez Exp $"
/*======================================================================
 *
 *  util_lcase: lower case a string
 *  util_ucase: upper case a string
 *
 *====================================================================*/
#include "util.h"

char *util_lcase(char *c)
{
int i;

    if (c == NULL) {
        errno = EINVAL;
        return NULL;
    }

    for (i=0;i < (int) strlen(c);i++) if (isupper(c[i])) c[i] = tolower(c[i]);
    return c;

}

char *util_ucase(char *c)
{
int    i;

    if (c == NULL) {
        errno = EINVAL;
        return NULL;
    }

    for (i=0;i < (int) strlen(c);i++) if (islower(c[i])) c[i] = toupper(c[i]);
    return c;

}

/* Revision History
 *
 * $Log: case.c,v $
 * Revision 1.4  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2004/07/26 23:20:41  dechavez
 * removed uneccesary includes
 *
 * Revision 1.2  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
