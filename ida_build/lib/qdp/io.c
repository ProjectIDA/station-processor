#pragma ident "$Id: io.c,v 1.9 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * UDP I/O
 *
 *====================================================================*/
#include "qdp.h"

static BOOL RandomError()
{
    return rand() % 5 == 0 && rand() % 2 == 0 ? TRUE : FALSE;
}

static void DissectDACK(QDP *qp, QDP_PKT *pkt)
{
int i, j, bit;
UINT16 throttle;
UINT32 ack[4];
UINT8 *ptr;

    ptr = pkt->payload;
    ptr += utilUnpackUINT16(ptr, &throttle);
    ptr += sizeof(UINT16);
    for (i = 0; i < 4; i++) ptr += utilUnpackUINT32(ptr, &ack[i]);

    for (bit = 0, i = 0; i < 4; i++) {
        for (j = 0; j < 32; j++, bit++) {
            if (ack[i] & (1 << j)) qdpDebug(qp, "%s %hu acknowledged", qp->peer.ident, pkt->hdr.ack + bit);
        }
    }
}

/* Trash the link at random intervals */

THREAD_FUNC TrashLinkThread(void *argptr)
{
QDP *qp;
QDP_PKT pkt;
static char *fid = "TrashLinkThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s TrashLinkThread", fid);
    SEM_POST(&qp->sem);

    qdpEncode_C1_DSRV(&pkt, qp->par.serialno);
    qdpHostToNet(&pkt);

    while (1) {
        if (qdpState(qp) == QDP_STATE_OFF) {
            qdpError(qp, "%s: %s exits", qp->peer.ident, fid);
            THREAD_EXIT((void *) 0);
        }
        utilDelayMsec(qp->par.trash.interval);
        if (RandomError()) {
            MUTEX_LOCK(&qp->mutex);
                udpioSend(&qp->ctrl, qp->peer.addr, qp->peer.port.ctrl, pkt.raw, pkt.len);
            MUTEX_UNLOCK(&qp->mutex);
            qdpWarn(qp, "%s deregistration requested *** DEREGISTER ***", qp->peer.ident);
        }
    }
}

void qdpHostToNet(QDP_PKT *pkt)
{
UINT8 *ptr;

    ptr = pkt->raw + 4; /* skip over CRC for now */
    *ptr++ = pkt->hdr.cmd;
    *ptr++ = pkt->hdr.ver;
    ptr += utilPackUINT16(ptr, pkt->hdr.dlen);
    ptr += utilPackUINT16(ptr, pkt->hdr.seqno);
    ptr += utilPackUINT16(ptr, pkt->hdr.ack);
    ptr += utilPackBytes(ptr, pkt->payload, pkt->hdr.dlen);
    pkt->hdr.crc = qdpCRC(pkt->raw + 4, pkt->hdr.dlen + QDP_CMNHDR_LEN - 4);
    utilPackUINT32(pkt->raw, pkt->hdr.crc);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpNetToHost(QDP_PKT *pkt)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    ptr = pkt->raw;
    ptr += qdpDecodeCMNHDR(ptr, &pkt->hdr);
    pkt->payload = ptr;
    pkt->len = pkt->hdr.dlen + QDP_CMNHDR_LEN;
}

MSGQ_MSG *qdpRecvPkt(QDP *qp, UDPIO *up, int *status)
{
int got;
QDP_PKT *pkt;
MSGQ_MSG *msg;
static char *fid = "qdpRecvPkt";

    msg = qdpGetEmptyPktMsg(fid, qp, MSGQ_WAIT);
    pkt = (QDP_PKT *) msg->data;
    got = udpioRecv(up, pkt->raw, QDP_MAX_MTU);
    if (got > 0) {
        qdpNetToHost(pkt);
        if (qp->par.trash.pkts && RandomError()) {
            ++pkt->payload[0];
            qdpWarn(qp, "%s packet intentially corrupted *** TRASH ***", qp->peer.ident);
        }
        if (qdpVerifyCRC(pkt)) {
            if (pkt->len == got) {
                qdpDebug(qp, "%s -> %s(%d, %d)", qp->peer.ident, qdpCmdString(pkt->hdr.cmd), pkt->hdr.seqno, pkt->hdr.ack);
            } else {
                qdpWarn(qp, "%s: WARNING: len=%d, got=%d", fid, pkt->len, got);
            }
            *status = QDP_OK;
        } else {
            *status = QDP_CRCERR;
            qdpReturnPktMsgToHeap(fid, qp, msg);
            msg = NULL;
        }
    } else if (got == 0) {
        *status = QDP_TIMEOUT;
        qdpReturnPktMsgToHeap(fid, qp, msg);
        msg = NULL;
    } else {
        *status = QDP_IOERROR;
        qdpError(qp, "%s I/O error reading socket", qp->peer.ident);
        qdpReturnPktMsgToHeap(fid, qp, msg);
        msg = NULL;
    }

    return msg;
}

void qdpSendPkt(QDP *qp, int which, QDP_PKT *pkt)
{
UDPIO *up;
char *desc;
int port;
static char *ctrl = "ctrl";
static char *data = "data";
QDP_PKT TrashedPkt;
static char *fid = "qdpSendPkt";

    if (qp == NULL || pkt == NULL) return;

    if (which == QDP_CTRL) {
        up = &qp->ctrl;
        port = qp->peer.port.ctrl;
        desc = ctrl;
    } else {
        up = &qp->data;
        port = qp->peer.port.data;
        desc = data;
    }

    if (qp->par.trash.pkts && RandomError()) {
        qdpCopyPkt(&TrashedPkt, pkt);
        qdpHostToNet(&TrashedPkt);
        ++TrashedPkt.payload[0];
        qdpWarn(qp, "%s sent an intentially corrupted packet *** TRASH ***", qp->peer.ident);
        MUTEX_LOCK(&qp->mutex);
            udpioSend(up, qp->peer.addr, port, TrashedPkt.raw, TrashedPkt.len);
        MUTEX_UNLOCK(&qp->mutex);
    } else {
        qdpHostToNet(pkt);
            MUTEX_LOCK(&qp->mutex);
                udpioSend(up, qp->peer.addr, port, pkt->raw, pkt->len);
            MUTEX_UNLOCK(&qp->mutex);
        qdpNetToHost(pkt);
        qdpDebug(qp, "%s(%hu, %hu) -> %s (%s)", qdpCmdString(pkt->hdr.cmd), pkt->hdr.seqno, pkt->hdr.ack, qp->peer.ident, desc);
        if (pkt->hdr.cmd == QDP_DT_DACK) DissectDACK(qp, pkt);
    }
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
 * $Log: io.c,v $
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
