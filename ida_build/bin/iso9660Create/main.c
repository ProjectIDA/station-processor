#pragma ident "$Id: main.c,v 1.1 2008/03/10 20:50:05 dechavez Exp $"
/*======================================================================
 *
 *  Create a new ISO 9660 image
 *
 *====================================================================*/
#include "iso9660.h"

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [-v] name [ vol_id ]\n", myname);
    exit(1);
}

int main (int argc, char **argv)
{
int i;
ISO9660 *iso;
char *path = NULL;
char *label = NULL;
BOOL verbose = FALSE;
static char *DefaultLabel = "";

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (path == NULL) {
            path = argv[i];
        } else if (label == NULL) {
            label = argv[i];
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (path == NULL) help(argv[0]);
    if (label == NULL) label = DefaultLabel;

    if (verbose) printf("%s %s\n", argv[0], VersionIdentString);

    if ((iso = iso9660Create(path, label)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror(path);
        exit(1);
    }

    if (verbose) printf("ISO image `%s' created OK, label=`%s'\n", path, label);

    iso9660Close(iso);
    exit(0);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2008/03/10 20:50:05  dechavez
 * initial release
 *
 */
