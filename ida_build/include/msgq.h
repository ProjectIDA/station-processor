#pragma ident "$Id: msgq.h,v 1.9 2006/05/17 23:21:24 dechavez Exp $"
/*======================================================================
 *
 *  Message Queue library header file
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef msgq_h_included
#define msgq_h_included

/* platform specific stuff */

#include "platform.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for message queue I/O */
 
#define MSGQ_WAIT   ((UINT16) 0)
#define MSGQ_NOWAIT ((UINT16) 1)

/* typedefs */

typedef struct msgq_msg {
    UINT8  *data;
    UINT32 len;
    UINT32 maxlen;
    struct msgq_msg *next;
} MSGQ_MSG;

typedef struct {
    MSGQ_MSG  head;  /* the messages        */
    MUTEX     mp;    /* for protection      */
    SEMAPHORE sp;    /* for synchronization */
    UINT32    nfree; /* for optimization    */
    UINT32    lowat; /* for optimization    */
    UINT32    hiwat; /* for optimization    */
    LOGIO    *lp;    /* for internal logs   */
} MSGQ;

typedef struct {
    MSGQ heap;  /* empty messages */
    MSGQ full;  /* full messages */
} MSGQ_BUF;

/* function prototypes */

/* msgq.c */
BOOL msgqInit(MSGQ *queue, UINT32 nelem, UINT32 maxelem, UINT32 maxlen);
VOID msgqSetLog(MSGQ *queue, LOGIO *lp);
MSGQ_MSG *msgqGet(MSGQ *queue, UINT16 flag);
VOID msgqPut(MSGQ *queue, MSGQ_MSG *newm);
UINT32 msgqNfree(MSGQ *queue);
UINT32 msgqLowat(MSGQ *queue);
UINT32 msgqHiwat(MSGQ *queue);
VOID msgqFlush(MSGQ *full, MSGQ *empty);
BOOL msgqCheck(MSGQ_MSG *msg);
BOOL msgqInitBuf(MSGQ_BUF *buf, UINT32 nelem, UINT32 size);

/* version.c */
VERSION *msgqVersion(VOID);
char *msgqVersionString(VOID);

#ifdef __cplusplus
}
#endif

#endif /* msgq_h_included */

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
 * $Log: msgq.h,v $
 * Revision 1.9  2006/05/17 23:21:24  dechavez
 * added copyright notice
 *
 * Revision 1.8  2006/05/04 20:34:40  dechavez
 * added MSGQ_BUF data type
 *
 * Revision 1.7  2004/06/25 18:34:57  dechavez
 * C++ compatibility
 *
 * Revision 1.6  2003/12/10 05:35:26  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.5  2003/06/09 23:39:10  dechavez
 * added prototypes for portable version of the API
 *
 * Revision 1.4  2001/05/20 15:57:44  dec
 * switch to platform.h MUTEX, added sanity fields to test for corruption
 *
 * Revision 1.3  2001/05/07 22:35:48  dec
 * replace stdtypes.h with platform.h
 *
 * Revision 1.2  2000/09/20 00:57:07  dec
 * bounds checking mods
 *
 * Revision 1.1  2000/07/24 19:12:23  dec
 * Initial release, extracted from ispd
 *
 */
