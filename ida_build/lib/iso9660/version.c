#pragma ident "$Id: version.c,v 1.2 2008/03/10 20:51:17 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "iso9660.h"

static VERSION version = {1, 1, 0};

/* iso9660 library release notes

1.1.0   03/10/2008
        string.c: initial release

1.0.0   03/04/2008
        iso9660.c: initial release

 */

char *iso9660VersionString()
{
static char string[] = "iso9660 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "iso9660 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *iso9660Version()
{
    return &version;
}
