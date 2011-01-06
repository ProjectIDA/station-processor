#pragma ident "$Id: checkout.c,v 1.1 2009/02/04 17:32:45 dechavez Exp $"
/*======================================================================
 *
 *  checkout request
 *
 *====================================================================*/
#include "q330cc.h"

BOOL VerifyCheckout(Q330 *q330)
{
    q330->cmd.code = Q330_CMD_CO;
    if (q330->cmd.arglst->count != 0) return BadArgLstCount(q330);
    return TRUE;
}

void checkout(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_CHECKOUT co;

    if (!qdpRequestCheckout(q330->qdp, &co, q330->addr.name)) {
        fprintf(stderr, "qdpRequestCheckout failed\n");
    } else {
        qdpPrintCheckoutPackets(stdout, &co);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: checkout.c,v $
 * Revision 1.1  2009/02/04 17:32:45  dechavez
 * initial release
 *
 */
