#pragma ident "$Id: action.c,v 1.12 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * FSA event handlers for all events except for handling of DT_DATA.
 * That is taken care of in reorder.c
 *
 *====================================================================*/
#include "qdp.h"
#include "action.h"

/* Figure out if a packet is a reply to a FSA or user command */

#define FROM_AUTOMATON 0
#define FROM_USER      1
#define FROM_NOBODY    2

static int sender(QDP *qp, QDP_PKT *pkt)
{
QDP_PKT *usr;

    usr = (qp->cmd.msg != NULL) ? (QDP_PKT *) qp->cmd.msg->data : NULL;
    if (usr != NULL && usr->hdr.seqno == pkt->hdr.ack) return FROM_USER;
    if (qp->pkt.hdr.seqno == pkt->hdr.ack) return FROM_AUTOMATON;

    return FROM_NOBODY;
}

/* Send an admin (library generated) packet */

void ActionSEND(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSEND";

    qdpSendPkt(qp, QDP_CTRL, &qp->pkt);
}

/* Issue server request */

void ActionSRVRQ(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSRVRQ";

    qdpEncode_C1_RQSRV(&qp->pkt, qp->par.serialno);
    ActionSEND(qp, event);
}

/* Build server challenge response */

void ActionBLDRSP(QDP *qp, QDP_EVENT *event)
{
QDP_TYPE_C1_SRVCH c1_srvch;
QDP_TYPE_C1_SRVRSP c1_srvrsp;
static char *fid = "ActionBLDRSP";

    qdpDecode_C1_SRVCH(((QDP_PKT *) event->msg->data)->payload, &c1_srvch);
    c1_srvrsp.serialno = qp->par.serialno;
    c1_srvrsp.challenge = c1_srvch.challenge;
    c1_srvrsp.dp = c1_srvch.dp;
    c1_srvrsp.random = utilTimeStamp();
    qdpMD5(&c1_srvrsp, qp->par.authcode);
    qdpEncode_C1_SRVRSP(&qp->pkt, &c1_srvrsp);
}

/* This layer up (ie, newly registered) */

void ActionTLU(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionTLU";

    qp->regerr = QDP_CERR_NOERR;
    qdpSetStatsTstamp(qp);
    qdpIncrStatsNreg(qp);
    qdpInfo(qp, "%s handshake complete", qp->peer.ident);
    SEM_POST(&qp->sem);
    if (qp->dataLink) {
        if (qp->par.meta.func != NULL) (qp->par.meta.func)(qp->par.meta.arg, &qp->meta);
        utilStartTimer(&qp->recv.timer, qp->recv.ack_to);
        utilStartTimer(&qp->open, (UINT64) qp->par.interval.open * NANOSEC_PER_MSEC);
    }
}

/* This layer down (ie, no longer registered) */

void ActionTLD(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionTLD";

    qdpInfo(qp, "%s connection closed", qp->peer.ident);
    qdpLogStats(qp);
    qdpResetSessionStats(qp);
    qp->meta.raw.token.nbyte = 0;
}

/* Sleep for a bit */

void ActionSLEEP(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionSLEEP";

    qdpDebug(qp, "%s: sleep for %d msec", fid, qp->par.interval.retry);
    utilDelayMsec((UINT32) qp->par.interval.retry);
    qdpDebug(qp, "%s: awake", fid);
}

/* Note registration error */

void ActionREGERR(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionREGERR";

    pkt = (QDP_PKT *) event->msg->data;
    qp->regerr = qdpErrorCode(pkt);
    qdpInfo(qp, "%s registration error %d (%s)", qp->peer.ident, qp->regerr, qdpErrString(qp->regerr));
    qdpReturnPktMsgToHeap(fid, qp, event->msg);

}

/* Process a C1_CERR on a registered link */

void ActionCERR(QDP *qp, QDP_EVENT *event)
{
int error;
QDP_PKT *pkt, *usr;
static char *fid = "ActionCERR";

    pkt = (QDP_PKT *) event->msg->data;
    error = qdpErrorCode(pkt);

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpError(qp, "%s automaton command error %d (%s)", qp->peer.ident, error, qdpErrString(error));
        ActionFAIL(qp, event);
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qdpWarn(qp, "%s -> %s error %d (%s)", qdpCmdString(usr->hdr.cmd), qp->peer.ident, error, qdpErrString(error));
        qp->cmd.ok = FALSE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpReturnPktMsgToHeap(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        qdpWarn(qp, "unexpected error %d (%s) from %s", error, qdpErrString(error), qp->peer.ident);
        ActionDROP(qp, event);
    }
}

/* Process a C1_CACK on a registered link */

void ActionCACK(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt, *usr;
static char *fid = "ActionCACK";

    pkt = (QDP_PKT *) event->msg->data;

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpDebug(qp, "%s heartbeat ACK received", qp->peer.ident);
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s -> %s ACK received", qdpCmdString(usr->hdr.cmd), qp->peer.ident);
        qp->cmd.ok = TRUE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpReturnPktMsgToHeap(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        ActionDROP(qp, event);
    }
}

