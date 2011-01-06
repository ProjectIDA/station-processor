#pragma ident "$Id: reorder.c,v 1.7 2008/12/10 03:12:44 dechavez Exp $"
/*======================================================================
 * 
 * Reorder buffer for DT_DATA packets
 *
 *====================================================================*/
#include "qdp.h"
#include "action.h"

/* Sequence number comparisons */

#define SEQ_GE(a, b) ((INT16) ((a)-(b)) >= 0)
#define SEQ_LE(a, b) ((INT16) ((a)-(b)) <= 0)

/* See if a packet is in the receive buffer window */

static BOOL OutOfWindow(QDP *qp, QDP_EVENT *event, UINT16 min, UINT16 max)
{
QDP_PKT *pkt;

    pkt = (QDP_PKT *) event->msg->data;
    qdpDebug(qp, "compare %hu with limits %hu and %hu", pkt->hdr.seqno, min, max);
    if (SEQ_GE(pkt->hdr.seqno, min) && SEQ_LE(pkt->hdr.seqno, max)) return FALSE;
    qdpDebug(qp, "%s data packet %hu is out of window (%hu - %hu)", qp->peer.ident, pkt->hdr.seqno, min, max);
    ActionDROP(qp, event);

    return TRUE;
}

static void SetAckBit(QDP *qp, UINT8 bit)
{
    if (bit >= 96) {
        qp->recv.ack[3] |= 1 << (bit - 96);
    } else if (bit >= 64) {
        qp->recv.ack[2] |= 1 << (bit - 64);
    } else if (bit >= 32) {
        qp->recv.ack[1] |= 1 << (bit - 32);
    } else {
        qp->recv.ack[0] |= 1 << bit;
    }
    qdpDebug(qp, "%s set ack bit %hu", qp->peer.ident, bit);
}

static void LogDup(QDP *qp, QDP_PKT *old, QDP_PKT *new)
{
    qdpError(qp, "old: cmd=0x%02x ver=0x%02x dlen=%hu len=%hu crc=0x%04x seqno=%hu, ack=%hu",
        old->hdr.cmd, old->hdr.ver, old->hdr.dlen, old->len, old->hdr.crc, old->hdr.seqno, old->hdr.ack
    );
    logioHexDump(qp->lp, LOG_INFO, old->payload, old->hdr.dlen);
    qdpError(qp, "new: cmd=0x%02x ver=0x%02x dlen=%hu len=%hu crc=0x%04x seqno=%hu, ack=%hu",
        new->hdr.cmd, new->hdr.ver, new->hdr.dlen, new->len, new->hdr.crc, new->hdr.seqno, new->hdr.ack
    );
    logioHexDump(qp->lp, LOG_INFO, new->payload, new->hdr.dlen);
}

static BOOL DuplicateData(QDP *qp, QDP_EVENT *event, UINT16 index)
{
QDP_PKT *old, *new;
static char *fid = "DuplicateData";

    if (qp->recv.msg[index] == NULL) return FALSE;

    old = (QDP_PKT *) qp->recv.msg[index]->data;
    new = (QDP_PKT *) event->msg->data;

    if (old->len != new->len || memcmp(old->raw, new->raw, old->len) != 0) {
        qdpError(qp, "%s: slot %hu is already full!", fid, index);
        LogDup(qp, old, new);
        ActionFAIL(qp, event);
        return TRUE;
    }

    qdpReturnPktMsgToHeap(fid, qp, event->msg);
    qdpInfo(qp, "duplicate %s DT_DATA(%hu, %hu) dropped", qp->peer.ident, new->hdr.seqno, new->hdr.ack);
    qdpIncrStatsDupData(qp);
    return TRUE;
}

/* Initialize data acknowledge */

void ActionINITDACK(QDP *qp)
{
int i;

    qp->recv.ack_seqno = qp->recv.last_packet - 1;
    for (i = 0; i < 4; i++) qp->recv.ack[i] = 0;
    SetAckBit(qp, 0);
}

/* Data packets go into the reorder buffer */

void ActionDATA(QDP *qp, QDP_EVENT *event)
{
UINT8 index, tmpndx;
QDP_PKT *pkt, *dbg;
UINT32 dtseqno;
static char *fid = "ActionDATA";

    pkt = (QDP_PKT *) event->msg->data;
    utilUnpackUINT32(pkt->payload, &dtseqno);
    qdpDebug(qp, "process %s DT_DATA %hu %lu", qp->peer.ident, pkt->hdr.seqno, dtseqno);

    qdpIncrStatsData(qp);
    utilResetTimer(&qp->open);

/* Reject the packet if it is outside the window (this can happen if DT_DACK is corrupted) */

    if (OutOfWindow(qp, event, qp->recv.last_packet, qp->recv.last_packet + SEQNO_MASK)) return;

/* Ignore the packet if it is a duplicate */

    index = pkt->hdr.seqno & SEQNO_MASK;
    if (DuplicateData(qp, event, index)) return;

/* Insert packet into the reorder buffer */

    qdpDebug(qp, "%s: insert packet %hu into slot %hu", fid, pkt->hdr.seqno, index);
    qp->recv.msg[index] = event->msg;
    ++qp->recv.count;

/* Release any contiguous packets up to the application */

    tmpndx = qp->recv.last_packet & SEQNO_MASK;
    while (qp->recv.msg[tmpndx] != NULL) {
        dbg = (QDP_PKT *) qp->recv.msg[tmpndx]->data;
        qdpDebug(qp, "%s packet %hu in slot %hu ready to deliver", qp->peer.ident, dbg->hdr.seqno, tmpndx);
        qdpForwardFullPktMsg(fid, qp, qp->recv.msg[tmpndx]);
        qp->recv.msg[tmpndx] = NULL;
        ++qp->recv.last_packet;
        qdpDebug(qp, "%s slot %hu cleared, last_packet=%hu", qp->peer.ident, tmpndx, qp->recv.last_packet);
        tmpndx = qp->recv.last_packet & SEQNO_MASK;
    }
    qdpDebug(qp, "last_packet = %hu after continutity checks", qp->recv.last_packet);

/* Update the acknowledge bitmask */

    SetAckBit(qp, (UINT8) (index - (qp->recv.ack_seqno & SEQNO_MASK)) & SEQNO_MASK);
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
 * $Log: reorder.c,v $
 * Revision 1.7  2008/12/10 03:12:44  dechavez
 * made "out of window" messages debug comments instead of warnings
 *
 * Revision 1.6  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
