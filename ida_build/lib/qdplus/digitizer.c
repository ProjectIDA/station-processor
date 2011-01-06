#pragma ident "$Id: digitizer.c,v 1.2 2007/05/17 22:25:05 dechavez Exp $"
/*======================================================================
 *
 * QDPLUS_DIGITIZER utilities
 *
 *====================================================================*/
#include "qdplus.h"

/* Destroy all these lists cleanly */

void qdplusDestroyDigitizer(QDPLUS_DIGITIZER *digitizer)
{
    if (digitizer == NULL) return;
    qdpDestroyLCQ(&digitizer->lcq);
    listDestroy(digitizer->state);
}

void qdplusDestroyDigitizerList(LNKLST *list)
{
LNKLST_NODE *crnt;
QDPLUS_DIGITIZER *digitizer;

    if (list == NULL) return;
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        qdplusDestroyDigitizer(digitizer);
        crnt = listNextNode(crnt);
    }
    listDestroy(list);
}

/* Find an existing digitizer entry */

QDPLUS_DIGITIZER *LocateDigitizer(QDPLUS *handle, UINT64 serialno)
{
QDPLUS_DIGITIZER *digitizer;
LNKLST_NODE *crnt;

    crnt = listFirstNode(handle->list.digitizer);
    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        if (digitizer->serialno == serialno) return digitizer;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

/* Create a new digitizer entry */

static QDPLUS_DIGITIZER *CreateDigitizer(QDPLUS *handle, UINT64 serialno)
{
QDPLUS_DIGITIZER new, *digitizer;

    new.serialno = serialno;
    new.state = NULL;

    if (!qdpInitLCQ(&new.lcq, &handle->par.lcq)) return NULL;
    if (!listAppend(handle->list.digitizer, &new, sizeof(QDPLUS_DIGITIZER))) return NULL;
    if ((digitizer = LocateDigitizer(handle, serialno)) == NULL) return NULL;

    return digitizer;
}

/* Get the handle for a given digitizer, creating one if necessary */

QDPLUS_DIGITIZER *qdplusGetDigitizer(QDPLUS *handle, UINT64 serialno)
{
QDPLUS_DIGITIZER *digitizer;

    if ((digitizer = LocateDigitizer(handle, serialno)) != NULL) {
        return digitizer;
    } else {
        return CreateDigitizer(handle, serialno);
    }
}

/* Load a packet into the handle */

QDPLUS_DIGITIZER *qdplusLoadPacket(QDPLUS *handle, QDPLUS_PKT *pkt)
{
QDP_LCQ *lcq;
QDPLUS_DIGITIZER *digitizer;

    if (handle == NULL || pkt == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((digitizer = qdplusGetDigitizer(handle, pkt->serialno)) == NULL) return NULL;
    lcq = &digitizer->lcq;
    lcq->state.seqno = pkt->seqno;

    if (lcq->fresh) {
        if (!qdplusInitializeLCQMetaData(lcq, handle->list.meta, pkt->serialno, &pkt->seqno)) {
            return NULL;
        }
    }

    return digitizer;
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
 * $Log: digitizer.c,v $
 * Revision 1.2  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
