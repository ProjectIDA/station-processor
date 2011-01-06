#pragma ident "$Id: server.c,v 1.10 2003/12/23 23:49:14 dechavez Exp $"
/*======================================================================
 *
 *  NRTS push server.  Accepts nrts_push connections from remote systems
 *  with intermittent network connections.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "nrts_push.h"

#define NRTS_DEFHOME "/usr/nrts"

static int sd;

static void logexit(int status)
{
    util_log(1, "exit(%d)", status);
    exit(status);
}

void GracefulExit(int sig)
{
    util_log(1, "going down on %s", util_sigtoa(sig));
    logexit(0);
}

static BOOL InstallSignalHandlers()
{
    if (signal(SIGINT, GracefulExit) == SIG_ERR) return FALSE;
    if (signal(SIGTERM, GracefulExit) == SIG_ERR) return FALSE;
    if (signal(SIGQUIT, GracefulExit) == SIG_ERR) return FALSE;

    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) return FALSE;
    if (signal(SIGCLD, SIG_IGN) == SIG_ERR) return FALSE;

    return TRUE;
}

static char *get_peer(int sd, char *buffer)
{
int addrlen, h_errno;
struct sockaddr_in cli_addr, *cli_addrp;
struct hostent *hp, result;
static char *fid = "peer";
 
    addrlen = sizeof(cli_addr);
    cli_addrp = &cli_addr;
    if (getpeername(sd, (struct sockaddr *)cli_addrp, &addrlen) != 0) {
        util_log(1, "%s: getpeername: %s", fid, strerror(errno));
        exit(0);
    }
    strcpy(buffer, inet_ntoa(cli_addrp->sin_addr));
    return buffer;
}

static void server_init(int port)
{
int i;
struct sockaddr_in serv_addr;
static char *fid = "server_init";

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        util_log(1, "%s: socket: %s", fid, strerror(errno));
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
        util_log(1, "%s: bind: %s", fid, strerror(errno));
        exit(1);
    }

    listen(sd, 5);

}

#define ARGLEN     32

#define NRTS_WRTDL "nrts_wrtdl"
#define WRTDL_ARGC 6

static void execWrtdl(char *host, char *system)
{
int pid;
char argv[WRTDL_ARGC][ARGLEN];


    sprintf(argv[0], "%s", NRTS_WRTDL);
    sprintf(argv[1], "if=@%s", host);
    sprintf(argv[2], "log=syslogd");
    sprintf(argv[3], "-keepup");
    sprintf(argv[4], "-retry");
    sprintf(argv[5], "%s", system);

    if ((pid = fork()) < 0) util_log(1, "fork: %s", strerror(errno));
    if (pid != 0) return;

    util_log(1, "exec %s %s %s %s %s %s", 
        argv[0],
        argv[1],
        argv[2],
        argv[3],
        argv[4],
        argv[5]
    );

    execlp(
        NRTS_WRTDL,
        argv[0],
        argv[1],
        argv[2],
        argv[3],
        argv[4],
        argv[5],
        (char *) 0
    );

    util_log(1, "execlp: %s: %s", NRTS_WRTDL, strerror(errno));
    exit(2);
}

#define DRMD "drmd"
#define DRMD_ARGC 3

static void execDrmd(char *home, char *system)
{
int pid;
char argv[DRMD_ARGC][ARGLEN];

    if ((pid = fork()) < 0) util_log(1, "fork: %s", strerror(errno));
    if (pid != 0) return;

    sprintf(argv[0], "%s", DRMD);
    sprintf(argv[1], "home=%s/drm/push/%s", home, system);
    sprintf(argv[2], "-once");

    util_log(1, "exec %s %s %s", argv[0], argv[1], argv[2]);

    execlp(DRMD, argv[0], argv[1], argv[2], (char *) 0);

    util_log(1, "execlp: %s: %s", DRMD, strerror(errno));
    exit(2);
}

static void serve(char *home, int client)
{
int nread;
char *peer;
char system[PUSHD_SYSCODE_BUFLEN+1], buffer[1024];

    peer = get_peer(client, buffer);

/* Simple protocol... read PUSHD_SYSCODE_BUFLEN bytes from peer */

    nread = util_read(client, system, PUSHD_SYSCODE_BUFLEN, 30);
    shutdown(client, 2);
    close(client);

    if (nread != PUSHD_SYSCODE_BUFLEN) {
        util_log(1, "error retrieving system name from %s", peer);
        return;
    }

/* Make sure we have a null-terminated string */

    system[PUSHD_SYSCODE_BUFLEN] = 0;
    util_log(1, "connection from %s@%s", system, peer);

/* Fork and exec nrts_wrtdl on this system */

    execWrtdl(peer, system);

/* Fork and exec drmd on this system */

    execDrmd(home, system);
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [home=HOME_DIR port=PORT_NUMBER]\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
char *home = NULL;
char *log  = "syslogd";
int port = NRTS_PUSHD_PORT;
struct sockaddr_in cli_addr;
int background = 1;
int i, client, len = sizeof(cli_addr);

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "bg=", strlen("bg=")) == 0) {
            background = atoi(argv[i] + strlen("bg="));
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (home == NULL) {
        home = getenv(NRTS_HOME);
        if (home == NULL) home = NRTS_DEFHOME;
    }

    if (background) util_bground(1, 0);
    util_logopen(log, 1, 9, 1, (char *) NULL, argv[0]);
    if (!InstallSignalHandlers()) {
        util_log(1, "failed to install signal handlers");
        logexit(1);
    }

    server_init(port);

    util_log(1, "NRTS push daemon (%s) installed at port %d", VERSION, port);

    while (1) {
        do {
            client = accept(sd, (struct sockaddr *) &cli_addr, &len);
            if (client < 0) {
                util_log(1, "accept: %s", strerror(errno));
                if (errno == EAGAIN || errno == EINTR) {
                    util_log(1, "accept error IGNORED");
                }
            }
        } while (client < 0 && (errno == EAGAIN || errno == EINTR));
        if (client < 0) {
            util_log(1, "accept: %s", strerror(errno));
        } else {
            serve(home, client);
        }
    }
}

/* Revision History
 *
 * $Log: server.c,v $
 * Revision 1.10  2003/12/23 23:49:14  dechavez
 * reworked signal handling to (re) eliminate zombies (which came back after
 * util_bground was reworked in libutil 2.2.0)
 *
 * Revision 1.9  2003/12/10 06:24:50  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.8  2002/02/27 00:16:58  dec
 * 1.2.1, invoke util_bground with ignchild option set to handle
 * defunct processes from forked children
 *
 * Revision 1.7  2001/06/13 16:29:40  ftp
 * added LINUX ifdef to sigwait selection
 *
 * Revision 1.6  2001/05/20 18:43:13  dec
 * use platform.h MUTEX and THREAD macros
 *
 * Revision 1.5  2001/04/21 16:02:22  dec
 * added drmd support
 *
 * Revision 1.4  2001/04/17 19:14:03  nobody
 * Parent does shutdown and close on socket, eliminating lingering BOUND
 * sockets.  Added signal handling thread.
 *
 * Revision 1.3  2001/02/26 23:07:24  dec
 * use client's IP address instead of name for remote connection
 *
 * Revision 1.2  2000/10/12 16:04:35  dec
 * added version number support
 *
 * Revision 1.1  2000/10/12 15:50:28  dec
 * release 0.9.0
 *
 */
