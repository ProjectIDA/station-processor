#pragma ident "$Id: init.c,v 1.17 2008/10/02 22:48:30 dechavez Exp $"
/*======================================================================
 * 
 * Initialization routine.  Completes the handle and starts the machine.
 *
 *====================================================================*/
#define INCLUDE_FSA_TABLE
#include "qdp.h"

static void InitReorderBuffer(QDP *qp)
{
int i;

    qp->recv.count = 0;
    qp->recv.last_packet = 0;
    for (i = 0; i < 4; i++) qp->recv.ack[i] = 0;
    for (i = 0; i < QDP_MAX_WINDOW_NBUF; i++) qp->recv.msg[i] = NULL;
    utilInitTimer(&qp->recv.timer);
}

static BOOL InitPktQ(QDP *qp)
{
    if (!msgqInitBuf(&qp->Q.pkt, qp->par.nbuf, sizeof(QDP_PKT))) return FALSE;
    msgqSetLog(&qp->Q.pkt.heap, qp->lp);
    msgqSetLog(&qp->Q.pkt.full, qp->lp);

    return TRUE;
}

static BOOL InitEventQ(QDP *qp)
{
    if (!msgqInitBuf(&qp->Q.event, 1, sizeof(QDP_EVENT))) return FALSE;
    msgqSetLog(&qp->Q.event.heap, qp->lp);
    msgqSetLog(&qp->Q.event.full, qp->lp);

    return TRUE;
}

static BOOL InitCmdQ(QDP *qp)
{
MSGQ_MSG *msg;
QDP_CMD *cmd;

    if (!msgqInitBuf(&qp->Q.cmd, 1, sizeof(QDP_PKT))) return FALSE;
    msgqSetLog(&qp->Q.event.heap, qp->lp);
    msgqSetLog(&qp->Q.event.full, qp->lp);

    SEM_INIT(&qp->cmd.sem, 0, 1);
    qp->cmd.msg = NULL;

    return TRUE;
}

BOOL qdpInit(QDP *qp, QDP_PAR *par, LOGIO *lp)
{
struct sockaddr_in addr;
static char *fid = "qdpInit";

    if (qp == NULL || par == NULL ) {
        logioMsg(lp, LOG_ERR, "%s: NULL input not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    qp->lp = lp;
    qp->par = *par;
    MUTEX_INIT(&qp->mutex);
    SEM_INIT(&qp->sem, 0, 1);
    qp->regerr = QDP_CERR_NOERR;
    qp->state = QDP_STATE_IDLE;
    InitReorderBuffer(qp);
    utilInitTimer(&qp->open);
    qdpInitMemBlk(&qp->meta.raw.token);

    snprintf(qp->peer.ident, QDP_IDENT_LEN + 1, "%016llX", qp->par.serialno);
    if (!utilSetHostAddr(&addr, par->host, 0)) {
        logioMsg(lp, LOG_ERR, "%s: utilSetHostAddr: %s: %s", fid, par->host, strerror(errno));
        return FALSE;
    }
    qp->peer.addr = ntohl(addr.sin_addr.s_addr);

    switch (par->port.link) {
      case QDP_CFG_PORT:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_CONFIG;
        qp->peer.port.data = 0;
        qp->dataLink = FALSE;
        break;
      case QDP_LOGICAL_PORT_1:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_1_CTRL;
        qp->peer.port.data = par->port.base + QDP_PORT_1_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_1;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP1;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_2:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_2_CTRL;
        qp->peer.port.data = par->port.base + QDP_PORT_2_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_2;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP2;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_3:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_3_CTRL;
        qp->peer.port.data = par->port.base + QDP_PORT_3_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_3;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP3;
        qp->dataLink = TRUE;
        break;
      case QDP_LOGICAL_PORT_4:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_4_CTRL;
        qp->peer.port.data = par->port.base + QDP_PORT_4_DATA;
        qp->par.hbeat.bitmap |= QDP_STATUS_DATA_PORT_4;
        qp->meta.raw.token.type = QDP_MEM_TYPE_CONFIG_DP4;
        qp->dataLink = TRUE;
        break;
      case QDP_SFN_PORT:
        qp->peer.port.ctrl = par->port.base + QDP_PORT_SPECIAL;
        qp->peer.port.data = 0;
        qp->dataLink = FALSE;
        break;
      default:
        logioMsg(lp, LOG_ERR, "%s: illegal link port '%d'", fid, par->port.link);
        errno = EINVAL;
        return FALSE;
    }
    if (!udpioInit(&qp->ctrl, par->port.ctrl.value, par->timeout.ctrl, lp)) {
        logioMsg(lp, LOG_ERR, "%s: udpioInit (ctrl port %d): %s", fid, par->port.ctrl.value, strerror(errno));
        return FALSE;
    }
    qdpDebug(qp, "UDP ctrl port %d ready", qp->peer.port.ctrl);
    if (qp->peer.port.data != 0) {
        if (!udpioInit(&qp->data, par->port.data.value, par->timeout.data, lp)) {
            logioMsg(lp, LOG_ERR, "%s: udpioInit (data port %d): %s", fid, par->port.data.value, strerror(errno));
            return FALSE;
        }
        qdpDebug(qp, "UDP data port %d ready", qp->peer.port.data);
    }

/* select the appropriate automaton */

    qp->fsa = qp->dataLink ? DataFSA : ControlFSA;

/* init the circular buffer message queues */

    if (!InitEventQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitEventQ: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!InitPktQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitPktQ: %s", fid, strerror(errno));
        return FALSE;
    }
    if (!InitCmdQ(qp)) {
        logioMsg(lp, LOG_ERR, "%s: InitCmdQ: %s", fid, strerror(errno));
        return FALSE;
    }

/* init the stats */

    qdpInitStats(qp);

/* init the outbound sequence number */

    qp->seqno = 0;

/* init the timer */

    qp->start = utilTimeStamp();

/* done */

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
 * $Log: init.c,v $
 * Revision 1.17  2008/10/02 22:48:30  dechavez
 * used QDP_STATUS_DATA_PORT_x instead of QDP_LOGICAL_PORT_x_STATUS
 *
 * Revision 1.16  2007/10/31 17:14:18  dechavez
 * replaced sprintf with snprintf
 *
 * Revision 1.15  2007/09/06 18:28:17  dechavez
 * include port number in udpioInit failure messages
 *
 * Revision 1.14  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
