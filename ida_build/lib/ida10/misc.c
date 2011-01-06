#pragma ident "$Id: misc.c,v 1.1 2006/12/22 02:46:59 dechavez Exp $"
/*======================================================================
 *
 *  Miscellaneous stuff
 *
 *====================================================================*/
#include "ida10.h"

/* insert a 32 bit sequence number into those packets that support it */

BOOL ida10InsertSeqno32(UINT8 *packet, UINT32 seqno)
{
int offset;

    if (packet == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* make sure it is an IDA10 packet */

    if (packet[2] != 10) {
        errno = EINVAL;
        return FALSE;
    }

/* find the appropriate offset, if any */

    switch (packet[3]) {
      case 2:
      case 3: offset = 28; break;
      case 4: offset = 36; break;
      default:
        return TRUE;
    }

    utilPackUINT32(&packet[offset], seqno);

    return TRUE;
}

/* Revision History
 *
 * $Log: misc.c,v $
 * Revision 1.1  2006/12/22 02:46:59  dechavez
 * initial release
 *
 */
