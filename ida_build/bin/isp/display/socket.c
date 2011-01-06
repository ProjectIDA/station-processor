#pragma ident "$Id: socket.c,v 1.6 2003/10/02 20:24:51 dechavez Exp $"
/*======================================================================
 *
 *  Socket related commands.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <curses.h>
#include "isp_console.h"

#define UNCONNECTED -1

extern WINDOW *full_screen;
extern char *server;
extern int port;

static int sd           = UNCONNECTED;
static MUTEX     sd_mutex = MUTEX_INITIALIZER;
static SEMAPHORE sd_sema;
static int TimeOut      = -1;
static BOOL ShutdownInProgress = FALSE;

static char buffer[ISP_BUFLEN];

int Connected()
{
int retval;

    MUTEX_LOCK(&sd_mutex);
        retval = (sd == UNCONNECTED) ? 0 : CONNECTED;
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

int TakeCharge()
{
short stmp;
int retval;

    MUTEX_LOCK(&sd_mutex);
        if (sd != UNCONNECTED) {
            stmp = htons(ISP_OPERATOR);
            memcpy(buffer+4, &stmp, 2);
            retval = isp_send(sd, ISP_STATE, buffer, 2, TimeOut);
        } else {
            retval = ISP_OK;
        }
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

int RemoteCommand(int command)
{
int retval;

    MUTEX_LOCK(&sd_mutex);
        if (sd != UNCONNECTED) {
            retval = isp_send(sd, command, buffer, 0, TimeOut);
        } else {
            retval = ISP_OK;
        }
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

void Quit(int status)
{
    MUTEX_LOCK(&sd_mutex);
        ShutdownInProgress = TRUE;
    MUTEX_UNLOCK(&sd_mutex);
    RemoteCommand(ISP_DISCONNECT);
    RequestShutdown(status);
}

void Reconnect(int where)
{
    MUTEX_LOCK(&sd_mutex);
        if (!ShutdownInProgress) {
            util_log(1, "reconnect from location %d", where);
            if (sd != UNCONNECTED) {
                isp_send(sd, ISP_DISCONNECT, buffer, 0, TimeOut);
                shutdown(sd, 2);
                close(sd);
                sd = UNCONNECTED;
                SEM_DESTROY(&sd_sema);
                SEM_INIT(&sd_sema, 0, 1);
            }
        }
    MUTEX_UNLOCK(&sd_mutex);

    while(!Connected()) sleep(1);
}

int SendDasCal(struct isp_dascal *cal)
{
int retval;

    MUTEX_LOCK(&sd_mutex);
        if (sd != UNCONNECTED) {
            retval = isp_send_dascal(sd, buffer, cal, TimeOut);
        } else {
            retval = ISP_OK;
        }
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

int SendDasCnf(struct isp_dascnf *cnf)
{
int retval;

    MUTEX_LOCK(&sd_mutex);
        if (sd != UNCONNECTED) {
            retval = isp_send_dascnf(sd, buffer, cnf, TimeOut);
        } else {
            retval = ISP_OK;
        }
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

int SetTimeout(int to)
{
int retval;
short stmp;

    MUTEX_LOCK(&sd_mutex);
        if (sd != UNCONNECTED) {
            TimeOut = to;
            stmp = htons(TimeOut);
            memcpy(buffer+4, &stmp, 2);
            retval = isp_send(sd, ISP_TIMEOUT, buffer, 2, TimeOut);
        } else {
            retval = ISP_OK;
        }
    MUTEX_UNLOCK(&sd_mutex);

    return retval;
}

static void Decode(int type, void *data)
{
    switch (type) {
      case ISP_NOP:
        break;

      case ISP_DISCONNECT:
        Quit(0);
        break;

      case ISP_CHNMAP:
        SaveChanMap(data);
        break;

      case ISP_STREAMMAP:
        SaveStreamMap(data);
        break;

      case ISP_STATUS:
        SaveStatus(data);
        break;

      case ISP_STATE:
        SaveState(data);
        break;

      case ISP_PARAM:
        SaveParams(data);
        break;

      case ISP_DASCNF:
        SaveDasCnf(data);
        break;

      default:
        break;
    }
}

static void *ConnectThread(void *dummy)
{
int mysd = UNCONNECTED;
static char *fid = "ConnectThread";

    NoteConnecting();

    do {
        util_log(2, "%s: call util_connect()", fid);
        mysd = util_connect(server, (char *) NULL, port, "tcp", 0, 0);
        util_log(2, "%s: util_connect() returns %d", fid, mysd);
        if (mysd < 0) {
            EndWin();
            fprintf(stderr, "can't connect to %s.%d: %s\n",
                server, port, syserrmsg(errno)
            );
            RequestShutdown(1);
        } else if (mysd < 1) {
            sleep(1);
        }
    } while (mysd < 1);

    util_log(2, "%s: saving socket descriptor", fid);
    MUTEX_LOCK(&sd_mutex);
        sd = mysd;
    MUTEX_UNLOCK(&sd_mutex);
    util_log(2, "%s: posting semaphore", fid);
    SEM_POST(&sd_sema);

    InitSoh();
}

static void *ReadThread(void *dummy)
{
int type;
void *data;
char private_buffer[ISP_BUFLEN];
static char *fid = "ReadThread";

    util_log(1, "%s started, tid = %d", fid, THREAD_SELF());

    while (1) {
        if (!Connected()) {
            SEM_WAIT(&sd_sema);
        }
        type = isp_recv(sd, private_buffer, TimeOut, &data);
        if (type != ISP_ERROR) {
            Decode(type, data);
        } else {
            Reconnect(0);
        }
    }
}

void ServerConnect(char *server_name, int port_number, int to, BOOL ReadToo)
{
THREAD tid;

    SEM_INIT(&sd_sema, 0, 1);
       server  = server_name;
       port    = port_number;
       TimeOut = to;
       util_log(1, "start background server connection");

    if (!THREAD_CREATE(&tid, ConnectThread, NULL)) {
        EndWin();
        fprintf(stderr, "failed to create ConnectThread\n");
        exit(1);
    }

    if (ReadToo) {
        if (!THREAD_CREATE(&tid, ReadThread, NULL)) {
            EndWin();
            fprintf(stderr, "failed to create ReadThread\n");
            exit(1);
        }
    }
}


/* Revision History
 *
 * $Log: socket.c,v $
 * Revision 1.6  2003/10/02 20:24:51  dechavez
 * 2.3.1 mods (fixed console hang on exit, improved dumb terminal refresh
 * a bit, added idle timeout)
 *
 * Revision 1.5  2002/09/09 21:50:43  dec
 * RequestShutdown instead of forced exit()
 *
 * Revision 1.4  2002/05/21 17:00:15  dec
 * replace explicit window termination with call to RequestShutdown()
 *
 * Revision 1.3  2001/05/20 17:44:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
