#pragma ident "$Id: msgq.c,v 1.12 2006/05/17 23:23:24 dechavez Exp $"
/*======================================================================
 *
 *  Simple minded message queue to handle buffering of data between
 *  threads.  This implementation uses the mutex and semaphore macros.
 *
 *  A message is pulled off a queue with msgqGet(), and put on a queue
 *  with msgqPut().  Use msgqInit() to initialize a queue and fill it
 *  it with pointers to pre-sized buffers.
 *
 *  The simple minded part is that there is no notion of message type.
 *  All messages are UINT8 pointers, and it is assumed that the
 *  application knows what to do with them, and won't clobber memory.
 *  Furthermore, there is no way provided to delete these queues.  This
 *  is basically because up to now the applications that use these want
 *  to have the queues around forever and so it hasn't been an issue.
 *
 *====================================================================*/
#include "msgq.h"

#define TESTLEN  ((size_t) 4)
#define TESTCHAR 'e'

/* Initialize a queue with pre-sized buffers */

BOOL msgqInit(MSGQ *queue, UINT32 nelem, UINT32 maxelem, UINT32 maxlen)
{
UINT32 i;
MSGQ_MSG *crnt, *newm;

    MUTEX_INIT(&queue->mp);
    SEM_INIT(&queue->sp, nelem, maxelem);
    queue->lp = (LOGIO *) NULL;
    queue->head.next = (MSGQ_MSG *) NULL;
    queue->head.data = (UINT8 *) NULL;
    queue->nfree = nelem;
    queue->lowat = nelem;
    queue->hiwat = 0;

    crnt = &queue->head;
    for (i = 0; i < nelem; i++) {

    /* create the message object */

        newm = (MSGQ_MSG *) malloc(sizeof(MSGQ_MSG));
        if (newm == (MSGQ_MSG *) NULL) return FALSE;

    /* initialize the message object */

        newm->maxlen = maxlen;
        newm->data   = (UINT8 *) malloc(maxlen+TESTLEN);
        newm->len    = 0;
        if (newm->data == (UINT8 *) NULL) return FALSE;
        memset((void *) newm->data, (int) TESTCHAR, (size_t) maxlen+TESTLEN);
        newm->next = (MSGQ_MSG *) NULL;

    /* add it to the end of the list */

        crnt->next = newm;
        crnt       = crnt->next;
    }

    return TRUE;
}

/* Enable logging */

VOID msgqSetLog(MSGQ *queue, LOGIO *lp)
{
    MUTEX_LOCK(&queue->mp);
        queue->lp = lp;
    MUTEX_UNLOCK(&queue->mp);
}

/* Pull the next message off the queue */

MSGQ_MSG *msgqGet(MSGQ *queue, UINT16 flag)
{
MSGQ_MSG *head, *msg;
static char *fid = "msgqGet";

    if (queue == (MSGQ *) NULL) {
        logioMsg(queue->lp, LOG_ERR, "%s: ABORT - NULL queue pointer received!", fid);
        abort();
    }

    if (flag == MSGQ_WAIT) {
        SEM_WAIT(&queue->sp);
    } else if (SEM_TRYWAIT(&queue->sp) != 0) {
        return (MSGQ_MSG *) NULL;
    }

    MUTEX_LOCK(&queue->mp);
        head = &queue->head;
        if (head->next == (MSGQ_MSG *) NULL) {
            logioMsg(queue->lp, LOG_ERR, "%s: ABORT - QUEUE MANAGMENT IS HOSED!", fid);
            abort();
        }
        msg = head->next;
        head->next = msg->next;
        if (--queue->nfree < queue->lowat) queue->lowat = queue->nfree;
    MUTEX_UNLOCK(&queue->mp);

    return msg;
}

VOID msgqPut(MSGQ *queue, MSGQ_MSG *newm)
{
MSGQ_MSG *crnt;

    MUTEX_LOCK(&queue->mp);
        crnt = &queue->head;
        while (crnt->next != (MSGQ_MSG *) NULL) crnt = crnt->next;
        crnt->next = newm;
        newm->next = (MSGQ_MSG *) NULL;
        SEM_POST(&queue->sp);
        if (++queue->nfree > queue->hiwat) queue->hiwat = queue->nfree;
    MUTEX_UNLOCK(&queue->mp);
}

UINT32 msgqNfree(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->nfree;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

UINT32 msgqLowat(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->lowat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

UINT32 msgqHiwat(MSGQ *queue)
{
UINT32 retval;

    MUTEX_LOCK(&queue->mp);
        retval = queue->hiwat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

/* Flush a queue */

VOID msgqFlush(MSGQ *full, MSGQ *empty)
{
MSGQ_MSG *msg;

    while ((msg = msgqGet(full, MSGQ_NOWAIT)) != (MSGQ_MSG *) NULL) {
        msgqPut(empty, msg);
    }
}

/* Check for message overruns */

BOOL msgqCheck(MSGQ_MSG *msg)
{
static UINT8 tbuf[TESTLEN] = {TESTCHAR, TESTCHAR, TESTCHAR, TESTCHAR};
static CHAR *fid = "msgqCheck";

    if (msg == (MSGQ_MSG *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (msg->maxlen <= 0) {
        errno = EINVAL;
        return FALSE;
    }

    if (msg->data == (UINT8 *) NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (memcmp((void *) (msg->data + msg->maxlen), (void *) tbuf, (size_t) TESTLEN) != 0) {
        errno = EMSGSIZE;
        return FALSE;
    }

    if (msg->len > msg->maxlen) {
        errno = EMSGSIZE;
        return FALSE;
    }

    return TRUE;
}

BOOL msgqInitBuf(MSGQ_BUF *buf, UINT32 nelem, UINT32 size)
{
    if (!msgqInit(&buf->heap, nelem, nelem, size)) return FALSE;
    if (!msgqInit(&buf->full,     0, nelem, size)) return FALSE;

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: msgq.c,v $
 * Revision 1.12  2006/05/17 23:23:24  dechavez
 * added copyright notice
 *
 * Revision 1.11  2006/05/17 18:24:47  dechavez
 * replaced previous msgq.c which has been deprecated off to oldmsgq library
 *
 * Revision 1.2  2006/05/04 20:36:16  dechavez
 * added msgqInitBuf()
 *
 * Revision 1.1  2003/06/10 00:12:08  dechavez
 * imported from ESSW
 *
 */
