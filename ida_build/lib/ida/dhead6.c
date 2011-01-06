#pragma ident "$Id: dhead6.c,v 1.2 2006/06/26 22:43:03 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 6 data record header.
 *
 *  A rev 6 data record is identical to a rev 5 except that the DAS
 *  filters 3 and 4 have half the decimation as in rev 5.
 *
 *====================================================================*/
#include "ida.h"
#include "util.h"
#include "protos.h"

int dhead_rev6(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
int retval;

    if ((retval = dhead_rev5(ida, dest, src)) != 0) return retval;

    if (dest->dl_filter == 3 || dest->dl_filter == 4) {
        dest->decim /= 2;
        dest->sint  /= 2.0;
    }

    return 0;
}

/* Revision History
 *
 * $Log: dhead6.c,v $
 * Revision 1.2  2006/06/26 22:43:03  dechavez
 * included protos.h for prototypes
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
