#pragma ident "$Id: version.c,v 1.14 2008/01/15 19:41:12 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "isi/db.h"

static VERSION version = {1, 3, 4};

/* isidb library release notes

1.3.4   01/15/2008
        datlen.c: fixed error in building channel name for comparison
        wrdsiz.c: fixed error in building channel name for comparison

1.3.3   10/31/2007
        chnlocmap.c: replaced string memcpy with strlcpy
        inst.c: replaced string memcpy with strlcpy
        revs.c: replaced string memcpy with strlcpy
        sitechan.c: replaced string memcpy with strlcpy
        sitechanflag.c: replaced string memcpy with strlcpy
        stamap.c: replaced string memcpy with strlcpy
        system.c: replaced string memcpy with strlcpy

1.3.2   10/05/2007
        root.c: added support for NRTS_HOME environment variable

1.3.1   06/04/2007
        system.c: fixed pointer problem in isidbLookupSysFlagsByName()

1.3.0   01/11/2007
        renamed all function to use isidb prefix

1.2.4   01/08/2007
        chnlocmap.c: switch to size-bounded string operations
        datlen.c: switch to size-bounded string operations
        root.c: switch to size-bounded string operations
        sint.c: switch to size-bounded string operations
        sintchanflag.c: switch to size-bounded string operations
        system.c: switch to size-bounded string operations
        version.c: switch to size-bounded string operations
        wrdsiz.c: switch to size-bounded string operations

1.2.3   01/05/2007
        sitechanflag.c: initial release
        system.c: added isidbLookupSystemByName()

1.2.2   11/08/2006
        glob.c: added ttag to ISI_DEBUG

1.2.1   11/08/2006
        system.c: added isidbLookupSysFlagsByName()

1.2.0   08/30/2006
        system.c: added ISI_SYSTEM support

1.1.2   06/26/2006
        coords.c: cast UndefinedCoords members to REAL32
        sitechan.c: fixed signed/unsigned comparison warnings
        srate.c: fixed uninitialized log handle error

1.1.1   04/20/2006
        calib.c: support builds witout MYSQL support
        chan.c:      "     "       "      "      "
        chnlocmap.c: "     "       "      "      "
        coords.c:    "     "       "      "      "
        datlen.c:    "     "       "      "      "
        inst.c:      "     "       "      "      "
        revs.c:      "     "       "      "      "
        sint.c:      "     "       "      "      "
        site.c:      "     "       "      "      "
        sitechan.c:  "     "       "      "      "
        srate.c:     "     "       "      "      "
        stamap.c:    "     "       "      "      "
        system.c:    "     "       "      "      "
        tqual.c:     "     "       "      "      "
        version.c:   "     "       "      "      "
        wrdsiz.c:    "     "       "      "      "


1.1.0   03/15/2006
        debug.c: initial release (faked MySQL support)
        root.c: initial release (faked MySQL support)
        trecs.c: initial release (faked MySQL support)

1.0.0   02/08/2006
        calib.c: initial release
        chan.c: initial release
        chnlocmap.c: initial release
        coords.c: initial release
        datlen.c: initial release
        inst.c: initial release
        revs.c: initial release
        sint.c: initial release
        site.c: initial release
        sitechan.c: initial release
        srate.c: initial release
        stamap.c: initial release
        system.c: initial release
        tqual.c: initial release
        wrdsiz.c: initial release
 */

char *isidbVersionString()
{
static char string[] = "isidb library version 100.100.100 and slop";

    snprintf(string, strlen(string), "isidb library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *isidbVersion()
{
    return &version;
}
