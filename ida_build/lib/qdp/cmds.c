#pragma ident "$Id: cmds.c,v 1.6 2008/10/02 22:51:57 dechavez Exp $"
/*======================================================================
 * 
 * Simple command utilities
 *
 *====================================================================*/
#include "qdp.h"

BOOL qdpPostCmd(QDP *qp, QDP_PKT *pkt, BOOL wait)
{
BOOL result;
QDP_PKT *cmd;
MSGQ_MSG *msg;

    if (qp == NULL || pkt == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    msg = msgqGet(&qp->Q.cmd.heap, MSGQ_WAIT);
    cmd = (QDP_PKT *) msg->data;
    qdpCopyPkt(cmd, pkt);
    msgqPut(&qp->Q.cmd.full, msg);
    if (wait) {
        SEM_WAIT(&qp->cmd.sem);
        return qp->cmd.ok;
    } else {
        return TRUE;
    }
}

BOOL qdpDeregister(QDP *qp, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_DSRV(&pkt, qp->par.serialno);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpRqflgs(QDP *qp, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_RQFLGS(&pkt, (UINT16) qp->par.port.link);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpCtrl(QDP *qp, UINT16 flags, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_CTRL(&pkt, flags);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpNoParCmd(QDP *qp, int command, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_NoParCmd(&pkt, command);
    return qdpPostCmd(qp, &pkt, wait);
}

BOOL qdpRqstat(QDP *qp, UINT32 bitmap, BOOL wait)
{
QDP_PKT pkt;

    qdpEncode_C1_RQSTAT(&pkt, bitmap);
    return qdpPostCmd(qp, &pkt, wait);
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
 * $Log: cmds.c,v $
 * Revision 1.6  2008/10/02 22:51:57  dechavez
 * added qdpNoParCmd, qdpRqstat
 *
 * Revision 1.5  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
