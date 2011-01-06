#pragma ident "$Id"
/*======================================================================
 *
 *  Send heartbeat to the client, if necessary
 *
 *====================================================================*/
#include "isid.h"

VOID MaintainHeartbeat(CLIENT *client)
{
time_t now, last;
UINT32 TimeSinceLastWrite;

    if (client->finished) return;

    now = time(NULL);
    last = iacpGetSendTstamp(client->iacp);
    TimeSinceLastWrite = ((UINT32) now - (UINT32) last) * MSEC_PER_SEC;
    if (TimeSinceLastWrite < client->interval.hbeat) return;

    if (!iacpHeartbeat(client->iacp)) {
        LogMsg(LOG_INFO, "%s: iacpHeartbeat failed: %s", client->ident, strerror(errno));
        client->result = IACP_ALERT_IO_ERROR;
        client->finished = TRUE;
    }
    LogMsg(LOG_DEBUG, "%s: heartbeat sent", client->ident);
}

VOID PauseForNewData(CLIENT *client)
{
    if (client->finished) return;
    LogMsg(LOG_DEBUG, "%s: utilDelayMsec(%lu)", client->ident, client->interval.poll);
    utilDelayMsec(client->interval.poll);
}

/* Revision History
 *
 * $Log: hbeat.c,v $
 * Revision 1.1  2007/01/11 20:06:07  dechavez
 * initial release
 *
 */
