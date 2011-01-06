#pragma ident "$Id: client.c,v 1.20 2008/01/25 22:14:15 dechavez Exp $"
/*======================================================================
 *
 *  Manage the CLIENT list
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_CLIENT

static struct {
    UINT32 num;
    CLIENT *client;
} ClientList;

/* Close a connection with a client */

VOID CloseClientConnection(CLIENT *client)
{

/* If we are breaking this connection, tell the client why */

    if (!client->brkrcvd) iacpSendAlert(client->iacp, client->result);

/* Close the socket and clear the handle */

    iacpClose(client->iacp);
    client->iacp = NULL;
    isiResetIncoming(&client->incoming);
    isiClearDataRequest(&client->datreq);
    client->last.msg.len = 0;
    if (client->last.datreq != NULL) {
        if (client->last.datreq->req.twind != NULL) free(client->last.datreq->req.twind);
        if (client->last.datreq->req.seqno != NULL) free(client->last.datreq->req.seqno);
        client->last.datreq = NULL;
    }
    listClear(&client->history);
}

/* Grab an available slot */

CLIENT *NextAvailableClient(IACP *iacp)
{
CLIENT *client;
UINT32 i, MinimumPollInterval;

    for (client = NULL, i = 0; client == NULL && i < ClientList.num; i++) {
        MUTEX_LOCK(&ClientList.client[i].mutex);
            if (ClientList.client[i].iacp == NULL) {
                client = &ClientList.client[i];
                client->iacp     = iacp;
                client->finished = FALSE;
                client->result   = IACP_ALERT_NONE;
                client->ident    = iacpPeerIdent(iacp);
                MinimumPollInterval = iacpGetTimeoutInterval(client->iacp) / 2;
                if (MinimumPollInterval < DEFAULT_POLL_INTERVAL) {
                    client->interval.poll = MinimumPollInterval;
                } else {
                    client->interval.poll = DEFAULT_POLL_INTERVAL;
                }
                client->interval.hbeat = MinimumPollInterval;
            }
        MUTEX_UNLOCK(&ClientList.client[i].mutex);
    }

    return client;
}

/* Drive report generator for each active client */

int PrintActiveClientReport(FILE *fp, long tstamp)
{
UINT32 i, count = 0;
CLIENT *client;

    for (i=0; i < ClientList.num; i++) {
        client = &ClientList.client[i];
        MUTEX_LOCK(&client->mutex);
            if (client->iacp != NULL) {
                ClientReport(fp, client, tstamp);
                ++count;
            }
        MUTEX_UNLOCK(&client->mutex);
    }

    return count;
}

VOID InitClientList(PARAM *par)
{
UINT32 i;
static char *fid = "InitClientList";

    ClientList.num = par->maxclient;
    ClientList.client = (CLIENT *) calloc(1,ClientList.num * sizeof(CLIENT));
    if (ClientList.client == NULL) {
        LogMsg(LOG_ERR, "%s: calloc: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }

    for (i = 0; i < ClientList.num; i++) {
        MUTEX_INIT(&ClientList.client[i].mutex);
        ClientList.client[i].index = i;
        ClientList.client[i].iacp     = (IACP *) NULL;
        ClientList.client[i].ident    = (char *) NULL;
        ClientList.client[i].finished  = FALSE;
        ClientList.client[i].result    = IACP_ALERT_NONE;
        ClientList.client[i].master   = par->master;
        isiInitIncoming(&ClientList.client[i].incoming);
        isiInitDataRequest(&ClientList.client[i].datreq);
        ClientList.client[i].send.buflen  = par->buflen.send;
        ClientList.client[i].send.buf = (UINT8 *) malloc(ClientList.client[i].send.buflen);
        if (ClientList.client[i].send.buf == NULL) {
            LogMsg(LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 2);
        }
        ClientList.client[i].recv.buflen  = par->buflen.recv;
        ClientList.client[i].recv.buf = (UINT8 *) malloc(ClientList.client[i].recv.buflen);
        if (ClientList.client[i].recv.buf == NULL) {
            LogMsg(LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 3);
        }
        ClientList.client[i].temp.buflen  = par->buflen.send * 2; /* bigger to handle compression failure */
        ClientList.client[i].temp.buf = (UINT8 *) malloc(ClientList.client[i].temp.buflen);
        if (ClientList.client[i].temp.buf == NULL) {
            LogMsg(LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 4);
        }
        ClientList.client[i].last.msg.len = 0;
        ClientList.client[i].last.msg.data = (UINT8 *) malloc(ClientList.client[i].recv.buflen);
        if (ClientList.client[i].last.msg.data == NULL) {
            LogMsg(LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
            GracefulExit(MY_MOD_ID + 3);
        }
        ClientList.client[i].last.datreq = NULL;
        ClientList.client[i].pkt.indx = -1;
        listInit(&ClientList.client[i].history);
    }
}

/* Revision History
 *
 * $Log: client.c,v $
 * Revision 1.20  2008/01/25 22:14:15  dechavez
 * removed options from client (now part of data request)
 *
 * Revision 1.19  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.18  2007/06/14 21:57:54  dechavez
 * Changed history list to instance instead of pointer
 *
 * Revision 1.17  2007/05/17 22:22:31  dechavez
 * 3.1.0 (beta 1)
 *
 * Revision 1.16  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.15  2006/11/13 23:48:27  dechavez
 * removed MSEC_PER_SEC macro (now defined in util.h)
 *
 * Revision 1.14  2006/09/29 19:58:33  dechavez
 * added NeedHeartbeat flag to CLIENT
 *
 * Revision 1.13  2006/07/10 21:11:18  dechavez
 * calloc instead of malloc client structures
 *
 * Revision 1.12  2006/02/09 20:23:56  dechavez
 * support for new pollint client parameter
 *
 * Revision 1.11  2005/10/26 23:22:55  dechavez
 * fixed race condition in ClientThreadExit (aap)
 *
 * Revision 1.10  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.9  2005/06/30 01:41:49  dechavez
 * init new isi dl field
 *
 * Revision 1.8  2005/06/24 21:54:07  dechavez
 * checkpoint, additional seqno support in place, but not yet ready
 *
 * Revision 1.7  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.6  2005/01/28 02:05:34  dechavez
 * fixed race condition causing ClientTimeSinceLastWrite() to sometimes
 * give incorrect results
 *
 * Revision 1.5  2004/04/26 21:19:18  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.4  2004/01/29 19:06:15  dechavez
 * remove race condition in CloseClientConnection() log message
 *
 * Revision 1.3  2003/11/26 19:34:20  dechavez
 * changed units of ClientTimeSinceLastWrite to msec
 *
 * Revision 1.2  2003/11/04 00:28:24  dechavez
 * use isiInitIncoming and isiResetIncoming instead of setting explicit fields
 * in former INCOMING structure
 *
 * Revision 1.1  2003/10/16 18:07:22  dechavez
 * initial release
 *
 */
