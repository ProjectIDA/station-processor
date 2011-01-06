#pragma ident "$Id: server.c,v 1.3 2007/01/23 02:52:58 dechavez Exp $"
/*======================================================================
 * 
 * Set self up as an IACP server listening on the specified port.
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "iacp.h"
#include "util.h"

static IACP *Fail(IACP *iacp, int sd)
{
    if (sd != INVALID_SOCKET) utilCloseSocket(sd);
    if (iacp != (IACP *) NULL) free(iacp);

    return (IACP *) NULL;
}

IACP *iacpServer(int port, IACP_ATTR *user_attr, LOGIO *lp, int debug)
{
IACP *iacp;
struct sockaddr_in serverAddr;
static int yes = 1;
static struct linger linger = {0, 0};
static int sockAddrSize = sizeof(struct sockaddr_in);
static char *fid = "iacpServer";

/* Create/fill the handle */
 
    if ((iacp = (IACP *) malloc(sizeof(IACP))) == (IACP *) NULL) {
        logioMsg(lp, LOG_INFO, "%s: malloc: error %d", fid, errno);
        return (IACP *) NULL;
    }
    iacpInitHandle(iacp, fid, port, user_attr, lp, debug);
    iacp->attr.at_retry = FALSE;

/* Set up the local address */

    bzero((char *) &serverAddr, sockAddrSize);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons((short)port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

/* Create the socket */

    if ((iacp->sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        logioMsg(lp, LOG_INFO, "%s: socket: error %d", fid, errno);
        return Fail(iacp, iacp->sd);
    }

/* Set options */

    setsockopt(iacp->sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(int));
    setsockopt(iacp->sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &yes, sizeof(int));
    setsockopt(iacp->sd, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(linger));

/* Bind socket to local address */

    if (bind(iacp->sd, (struct sockaddr *) &serverAddr, sockAddrSize) != 0) {
        logioMsg(lp, LOG_INFO, "%s: bind: error %d", fid, errno);
        return Fail(iacp, iacp->sd);
    }

/* Create queue for client connections */

    if (listen(iacp->sd, 5) != 0) {
        logioMsg(lp, LOG_INFO, "%s: listen: error %d", fid, errno);
        return Fail(iacp, iacp->sd);
    }

/* Return handle for this connection */

    return iacp;
}

/* Revision History
 *
 * $Log: server.c,v $
 * Revision 1.3  2007/01/23 02:52:58  dechavez
 * changed LOG_ERR messages to LOG_INFO
 *
 * Revision 1.2  2003/12/10 06:13:37  dechavez
 * include util.h to calm solaris cc
 *
 * Revision 1.1  2003/06/09 23:50:27  dechavez
 * initial release
 *
 */
