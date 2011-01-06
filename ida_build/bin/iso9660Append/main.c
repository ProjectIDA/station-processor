#pragma ident "$Id: main.c,v 1.1 2008/03/10 20:48:20 dechavez Exp $"
/*======================================================================
 *
 *  Append a file to an ISO 9660 image
 *
 *====================================================================*/
#include "iso9660.h"

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s image_name file_name\n", myname);
    exit(1);
}

int main (int argc, char **argv)
{
int i, error;
ISO9660 *iso;
char *image = NULL;
char *path = NULL;
BOOL verbose = FALSE;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (image == NULL) {
            image = argv[i];
        } else if (path == NULL) {
            path = argv[i];
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (path == NULL || image == NULL) help(argv[0]);

    if (verbose) printf("%s %s\n", argv[0], VersionIdentString);

    if ((iso = iso9660Open(image, &error)) == NULL) {
        fprintf(stderr, "%s: iso9660Open: %s: %s\n", argv[0], image, iso9660ErrorString(error));;
        exit(1);
    }

    if (iso9660Append(iso, path) != 0) {
        fprintf(stderr, "%s: %s: %s\n", argv[0], path, iso9660ErrorString(error));;
        exit(1);
    }

    if (verbose) printf("File `%s' appended to ISO image `%s'\n", path, image);

    iso9660Close(iso);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1  2008/03/10 20:48:20  dechavez
 * initial release
 *
 */
