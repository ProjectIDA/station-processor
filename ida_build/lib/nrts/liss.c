#pragma ident "$Id: liss.c,v 1.2 2005/07/26 00:01:43 dechavez Exp $"
/*======================================================================
 *
 *  LISS packet support
 *
 *====================================================================*/
#include "nrts/dl.h"

BOOL nrtsLissDecoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out)
{
    errno = UNSUPPORTED;
    return FALSE;
}

/* Revision History
 *
 * $Log: liss.c,v $
 * Revision 1.2  2005/07/26 00:01:43  dechavez
 * switched to current calling syntax (still unsupported, though)
 *
 */
