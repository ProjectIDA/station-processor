#pragma ident "$Id: version.c,v 1.19 2008/01/15 19:35:23 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "dbio.h"

static VERSION version = {3, 3, 0};

/* dbio library release notes

3.3.0   01/15/2008
        ff.c: fixed convoluted logic in looking up sint map

3.2.7   10/30/2007
        ff.c: replaced string memcpy with strlcpy

3.2.6   10/05/2007
        glob.c: added support for NRTS_HOME environment variable

3.2.5   03/07/2007
        ff.c: fixed insane FreeChanmap()

3.2.4   01/11/2007
        ff.c: snprintf
        util.c: snprintf

3.2.3   01/08/2007
        ff.c: strlcpy() instead of strncpy()
        glob.c: strlcpy() instead of strncpy()
        open.c: strlcpy() instead of strncpy()
        version.c: snprintf() instead of sprintf()

3.2.2   12/21/2006
        open.c: log the reason for mysql_real_connect() failures

3.2.1   11/08/20063.2.1   11/08/2006
        glob.c: added ttag to ISI_DEBUG

3.2.0   08/30/2006
        ff.c: added support for ISI_SYSTEM

3.1.6   07/07/2006
        glob.c: removed duplicate code

3.1.5   06/26/2006
        ffdb.h: change trecs to UINT32
        glob.c: change trecs to UINT32

3.1.4   06/16/2006
        open.c: aap mysql_real_connect changes, presumably to calm Win32 builds

3.1.3   03/20/2006
        util.c: support builds witout MYSQL support

3.1.2   03/15/2006
        ff.c: check for NULL map in FreeChanmap(), OK if isi.cfg is missing
        glob.c: fixed empty return when path is NULL

3.1.1   03/14/2006
        ff.c: fixed typo generating isi.cfg path name
        glob.c: fixed parse error for home dir, detect/ignore dbid

3.1.0   03/13/2006
        ff.c: added IDAFF_GLOB support
        glob.c: initial release (brought over from libisidl)

3.0.0   02/08/2006
        close.c: idaff support
        open.c: added LOGIO parameter, idaff support
        ff.c: initial release
        util.c: initial release

2.0.0   07/25/2005
        closec: support improved handle
        open.c: support improved handle

1.0.0   04/22/2004 (cvs rtag libdbio_1_0_0 libdbio)
        Initial release
  
 */

char *dbioVersionString()
{
static char string[] = "dbio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "dbio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *dbioVersion()
{
    return &version;
}
