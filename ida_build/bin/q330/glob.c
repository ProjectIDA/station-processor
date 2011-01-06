#pragma ident "$Id: glob.c,v 1.2 2009/02/04 17:47:19 dechavez Exp $"
/*======================================================================
 *
 *  Request Global Programming
 *
 *====================================================================*/
#include "q330cc.h"

BOOL VerifyGlob(Q330 *q330)
{
    q330->cmd.code = Q330_CMD_GLOB;
    if (q330->cmd.arglst->count != 0) return BadArgLstCount(q330);
    return TRUE;
}

BOOL glob(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C1_GLOB glob;

    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) {
        fprintf(stderr, "qdp_C1_RQDCP failed\n");
    } else {
        qdpPrint_C1_GLOB(stdout, &glob);
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
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
 * $Log: glob.c,v $
 * Revision 1.2  2009/02/04 17:47:19  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.1  2009/01/27 06:05:36  dechavez
 * initial release
 *
 */
