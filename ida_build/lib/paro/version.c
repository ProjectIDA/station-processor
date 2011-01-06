#pragma ident "$Id: version.c,v 1.8 2009/05/14 16:29:59 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "paro.h"

static VERSION version = {1, 3, 0};

/* paro library release notes

1.3.0   05/14/2009
        paro.h: added PARO_BR
        paro.c: added support for PARO_BR (baud rate)

1.2.1   03/24/2009
        paro.c: removed param from paroOpen, added debug.  Removed verbose
           from param field and print now based on handle debug flag.

1.2.0   03/20/2009
        paro.c: added XM, IA, XT, XN, PI, and TI support, added device
           revision awarness (not all units support these new commands)

1.1.1   01/07/2007
        version.c: snprintf() instead of sprintf()

1.1.0   10/17/2006
        paro.h: added verbose field to PARO_PARAM and PARO structures,
                increased buffer size to 64 (not really needed)
        paro.c: added printf's when verbose flag is set, make sure that
                errno is preserved when paroOpen fails

1.0.2   06/26/2006
        paro.c: treat PA and PM as the floats they are instead of ints

1.0.1   04/03/2006
        paro.c: turned down logging volume, cleaned up memory/device leaks
                in paroOpen()

1.0.0   03/30/2006
        paro.c: reworked to use new libttyio calls

 */

char *paroVersionString()
{
static char string[] = "paro library version 100.100.100 and slop";

    snprintf(string, strlen(string), "paro library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *paroVersion()
{
    return &version;
}
