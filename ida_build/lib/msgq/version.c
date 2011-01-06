#pragma ident "$Id: version.c,v 1.5 2007/01/07 17:53:27 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "msgq.h"

static VERSION version = {2, 1, 1};

/* msgq library release notes

2.1.1   01/07/2007
        version.c: snprintf() instead of sprintf()

2.1.0   05/04/2006
        new.c: added msgqInitBuf()

2.0.2   02/11/2005 (cvs rtag libmsgq_2_0_2 libmsgq)
        msgq.c: win32 portability mods (aap)

2.0.1   11/21/2003 (cvs rtag libmsgq_2_0_1 libmsgq)
        msgq.c: replaced util_hexdmp with utilPrintHexDump

2.0.0   06/09/2003 (cvs rtag libmsgq_2_0_0 libmsgq)
        ESSW imported release
  
 */

char *msgqVersionString(VOID)
{
static char string[] = "msgq library version 100.100.100 and slop";

    snprintf(string, strlen(string), "msgq library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *msgqVersion(VOID)
{
    return &version;
}
