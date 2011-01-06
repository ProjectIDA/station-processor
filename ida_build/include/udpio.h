#pragma ident "$Id: udpio.h,v 1.4 2006/05/17 23:21:24 dechavez Exp $"
/*======================================================================
 *
 *  Include file for udpio library routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2003, 2006 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef udpio_h_included
#define udpio_h_included

#include "platform.h"
#include "logio.h"

/* I/O statistics */

typedef struct {
    MUTEX mutex;        /* for protection */
    struct {
        UINT64 bytes;   /* total number of bytes */
        UINT64 pkts;    /* total number of packets */
    } total;
    struct {
        int max;        /* largest packet */
        int min;        /* smallest packet */
    } len;
    UINT64 tstamp;      /* time of last I/O */
    struct {
        UINT64 count;   /* total number of errors */
        UINT64 tstamp;  /* time of last error */
    } error;
} UDPIO_STATS;

/* handle */

typedef struct {
    SOCKET sd;                    /* datagram socket */
    UINT64 start;                 /* initialization time */
    struct {
        UDPIO_STATS xmit;         /* transmit statistics */
        UDPIO_STATS recv;         /* receive statistics */
    } stats;
    struct {
        struct sockaddr_in addr;  /* recv peer address */
        char ident[MAXPATHLEN+1]; /* peer ident string */
    } peer;
    struct timeval to;            /* read timeout */
    LOGIO *lp;                    /* for logging */
} UDPIO;

/* function prototypes */

/* udpio.c */

void udpioGetStats(UDPIO *up, UDPIO_STATS *xmit, UDPIO_STATS *recv);
BOOL udpioInit(UDPIO *up, int port, int to, LOGIO *lp);
int udpioRecv(UDPIO *up, char *buf, int buflen);
int udpioSend(UDPIO *up, UINT32 ip, int port, char *buf, int len);

/* version.c */
char *udpioVersionString(void);
VERSION *udpioVersion(void);

#endif /* udpio_h_included */

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
 * $Log: udpio.h,v $
 * Revision 1.4  2006/05/17 23:21:24  dechavez
 * added copyright notice
 *
 * Revision 1.3  2006/05/06 02:13:46  dechavez
 * added prototypes for version funcs
 *
 * Revision 1.2  2006/05/04 20:33:27  dechavez
 * added stats, updated prototypes
 *
 * Revision 1.1  2003/05/12 23:19:30  dechavez
 * created
 *
 */
