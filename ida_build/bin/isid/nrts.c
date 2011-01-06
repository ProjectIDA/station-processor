#pragma ident "$Id: nrts.c,v 1.10 2007/01/11 22:02:18 dechavez Exp $"
/*======================================================================
 *
 *  Process NRTS related messages (non-packet)
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_NRTS

VOID ProcessNrtsSohReq(CLIENT *client)
{
LNKLST list;
static char *fid = "ProcessNrtsSohReq";

    if (!listInit(&list)) {
        LogMsg(LOG_ERR, "%s: listInit: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    if (!isidlBuildStreamSohList(client->master, &list)) {
        LogMsg(LOG_ERR, "%s: isiBuildStreamSohList: error %d", fid, errno);
        GracefulExit(MY_MOD_ID + 2);
    }

    if (!isiIacpSendStreamSohList(client->iacp, &list)) {
        LogMsg(LOG_ERR, "%s: isiIacpSendStreamSohList: error %d", client->ident, errno);
    }
    LogMsg(LOG_DEBUG, "%s: %lu stream SOH records sent", client->ident, list.count);

    listDestroy(&list);
    client->finished = TRUE;
    client->result = IACP_ALERT_REQUEST_COMPLETE;
}

VOID ProcessNrtsCnfReq(CLIENT *client)
{
LNKLST list;
static char *fid = "ProcessNrtsCnfReq:ProcessCnfReq";

    if (!listInit(&list)) {
        LogMsg(LOG_ERR, "%s: listInit: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

    if (!isidlBuildStreamCnfList(client->master, &list)) {
        LogMsg(LOG_ERR, "%s: isiBuildStreamCnfList: error %d", fid, errno);
        GracefulExit(MY_MOD_ID + 4);
    }

    if (!isiIacpSendStreamCnfList(client->iacp, &list)) {
        LogMsg(LOG_ERR, "%s: isiIacpSendStreamCnfList: error %d", client->ident, errno);
    }
    LogMsg(LOG_DEBUG, "%s: %lu stream config records sent", client->ident, list.count);

    listDestroy(&list);
    client->finished = TRUE;
    client->result = IACP_ALERT_REQUEST_COMPLETE;
}

VOID ProcessNrtsWfdiscReq(CLIENT *client)
{
LNKLST list;
UINT32 maxdur;
static char *fid = "ProcessNrtsWfdiscReq";

    utilUnpackUINT32(client->recv.frame.payload.data, &maxdur);
    LogMsg(LOG_DEBUG, "%s: set maxdur=%lu", client->ident, maxdur);

    if (!listInit(&list)) {
        LogMsg(LOG_ERR, "%s: listInit: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 5);
    }

    if (!isidlGetWfdiscList(client->master, &list, maxdur)) {
        LogMsg(LOG_ERR, "%s: isidlGetWfdiscList: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 6);
    }

    if (!isiIacpSendWfdiscList(client->iacp, &list)) {
        LogMsg(LOG_ERR, "%s: isiIacpSendWfdiscList: %s", client->ident, strerror(errno));
    }
    LogMsg(LOG_DEBUG, "%s: %lu wfdisc records sent", client->ident, list.count);

    listDestroy(&list);
    client->finished = TRUE;
    client->result = IACP_ALERT_REQUEST_COMPLETE;
}

/* Revision History
 *
 * $Log: nrts.c,v $
 * Revision 1.10  2007/01/11 22:02:18  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.9  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.8  2006/02/14 17:04:58  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.7  2005/10/17 21:11:15  dechavez
 * remove dead code
 *
 * Revision 1.6  2005/09/30 22:51:39  dechavez
 * demoted various messages to LOG_DEBUG
 *
 * Revision 1.5  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.4  2005/06/24 21:54:07  dechavez
 * checkpoint, additional seqno support in place, but not yet ready
 *
 * Revision 1.3  2004/04/26 21:18:28  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.2  2003/11/04 00:22:48  dechavez
 * Changed packet type log level to LOG_INFO, added support for ISI_IACP_REQ_POLICY
 * packets, use isiAppendStreamReq() instead of AppendStreamReq()
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
