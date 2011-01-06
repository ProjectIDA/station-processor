#pragma ident "$Id: version.c,v 1.13 2009/02/23 21:50:51 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "qdplus.h"

static VERSION version = {1, 2, 0};

/* qdplus library release notes

1.2.0   02/23/2009
        print.c: added qdplusPrintPkt()

1.1.1   12/16/2008
        state.c: fixed empty qhlp state file bug

1.1.0   12/15/2008
        handle.c: set handle->state in qdplusCreateHandle()
        state.c: use clean/dirty flag in state file

1.0.0   05/17/2007
        Initial production release
  
 */

char *qdplusVersionString()
{
static char string[] = "qdplus library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qdplus library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qdplusVersion()
{
    return &version;
}
