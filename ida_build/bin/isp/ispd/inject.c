#pragma ident "$Id: inject.c,v 1.6 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Server thread for ISP injection service.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
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
#include <string.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_INJECT

extern struct isp_status *Status;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;

static int sd, client;

static void ServiceConnection()
{
char peer[IDA_MAXRECLEN];
char empty[IDA_MAXRECLEN];
int done, count;
OLD_MSGQ_MSG *msg;
OLD_MSGQ *dest;
static char *fid = "isp_inject:ServceConnection";

    util_peer(client, peer, IDA_MAXRECLEN);
    util_log(1, "injection request from %s", peer);
    memset(empty, 0, IDA_MAXRECLEN);

    util_noblock(client);
    done = count = 0;
    while (!done) {
        msg = msgq_get(&Heap->packets, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt Heap->packets message received", fid);
            ispd_die(MY_MOD_ID + 1);
        }
        if (util_read(client, msg->data, IDA_BUFSIZ, 10) == IDA_BUFSIZ) { /* IDA10 OK */
            if (memcmp(msg->data, empty, IDA_BUFSIZ) == 0) { /* IDA10 OK */
                done = 1;
                dest = &Heap->packets;
            } else {
                ++count;
                MUTEX_LOCK(&Status->lock);
                    ++Status->count.njec;
                MUTEX_UNLOCK(&Status->lock);
                dest = &Q->das_process;
            }
        } else {
            util_log(1, "read error from %s: %s", peer, syserrmsg(errno));
            done = 1;
            dest = &Heap->packets;
        }
        msgq_put(dest, msg);
    }

    util_log(1, "%d records received from %s", count, peer);
}

static THREAD_FUNC ServerThread(void *arg)
{
u_short port;
struct sockaddr_in cli_addr;
#ifdef socklen_t
socklen_t len = sizeof(cli_addr);
#else
int len = sizeof(cli_addr);
#endif
static char *fid = "isp_inject:ServerThread";

    port = *((u_short *) arg);
    util_log(1, "ISP injection service installed at port %hd", port);

/* Wait for connections */

    while (1) {
        client = accept(sd, (struct sockaddr *) &cli_addr, &len);
        if (client < 0 && errno != EINTR) {
            util_log(1, "%s: accept: %s", fid, syserrmsg(errno));
            util_log(1, "isp injection service aborted");
            THREAD_EXIT(0);
        } else if (client >= 0) {
            ServiceConnection();
            shutdown(client, 2);
            close(client);
            client = 0;
        }
    }
}

#ifdef INCLUDE_THIS
void shutdown_inject()
{
    if (client > 0) {
        shutdown(client, 2);
        close(client);
    }
}
#endif

void isp_inject()
{
THREAD tid;
int i, yes=1, ilen=sizeof(int);
struct servent *sp;
struct sockaddr_in serv_addr;
static u_short port;
static char *fid = "isp_inject";

/* Port number is taken from services */

    if ((sp = getservbyname(ISP_INJECT, "tcp")) == (struct servent *) NULL) {
        util_log(1, "%s: getservbyname(%s, %s): %s",
            fid, ISP_INJECT, "tcp", syserrmsg(errno)
        );
        ispd_die(MY_MOD_ID + 2);
    }
    port = ntohs(sp->s_port);

/* Setup as a server */

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        util_log(1, "%s: socket: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 3);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, ilen);

    if (bind(sd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        util_log(1, "%s: bind: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 4);
    }

    listen(sd, 5);

/* Start a thread to handle all future connections */

    if (!THREAD_CREATE(&tid, ServerThread, &port)) {
        util_log(1, "%s: failed to create ServerThread", fid);
        ispd_die(MY_MOD_ID + 5);
    }
}

/* Revision History
 *
 * $Log: inject.c,v $
 * Revision 1.6  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.5  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2001/01/12 23:38:05  dec
 * set SO_REUSEADDR socket option
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
