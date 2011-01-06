#pragma ident "$Id: version.c,v 1.1 2008/08/20 20:45:27 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "iboot.h"

static VERSION version = {1, 0, 0};

/* iboot library release notes

1.0.0   08/20/2008
        Initial release
  
 */

char *ibootVersionString()
{
static char string[] = "iboot library version 100.100.100 and slop";

    snprintf(string, strlen(string), "iboot library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *ibootVersion()
{
    return &version;
}
