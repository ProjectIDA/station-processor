#pragma ident "$Id: ttyio.h,v 1.7 2006/05/17 23:21:24 dechavez Exp $"
/*======================================================================
 *
 * Defines, templates, and prototypes TTY I/O library
 *
 *====================================================================*/
#ifndef ttyio_include_defined
#define ttyio_include_defined

#ifdef __cplusplus
extern "C" {
#endif

/* platform specific stuff */

#include "platform.h"
#include "logio.h"

/* Constants and enums */

#define TTYIO_PARITY_BAD   -1
#define TTYIO_PARITY_NONE   0
#define TTYIO_PARITY_ODD    1
#define TTYIO_PARITY_EVEN   2
#define TTYIO_PARITY_MARK   3
#define TTYIO_PARITY_SPACE  4

#define TTYIO_FLOW_BAD  -1
#define TTYIO_FLOW_NONE  0
#define TTYIO_FLOW_SOFT  1
#define TTYIO_FLOW_HARD  2

#define TTYIO_MINTO      1

/* Connection attributes */

typedef struct {
    BOOL   at_lock;   /* exclusive use lock */
    INT32  at_to;     /* i/o timeout interval (msec) */
    INT32  at_ibaud;  /* input baud rate */
    INT32  at_obaud;  /* output baud rate */
    int    at_parity; /* parity setting */
    int    at_flow;   /* flow control */
    int    at_sbits;  /* number of stop bits */
    INT32  at_pipe;   /* if non-zero, read pipe len */
} TTYIO_ATTR;

/* port settings */

#ifdef unix
typedef struct {
    struct termios old;
    struct termios new;
} TTYIO_TERMIOS;
#endif

#ifdef WIN32
typedef struct {
    DCB oldDcb;
    DCB newDcb;
} TTYIO_DCB;
#endif /* WIN32 */

/* xmit and receive stats */

typedef struct {
    UINT32 error; /* last read or write status */
    time_t tstamp; /* time stamp of last data in/out */
    UINT32 nbyte;  /* number of bytes in/out */
} TTYIO_STATS;

/* Connection handle */

typedef struct {
    MUTEX         mutex;              /* for protection        */
    LOGIO         *lp;                /* for logging           */
    char          name[MAXPATHLEN+1]; /* device name           */
    TTYIO_ATTR    attr;               /* connection attributes */
    TTYIO_STATS   rcv;                /* receive stats         */
    TTYIO_STATS   snd;                /* transmit stats        */
#ifdef unix
    int           fd;                 /* file handle           */
    TTYIO_TERMIOS termio;             /* port settings         */
    struct {
        BOOL active;                  /* TRUE if using pipe    */
        BOOL failed;                  /* TRUE if problems      */
        int in;                       /* pipe input descriptor */
        int out;                      /* pipe output     "     */
        THREAD tid;                   /* thread feeding pipe   */
        SEMAPHORE semaphore;          /* for shutdown sync     */
        UINT8 *buf;                   /* for reads             */
    } pipe;
#endif
#ifdef WINNT
    HANDLE        fd;                 /* file handle           */
    TTYIO_DCB     dcb;                /* port settings         */
    COMMTIMEOUTS  cto;                /*   "     "             */
    struct {
        BOOL active;                  /* TRUE if using pipe    */
        BOOL failed;                  /* TRUE if problems      */
        HANDLE in;                    /* pipe input descriptor */
        HANDLE out;                   /* pipe output     "     */
        THREAD tid;                   /* thread feeding pipe   */
        int priority;                 /* read thread priority  */
        SEMAPHORE semaphore;          /* for shutdown sync     */
        UINT8 *buf;                   /* for reads             */
    } pipe;
#endif
} TTYIO;

/* various defaults */

#ifdef INCLUDE_TTYIO_DEFAULT_ATTR

#ifndef TTYIO_DEFLOCK
#define TTYIO_DEFLOCK  TRUE /* set exclusive use */
#endif

#ifndef TTYIO_DEFTO
#define TTYIO_DEFTO TTYIO_MINTO 
#endif

#ifndef TTYIO_DEFIBAUD
#define TTYIO_DEFIBAUD 38400
#endif

#ifndef TTYIO_DEFOBAUD
#define TTYIO_DEFOBAUD TTYIO_DEFIBAUD
#endif

#ifndef TTYIO_DEFPARITY
#define TTYIO_DEFPARITY TTYIO_PARITY_NONE
#endif

#ifndef TTYIO_DEFFLOW
#define TTYIO_DEFFLOW TTYIO_FLOW_NONE
#endif

#ifndef TTYIO_DEFSBITS
#define TTYIO_DEFSBITS 2
#endif

static TTYIO_ATTR TTYIO_DEFAULT_ATTR = {
    TTYIO_DEFLOCK,
    TTYIO_DEFTO,
    TTYIO_DEFIBAUD,
    TTYIO_DEFOBAUD,
    TTYIO_DEFPARITY,
    TTYIO_DEFFLOW,
    TTYIO_DEFSBITS
};

#endif /* INCLUDE_TTYIO_DEF_ATTR */

/* misc.c */
BOOL ttyioStartReadThread(TTYIO *tp);

/* string.c */
int ttyioStringToFlow(char *target);
int ttyioStringToParity(char *target);
char *ttyioFlowToString(int target);
char *ttyioParityToString(int target);

/* ttyio.c */
THREAD_FUNC ttyioReadThread(void *argptr);
INT32 ttyioGetOutputSpeed(TTYIO *tp);
INT32 ttyioGetInputSpeed(TTYIO *tp);
INT32 ttyioGetTimeout(TTYIO *tp);
VOID ttyioSetTimeout(TTYIO *tp, INT32 to);
TTYIO *ttyioOpen( char *port, BOOL lock, INT32 ibaud, INT32 obaud, int parity, int flow, int sbits, INT32 to, INT32 pipe, LOGIO *lp);
TTYIO *ttyioClose(TTYIO *tp);
INT32 ttyioRead(TTYIO *tp, UINT8 *buf, INT32 want);
INT32 ttyioWrite(TTYIO *tp, UINT8 *buf, INT32 len);
VOID ttyioSetLog(TTYIO *tp, LOGIO *lp);

/* version.c */
VERSION *ttyioVersion(VOID);
char *ttyioVersionString(VOID);

#ifdef __cplusplus
}
#endif

#endif

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
 * $Log: ttyio.h,v $
 * Revision 1.7  2006/05/17 23:21:24  dechavez
 * added copyright notice
 *
 * Revision 1.6  2005/10/06 21:56:42  dechavez
 * updated ttyioOpen prototype to include LOGIO
 *
 * Revision 1.5  2005/06/10 15:19:38  dechavez
 * removed ibuf and obuf from handle, added new prototypes
 *
 * Revision 1.4  2005/05/26 23:17:35  dechavez
 * new Unix/Windows compatible version
 *
 */
