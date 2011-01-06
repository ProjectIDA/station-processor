#pragma ident "$Id: dhlen.c,v 1.5 2006/02/08 23:04:10 dechavez Exp $"
/*======================================================================
 *
 *  Determine size of data and header parts of an IDA data record
 *
 *====================================================================*/
#include "ida.h"

void ida_dhlen(int rev, int *head, int *data)
{
    if (rev >= 5 && rev <= 8) {
        *head =  60;
        *data = 960;
    } else {
        *head =  64;
        *data = 960;
    }
}

/* Revision History
 *
 * $Log: dhlen.c,v $
 * Revision 1.5  2006/02/08 23:04:10  dechavez
 * removed the stream 0 overload
 *
 * Revision 1.4  2005/08/26 18:29:24  dechavez
 * added rev 9 support
 *
 * Revision 1.3  2005/05/25 22:37:54  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
