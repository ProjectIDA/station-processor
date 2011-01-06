#pragma ident "$Id: version.c,v 1.9 2007/01/07 17:46:29 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "sanio.h"

static VERSION version = {1, 2, 4};

/* sanio library release notes

1.2.4  01/07/2007
       version.c: snprintf() instead of sprintf()

1.2.3  12/16/2004 (cvs rtag libsanio_1_2_3 libsanio)
       pack.c: recognize (and ignore) SANIO_CONFIG_PLLNMEAN
       sanio.h: defined SANIO_CONFIG_PLLNMEAN

1.2.2  06/24/2004 (cvs rtag libsanio_1_2_2 libsanio)
       sanio.h, pack.c: rename "operator" field to "isOperator"

1.2.1  12/09/2003 (cvs rtag libsanio_1_2_1 libsanio)
       connect.c: use sign/unsigned specific utilUnpack...s
       pack.c: use sign/unsigned specific utilUnpack...s, casts to calm
           solaris cc

1.2.0  09/03/2002 (cvs rtag libsanio_1_2_0 libsanio)
       SAN kernel 1.11.2 support

1.1.2  09/18/2001 (cvs rtag libsanio_1_1_2 libsanio)
       Fix sanioConnect error when util_connect fails with sd=0
       Added additional constants to sanioMessageTypeToString()

1.1.1  05/31/2001 (cvs rtag libsanio_1_1_1 libsanio)
       Recognize (but ignore) SAN_CONFIG_ASNPARAMS

1.1.0  05/02/2001 (cvs rtag libsanio_1_1_0 libsanio)
       SAN kernel 1.3.0 support, and added sanioCmndToString()

1.0.0  01/24/2001 (cvs rtag libsanio_1_0_0 libsanio)
       Initial production release

 */

char *sanioVersionString()
{
static char string[] = "sanio library version 100.100.100 and slop";

    snprintf(string, strlen(string), "sanio library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *sanioVersion()
{
    return &version;
}
