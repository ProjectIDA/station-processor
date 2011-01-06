#pragma ident "$Id: version.c,v 1.8 2007/01/07 17:33:55 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "cssio.h"

static VERSION version = {2, 1, 6};

/* cssio library release notes

2.1.6   01/07/2007
        css2sac.c: strlcpy() instead of strcpy()
        togse.c: strlcpy() instead of strcpy()
        wf28to30.c: strlcpy() instead of strcpy()
        wwfdisc.c: strlcpy() instead of strcpy()
        version.c: snprintf() instead of sprintf()

2.1.5   10/11/2005
        desc.c: initial release

2.1.4   03/31/2005 (cvs rtag libcssio_2_1_4 libcssio)
        css2sac.c: fixed bug generating sac time stamp

2.1.3   02/10/2005 (cvs rtag libcssio_2_1_3 libcssio)
        css2sac.c, togse.c: aap win32 portability mods

2.1.2   08/24/2003 (cvs rtag libcssio_2_1_2 libcssio)
        togse.c: split long chan names into chan and auxid

2.1.1   11/13/2003 (cvs rtag libcssio_2_1_1 libcssio)
        css2sac.c: moved SAC specific stuff into css2sac to allow exporting the
        rest of the library without having to drag the sac stuff as well.

2.1.0   10/15/2003 (cvs rtag libcssio_2_1_0 libcssio)
        wwfdisc.c: added reentrant cssioWfdiscString() and recast
            wdtoa() as a call to it with a static (gag) buffer

2.0.0   06/19/2003
        ESSW imported release
  
 */

char *cssioVersionString(VOID)
{
static char string[] = "cssio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "cssio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *cssioVersion(VOID)
{
    return &version;
}
