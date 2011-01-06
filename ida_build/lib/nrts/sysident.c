#pragma ident "$Id: sysident.c,v 1.2 2004/06/24 17:34:52 dechavez Exp $"
/*======================================================================
 *
 * Given a string which is the name of a supported NRTS system type,
 * return the numeric equivalent or 0 if not supported.
 *
 *====================================================================*/
#include "nrts.h"

int nrts_sysident(string)
char *string;
{
    if (strcasecmp(string, "ida") == 0) {
        return NRTS_IDA;
    } else if (strcasecmp(string, "asl") == 0) {
        return NRTS_ASL;
    } else if (strcasecmp(string, "ida10") == 0) {
        return NRTS_IDA10;
    } else {
        return 0;
    }
}

/* Revision History
 *
 * $Log: sysident.c,v $
 * Revision 1.2  2004/06/24 17:34:52  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:31  dec
 * import existing IDA/NRTS sources
 *
 */
