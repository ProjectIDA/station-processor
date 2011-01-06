#pragma ident "$Id: desc.c,v 1.1 2005/10/11 22:42:18 dechavez Exp $"
/*======================================================================
 *
 *  Write datascope style descriptor
 *
 *====================================================================*/
#include "cssio.h"

BOOL cssioWriteDescriptor(char *base, char *prefix)
{
#ifdef unix

FILE *fp;
char *dbpath, path[MAXPATHLEN+1];

    if (base == NULL || prefix == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    sprintf(path,"%s%c%s", base, PATH_DELIMITER, prefix);
    if ((fp = fopen(path, "w")) == NULL) return FALSE;
    fprintf(fp, "css3.0\n");
    if ((dbpath = getenv(CSS30_DATABASE_ENVSTR)) != NULL) fprintf(fp, "%s\n", dbpath);
    fclose(fp);

#endif /* unix */

    return TRUE;
}

/* Revision History
 *
 * $Log: desc.c,v $
 * Revision 1.1  2005/10/11 22:42:18  dechavez
 * initial release
 *
 */
