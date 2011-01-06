#pragma ident "$Id: msgq.c,v 1.3 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 *  packet message queue I/O
 *
 *====================================================================*/
#include "qdp.h"

MSGQ_MSG *qdpGetEmptyPktMsg(char *fid, QDP *qp, int wait)
{
MSGQ_MSG *msg;

    qdpDebug(qp, "%s: requesting empty packet message from heap", fid);
    msg = msgqGet(&qp->Q.pkt.heap, wait);
    if (msg != NULL) {
        if (!msgqCheck(msg)) {
            qdpError(qp, "%s: message queue is corrupt", fid);
            exit(1);
        }
        qdpDebug(qp, "%s: empty packet message received", fid);
    } else {
        qdpDebug(qp, "%s: no empty packets available and wait not set", fid);
    }
    return msg;
}

void qdpReturnPktMsgToHeap(char *fid, QDP *qp, MSGQ_MSG *msg)
{
    qdpDebug(qp, "%s: packet message returned to heap", fid);
    if (!msgqCheck(msg)) {
        qdpError(qp, "%s: message queue is corrupt", fid);
        exit(1);
    }
    msgqPut(&qp->Q.pkt.heap, msg);
}

void qdpForwardFullPktMsg(char *fid, QDP *qp, MSGQ_MSG *msg)
{
QDP_PKT *pkt;

    pkt = (QDP_PKT *) msg->data;
    if (qp->par.user.func != NULL) {
        qdpDebug(qp, "%s: call user supplied packet function", fid);
        (qp->par.user.func)(qp->par.user.arg, (QDP_PKT *) msg->data);
        qdpDebug(qp, "%s: user supplied packet function returns", fid);
        qdpReturnPktMsgToHeap(fid, qp, msg);
    } else {
        qdpDebug(qp, "%s: packet message inserted in full queue", fid);
        if (!msgqCheck(msg)) {
            qdpError(qp, "%s: message queue is corrupt", fid);
            exit(1);
        }
        msgqPut(&qp->Q.pkt.full, msg);
    }
}

MSGQ_MSG *qdpGetFullPktMsg(char *fid, QDP *qp, int wait)
{
MSGQ_MSG *msg;

    qdpDebug(qp, "%s: requesting full packet message", fid);
    msg = msgqGet(&qp->Q.pkt.full, wait);
    if (msg != NULL) {
        if (!msgqCheck(msg)) {
            qdpError(qp, "%s: message queue is corrupt", fid);
            exit(1);
        }
        qdpDebug(qp, "%s: full packet message received", fid);
    } else {
        qdpDebug(qp, "%s: no full packets available and wait not set", fid);
    }
    return msg;
}

BOOL qdpNextPkt(QDP *qp, QDP_PKT *dest)
{
QDP_PKT *src;
MSGQ_MSG *msg;
static char *fid = "qdpNextPkt";

    if (qdpState(qp) == QDP_STATE_OFF) return FALSE;

    msg = qdpGetFullPktMsg(fid, qp, MSGQ_WAIT);
        src = (QDP_PKT *) msg->data;
        qdpCopyPkt(dest, src);
    qdpReturnPktMsgToHeap(fid, qp, msg);

    return TRUE;
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
 * $Log: msgq.c,v $
 * Revision 1.3  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
