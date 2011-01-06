#pragma ident "$Id: clock.c,v 1.2 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 *
 * Clock quality calculation, as per Q330 DP Writer's Guide
 *
 *====================================================================*/
#include "qdp.h"

UINT8 qdpClockQuality(QDP_TOKEN_CLOCK *dp_clock, QDP_MN232_DATA *mn232)
{
UINT8 result;

    if (mn232->qual >= QDP_PLL_TRACK || (mn232->qual & QDP_CQ_IS_LOCKED) != 0) {
        switch (mn232->qual & QDP_PLL_MASK) {
          case QDP_PLL_LOCK:  result = dp_clock->pll.locked; break;
          case QDP_PLL_TRACK: result = dp_clock->pll.track; break;
          case QDP_PLL_HOLD:  result = dp_clock->pll.hold; break;
          default:            result = dp_clock->pll.off; break;
        }
    } else if (mn232->qual & QDP_CQ_HAS_BEEN_LOCKED != 0) {
        if (dp_clock->maxlim != 0) {
            result = dp_clock->maxhbl - (mn232->loss / dp_clock->maxlim);
        } else {
            result = dp_clock->maxhbl;
        }
        if (result < dp_clock->minhbl) result = dp_clock->minhbl;
    } else {
        result = dp_clock->nbl;
    }

    return result;
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
 * $Log: clock.c,v $
 * Revision 1.2  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
