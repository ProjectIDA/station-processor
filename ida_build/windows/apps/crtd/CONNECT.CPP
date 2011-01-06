/* @(#)connect.c	1.8 9/6/96 */
/*======================================================================
 *
 *  Establish a socket connection with a remote server.
 *  Returns socket descriptor if successful, a value less than 0 if not.
 *
 *  MT-unsafe!
 *
 *====================================================================*/
#include "stdafx.h"
#include <winsock.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include "util.h"

#define DEFAULT_PROTOCOL "tcp"

int util_connect(char *rmthost, char *service, int port, char *protocol, int sndbuf, int rcvbuf)
	{
	unsigned long addr;
	int sd, addrlen;
	struct hostent *hp;
	struct servent *sp;
	char *host;
	char *ipaddress;
	int val, ilen, connected, slevel, rlevel;
	struct sockaddr_in peer_addr;

    if (rmthost == (char *) NULL) {
//        util_log(1, "%s: remote host is not defined!", fid);
//        errno = EINVAL;
        return -1;
		}

/*  Address of server is taken from host name  */

    if ((hp = gethostbyname(rmthost)) == NULL) {
        switch (h_errno) {
          case HOST_NOT_FOUND:
            if ((addr = inet_addr(rmthost)) == -1) 
				{
//                util_log(1, "%s: host not found", rmthost);
				} 
			else 
				if ((hp = gethostbyaddr((char *)&addr,4,AF_INET)) == NULL) 
					{
//                util_log(1, "can't find host `%s'", rmthost);
					}
            break;
          case TRY_AGAIN:
//            util_log(1, "gethostbyname: %s: try again later", rmthost);
            break;
          case NO_DATA:
//            util_log(1, "%s: no address available", rmthost);
            break;
          default:;
//            util_log(1, "%s: gethostbyname: %s", fid, syserrmsg(errno));
        }
        return -2;
    }

/*  If communications protocol is not specified, use the default */

    if (protocol == NULL) protocol = DEFAULT_PROTOCOL;

/*  If service name is given, lookup port number  */

    if (service != NULL) {
        if ((sp = getservbyname(service, protocol)) == NULL) {
//            util_log(1, "%s: getservbyname(%s, %s): %s",
//                fid, service, protocol, syserrmsg(errno) );
            return -3;
        }
        port = ntohs(sp->s_port);
    } else if (port <= 0) {
//        util_log(1, "%s: both service and port are undefined!", fid);
        return -4;
    }

/*  Name socket  */

    memcpy(&peer_addr.sin_addr, hp->h_addr, hp->h_length);
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port   = htons(port);

/*  Connect and bind  */

    val  = 1;
    ilen = sizeof(int);
    slevel = rlevel = 1;
    connected = 0;
    while (!connected) {

        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//            util_log(1, "%s: socket: %s", fid, syserrmsg(errno));
            return -5;
        }

        if (setsockopt(sd,SOL_SOCKET,SO_KEEPALIVE,(char *) &val,ilen) != 0)
			{
//            util_log(1, "%s: warning: setsockopt(keepalive): %s",
//                fid, syserrmsg(errno));
			}

        if (sndbuf > 0) {
            if (
                setsockopt(
                    sd, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuf, ilen
                ) != 0
            ) {
//                util_log(1, "%s: warning: setsockopt(sndbuf = %d): %s",
//                    fid, sndbuf, syserrmsg(errno));
            } else {
//                util_log(slevel++, "socket sndbuf set to %d bytes", sndbuf);
            }
        }

        if (rcvbuf > 0) 
			{
            if ( setsockopt( sd, SOL_SOCKET, SO_RCVBUF, (char *) &rcvbuf, ilen) != 0)
				{
//                util_log(1, "%s: warning: setsockopt(rcvbuf = %d): %s",
//                    fid, rcvbuf, syserrmsg(errno));
				} 
			else 
				{
//                util_log(rlevel++, "socket rcvbuf set to %d bytes", rcvbuf);
				}
			}

        if (connect(sd,(struct sockaddr *)&peer_addr,sizeof(struct sockaddr_in)) != 0)
			{
            closesocket(sd);
			} 
		else 
			{
            connected = 1;
			}

    }

    addrlen = sizeof(peer_addr);

    if (getpeername(sd, (struct sockaddr *)&peer_addr, &addrlen) == 0) 
		{
        hp = gethostbyaddr((char *) &peer_addr.sin_addr, sizeof(struct in_addr),
                peer_addr.sin_family);
        ipaddress = inet_ntoa(peer_addr.sin_addr);
        host      = (hp != NULL) ? hp->h_name : NULL;
        if (host != NULL) 
			{
//            util_log(1, "connected to %s:%d", host, port);
			}
		else 
			{
//            util_log(1, "connected to %s:%d", ipaddress, port);
			}
		} 
		else 
			{
//			util_log(1, "%s: getpeername: %s", fid, syserrmsg(errno));
			}

    return sd;
	}
