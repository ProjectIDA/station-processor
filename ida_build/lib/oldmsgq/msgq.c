#pragma ident "$Id: msgq.c,v 1.3 2006/12/06 22:35:46 dechavez Exp $"
/*======================================================================
 *
 *  Simple minded message queue to handle buffering of data between
 *  threads.
 *
 *  A message is requested from the upstream thread or heap with
 *  with msgq_get(), and is passed to a down stream thread or returned
 *  to the heap with msgq_put().
 *
 *  It assumes I know what I am doing, and no checking on message
 *  lengths and the like is done...
 *
 *  To mmap the buffers, set the prefix field to something other than 0
 *  in which case the buffers will be mmap()'d to disk files which have
 *  the name PREFIXseqno, where seqno is a sequence number.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "platform.h"
#include "oldmsgq.h"
#include "util.h"

#define TESTLEN  ((size_t) 4)
#define TESTCHAR 'e'

static void DumpQueue(char *fid, OLD_MSGQ *queue, int loglevel)
{
    util_log(loglevel, "%s:  addr=0x%08x", fid, queue);
    util_log(loglevel, "%s:  next=0x%08x", fid, queue->head.next);
    util_log(loglevel, "%s: count=%d",     fid, queue->count);
    util_log(loglevel, "%s: lowat=%d",     fid, queue->lowat);
    util_log(loglevel, "%s: hiwat=%d",     fid, queue->hiwat);
}

static char *allocate_data(int buflen, char *prefix, int index)
{
char path[MAXPATHLEN+1];
int  fd, prot, flag, clear_count, need_set, flags;
char *addr, *result;
off_t off;
struct stat statbuf;
static char *fid = "msgq_init: allocate_data";

/* If no prefix is supplied, use regular memory */

    if (prefix == (char *) 0) return (char *) malloc(buflen);

/* Otherwise, use mmap()'d disk files */

    sprintf(path, "./%s%02d", prefix, index);

    need_set = (stat(path, &statbuf) != 0 || statbuf.st_size < buflen);

#ifdef O_SYNC
    flags = O_RDWR | O_SYNC | O_CREAT;
#else
    flags = O_RDWR |          O_CREAT;
#endif

    if ((fd = open(path, flags, 0644)) == -1) {
        util_log(1, "%s: open: %s: %s", fid, path, syserrmsg(errno));
        return (char *) NULL;
    }

    if (ftruncate(fd, buflen) != 0) {
        util_log(1, "%s: ftruncate: %s: %s", fid, path,syserrmsg(errno));
        return (char *) NULL;
    }

    addr = (char *) 0;
    prot = PROT_READ | PROT_WRITE;
    flag = MAP_SHARED;
    off  = (off_t) 0;

    result = (char *) mmap(addr, buflen, prot, flag, fd, off);
 
    if (result == (char *) MAP_FAILED) {
        util_log(1, "%s: mmap: %s: %s", fid, path, syserrmsg(errno));
        result == (char *) NULL;
    }

    close(fd);

    /* The following insures that the necessary disk space is really
     * consumed.  Using ftruncate() alone does not cause the files to
     * be as big as they say they are (comparing what `ls' says to what
     * `df' says).  Not wanting to be suprised late in the game, when
     * activity elsewhere has caused the disk to get full, this should
     * insure that we won't have problems.  It is only done if the
     * existing file size is too small, as this can be a time consuming
     * operation.  If the file is already big enough, then we just zero
     * out the nrec field at the front.
     */

    clear_count = need_set ? buflen : sizeof(size_t);
    memset(result, (char) 0, clear_count);

    return result;
}

