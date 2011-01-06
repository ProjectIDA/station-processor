#pragma ident "$Id: service.c,v 1.7 2009/03/20 21:03:34 dechavez Exp $"
/*======================================================================
 *
 *  Service a single client connection.
 *
 *====================================================================*/
#include "isid.h"

static VOID LogRecvError(IACP *iacp)
{
int error, status, LogLevel;

    error = iacpGetRecvError(iacp);
    status = iacpGetRecvStatus(iacp);

    switch(error) {
      case ECONNRESET:
        LogMsg(LOG_DEBUG, "%s: connection reset by peer", iacpPeerIdent(iacp));
        LogLevel = LOG_DEBUG;
        break;

      default:
        LogLevel = LOG_INFO;
        break;
    }

    LogMsg(LogLevel, "%s: iacpRecvFrame: error=%d, status=%d", iacpPeerIdent(iacp), error, status);
}

/* Thread to service one client */

static THREAD_FUNC ServiceThread(void *argptr)
{
CLIENT *client;
static char *fid = "ServiceThread";

    client = (CLIENT *) argptr;

    LogMsg(LOG_DEBUG, "%s: thread started, id = 0x%x", fid, THREAD_SELF());

    while (!client->finished) {

        if (iacpRecvFrame(client->iacp, &client->recv.frame, client->recv.buf, client->recv.buflen)) {

            /* Make sure signature is OK */

            if (!client->recv.frame.auth.verified) {
                LogMsg(LOG_ERR, "%s: authentication failed", client->ident);
                iacpSendAlert(client->iacp, IACP_ALERT_FAILED_AUTH);
                client->finished = TRUE;
            }

            /* Save a copy of this message for status reports */

            if (client->recv.frame.payload.type != IACP_TYPE_NULL) {
                client->last.msg.type = client->recv.frame.payload.type;
                client->last.msg.len  = client->recv.frame.payload.len;
                memcpy(client->last.msg.data, client->recv.frame.payload.data, client->recv.frame.payload.len);
            }

            /* Respond to the message */

            RespondToClientMessage(client);

        } else {

            LogRecvError(client->iacp);
            if (iacpGetRecvError(client->iacp) != ETIMEDOUT) iacpSendAlert(client->iacp, IACP_ALERT_IO_ERROR);
            client->finished = TRUE;

        }
    }

    CloseClientConnection(client);

    LogMsg(LOG_DEBUG, "%s: thread 0x%x exits", fid, THREAD_SELF());
    THREAD_EXIT((void *) 0);
}

/* Break a newly established connection w/o servicing it */

static VOID BreakNewConnection(IACP *iacp, UINT32 cause)
{
    LogMsg(LOG_INFO, "%s: connection aborted", iacpPeerIdent(iacp));
    iacpDisconnect(iacp, cause);
}

/* Service a new connection */

VOID ServiceConnection(IACP *iacp)
{
THREAD tid;
CLIENT *client;
static CHAR *fid = "ServiceConnection";

    BlockOnShutdown();

/* Grab the next available CLIENT slot */

    if ((client = NextAvailableClient(iacp)) == NULL) {
        LogMsg(LOG_WARN, "WARNING: new connection rejected (threshold reached)");
        BreakNewConnection(iacp, IACP_ALERT_SERVER_BUSY);
        return;
    }

/* Otherwise, leave behind a thread to deal with it */

    if (!THREAD_CREATE(&tid, ServiceThread, client)) {
        LogMsg(LOG_ERR, "%s: %s: THREAD_CREATE: %s", client->ident, fid, strerror(errno));
        BreakNewConnection(iacp, IACP_ALERT_SERVER_FAULT);
        return;
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: service.c,v $
 * Revision 1.7  2009/03/20 21:03:34  dechavez
 * THREAD_DETACH
 *
 * Revision 1.6  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.5  2006/09/29 19:56:15  dechavez
 * set flag instead of sending message when it's time for a heartbeat
 *
 * Revision 1.4  2005/09/30 22:36:53  dechavez
 * demoted connection reset message to LOG_DEBUG
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/11/04 20:07:53  dechavez
 * replace sleep for hardcoded seconds with utilDelayMsec for HEARTBEAT_LOOP_INTERVAL
 * msecs
 *
 * Revision 1.1  2003/10/16 18:07:25  dechavez
 * initial release
 *
 */
