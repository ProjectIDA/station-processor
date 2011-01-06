#pragma ident "$Id: version.c,v 1.6 2009/03/17 18:21:52 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "ttyio.h"

static VERSION version = {1, 2, 3};

/* ttyio library release notes

1.2.3   03/17/2009
        misc.c: THREAD_DETACH in ttyioStartReadThread

1.2.2   01/07/2007
        version.c: snprintf() instead of sprintf()

1.2.1   11/03/2005
        misc.c: fixed SetThreadPriority parameters
        win32.c: fixed fid declaration

1.2.0   10/06/2005
        unix.c: added LOGIO argument to ttyioOpen()
        win32.c: added LOGIO argument to ttyioOpen(), switched to logioMsg logging

1.1.0   06/10/2005
        string.c: initial release
        unix.c: added ttyioSetLog, removed ibuf and obuf from handle
        win32.c: removed ibuf and obuf from handle

1.0.0   05/26/2005
        Initial release
  
 */

char *ttyioVersionString()
{
static char string[] = "ttyio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "ttyio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *ttyioVersion()
{
    return &version;
}
