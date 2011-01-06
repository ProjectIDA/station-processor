#pragma ident "$Id: accept.c,v 1.4 2007/01/23 02:52:58 dechavez Exp $"
/*======================================================================
 * 
 * Accept new IACP client connection and complete handshake.
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_OPTS
#include "iacp.h"
#include "util.h"

IACP *iacpAccept(IACP *server)
{
IACP *iacp;
int LogLevel;
struct sockaddr_in cli_addr;
static struct linger linger = {0, 0};
int len = sizeof(cli_addr);
static char *fid = "iacpAccept";

    LogLevel = iacpGetDebug(server) ? LOG_INFO : LOG_DEBUG;

    if (server == NULL) {
        logioMsg(server->lp, LogLevel, "%s: invalid argument(s)", fid);
        errno = EINVAL;
        return NULL;
    }
 
/* Create/fill the handle */
 
    if ((iacp = (IACP *) malloc(sizeof(IACP))) == (IACP *) NULL) {
        logioMsg(server->lp, LogLevel, "%s: malloc: error %d", fid, errno);
        return NULL;
    }
    iacpInitHandle(iacp, NULL, server->port, &server->attr, server->lp, server->debug);

/* Accept a new connection */

    iacp->sd = INVALID_SOCKET;
    while (iacp->sd == INVALID_SOCKET) {
        iacp->sd = accept(server->sd, (struct sockaddr *) &cli_addr, &len);
        if (iacp->sd == INVALID_SOCKET && errno != EINTR) {
            logioMsg(server->lp, LogLevel, "%s: accept: error %d", fid, errno);
            return iacpClose(iacp);
        }
    }

/* Verify we are still accepting connections */

    if (iacpGetDisabled(server)) {
        logioMsg(server->lp, LOG_DEBUG, "%s: disabled flag set, breaking connection", fid);
        return iacpClose(iacp);
    }

/* Set socket options and complete the handle */

    utilSetNonBlockingSocket(iacp->sd);
    setsockopt(iacp->sd, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));
    iacp->connect = time(NULL);
    iacp->port = utilPeerPort(iacp->sd);
    iacp->attr = server->attr;
    utilPeerAddr(iacp->sd, iacp->peer.addr, INET_ADDRSTRLEN);
    utilPeerName(iacp->sd, iacp->peer.name, MAXPATHLEN);
    sprintf(iacp->peer.ident, "pid?@%s", iacp->peer.name);

/* Do the handshake */

    if (!iacpServerHandshake(iacp)) {
        logioMsg(iacp->lp, LogLevel, "%s: handshake failed", fid);
        server->recv.error = iacp->recv.error;
        return iacpClose(iacp);
    }

/* Return handle for this connection */

    return iacp;
}

/* Revision History
 *
 * $Log: accept.c,v $
 * Revision 1.4  2007/01/23 02:52:58  dechavez
 * changed LOG_ERR messages to LOG_INFO
 *
 * Revision 1.3  2005/01/28 01:51:58  dechavez
 * use iacpInitHandle instead of copy to initialize new handle
 *
 * Revision 1.2  2003/10/16 16:36:30  dechavez
 * Many bug fixes and enhancements, to numerous to mention.
 *
 * Revision 1.1  2003/06/09 23:50:26  dechavez
 * initial release
 *
 */