/* Process a non-data packet on a registered link */

void ActionCTRL(QDP *qp, QDP_EVENT *event)
{
int error;
QDP_PKT *pkt, *usr;
static char *fid = "ActionCTRL";

    pkt = (QDP_PKT *) event->msg->data;

    switch (sender(qp, pkt)) {
      case FROM_AUTOMATON:
        qdpDebug(qp, "%s heartbeat %s received", qp->peer.ident, qdpCmdString(pkt->hdr.cmd));
        if (qp->par.hbeat.forward) {
            qdpForwardFullPktMsg(fid, qp, event->msg);
        } else {
            qdpReturnPktMsgToHeap(fid, qp, event->msg);
        }
        break;
      case FROM_USER:
        usr = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s -> %s returns %s", qdpCmdString(usr->hdr.cmd), qp->peer.ident, qdpCmdString(pkt->hdr.cmd));
        qp->cmd.ok = TRUE;
        msgqPut(&qp->Q.cmd.heap, qp->cmd.msg);
        qp->cmd.msg = NULL;
        qdpForwardFullPktMsg(fid, qp, event->msg);
        SEM_POST(&qp->cmd.sem);
        break;
      default:
        ActionDROP(qp, event);
    }
}

/* Request configuration data */

void ActionRQCNF(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionRQCNF";

    qdpInfo(qp, "%s %s configuration requested", qp->peer.ident, qdpPortString(qp->par.port.link));

    qdpEncode_C1_RQFLGS(&qp->pkt, (UINT16) qp->par.port.link - 1);
    qp->pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Load configuration data (initialize reorder buffer) */

#define MS100_TO_NANOSEC 
void ActionLDCNF(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionLDCNF";

    qdpInfo(qp, "%s %s configuration received", qp->peer.ident, qdpPortString(qp->par.port.link));
    pkt = (QDP_PKT *) event->msg->data;
    qdpDecode_C1_COMBO(pkt->payload, &qp->combo);
    memcpy(qp->meta.raw.combo, pkt->payload, pkt->hdr.dlen);

    qp->recv.last_packet = qp->combo.log.dataseq;
    qp->recv.ack_to = ((UINT64) qp->combo.log.ack_to * 100) * NANOSEC_PER_MSEC;
    ActionINITDACK(qp);
    qdpDebug(qp, "%s ack_to set to %llu msec", qp->peer.ident, qp->recv.ack_to / NANOSEC_PER_MSEC);
    qdpDebug(qp, "%s last_packet set to %hu", qp->peer.ident, qp->recv.last_packet);
    qdpForwardFullPktMsg(fid, qp, event->msg);
}

/* Request DP tokens */

void ActionRQMEM(QDP *qp, QDP_EVENT *event)
{
int i;
QDP_TYPE_C1_RQMEM c1_rqmem;
static char *fid = "ActionRQMEM";

    if (qp->meta.raw.token.nbyte == 0) {
        qdpInfo(qp, "%s %s DP tokens requested", qp->peer.ident, qdpPortString(qp->par.port.link));
        c1_rqmem.offset = 0;
    } else {
        c1_rqmem.offset = qp->c1_mem.seg * QDP_MAX_C1_MEM_PAYLOAD;
    }
    c1_rqmem.nbyte = 0;
    c1_rqmem.type = qp->meta.raw.token.type;
    qdpDebug(qp, "%s: offset=%lu, nbyte=%hu, type=%s", fid, c1_rqmem.offset, c1_rqmem.nbyte, qdpMemTypeString(c1_rqmem.type));
    for (i = 0; i < 4; i++) c1_rqmem.passwd[i] = 0;
    qdpEncode_C1_RQMEM(&qp->pkt, &c1_rqmem);
    qp->pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Save DP tokens */

void ActionLDMEM(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionLDMEM";

    if (qdpSaveMem(&qp->meta.raw.token, &qp->c1_mem)) qdpInfo(qp, "%s %s DP tokens received, nbyte=%hu", qp->peer.ident, qdpPortString(qp->par.port.link), qp->meta.raw.token.nbyte);
    qdpForwardFullPktMsg(fid, qp, event->msg);
}

/* Open a data port */

void ActionOPEN(QDP *qp, QDP_EVENT *event)
{
QDP_PKT pkt;
static char *fid = "ActionOPEN";

    if (!qp->dataLink) return;

    qdpInitPkt(&pkt, QDP_DT_OPEN, 0, 0);
    qdpSendPkt(qp, QDP_DATA, &pkt);
    utilResetTimer(&qp->open);
}

/* Send DT_OPEN when no data received after specified interval */

void ActionDTO(QDP *qp, QDP_EVENT *event)
{
QDP_PKT pkt;
static char *fid = "ActionDTO";

    if (!qp->dataLink) return;

    if (utilTimerExpired(&qp->open)) ActionOPEN(qp, event);
}

/* Send a heartbeat (status request) */

void ActionHBEAT(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionHBEAT";

    qdpEncode_C1_RQSTAT(&qp->pkt, qp->par.hbeat.bitmap);
    qp->pkt.hdr.seqno = ++qp->seqno;
    ActionSEND(qp, event);
}

/* Send any pending user command to the digitizer */

void ActionUSRCMD(QDP *qp, QDP_EVENT *event)
{
QDP_PKT *pkt;
static char *fid = "ActionUSRCMD";

    if (qp->cmd.msg != NULL) {
        pkt = (QDP_PKT *) qp->cmd.msg->data;
        if (pkt->hdr.seqno == 0) pkt->hdr.seqno = ++qp->seqno;
        qdpSendPkt(qp, QDP_CTRL, pkt);
    }
}

/* Send DT_DACK if needed */

static BOOL AckRequired(QDP *qp)
{
static char *fid = "AckRequired";

/* Do send an ack if the ACK timeout interval has passed */

    if (utilTimerExpired(&qp->recv.timer)) {
        qdpDebug(qp, "%s ack timer expired with count=%hu, forcing DT_DACK", qp->peer.ident, qp->recv.count);
        return TRUE;
    }

/* Do send an ack if we've received exceeded the acknowledge count */

    if (qp->recv.count >= qp->combo.log.ack_cnt) {
        qdpDebug(qp, "%s recv count=%hu, ack_cnt=%hu, DT_DACK required", qp->peer.ident, qp->recv.count, qp->combo.log.ack_cnt);
        return TRUE;
    }

/* Otherwise, we wait */

    qdpDebug(qp, "%s recv count=%hu, ack_cnt=%hu, no DT_DACK needed", qp->peer.ident, qp->recv.count, qp->combo.log.ack_cnt);
    return FALSE;
}

void ActionDACK(QDP *qp, QDP_EVENT *event)
{
int i;
QDP_PKT pkt;
UINT8 *ptr;
static char *fid = "ActionDACK";

    if (!qp->dataLink) return;

    if (!AckRequired(qp)) return;

    qdpInitPkt(&pkt, QDP_DT_DACK, 0, qp->recv.ack_seqno);
    ptr = pkt.payload;
    ptr += utilPackUINT16(ptr, 0); /* DYNAMIC THROTTLE NOT CURRENTLY SUPPORTED */
    ptr += sizeof(UINT16); /* skip over spare */
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, qp->recv.ack[i]);
    ptr += sizeof(UINT32); /* skip over spare */
    pkt.hdr.dlen = (int) (ptr - pkt.payload);
    qdpSendPkt(qp, QDP_DATA, &pkt);
    ActionINITDACK(qp);
    qp->recv.count = 0;
    utilResetTimer(&qp->recv.timer);
    
}

/* DT_FILL packets get tossed */

void ActionFILL(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionFILL";

    qdpDebug(qp, "%s", fid);
    qdpIncrStatsFill(qp);
    qdpReturnPktMsgToHeap(fid, qp, event->msg);
}

/* Drop an unexpected packet */

void ActionDROP(QDP *qp, QDP_EVENT *event)
{
UINT8 cmd;
static char *fid = "ActionDROP";

    cmd = ((QDP_PKT *) event->msg->data)->hdr.cmd;
    qdpInfo(qp, "%s: spurious %s dropped", qp->peer.ident, qdpCmdString(cmd));
    qdpReturnPktMsgToHeap(fid, qp, event->msg);
    qdpIncrStatsDrop(qp);
}

/* Flush the reorder buffer */

void ActionFLUSH(QDP *qp, QDP_EVENT *event)
{
int i, count;
static char *fid = "ActionFLUSH";

    if (!qp->dataLink) return;

    for (i = 0; i < 4; i++) qp->recv.ack[0] = 0;

    for (count = 0, i = 0; i < QDP_MAX_WINDOW_NBUF; i++) {
        if (qp->recv.msg[i] != NULL) {
            qdpReturnPktMsgToHeap(fid, qp, event->msg);
            qp->recv.msg[i] = NULL;
            ++count;
        }
    }
    qp->recv.count = 0;
    qdpDebug(qp, "tossed %d packets from %s reorder buffer", count, qp->peer.ident);
}

/* Note and toss corrupt packets */

void ActionCRCERR(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionCRCERR";

    qdpInfo(qp, "%s CRC error, packet dropped", qp->peer.ident);
    qdpIncrStatsCrc(qp);
}

/* Library abort */

void ActionFAIL(QDP *qp, QDP_EVENT *event)
{
static char *fid = "ActionFAIL";

    qdpError(qp, "%s: FATAL ERROR: unable to continue", qp->peer.ident);
    ActionTLD(qp, event);
    event->next = QDP_STATE_OFF;
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
 * $Log: action.c,v $
 * Revision 1.12  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
