#pragma ident "$Id: connect.c,v 1.7 2005/05/25 22:41:46 dechavez Exp $"
/*======================================================================
 *
 *  Establish a socket connection with a remote server.
 *  Returns socket descriptor if successful, a value less than 0 if not.
 *
 *  MT-unsafe!
 *
 *====================================================================*/
#include "util.h"

#ifndef _WIN32
extern int h_errno;
#endif

#define DEFAULT_PROTOCOL "tcp"

int util_connect(char *server, char *service, int port, char *protocol, int sndbuf, int rcvbuf)
{
unsigned long addr;
int sd, addrlen;
struct hostent *hp;
struct servent *sp;
char *host;
char *ipaddress;
int val, ilen;
struct sockaddr_in peer_addr;
BOOL SetNoBlock = TRUE;
static char *fid = "util_connect";

    if (server == (char *) NULL) {
        util_log(1, "%s: remote host is not defined!", fid);
        errno = EINVAL;
        return -1;
    }

    if (port < 0) {
        port = -port;
        SetNoBlock = FALSE;
    }

/*  Address of server is taken from host name  */

    while ((hp = gethostbyname(server)) == (struct hostent *) NULL) {
        switch (h_errno) {
          case HOST_NOT_FOUND:
            if ((addr = inet_addr(server)) == -1) {
                util_log(1, "%s: host not found", server);
            } else if ((hp = gethostbyaddr((char *)&addr,4,AF_INET)) == NULL) {
                util_log(1, "can't find host `%s'", server);
            }
            return -2;
          case TRY_AGAIN:
            util_log(1, "gethostbyname: %s: try again later", server);
            sleep(30);
            break;
          case NO_DATA:
            util_log(1, "%s: no address available", server);
            return -2;
          default:
            util_log(1, "%s: gethostbyname: %s", fid, syserrmsg(errno));
            return -2;
        }
    }

/*  If communications protocol is not specified, use the default */

    if (protocol == (char *) NULL) protocol = DEFAULT_PROTOCOL;

/*  If service name is given, lookup port number  */

    if (service != (char *) NULL) {
        if ((sp = getservbyname(service, protocol)) == (struct servent *) NULL) {
            util_log(1, "%s: getservbyname(%s, %s): %s",
                fid, service, protocol, syserrmsg(errno)
            );
            return -3;
        }
        port = ntohs(sp->s_port);
    } else if (port <= 0) {
        util_log(1, "%s: both service and port are undefined!", fid);
        return -4;
    }

/*  Name socket  */

    memcpy(&peer_addr.sin_addr, hp->h_addr, hp->h_length);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port   = htons((short) port);

/*  Connect and bind  */

    val  = 1;
    ilen = sizeof(int);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        util_log(1, "%s: socket: %s", fid, syserrmsg(errno));
        return -5;
    }

    if (setsockopt(sd,SOL_SOCKET,SO_KEEPALIVE,(char *) &val,ilen) != 0){
        util_log(1, "%s: warning: setsockopt(keepalive): %s",
            fid, syserrmsg(errno)
        );
    }

    if (sndbuf > 0) {
        if (
            setsockopt(
                sd, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuf, ilen
            ) != 0
        ) {
            util_log(1, "%s: warning: setsockopt(sndbuf = %d): %s",
                fid, sndbuf, syserrmsg(errno)
            );
        } else {
            util_log(1, "socket sndbuf = %d bytes", sndbuf);
        }
    }

    if (rcvbuf > 0) {
        if (
            setsockopt(
                sd, SOL_SOCKET, SO_RCVBUF, (char *) &rcvbuf, ilen
            ) != 0
        ) {
            util_log(1, "%s: warning: setsockopt(rcvbuf = %d): %s",
                fid, rcvbuf, syserrmsg(errno)
            );
        } else {
            util_log(1, "socket rcvbuf = %d bytes", rcvbuf);
        }
    }

    if (service != (char *) NULL) {
        util_log(1, "contact %s.%s", server, service);
    } else {
        util_log(1, "contact %s.%d", server, port);
    }

    if (
        connect(
            sd,(struct sockaddr *)&peer_addr,sizeof(struct sockaddr_in)
        ) != 0
    ) {
        close(sd);
        return 0;
    }

    addrlen = sizeof(peer_addr);

    if (getpeername(sd, (struct sockaddr *)&peer_addr, &addrlen) == 0) {
        hp = gethostbyaddr((char *) &peer_addr.sin_addr, sizeof(struct in_addr),
                peer_addr.sin_family);
        ipaddress = inet_ntoa(peer_addr.sin_addr);
        host      = (hp != (struct hostent *) NULL) ? hp->h_name : (char *) NULL;
        if (host != (char *) NULL) {
            util_log(1, "connected to %s.%d", host, port);
        } else {
            util_log(1, "connected to %s.%d", ipaddress, port);
        }
    } else {
        util_log(1, "%s: warning: getpeername: %s", fid, syserrmsg(errno));
    }

    if (SetNoBlock) utilSetNonBlockingSocket(sd);

    return sd;
}

/* Revision History
 *
 * $Log: connect.c,v $
 * Revision 1.7  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.6  2004/09/28 22:36:18  dechavez
 * Allow for blocking sockets by sending negative port numbers
 *
 * Revision 1.5  2004/07/26 23:05:47  dechavez
 * removed uneccesary includes, set socket to non-blocking via utilSetNonBlockingSocket()
 *
 * Revision 1.4  2001/05/20 16:10:57  dec
 * set socket to non-blocking
 *
 * Revision 1.3  2001/05/07 22:40:12  dec
 * ANSI function declarations
 *
 * Revision 1.2  2000/10/06 19:57:48  dec
 * sleep and retry when gethostbyname says try again later
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
