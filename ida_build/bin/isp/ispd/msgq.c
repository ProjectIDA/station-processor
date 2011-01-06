#pragma ident "$Id: msgq.c,v 1.6 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Initialize the message queues.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <errno.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_MSGQ

extern ISP_PARAMS *Params;
extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;

static void PrintQueue(char *name, OLD_MSGQ *queue)
{
OLD_MSGQ *addr;
long count, lowat, hiwat;

    MUTEX_LOCK(&queue->mp);
        addr  = queue;
        count = queue->count;
        lowat = queue->lowat;
        hiwat = queue->hiwat;
    MUTEX_UNLOCK(&queue->mp);

    util_log(1, "%s=0x%08x, %3d %3d %3d", name, addr, count, lowat, hiwat);
}

void ShowMessageQueues()
{
    PrintQueue("  Heap->packets", &Heap->packets);
    PrintQueue(" Heap->commands", &Heap->commands);
    PrintQueue("Heap->barometer", &Heap->barometer);
    PrintQueue("     Heap->obuf", &Heap->obuf);
    PrintQueue("        Q->baro", &Q->baro);
    PrintQueue("    Q->das_read", &Q->das_read);
    PrintQueue("   Q->das_write", &Q->das_write);
    PrintQueue(" Q->das_process", &Q->das_process);
    PrintQueue("      Q->massio", &Q->massio);
    PrintQueue("        Q->nrts", &Q->nrts);
    PrintQueue("        Q->obuf", &Q->obuf);
}

void InitMsgqs()
{
int buflen;
static char *fid = "InitMsgqs";

    util_log(1, "initializing message queues and massio buffers");

/* Do the heap */

    if (!msgq_init(
        &Heap->packets,   Params->rawq, IDA_MAXRECLEN, (char *) 0
    )) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 1);
    }

    if (!msgq_init(
        &Heap->commands,  Params->cmdq, ISPD_MAX_CMDLEN, (char *) 0
    )) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 2);
    }

    if (!msgq_init(
        &Heap->barometer, Params->barq, sizeof(ISPD_AUX_DATUM), (char *) 0
    )) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 3);
    }

    if (!msgq_init(
        &Heap->dpm, Params->dpmq, sizeof(ISPD_AUX_DATUM), (char *) 0
    )) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 3);
    }

    buflen = sizeof(size_t) + (Params->bfact * Params->obuf * IDA_MAXRECLEN);
    if (!msgq_init(
        &Heap->obuf, Params->numobuf, buflen, ISP_BUF_DIR
    )) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 4);
    }

/* Do the queues */
    
    if (!msgq_init(&Q->baro, 0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 5);
    }

    if (!msgq_init(&Q->dpm, 0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 5);
    }

    if (!msgq_init(&Q->das_read,    0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 6);
    }
    
    if (!msgq_init(&Q->das_write, 0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 7);
    }

    if (!msgq_init(&Q->das_process, 0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 8);
    }
    
    if (!msgq_init(&Q->massio,      0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 9);
    }
    
    if (!msgq_init(&Q->nrts,        0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 10);
    }

    if (!msgq_init(&Q->obuf, 0, 0, (char *) 0)) {
        util_log(1, "%s: msgq_init: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 11);
    }

    util_log(1, "message queues and massio buffers ready");
}

/* Revision History
 *
 * $Log: msgq.c,v $
 * Revision 1.6  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.5  2002/09/09 21:58:40  dec
 * added dpm support
 *
 * Revision 1.4  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.3  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