BOOL msgq_init(
    OLD_MSGQ *queue, int nelem, int maxlen, char *prefix
){
int i;
OLD_MSGQ_MSG *crnt, *new, *pool;
static char *fid = "msgq_init";

    strcpy(queue->sanity1, "Q1");
    strcpy(queue->sanity2, "Q2");
    strcpy(queue->sanity3, "Q3");
    strcpy(queue->sanity4, "Q4");
    strcpy(queue->sanity5, "Q5");
    strcpy(queue->sanity6, "Q6");
    strcpy(queue->sanity7, "Q7");

    MUTEX_INIT(&queue->mp);
    SEM_INIT(&queue->sp, nelem, nelem);
    queue->head.next = (OLD_MSGQ_MSG *) NULL;
    queue->head.data = (char *) NULL;
    queue->count = nelem;
    queue->lowat = nelem;
    queue->hiwat = 0;

    pool = (OLD_MSGQ_MSG *) malloc(sizeof(OLD_MSGQ_MSG) * nelem);

    if (pool == (OLD_MSGQ_MSG *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return FALSE;
    }

    crnt = &queue->head;
    for (i = 0; i < nelem; i++) {
        new = pool + i;
        new->data = allocate_data(maxlen+TESTLEN, prefix, i);
        if (new->data == (char *) NULL) {
            util_log(1, "%s: allocate_data: %s", fid, syserrmsg(errno));
            return FALSE;
        }
        strcpy(new->sanity1, "T1");
        strcpy(new->sanity2, "T2");
        strcpy(new->sanity3, "T3");
        strcpy(new->sanity4, "T4");
        strcpy(new->sanity5, "T5");
        new->len    = 0;
        new->maxlen = maxlen;
        new->next   = (OLD_MSGQ_MSG *) NULL;
        crnt->next  = new;
        crnt        = new;
        memset(new->data + new->maxlen, TESTCHAR, TESTLEN);
    }
    util_log(2, "INITIALIZED queue=0x%08x %3d %3d %3d",
        queue, queue->count, queue->lowat, queue->hiwat
    );

    return TRUE;
}

static void VerifyQueue(char *fid, OLD_MSGQ *queue)
{
BOOL ok;

    MUTEX_LOCK(&queue->mp);
        ok = TRUE;
        if (strcmp(queue->sanity1, "Q1") != 0) {
            util_log(1, "%s: 0x%08x sanity1 != Q1!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity2, "Q2") != 0) {
            util_log(1, "%s: 0x%08x sanity2 != Q2!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity3, "Q3") != 0) {
            util_log(1, "%s: 0x%08x sanity3 != Q3!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity4, "Q4") != 0) {
            util_log(1, "%s: 0x%08x sanity4 != Q4!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity5, "Q5") != 0) {
            util_log(1, "%s: 0x%08x sanity5 != Q5!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity6, "Q6") != 0) {
            util_log(1, "%s: 0x%08x sanity6 != Q6!", fid, queue);
            ok = FALSE;
        }
        if (strcmp(queue->sanity7, "Q7") != 0) {
            util_log(1, "%s: 0x%08x sanity7 != Q7!", fid, queue);
            ok = FALSE;
        }
        if (!ok) {
            util_log(1, "CORRPUT queue=0x%08x %3d %3d %3d",
                queue, queue->count, queue->lowat, queue->hiwat
            );
            abort();
        }
    MUTEX_UNLOCK(&queue->mp);
}

/* Get the next buffer from the queue */

OLD_MSGQ_MSG *msgq_get(OLD_MSGQ *queue, int flag)
{
OLD_MSGQ_MSG *head, *msg;
static char *fid = "msgq_get";

    while (1) {
        VerifyQueue(fid, queue);

        if (flag == OLD_MSGQ_WAITFOREVER) {
            SEM_WAIT(&queue->sp);
        } else if (SEM_TRYWAIT(&queue->sp) != 0) {
            return (OLD_MSGQ_MSG *) NULL;
        }

        MUTEX_LOCK(&queue->mp);
            msg = NULL;
            if (queue->count > 0) {
                head = &queue->head;
                if (head->next == (OLD_MSGQ_MSG *) NULL) {
                    DumpQueue(fid, queue, 1);
                    util_log(1, "%s: ABORT: NULL NEXT WITH NON_ZERO COUNT!",
                        fid
                    );
                    abort();
                } else {
                    msg = head->next;
                    head->next = msg->next;
                    if (!msgq_chkmsg(msg)) {
                        DumpQueue(fid, queue, 1);
                        util_log(1, "%s: corrupt message from queue 0x%08x",
                            fid, queue
                        );
                        util_log(1, "%s: ABORT", fid);
                        abort();
                    }
                    --queue->count;
                    if (queue->count < queue->lowat) queue->lowat = queue->count;
                }
            } else {
                util_log(2, "%s: ignore spurious(?) SEM_WAIT return:", fid);
#ifdef SUICIDE_ON_ERRORS
                DumpQueue(fid, queue, 2);
                kill(getpid(), SIGINT);
#endif
            }
        MUTEX_UNLOCK(&queue->mp);
        if (msg != NULL) return msg;
    }
}

void msgq_put(OLD_MSGQ *queue, OLD_MSGQ_MSG *new)
{
OLD_MSGQ_MSG *crnt;
static char *fid = "msgq_put";

    VerifyQueue(fid, queue);

    if (!msgq_chkmsg(new)) {
        util_log(1, "%s: ABORT: corrupt message sent to queue 0x%08x",
            fid, queue
        );
        abort();
    }

    MUTEX_LOCK(&queue->mp);
        crnt = &queue->head;
        while (crnt->next != (OLD_MSGQ_MSG *) NULL) crnt = crnt->next;
        crnt->next = new;
        new->next  = (OLD_MSGQ_MSG *) NULL;
        SEM_POST(&queue->sp);
        if (++queue->count > queue->hiwat) queue->hiwat = queue->count;
    MUTEX_UNLOCK(&queue->mp);
}

int msgq_count(OLD_MSGQ *queue)
{
int retval;
static char *fid = "msgq_count";

    VerifyQueue(fid, queue);

    MUTEX_LOCK(&queue->mp);
        retval = queue->count;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

int msgq_lowat(OLD_MSGQ *queue)
{
int retval;
static char *fid = "msgq_lowat";

    VerifyQueue(fid, queue);

    MUTEX_LOCK(&queue->mp);
        retval = queue->lowat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

int msgq_hiwat(OLD_MSGQ *queue)
{
int retval;
static char *fid = "msgq_hiwat";

    VerifyQueue(fid, queue);

    MUTEX_LOCK(&queue->mp);
        retval = queue->hiwat;
    MUTEX_UNLOCK(&queue->mp);

    return retval;
}

void msgq_clear(OLD_MSGQ *queue, OLD_MSGQ *heap)
{
OLD_MSGQ_MSG *msg;
static char *fid = "msgq_clear";

    VerifyQueue(fid, queue);

    msg = msgq_get(queue, OLD_MSGQ_NOBLOCK);
    while (msg != (OLD_MSGQ_MSG *) NULL) {
        msgq_put(heap, msg);
        msg = msgq_get(queue, OLD_MSGQ_NOBLOCK);
    }
}

BOOL msgq_chkmsg2(char *caller, OLD_MSGQ_MSG *msg)
{
static char tbuf[TESTLEN] = {TESTCHAR, TESTCHAR, TESTCHAR, TESTCHAR};
static char *fid = "msgq_chkmsg";

    if (msg == (OLD_MSGQ_MSG *) NULL) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: msg == NULL!", caller, fid);
        } else {
            util_log(1, "%s ERROR: msg == NULL!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (strcmp(msg->sanity1, "T1") != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: sanity1 != T1!", caller, fid);
        } else {
            util_log(1, "%s ERROR: sanity1 != T1!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (strcmp(msg->sanity2, "T2") != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: sanity2 != T2!", caller, fid);
        } else {
            util_log(1, "%s ERROR: sanity2 != T2!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (strcmp(msg->sanity3, "T3") != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: sanity3 != T3!", caller, fid);
        } else {
            util_log(1, "%s ERROR: sanity3 != T3!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (strcmp(msg->sanity4, "T4") != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: sanity4 != T4!", caller, fid);
        } else {
            util_log(1, "%s ERROR: sanity4 != T4!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (strcmp(msg->sanity5, "T5") != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: sanity5 != T5!", caller, fid);
        } else {
            util_log(1, "%s ERROR: sanity5 != T5!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (msg->maxlen <= 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: msg->maxlen=%ld <= 0!",
                caller, fid, msg->maxlen
            );
        } else {
            util_log(1, "%s ERROR: msg->maxlen=%ld <= 0!",
                fid, msg->maxlen
            );
        }
        errno = EINVAL;
        return FALSE;
    }

    if (msg->data == (char *) NULL) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: msg->data == NULL!", caller, fid);
        } else {
            util_log(1, "%s ERROR: msg->data == NULL!", fid);
        }
        errno = EINVAL;
        return FALSE;
    }

    if (memcmp(
        (void *) (msg->data + msg->maxlen), 
        (void *) tbuf,
        (size_t) TESTLEN
    ) != 0) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: buffer overflow (maxlen=%ld)!",
                caller, fid, msg->maxlen
            );
        } else {
            util_log(1, "%s ERROR: buffer overflow (maxlen=%ld)!",
                fid, msg->maxlen
            );
        }
        utilPrintHexDump(stdout, (UINT8 *) msg->data, (int) msg->maxlen);
        return FALSE;
    }

    if (msg->len > msg->maxlen) {
        if (caller != NULL) {
            util_log(1, "%s: %s ERROR: msg->len=%ld > %ld!",
                caller, fid, msg->len, msg->maxlen
            );
        } else {
            util_log(1, "%s ERROR: msg->len=%ld > %ld!",
                fid, msg->len, msg->maxlen
            );
        }
        return FALSE;
    }

    return TRUE;
}

BOOL msgq_chkmsg(OLD_MSGQ_MSG *msg)
{
    return msgq_chkmsg2(NULL, msg);
}

/* Revision History
 *
 * $Log: msgq.c,v $
 * Revision 1.3  2006/12/06 22:35:46  dechavez
 * renamed all MSGQ... items OLDMSGQ...
 *
 * Revision 1.2  2006/05/17 18:22:03  dechavez
 * fixed include to reference oldmsgq.h
 *
 * Revision 1.1  2006/05/17 18:20:59  dechavez
 * "initial" release (extracted from previously merged msgq library)
 *
 */
