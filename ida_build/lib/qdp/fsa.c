#pragma ident "$Id: fsa.c,v 1.12 2009/03/20 21:16:31 dechavez Exp $"
/*======================================================================
 * 
 * QDP Finite State Automaton
 *
 *====================================================================*/
#define INCLUDE_FSA_STRINGS
#include "qdp.h"
#include "action.h"

static void TerminateSelf(char *fid, QDP *qp)
{
    qdpError(qp, "%s: %s exits", qp->peer.ident, fid);
    if (--qp->nthread == 0) SEM_POST(&qp->sem);
    THREAD_EXIT((void *) 0);
}

static int TokenEvent(QDP *qp, MSGQ_MSG *msg)
{
QDP_PKT *pkt;

    pkt = (QDP_PKT *) msg->data;
    qdpDecode_C1_MEM(pkt->payload, &qp->c1_mem);
    if (qp->c1_mem.type != qp->meta.raw.token.type) return QDP_EVENT_CTRL;
    return (qp->c1_mem.seg == qp->c1_mem.nseg) ? QDP_EVENT_TOKEN_DONE : QDP_EVENT_TOKEN_SOME;
}

static int CtrlEvent(QDP *qp, MSGQ_MSG *msg)
{
QDP_PKT *pkt;

    pkt = (QDP_PKT *) msg->data;
    switch (pkt->hdr.cmd) {

      case QDP_C1_CACK:
        return QDP_EVENT_CACK;

      case QDP_C1_SRVCH:
        return QDP_EVENT_SRVCH;

      case QDP_C1_FLGS:
        return QDP_EVENT_CONFIG;

      case QDP_C1_MEM:
        return TokenEvent(qp, msg);

      case QDP_C1_CERR:
        switch (qdpErrorCode(pkt)) {
          case QDP_CERR_NOTR:
            qdpInfo(qp, "%s deregistered", qp->peer.ident);
            return QDP_EVENT_NOTREG;
          case QDP_CERR_TMSERV:
            return QDP_EVENT_TOOMANY;
          default:
            return QDP_EVENT_CERR;
        }

      default:
        return QDP_EVENT_CTRL;
    }
}

static int DataEvent(QDP *qp, MSGQ_MSG *msg)
{
    switch (((QDP_PKT *) msg->data)->hdr.cmd) {

      case QDP_DT_DATA:
        return QDP_EVENT_DATA;

      case QDP_DT_FILL:
        return QDP_EVENT_FILL;

      default:
        return QDP_EVENT_NOSUP;
    }
}

int qdpState(QDP *qp)
{
int retval;

    if (qp == NULL) {
        errno = EINVAL;
        return -1;
    }

    MUTEX_LOCK(&qp->mutex);
        retval = qp->state;
    MUTEX_UNLOCK(&qp->mutex);

    return retval;
}

/* Logged state transitions */

static void ChangeState(char *fid, QDP *qp, int state)
{
    MUTEX_LOCK(&qp->mutex);
        qp->state = state;
    MUTEX_UNLOCK(&qp->mutex);
    qdpDebug(qp, "%s: transition to state '%s'", fid, StateString[qp->state]);
}

/* Process one event */

