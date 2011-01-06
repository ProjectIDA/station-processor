#pragma ident "$Id: version.c,v 1.3 2007/09/26 23:02:55 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "detect.h"

static VERSION version = {1, 0, 1};

/* detect library release notes

1.0.1   09/26/2007
        stalta.c: update LTA constantly, but retain frozen LTA for
           use in computing detrigger ratio

1.0.0   09/25/2007
        Initial release, STA/LTA trigger
  
 */

char *detectVersionString()
{
static char string[] = "detect library version 100.100.100 and slop";

    snprintf(string, strlen(string), "detect library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *detectVersion()
{
    return &version;
}
