#pragma ident "$Id: version.c,v 1.4 2007/01/07 17:47:06 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "udpio.h"

static VERSION version = {1, 0, 3};

/* udpio library release notes

1.0.3  01/07/2007
       version.c: snprintf() instead of sprintf()

1.0.2  06/26/2006
       udpio.c: removed unreferenced local variables

1.0.1  05/18/2006
       udpio.c: fixed incorrect return value in udpioInit()

1.0.0  05/05/2006
       udpio.c: initial release

 */

char *udpioVersionString()
{
static char string[] = "udpio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "udpio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *udpioVersion()
{
    return &version;
}
