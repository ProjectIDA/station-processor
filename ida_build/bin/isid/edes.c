#pragma ident "$Id: edes.c,v 1.2 2009/03/20 21:03:34 dechavez Exp $"
/*======================================================================
 *
 *  Legacy NRTS (edes, aka port 14002) service
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_EDES

static void ServiceEdesConnection(IACP *client)
{
    return;
}

static THREAD_FUNC NrtsServer(void *argptr)
{
IACP *server, *client;
static char *fid = "NrtsServer";

    LogMsg(LOG_DEBUG, "%s: thread started, id = %d", fid, THREAD_SELF());

    server = (IACP *) argptr;

/* Service each connection in its own thread */

    LogMsg(LOG_INFO, "listening for NRTS requests at port %hu", server->port);
    while (1) {
        BlockOnShutdown();
        if ((client = iacpAccept(server)) != NULL) {
            if (!ShutdownInProgress()) {
                LogMsg(LOG_INFO, "%s: connection established", client->peer.ident);
                ServiceEdesConnection(client);
            } else {
                iacpDisconnect(client, IACP_ALERT_SHUTDOWN);
            }
        } else {
            LogMsg(LOG_INFO, "incoming NRTS connection failed: %s (ignored)", strerror(errno));
        }
    }
}

VOID StartNrtsServer(PARAM *par)
{
THREAD tid;
static IACP *server;
static char *fid = "StartStatusServer";

    if (par->nrts == 0) return;

    server = iacpServer(par->nrts, &par->attr, &par->lp, par->debug);
    if (server == (IACP *) NULL) {
        LogMsg(LOG_ERR, "Unable to start port %hu NRTS server: %s", par->nrts, strerror(errno));
        return;
    }
    server->attr.at_proto = IACP_PROTO_NRTS;

    if (!THREAD_CREATE(&tid, NrtsServer, (void *) server)) {
        LogMsg(LOG_ERR, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 0);
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: edes.c,v $
 * Revision 1.2  2009/03/20 21:03:34  dechavez
 * THREAD_DETACH
 *
 * Revision 1.1  2008/10/10 22:49:14  dechavez
 * initial (stubbed only) release
 *
 */
