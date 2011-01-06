#pragma ident "$Id: process.c,v 1.6 2007/05/17 22:25:05 dechavez Exp $"
/*======================================================================
 *
 * Process a QDPLUS packet
 *
 *====================================================================*/
#include "qdplus.h"

QDP_LCQ *qdplusProcessPacket(QDPLUS *handle, QDPLUS_PKT *pkt)
{
QDPLUS_DIGITIZER *digitizer;

    if (handle == NULL || pkt == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((digitizer = qdplusLoadPacket(handle, pkt)) == NULL) return NULL;
    return qdpProcessPacket(&digitizer->lcq, &pkt->qdp) ? &digitizer->lcq : NULL;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: process.c,v $
 * Revision 1.6  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
