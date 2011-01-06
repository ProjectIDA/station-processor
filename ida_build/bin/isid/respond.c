#pragma ident "$Id: respond.c,v 1.4 2008/01/25 22:12:48 dechavez Exp $"
/*======================================================================
 *
 *  Respond to a client message
 *
 *====================================================================*/
#include "isid.h"

static VOID LogUnexpectedFrame(CLIENT *client, UINT32 type)
{
    LogMsg(LOG_INFO, "%s: unexpected frame type %lu (0x%08x) received",
        client->ident, type, type
    );
}

static VOID SetFormatCode(CLIENT *client)
{
    utilUnpackUINT32(client->recv.frame.payload.data, &client->datreq.format);
    LogMsg(LOG_DEBUG, "%s: set datreq.format=%lu", client->ident, client->datreq.format);
    client->finished = FALSE;
}

static VOID SetCompressCode(CLIENT *client)
{
    utilUnpackUINT32(client->recv.frame.payload.data, &client->datreq.compress);
    LogMsg(LOG_DEBUG, "%s: set datreq.compress=%lu", client->ident, client->datreq.compress);
    client->finished = FALSE;
}

static VOID SetRequestOptions(CLIENT *client)
{
    utilUnpackUINT32(client->recv.frame.payload.data, &client->datreq.options);
    LogMsg(LOG_DEBUG, "%s: set datreq.options=0x%08x", client->ident, client->datreq.options);
    client->finished = FALSE;
}

static VOID SetPolicyCode(CLIENT *client)
{
    utilUnpackUINT32(client->recv.frame.payload.data, &client->datreq.policy);
    LogMsg(LOG_DEBUG, "%s: set datreq.policy=%lu", client->ident, client->datreq.policy);
    client->finished = FALSE;
}

static VOID SetTwindRequest(CLIENT *client)
{
UINT32 status;

    if (client->datreq.nreq == 0) {
        client->datreq.type = ISI_REQUEST_TYPE_TWIND;
        LogMsg(LOG_DEBUG, "%s: set datreq.type=%lu", client->ident, client->datreq.type);
    } else if (client->datreq.type != ISI_REQUEST_TYPE_TWIND) {
        LogMsg(LOG_INFO, "%s: mixed data request types", client->ident);
        client->result = IACP_ALERT_PROTOCOL_ERROR;
    }

    status = isiAppendTwindReq(&client->incoming, &client->recv.frame);
    if (status == IACP_ALERT_NONE) {
        LogMsg(LOG_DEBUG, "%s: isiAppendTwindReq OK, count=%lu", client->ident, client->incoming.list.count);
        client->finished = FALSE;
    } else {
        LogMsg(LOG_ERR, "%s: isiAppendTwindReq, error=%lu", client->ident, status);
    }

    client->result = status;
}

static VOID SetSeqnoRequest(CLIENT *client)
{
UINT32 status;

    if (client->datreq.nreq == 0) {
        client->datreq.type = ISI_REQUEST_TYPE_SEQNO;
        LogMsg(LOG_DEBUG, "%s: set datreq.type=%lu", client->ident, client->datreq.type);
    } else if (client->datreq.type != ISI_REQUEST_TYPE_SEQNO) {
        LogMsg(LOG_INFO, "%s: mixed data request types", client->ident);
        client->result = IACP_ALERT_PROTOCOL_ERROR;
    }

    status = isiAppendSeqnoReq(&client->incoming, &client->recv.frame);
    if (status == IACP_ALERT_NONE) {
        LogMsg(LOG_DEBUG, "%s: isiAppendSeqnoReq OK, count=%lu", client->ident, client->incoming.list.count);
        client->finished = FALSE;
    } else {
        LogMsg(LOG_ERR, "%s: isiAppendSeqnoReq, error=%lu", client->ident, status);
    }

    client->result = status;
}

static VOID UpdateStreamList(CLIENT *client)
{
UINT32 status;

    status = isiAppendSeqnoSlist(&client->datreq, &client->recv.frame);
    if (status == IACP_ALERT_NONE) {
        LogMsg(LOG_DEBUG, "%s: isiAppendSeqnoSlist OK, count=%lu", client->ident, client->datreq.slist.count);
        client->finished = FALSE;
    } else {
        LogMsg(LOG_ERR, "%s: isiAppendSeqnoSlist, error=%lu", client->ident, status);
    }
}

