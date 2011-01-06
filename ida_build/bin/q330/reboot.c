#pragma ident "$Id: reboot.c,v 1.3 2007/12/20 23:20:59 dechavez Exp $"
/*======================================================================
 *
 *  Reboot Q330
 *
 *====================================================================*/
#include "q330cc.h"

BOOL VerifyReboot(Q330 *q330)
{
    q330->cmd.code = Q330_CMD_REBOOT;
    if (q330->cmd.arglst->count != 0) return BadArgLstCount(q330);
    return TRUE;
}

void boot(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_REBOOT, TRUE)) {
        printf("reboot command failed\n");
    } else {
        printf("reboot command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: reboot.c,v $
 * Revision 1.3  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