static void EventHandler(QDP *qp, QDP_EVENT *event)
{
int i, action, next;
static char *fid = "EventHandler";

    qdpDebug(qp, "%s: %s (current state: %s)", fid, EventString[event->code], StateString[qp->state]);

    event->next = qp->fsa[qp->state].event[event->code].next; /* might get changed by action handler */

    for (i = 0; i < MAX_ACTION_PER_EVENT; i++) {

        action = qp->fsa[qp->state].event[event->code].action[i];

        if (action < QDP_MIN_ACTION || action > QDP_MAX_ACTION) {
            qdpError(qp, "%s: FATAL ERROR: invalid action code '%d' encountered", fid, action);
            ChangeState(fid, qp, QDP_STATE_OFF);
            return;
        }

        if (action != QDP_ACTION_NONE) {
            qdpDebug(qp, "%s: %s", fid, ActionString[action]);
        } else {
            break;
        }

        switch (action) {

          case QDP_ACTION_SEND:
            ActionSEND(qp, event);
            break;

          case QDP_ACTION_SRVRQ:
            ActionSRVRQ(qp, event);
            break;

          case QDP_ACTION_BLDRSP:
            ActionBLDRSP(qp, event);
            break;

          case QDP_ACTION_TLU:
            ActionTLU(qp, event);
            break;

          case QDP_ACTION_TLD:
            ActionTLD(qp, event);
            break;

          case QDP_ACTION_SLEEP:
            ActionSLEEP(qp, event);
            break;

          case QDP_ACTION_REGERR:
            ActionREGERR(qp, event);
            break;

          case QDP_ACTION_CERR:
            ActionCERR(qp, event);
            break;

          case QDP_ACTION_CACK:
            ActionCACK(qp, event);
            break;

          case QDP_ACTION_CTRL:
            ActionCTRL(qp, event);
            break;

          case QDP_ACTION_RQCNF:
            ActionRQCNF(qp, event);
            break;

          case QDP_ACTION_LDCNF:
            ActionLDCNF(qp, event);
            break;

          case QDP_ACTION_RQMEM:
            ActionRQMEM(qp, event);
            break;

          case QDP_ACTION_LDMEM:
            ActionLDMEM(qp, event);
            break;

          case QDP_ACTION_OPEN:
            ActionOPEN(qp, event);
            break;

          case QDP_ACTION_DTO:
            ActionDTO(qp, event);
            break;

          case QDP_ACTION_HBEAT:
            ActionHBEAT(qp, event);
            break;

          case QDP_ACTION_USRCMD:
            ActionUSRCMD(qp, event);
            break;

          case QDP_ACTION_DACK:
            ActionDACK(qp, event);
            break;

          case QDP_ACTION_DATA:
            ActionDATA(qp, event);
            break;

          case QDP_ACTION_FILL:
            ActionFILL(qp, event);
            break;

          case QDP_ACTION_DROP:
            ActionDROP(qp, event);
            break;

          case QDP_ACTION_FLUSH:
            ActionFLUSH(qp, event);
            break;

          case QDP_ACTION_CRCERR:
            ActionCRCERR(qp, event);
            break;

          case QDP_ACTION_FAIL:
            ActionFAIL(qp, event);
            break;

          default:
            qdpError(qp, "%s: FATAL ERROR: '%s' action not implemented", fid, ActionString[action]);
            ChangeState(fid, qp, QDP_STATE_OFF);
            return;
        }
    }

/* Transition to the next state */

    if (event->next != QDP_STATE_NOCHANGE) {
        ChangeState(fid, qp, event->next);
    } else {
        qdpDebug(qp, "%s: remain in state: %s", fid, StateString[qp->state]);
    }
}

/* Post an event */

static BOOL PostEvent(QDP *qp, int code, MSGQ_MSG *pktmsg)
{
int type;
char *string;
MSGQ_MSG *msg;
QDP_EVENT *event;
static char *fid = "PostEvent";

/* This will block while another event is currently being processed */

    msg = msgqGet(&qp->Q.event.heap, MSGQ_WAIT);

/* Can't post events if the machine is off */

    if (qp->state == QDP_STATE_OFF) {
        msgqPut(&qp->Q.event.heap, msg);
        return FALSE;
    }

/* Load in our event parameters and post */

    event = (QDP_EVENT *) msg->data;
    event->code = code;
    event->msg = pktmsg;

    msgqPut(&qp->Q.event.full, msg);

    return TRUE;
}

/* Dispatch events */

static THREAD_FUNC EventThread(void *argptr)
{
QDP *qp;
MSGQ_MSG *msg;
QDP_EVENT *event;
static char *fid = "EventThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s started", fid);
    ++qp->nthread;
    SEM_POST(&qp->sem);

    while (1) {
        msg = msgqGet(&qp->Q.event.full, MSGQ_WAIT);
        event = (QDP_EVENT *) msg->data;
        if (event->code < QDP_MIN_EVENT || event->code > QDP_MAX_EVENT) {
            qdpError(qp, "%s: FATAL ERROR: illegal event code '%d' received", fid);
            ChangeState(fid, qp, QDP_STATE_OFF);
        } else {
            EventHandler(qp, event);
        }
        msgqPut(&qp->Q.event.heap, msg);
        if (qp->state == QDP_STATE_OFF) TerminateSelf(fid, qp);
    }
}

/* Receive packets on the data port */

static THREAD_FUNC DataThread(void *argptr)
{
QDP *qp;
int status;
MSGQ_MSG *msg;
static char *fid = "DataThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s started", fid);
    ++qp->nthread;
    SEM_POST(&qp->sem);

    while (1) {
        msg = qdpRecvPkt(qp, &qp->data, &status);
        switch (status) {
          case QDP_OK:
            if (!PostEvent(qp, DataEvent(qp, msg), msg)) TerminateSelf(fid, qp);
            break;
          case QDP_TIMEOUT:
            if (!PostEvent(qp, QDP_EVENT_DTO, NULL)) TerminateSelf(fid, qp);
            break;
          case QDP_CRCERR:
            if (!PostEvent(qp, QDP_EVENT_CRCERR, NULL)) TerminateSelf(fid, qp);
            break;
          default:
            if (!PostEvent(qp, QDP_EVENT_IOERR, NULL)) TerminateSelf(fid, qp);
            break;
        }
    }
}

