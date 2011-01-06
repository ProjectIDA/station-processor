#pragma ident "$Id: main.c,v 1.3 2002/02/23 00:01:13 dec Exp $"
/*======================================================================
 *
 *  Server to listen for shutdown or reboot requests from local isp's.
 *  Should be run as root in order for it to work.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "isp.h"

#ifndef REBOOT
#define REBOOT "/usr/sbin/init 6"
#endif

#ifndef SHUTDOWN
#define SHUTDOWN "/usr/sbin/init 0"
#endif

static int sd;

static char *get_peer(int sd, char *buffer)
{
int addrlen, h_errno;
struct sockaddr_in cli_addr, *cli_addrp;
struct hostent *hp, result;
static char *fid = "get_peer";
 
    addrlen = sizeof(cli_addr);
    cli_addrp = &cli_addr;
    if (getpeername(sd, (struct sockaddr *)cli_addrp, &addrlen) != 0) {
        util_log(1, "%s: getpeername: %s", fid, syserrmsg(errno));
        return NULL;
    }
#ifdef SOLARIS
    hp = gethostbyaddr_r(
        (char *) &cli_addrp->sin_addr,
        sizeof(struct in_addr),
        cli_addrp->sin_family,
        &result,
        buffer,
        MAXPATHLEN,
        &h_errno
    );
#else
    hp = gethostbyaddr(
        (char *) &cli_addrp->sin_addr,
        sizeof(struct in_addr),
        cli_addrp->sin_family
    );
#endif
    if (hp != NULL) {
        strcpy(buffer, hp->h_name);
    } else {
        strcpy(buffer, inet_ntoa(cli_addrp->sin_addr));
    }

    return buffer;
}

static void server_init(int port)
{
int i;
struct sockaddr_in serv_addr;
static char *fid = "server_init";

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        util_log(1, "%s: socket: %s", fid, syserrmsg(errno));
        exit(1);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(
        sd,
        (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)
    ) != 0) {
        util_log(1, "%s: bind: %s", fid, syserrmsg(errno));
        exit(1);
    }

    listen(sd, 5);

}

static void serve(int client)
{
int nread, level;
short stmp;
char *peer;
char buffer[1024];

    peer = get_peer(client, buffer);
    if (peer != NULL) util_log(1, "connection from %s", peer);

    nread = util_read(client, &stmp, 2, 10);
    if (nread != 2) {
        util_log(1, "can't get level: %s", syserrmsg(errno));
    } else {
        level = ntohs(stmp);
        if (level == 6) {
            util_log(1, "REBOOT ON OPERATOR COMMAND");
            system(REBOOT);
        } else if (level == 0) {
            util_log(1, "SHUTTING DOWN ON OPERATOR COMMAND");
            system(SHUTDOWN);
        } else {
            util_log(1, "ignore unexted level %d", level);
        }
    }
    shutdown(client, 2);
    close(client);
    return;
}

int main(int argc, char **argv)
{
int port = ISP_BOOTMGR_PORT;
struct sockaddr_in cli_addr;
int client, len = sizeof(cli_addr);

    if (argc > 1) port = atoi(argv[1]);

    util_bground(0, 0);
    util_logopen("syslogd", 1, 9, 1, (char *) NULL, argv[0]);

    server_init(port);

    util_log(1, "ISP boot manager installed at port %d", port);

    while (1) {
        client = accept(sd, (struct sockaddr *) &cli_addr, &len);
        if (client < 0) {
            util_log(1, "accept: %s", syserrmsg(errno));
        } else {
            serve(client);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2002/02/23 00:01:13  dec
 * define result in get_peer, regardless of OS name
 *
 * Revision 1.2  2001/05/20 18:41:28  dec
 * gethostbyaddr_r for SOLARIS, gethostbyaddr for the rest
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
