#pragma ident "$Id: sanops.c,v 1.7 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Convenience routines for communicating with the SAN.  They don't
 *  interact with the SAN directly, but do stick messages in the
 *  command queue for processing by the SanCmnd thread.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "sanio.h"
#include "ispd.h"

static MUTEX mutex = MUTEX_INITIALIZER;

#define MY_MOD_ID ISPD_MOD_SANOPS

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;

static void FlushSanCmndQueue()
{
int cmnd, count = 0;
OLD_MSGQ_MSG *msg;

    while ((msg = msgq_get(&Q->das_write, OLD_MSGQ_NOBLOCK)) != NULL) {
        if (++count == 1) util_log(1, "flushing SAN command queue");
        memcpy(&cmnd, msg->data, sizeof(int));
        util_log(1, "%s dropped", sanioMessageTypeToString(cmnd));
        msgq_put(&Heap->commands, msg);
    }
    if (count) util_log(1, "%d unprocessed SAN commands dropped", count);
}

/* Load a SAN command into the message queue */

void SanCmnd(int cmnd)
{
int count;
OLD_MSGQ_MSG *msg;
static char *fid = "SanCmnd";

    MUTEX_LOCK(&mutex);
        if (cmnd < 0) {
            FlushSanCmndQueue();
        } else {
            msg = msgq_get(&Heap->commands, OLD_MSGQ_NOBLOCK);
            if (msg == NULL) {
                util_log(1, "WARNING: SAN command queue full!");
                FlushSanCmndQueue();
            } else {
                msg->len = sizeof(int);
                memcpy(msg->data, &cmnd, msg->len);
                msgq_put(&Q->das_write, msg);
            }
        }
    MUTEX_UNLOCK(&mutex);
}

/* SAN initialization procedure */

static void do_san_init()
{
int count;

/* Wait until sanio reports a config packet has been received */

    while(!have_dascnf()) sleep(1);

}
static void *SanInitThread(void *dummy)
{
    do_san_init();
    THREAD_EXIT(0);
}

void VerifySanConfig(int wait)
{
THREAD tid;
static char *fid = "VerifySanConfig";

    if (wait) {
        do_san_init();
        return;
    }

    if (!THREAD_CREATE(&tid, SanInitThread, NULL)) {
        util_log(1, "%s: failed to create SanInitThread", fid);
        ispd_die(MY_MOD_ID + 2);
    }

    return;
}

/* Revision History
 *
 * $Log: sanops.c,v $
 * Revision 1.7  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.6  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.5  2001/10/08 18:55:53  dec
 * use sanioMessageTypeToString()
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2001/05/02 17:38:54  dec
 * Force flush SAN command queue if it fills up
 *
 * Revision 1.2  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/11 20:38:43  dec
 * initial version
 *
 */
