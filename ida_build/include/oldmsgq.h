#pragma ident "$Id: oldmsgq.h,v 1.2 2006/12/06 22:35:20 dechavez Exp $"
/*======================================================================
 *
 *  Message Queue library header file (depcrecated version)
 *
 *====================================================================*/
#ifndef old_msgq_h_included
#define old_msgq_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OLD_MSGQ_WAITFOREVER 0
#define OLD_MSGQ_NOBLOCK     1

typedef struct old_msgq_msg {
    char sanity1[3];
    unsigned long len;
    char sanity2[3];
    unsigned long maxlen;
    char sanity3[3];
    struct old_msgq_msg *next;
    char sanity4[3];
    char *data;
    char sanity5[3];
} OLD_MSGQ_MSG;

typedef struct {
    char sanity1[3];
    OLD_MSGQ_MSG head;          /* the messages        */
    char sanity2[3];
    MUTEX mp;               /* for protection      */
    char sanity3[3];
    SEMAPHORE sp;           /* for synchronization */
    char sanity4[3];
    long count;             /* for optimization    */
    char sanity5[3];
    long lowat;             /* for optimization    */
    char sanity6[3];
    long hiwat;             /* for optimization    */
    char sanity7[3];
} OLD_MSGQ;

/* msgq.c */
OLD_MSGQ_MSG *msgq_get(OLD_MSGQ *queue, int flag);
void msgq_clear(OLD_MSGQ *queue, OLD_MSGQ *heap);
void msgq_put(OLD_MSGQ *queue, OLD_MSGQ_MSG *new);
BOOL msgq_init(OLD_MSGQ *queue, int nelem, int maxlen, char *);
int msgq_count(OLD_MSGQ *queue);
int msgq_lowat(OLD_MSGQ *queue);
int msgq_hiwat(OLD_MSGQ *queue);
BOOL msgq_chkmsg(OLD_MSGQ_MSG *msg);
BOOL msgq_chkmsg2(char *caller, OLD_MSGQ_MSG *msg);

#ifdef __cplusplus
}
#endif

#endif /* old_msgq_h_included */

/* Revision History
 *
 * $Log: oldmsgq.h,v $
 * Revision 1.2  2006/12/06 22:35:20  dechavez
 * renamed all MSGQ.. items OLDMSGQ...
 *
 * Revision 1.1  2006/05/17 18:19:05  dechavez
 * "initial" release (extracted from libmsgq)
 *
 */
