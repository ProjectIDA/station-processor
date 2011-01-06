#pragma ident "$Id: sanio.c,v 1.2 2001/09/18 23:24:51 dec Exp $"
/*======================================================================
 *
 *  Handle all I/O with the SAN
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2001 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "sanlog.h"

#define MY_MOD_ID SANLOG_MOD_SANIO

static BOOL ConnectToServer(SAN_ADDR *san, UINT8 *buf)
{
static char *fid = "ConnectToServer";

    if (!sanioConnect(san->sp)) {
        util_log(1, "%s: sanioConnect (%s:%d) failed!", fid, san->name, san->port);
        util_log(1, "%s: %s:%d thread exits", fid, san->name, san->port);;
        THREAD_EXIT(0);
    }

    return sanioSend(san->sp, SAN_LOGON, buf, 0);
}

static void CloseAndReconnect(SAN_ADDR *san, int cause, UINT8 *buf)
{
BOOL first = TRUE;
static char *fid = "CloseAndReconnect";

    util_log(1, "reconnect with %s:%d", san->name, san->port);
    do {
        if (first) sanioDisconnect(san->sp, cause, buf);
    } while (!ConnectToServer(san, buf));
}
    
/* Thread to handle C&C returns from the SAN */

static void *ReadThread(void *arg)
{
int type;
SAN_ADDR *san;
UINT8 buf[SANIO_BUFLEN], *data;
static char *fid = "ReadThread";

    san = (SAN_ADDR *) arg;
    util_log(2, "%s (%s:%d) started", fid, san->name, san->port);

    while (!ConnectToServer(san, buf)) CloseAndReconnect(san, SAN_ERROR, buf);

    while (1) {

        type = (int) sanioRecv(san->sp, &data);

        switch (type) {

          case SAN_LOGMSG:
            MUTEX_LOCK(&san->mutex);
                fprintf(san->fp, "%s", data);
                fflush(san->fp);
            MUTEX_UNLOCK(&san->mutex);
            break;

          case SAN_TIMEDOUT:
            util_log(1, "%s:%d read timeout", san->name, san->port);
            CloseAndReconnect(san, type, buf);
            break;

          case SAN_ERROR:
            util_log(1, "%s:%d I/O error", san->name, san->port);
            CloseAndReconnect(san, type, buf);
            break;

          case SAN_DISCONNECT:
            util_log(1, "%s:%d server disconnect", san->name, san->port);
            CloseAndReconnect(san, type, buf);
            break;
        }
    }
}

void StartSanioThread(SAN_ADDR *san, int timeout)
{
static char *fid = "StartSanioThread";

/* Start command and control threads */

    san->sp = sanioInit(san->name, san->port, timeout, TRUE);
    if (san->sp == NULL) {
        util_log(1, "%s: sanioInit failed", fid);
        GracefulExit(MY_MOD_ID + 10);
    }

    if (!THREAD_CREATE(&san->tid, ReadThread, (void *) san)) {
        util_log(1, "%s: failed to start ReadThread", fid);
        GracefulExit(MY_MOD_ID + 11);
    }
}

/* Revision History
 *
 * $Log: sanio.c,v $
 * Revision 1.2  2001/09/18 23:24:51  dec
 * release 1.1.2
 *
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