VOID RespondToClientMessage(CLIENT *client)
{
int code;
static char *fid = "RespondToClientMessage";

    client->brkrcvd = FALSE;
    client->finished = TRUE; /* for most anyway... multiparts will reset */

    switch (client->recv.frame.payload.type) {

/* quit now if the client has told us it is going away */

      case IACP_TYPE_ALERT:
        LogMsg(LOG_DEBUG, "%s: IACP_TYPE_ALERT", client->ident);
        code = iacpAlertCauseCode(&client->recv.frame);
        if (code == IACP_ALERT_DISCONNECT) {
            LogMsg(LOG_INFO, "%s: connection reset by peer", client->ident);
        } else {
            LogMsg(LOG_INFO, "%s: connection reset by peer (code=%lu)", client->ident, code);
        }
        client->brkrcvd = TRUE;
        break;

/* NULL means a multipart message is now complete, process it here */

      case IACP_TYPE_NULL:
        LogMsg(LOG_DEBUG, "%s: IACP_TYPE_NULL, count=%d", client->ident, client->incoming.list.count);
        if (client->incoming.list.count > 0) {
            switch (client->incoming.type) {
              case ISI_IACP_REQ_TWIND:
                ProcessTwindRequest(client);
                break;
              case ISI_IACP_REQ_SEQNO:
                ProcessSeqnoRequest(client);
                break;
              default:
                LogMsg(LOG_INFO, "%s: Unknown data request type %lu", client->ident, client->incoming.type);
                client->result = IACP_ALERT_UNSUPPORTED;
            }
        } else {
            LogUnexpectedFrame(client, client->recv.frame.payload.type);
            client->result = IACP_ALERT_PROTOCOL_ERROR;
        }
        listDestroy(&client->incoming.list);
        listInit(&client->incoming.list);
        break;

/* Simple requests are processed here */

      case ISI_IACP_REQ_SOH:
        LogMsg(LOG_DEBUG, "%s: SOH request", client->ident);
        ProcessNrtsSohReq(client);
        break;

      case ISI_IACP_REQ_CNF:
        LogMsg(LOG_DEBUG, "%s: configuration request", client->ident);
        ProcessNrtsCnfReq(client);
        break;

      case ISI_IACP_REQ_WFDISC:
        LogMsg(LOG_DEBUG, "%s: wfdisc request", client->ident);
        ProcessNrtsWfdiscReq(client);
        break;

/* mulit-part messages processed here */

      case ISI_IACP_REQ_FORMAT:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_FORMAT", client->ident);
        SetFormatCode(client);
        client->finished = FALSE;
        break;

      case ISI_IACP_REQ_COMPRESS:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_COMPRESS", client->ident);
        SetCompressCode(client);
        break;

      case ISI_IACP_REQ_OPTIONS:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_OPTIONS", client->ident);
        SetRequestOptions(client);
        break;

      case ISI_IACP_REQ_POLICY:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_POLICY", client->ident);
        SetPolicyCode(client);
        break;

      case ISI_IACP_REQ_TWIND:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_TWIND", client->ident);
        SetTwindRequest(client);
        break;

      case ISI_IACP_REQ_SEQNO:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_SEQNO", client->ident);
        SetSeqnoRequest(client);
        break;

      case ISI_IACP_REQ_STREAM:
        LogMsg(LOG_DEBUG, "%s: ISI_IACP_REQ_STREAM", client->ident);
        UpdateStreamList(client);
        break;

/* Give up if we don't know what it is */

      default:
        LogUnexpectedFrame(client, client->recv.frame.payload.type);
        client->result = IACP_ALERT_UNSUPPORTED;
        break;
    }
}

/* Revision History
 *
 * $Log: respond.c,v $
 * Revision 1.4  2008/01/25 22:12:48  dechavez
 * added support for ISI_IACP_REQ_OPTIONS and ISI_IACP_REQ_STREAM messages
 *
 * Revision 1.3  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.2  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
