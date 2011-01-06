#pragma ident "$Id"
/*======================================================================
 *
 *  Returns the filename component from a full path.  Allocates memory
 *  with strdup().
 *
 *====================================================================*/
#include "util.h"

#define MAXCOMPONENTS 256

char *utilBasename(char *path)
{
int ntoken;
char *copy, *token[MAXCOMPONENTS];
#define DELIMITERS "./\\"

    if ((copy = strdup(path)) == NULL) return NULL;

    ntoken = utilParse(copy, token, DELIMITERS, MAXCOMPONENTS, (char) NULL);
    return token[ntoken-1];
}

/* Revision History
 *
 * $Log: basename.c,v $
 * Revision 1.1  2003/06/10 00:04:46  dechavez
 * initial release
 *
 */
