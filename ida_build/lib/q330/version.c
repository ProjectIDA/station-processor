#pragma ident "$Id: version.c,v 1.7 2008/03/11 20:47:31 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "q330.h"

static VERSION version = {1, 0, 6};

/* q330 library release notes

1.0.6   03/11/2008
        cfg.c: silently ensure calib duration does not exceed hardware max

1.0.5   12/20/2007
        cfg.c: silently ensure calib divisor does not exceed hardware max

1.0.4   12/14/2007
        cfg.c: added calib support

1.0.3   10/31/2007
        cfg.c: q330LookupAddr() changed to pass Q330_ADDR
        register.c: initial release

1.0.2   09/25/2007
        cfg.c: set errno correctly when unable to find a cfg file, fix
           q330LookupDetector() return code when lookup fails

1.0.1   09/25/2007
        cfg.c: added detector support

1.0.0   09/21/2007
        cfg.c: initial release
 */

char *q330VersionString()
{
static char string[] = "q330 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "q330 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *q330Version()
{
    return &version;
}
