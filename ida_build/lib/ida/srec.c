#pragma ident "$Id: srec.c,v 1.4 2006/02/08 23:11:28 dechavez Exp $"
/*======================================================================
 *
 *  Copy from raw buffer to das stats structure
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

int ida_srec(IDA *ida, IDA_DAS_STATS *dest, UINT8 *src)
{

    if (ida->rev.value >= 8) {
        return srec_rev8(ida, dest, src);
    } else {
        errno = ENOTSUP;
        return -1;
    }
}

/* Revision History
 *
 * $Log: srec.c,v $
 * Revision 1.4  2006/02/08 23:11:28  dechavez
 * introduced IDA handle
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
