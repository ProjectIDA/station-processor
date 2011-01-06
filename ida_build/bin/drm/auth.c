#pragma ident "$Id: auth.c,v 1.1.1.1 2000/02/08 20:19:58 dec Exp $"
/*======================================================================
 *
 * See if the requestor appears on the authorized users list.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "service.h"

#define GETLINE (status = util_getline(fp,buffer,DRM_BUFLEN,'#',&lineno))

int authorized(char *address)
{
struct drm_names *name;
char buffer[DRM_BUFLEN], *token[8];
int i, status, lineno, ntoken;
static FILE *fp = (FILE *) NULL;
static char *fid = "authorized";

    name = drm_names();
    if (fp == (FILE *) NULL) fp = fopen(name->path.auth, "r");
    if (fp == (FILE *) NULL) return FALSE;
    rewind(fp);

    lineno = 0;

    while (GETLINE == 0) {
        ntoken = util_parse(buffer, token, " ", 8, 0);
        for (i = 0; i < ntoken; i++) {
            if (strcasecmp(token[i], address) == 0) return TRUE;
        }
    }

    util_log(1, "%s is not authorized for dial-up access", address);

    return FALSE;
}

/* Revision History
 *
 * $Log: auth.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
