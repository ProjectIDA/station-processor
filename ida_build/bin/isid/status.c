#pragma ident "$Id: status.c,v 1.3 2009/03/20 21:03:34 dechavez Exp $"
/*======================================================================
 *
 *  Status reports
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_STATUS

static THREAD_FUNC StatusServer(void *argptr)
{
FILE *fp;
UINT16 port;
int sd, peer;
struct sockaddr_in serv_addr, cli_addr;
int yes = 1, ilen = sizeof(int), clen = sizeof(cli_addr);
static char *fid = "StatusServer";

    LogMsg(LOG_DEBUG, "%s: thread started, id = %d", fid, THREAD_SELF());

/* Create socket and bind */

    port = *((UINT16 *) argptr);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        LogMsg(LOG_ERR, "%s: socket: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 1);
    }
    memset((void *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, ilen);

    if (bind( sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        LogMsg(LOG_ERR, "%s: bind: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 2);
    }

/* Start listening for connectinos */

    if (listen(sd, 5) != 0) {
        LogMsg(LOG_ERR, "%s: listen: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 3);
    }

/* Service one connection at a time */

    LogMsg(LOG_INFO, "listening for status requests at port %hu", port);
    while (1) {
        peer = INVALID_SOCKET;
        while (peer == INVALID_SOCKET) {
            peer = accept(sd, (struct sockaddr *) &cli_addr, &clen);
            if (peer == INVALID_SOCKET && errno != EINTR) {
                LogMsg(LOG_ERR, "%s: accept: %s (ignored)", fid, strerror(errno));
            } else {
                if ((fp = fdopen(peer, "w")) == NULL) {
                    LogMsg(LOG_ERR, "%s: fdopen: %s", fid, strerror(errno));
                } else {
                    PrintStatusReport(fp);
                    fclose(fp);
                }
                shutdown(peer, 2);
                close(peer);
            }
        }
    }
}

VOID StartStatusServer(PARAM *par)
{
THREAD tid;
static UINT16 StaticPort;
static char *fid = "StartStatusServer";

    StaticPort = par->status;

    if (!THREAD_CREATE(&tid, StatusServer, (void *) &StaticPort)) {
        LogMsg(LOG_ERR, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID + 0);
    }
    THREAD_DETACH(tid);
}

/* Revision History
 *
 * $Log: status.c,v $
 * Revision 1.3  2009/03/20 21:03:34  dechavez
 * THREAD_DETACH
 *
 * Revision 1.2  2004/04/26 21:17:43  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.1  2003/10/16 18:07:25  dechavez
 * initial release
 *
 */
