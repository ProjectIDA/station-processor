#pragma ident "$Id: open.c,v 1.4 2007/01/08 15:46:11 dechavez Exp $"
/*======================================================================
 * 
 * Establish a connection with a LISS server.
 *
 *====================================================================*/
#include "liss.h"
#include "util.h"

static LISS *failed(LISS *liss, int sd)
{
    shutdown(sd, 2);
#ifndef WINNT
    close(sd);
#else
    closesocket(sd);
#endif
    if (liss != NULL) free(liss);
    return NULL;
}

#ifndef WINNT
extern int h_errno;
#endif

LISS *lissClose(LISS *liss)
{
    if (liss == NULL) return NULL;

    if (liss->sd != INVALID_SOCKET) {
        shutdown(liss->sd, 2);
#ifndef WINNT
        close(liss->sd);
#else
        closesocket(liss->sd);
#endif
    }

    free(liss);
    return NULL;
}

LISS *lissOpen(char *server, int port, int blksiz, int to, LOGIO *lp)
{
static MUTEX LocalMutex = MUTEX_INITIALIZER;
unsigned long addr;
struct hostent *hp;
struct sockaddr_in peer_addr;
struct sockaddr *name;
int namelen, sd;
int val, ilen;
LISS *liss = NULL;
UINT32 count = 0;
static char *fid = "lissOpen";

#ifdef WINNT
    if (LocalMutex == MUTEX_INITIALIZER) {
        logioMsg(lp, LOG_DEBUG, "%s: local mutex initialized", fid);
        MUTEX_INIT(&LocalMutex);
    }
#endif

/* Argument check */

    if (server == (char *) NULL || port <= 0) {
        logioMsg(lp, LOG_ERR, "%s: null input(s)", fid);
        errno = EINVAL;
        return NULL;
    }
    if (blksiz <= 0) blksiz = LISS_DEFAULT_BLKSIZ;
    if (to <= 0) to = LISS_DEFAULT_TIMEOUT;

/*  Address of server is taken from host name  */

     MUTEX_LOCK(&LocalMutex);
        hp = gethostbyname(server);
        if (hp == (struct hostent *) NULL) {
            if (h_errno != HOST_NOT_FOUND) {
                logioMsg(lp, LOG_ERR, "%s: gethostbyname: %s: %d", 
#ifdef WINNT
                    fid, server, WSAGetLastError()
#else
                    fid, server, h_errno
#endif /* WINNT */
                );
                return NULL;
            }

        /* try again assuming server name is in dot decimal form */

            if ((addr = inet_addr(server)) == -1) {
                logioMsg(lp, LOG_ERR, "%s: inet_addr: %s", fid, strerror(errno));
                return NULL;
            } else {
                hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
            }
        }
    MUTEX_UNLOCK(&LocalMutex);

    if (hp == (struct hostent *) NULL) {
        logioMsg(lp, LOG_ERR, "%s: can't get address of host `%s'", fid, server);
        return NULL;
    }

/* Establish connection with server */

    memcpy(&peer_addr.sin_addr, hp->h_addr, hp->h_length);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port   = htons((u_short) port);
    name    = (struct sockaddr *) &peer_addr;
    namelen = (int) sizeof(struct sockaddr_in);
    val     = 1;
    ilen    = sizeof(int);

    sd = INVALID_SOCKET;
    while (sd == INVALID_SOCKET) {

    /* Create socket */

        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            logioMsg(lp, LOG_ERR, "%s: socket: %s", fid, strerror(errno));
            return NULL;
        }

    /* Set socket options.  These are really suggestions so we don't
     * care if they work or not and therefore don't test for failure.
     * Ignorance is bliss.
     */

        setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &val, ilen);

    /* Do the connection */
    
        if (connect(sd, name, namelen) != 0) {       
#ifndef WINNT
            logioMsg(lp, LOG_ERR, "%s: connect: %s:%d: %s",
                fid, server, port, strerror(errno)
            );
            close(sd);
#else
            logioMsg(lp, LOG_ERR, "%s: connect: %s:%d: error %d", 
                fid, server, port, WSAGetLastError()
            );
            closesocket(sd);
#endif /* WINNT */
            sd = INVALID_SOCKET;
            switch (errno) {
              case EACCES:
              case EAFNOSUPPORT:
              case EINVAL:
              case ELOOP:
              case ENOENT:
#ifdef ENOSR
              case ENOSR:
#endif
              case ENOTDIR:
              case ENOTSOCK:
              case EPROTOTYPE:
                logioMsg(lp, LOG_INFO, "%s: fatal error: %s", fid, strerror(errno));
                return NULL;
              case EINTR:
              case ETIMEDOUT:
              default:
                logioMsg(lp, LOG_INFO, "%s: %s not responding, still trying", fid, server);
                break;
            }
        }

    /* In case of failure, check retry policy and act accordingly */

        if (sd == INVALID_SOCKET) {
            logioMsg(lp, LOG_DEBUG, "%s: sleep(%hu)", fid, LISS_RETRY_INTERVAL);
            sleep((unsigned int) LISS_RETRY_INTERVAL);
            ++count;
        }
    }

    if (count != 0) logioMsg(lp, LOG_INFO, "%s: %s OK", fid, server);

/* At this point we have a working connection... build our handle */

    liss = (LISS *) malloc(sizeof(LISS));
    if (liss == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return failed(liss, sd);
    }

    liss->sd = sd;
    liss->lp = lp;
    strlcpy(liss->server, server, MAXPATHLEN);
    liss->port = (UINT16) port;
    liss->connect = time(NULL);
    liss->count = 0;
    liss->blksiz = blksiz;
    liss->to = to;

/* Set sd to non-blocking, so we can later use select() on it */

#ifndef WINNT /* NT sets non-blocking by default */
    if (fcntl(liss->sd, F_SETFL, O_NONBLOCK) == -1) {
        logioMsg(lp, LOG_ERR, "%s: fcntl: %s", fid, strerror(errno));
        return failed(liss, sd);
    }
#endif

/* Successful handshake, return the handle */

    return liss;
}

/* Revision History
 *
 * $Log: open.c,v $
 * Revision 1.4  2007/01/08 15:46:11  dechavez
 * opps.  strlcpy instead of strncpy
 *
 * Revision 1.3  2007/01/08 15:44:42  dechavez
 * snprintf() instead of sprintf()
 *
 * Revision 1.2  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