/* Receive packets on the control port. */

static THREAD_FUNC CtrlThread(void *argptr)
{
QDP *qp;
int status;
MSGQ_MSG *msg;
static char *fid = "CtrlThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s started", fid);
    ++qp->nthread;
    SEM_POST(&qp->sem);

/* Generate events from whatever comes (or doesn't come) over the wire */

    while (1) {
        msg = qdpRecvPkt(qp, &qp->ctrl, &status);
        switch (status) {
          case QDP_OK:
            if (!PostEvent(qp, CtrlEvent(qp, msg), msg)) TerminateSelf(fid, qp);
            break;
          case QDP_TIMEOUT:
            if (!PostEvent(qp, QDP_EVENT_CTO, NULL)) TerminateSelf(fid, qp);
            break;
          case QDP_CRCERR:
            if (!PostEvent(qp, QDP_EVENT_CRCERR, NULL)) TerminateSelf(fid, qp);
            break;
          default:
            if (!PostEvent(qp, QDP_EVENT_IOERR, NULL)) TerminateSelf(fid, qp);
            break;
        }
    }
}

/* Accept and process commands from the application */

static THREAD_FUNC CmdThread(void *argptr)
{
QDP *qp;
int status;
QDP_PKT *cmd;
MSGQ_MSG *msg;
static char *fid = "CmdThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s started", fid);
    ++qp->nthread;
    SEM_POST(&qp->sem);

/* Command queue has a depth of one, so at most one command will be pending at any given time */

    while (1) {
        qp->cmd.msg = msgqGet(&qp->Q.cmd.full, MSGQ_WAIT);
        cmd = (QDP_PKT *) qp->cmd.msg->data;
        qdpDebug(qp, "%s: %s command received from application", fid, qdpCmdString(cmd->hdr.cmd));
        cmd->hdr.seqno = 0;
        if (!PostEvent(qp, QDP_EVENT_CMDRDY, NULL)) TerminateSelf(fid, qp);
    }
}

/* Generate heartbeat events at a regular interval */

static THREAD_FUNC HeartbeatThread(void *argptr)
{
QDP *qp;
static char *fid = "HeartbeatThread";

    qp = (QDP *) argptr;
    qdpDebug(qp, "%s started", fid);
    ++qp->nthread;
    SEM_POST(&qp->sem);

    while (1) {
        if (!PostEvent(qp, QDP_EVENT_HBEAT, NULL)) TerminateSelf(fid, qp);
        utilDelayMsec(qp->par.hbeat.interval);
    }
}

BOOL qdpStartFSA(QDP *qp)
{
THREAD tid;
static char *fid = "qdpStartFSA";

    if (qp == NULL) {
        qdpError(qp, "%s: NULL input not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    qp->state = QDP_STATE_IDLE;

/* Start the event dispatcher */

    if (!THREAD_CREATE(&tid, EventThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: EventThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&qp->sem);

/* Start the data handler */

    if (qp->dataLink) {
        if (!THREAD_CREATE(&tid, DataThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: DataThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        SEM_WAIT(&qp->sem);
    }

/* Start the control thread */

    if (!THREAD_CREATE(&tid, CtrlThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: CtrlThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&qp->sem);

/* Everything is in place, post an UP event */

    PostEvent(qp, QDP_EVENT_UP, NULL);

/* Wait until initial registration has completed or failed */

    SEM_WAIT(&qp->sem);
    if (qp->regerr != QDP_CERR_NOERR) return FALSE;

/* Start the command processor */

    if (!THREAD_CREATE(&tid, CmdThread, (void *) qp)) {
        qdpError(qp, "%s: THREAD_CREATE: CmdThread: %s", fid, strerror(errno));
        return FALSE;
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&qp->sem);

/* Start the heartbeat thread if configured */

    if (qp->par.hbeat.interval != 0) {
        if (!THREAD_CREATE(&tid, HeartbeatThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: HeartbeatThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        SEM_WAIT(&qp->sem);
    }

/* Start the link trasher, if configured */

    if (qp->par.trash.link) {
        if (!THREAD_CREATE(&tid, TrashLinkThread, (void *) qp)) {
            qdpError(qp, "%s: THREAD_CREATE: TrashLinkThread: %s", fid, strerror(errno));
            return FALSE;
        }
        THREAD_DETACH(tid);
        SEM_WAIT(&qp->sem);
    }

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
 * $Log: fsa.c,v $
 * Revision 1.12  2009/03/20 21:16:31  dechavez
 * Fixed silly typo in THREAD_DETACH macro
 *
 * Revision 1.11  2009/03/17 18:23:53  dechavez
 * THREAD_DETACH all threads
 *
 * Revision 1.10  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
