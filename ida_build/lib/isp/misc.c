#pragma ident "$Id: misc.c,v 1.1 2002/03/15 22:45:08 dec Exp $"
/*======================================================================
 *
 *  Misc ISP helper functions
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2002 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "isp.h"

int ispPacketLength(UINT8 *packet)
{
IDA10_CMNHDR cmn;

    if (ida10UnpackCmnHdr(packet, &cmn) != 0) {
        return ida10PacketLength(&cmn);
    } else {
        return IDA_BUFSIZ;
    }
}

/* Revision History
 *
 * $Log: misc.c,v $
 * Revision 1.1  2002/03/15 22:45:08  dec
 * created
 *
 */
