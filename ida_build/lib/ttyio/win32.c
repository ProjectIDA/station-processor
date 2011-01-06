#pragma ident "$Id: win32.c,v 1.6 2006/05/17 23:23:10 dechavez Exp $"
/*======================================================================
 *
 *  WIN32 implementation of the ttyio library
 *
 *====================================================================*/
#define INCLUDE_TTYIO_DEFAULT_ATTR
#include "ttyio.h"
#include "util.h"

/* Read into a pipe */

static BOOL InitPipe(TTYIO *tp)
{
static char *fid = "ttyio:InitPipe";

    SEM_INIT(&tp->pipe.semaphore, 0, 1);
    tp->pipe.buf = (UINT8 *) malloc(tp->attr.at_pipe);
    if (tp->pipe.buf == NULL) {
        logioMsg(tp->lp, LOG_INFO, "%s: malloc: %s", fid, strerror(errno));
        return FALSE;
    }

    tp->pipe.priority = THREAD_PRIORITY_HIGHEST;

    if (CreatePipe(&tp->pipe.out, &tp->pipe.in, NULL, tp->attr.at_pipe * 2)) {
        tp->pipe.active = TRUE;
        return TRUE;
    } else {
        return FALSE;
    }
}

THREAD_FUNC ttyioReadThread(void *argptr)
{
TTYIO *tp;
COMSTAT ComStat;
DWORD dwErrFlags;
DWORD rcvd=0, sent=0;
BOOL done = FALSE;
static char *fid = "ttyioReadThread";

    tp = (TTYIO *) argptr;
    logioMsg(tp->lp, LOG_DEBUG, "%s: thread started id=%d", fid, THREAD_SELF());
    SEM_POST(&tp->pipe.semaphore);

    while (1) {
        ClearCommError(tp->fd, &dwErrFlags, &ComStat);
        if (!ReadFile(tp->fd, (char *) tp->pipe.buf, (DWORD) tp->attr.at_pipe, &rcvd, 0)) {
            logioMsg(tp->lp, LOG_INFO, "%s: ReadFile: %s", fid, strerror(errno));
            MUTEX_LOCK(&tp->mutex);
                done = tp->pipe.failed = TRUE;
            MUTEX_UNLOCK(&tp->mutex);
        } else if (rcvd > 0) {
            if (!WriteFile(tp->pipe.in, (char *) tp->pipe.buf, rcvd, &sent, 0)) {
                logioMsg(tp->lp, LOG_INFO, "%s: WriteFile: %s", fid, strerror(errno));
                MUTEX_LOCK(&tp->mutex);
                    done = tp->pipe.failed = TRUE;
                MUTEX_UNLOCK(&tp->mutex);
            }
        } else {
            sent = 0;
            Sleep(25);
        }

        if (done) {
            MUTEX_LOCK(&tp->mutex);
                tp->pipe.active = FALSE;
            MUTEX_UNLOCK(&tp->mutex);
            SEM_POST(&tp->pipe.semaphore);
            THREAD_EXIT(0);
        }
    }
}

/* Recover I/O speeds */

INT32 ttyioGetOutputSpeed(TTYIO *tp)
{
INT32 retval;

    MUTEX_LOCK(&tp->mutex);
        retval = tp->attr.at_obaud;
    MUTEX_UNLOCK(&tp->mutex);

    return retval;
}

INT32 ttyioGetInputSpeed(TTYIO *tp)
{
INT32 retval;

    MUTEX_LOCK(&tp->mutex);
        retval = tp->attr.at_ibaud;
    MUTEX_UNLOCK(&tp->mutex);

    return retval;
}

/* Timeout get/set routines */

INT32 ttyioGetTimeout(TTYIO *tp)
{
INT32 retval;

    MUTEX_LOCK(&tp->mutex);
        retval = tp->attr.at_to;
    MUTEX_UNLOCK(&tp->mutex);

    return retval;
}

VOID ttyioSetTimeout(TTYIO *tp, INT32 to)
{
    MUTEX_LOCK(&tp->mutex); 
        tp->attr.at_to = to;
        if (tp->attr.at_pipe) {
            tp->cto.ReadIntervalTimeout         = MAXDWORD;
            tp->cto.ReadTotalTimeoutMultiplier  = MAXDWORD;
            tp->cto.ReadTotalTimeoutConstant    = MAXDWORD;
            tp->cto.WriteTotalTimeoutMultiplier = 0;
            tp->cto.WriteTotalTimeoutConstant   = 5000;
        } else {
            tp->cto.ReadIntervalTimeout         = tp->attr.at_to;
            tp->cto.ReadTotalTimeoutMultiplier  = 0;
            tp->cto.ReadTotalTimeoutConstant    = 0;
            tp->cto.WriteTotalTimeoutMultiplier = 0;
            tp->cto.WriteTotalTimeoutConstant   = 5000;
        }
        SetCommTimeouts(tp->fd, &tp->cto);
    MUTEX_UNLOCK(&tp->mutex);
}

/* Open device */

TTYIO *ttyioOpen(
    char *port,
    BOOL lock,
    INT32 ibaud,
    INT32 obaud,
    int parity,
    int flow,
    int sbits,
    INT32 to,
    INT32 pipe,
    LOGIO *lp
){
TTYIO *tp;
static char *fid = "ttyioOpen";

    if (port == NULL || strlen(port) > MAXPATHLEN) {
        logioMsg(lp, LOG_ERR, "%s: bad port arg", fid);
        errno = EINVAL;
        return NULL;
    }

    if (ibaud != obaud) {
        logioMsg(lp, LOG_ERR, "%s: ibaud(%d) != obaud(%d): %s", fid, ibaud, obaud);
        errno = EINVAL;
        return NULL;
    }

    if ((tp = (TTYIO *) malloc(sizeof(TTYIO))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    MUTEX_INIT(&tp->mutex);
    tp->lp = lp;

    strcpy(tp->name, port);
    tp->attr = TTYIO_DEFAULT_ATTR;
    tp->attr.at_lock   = lock;
    tp->attr.at_ibaud  = ibaud;
    tp->attr.at_obaud  = obaud;
    tp->attr.at_flow   = flow;
    tp->attr.at_parity = parity;
    tp->attr.at_sbits  = sbits;
    tp->attr.at_to     = to;
    tp->attr.at_pipe   = pipe > 0 ? pipe : 0;

/* Open device and save current settings */

    if ((tp->fd = CreateFile(tp->name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    )) == INVALID_HANDLE_VALUE) {
        logioMsg(lp, LOG_ERR, "%s: CreateFile: %s", fid, strerror(errno));
        return ttyioClose(tp);
    }
    tp->dcb.oldDcb.DCBlength = tp->dcb.oldDcb.DCBlength = sizeof(DCB);
    GetCommState(tp->fd, &tp->dcb.oldDcb);
    GetCommState(tp->fd, &tp->dcb.newDcb);

/* Reconfigure port to desired settings */

    /* Raw 8-bit i/o */

    tp->dcb.newDcb.fBinary   = TRUE;
    tp->dcb.newDcb.ByteSize  = 8;

    /* stop bits */

    tp->dcb.newDcb.StopBits  = (tp->attr.at_sbits == 1) ? ONESTOPBIT : TWOSTOPBITS;

    /* baud rate */

    tp->dcb.newDcb.BaudRate  = tp->attr.at_ibaud;

    /* flow control */

    switch (tp->attr.at_flow) {
      case TTYIO_FLOW_HARD:
        tp->dcb.newDcb.fInX            = FALSE;
        tp->dcb.newDcb.fOutX           = FALSE;
        tp->dcb.newDcb.fOutxCtsFlow    = TRUE;
        tp->dcb.newDcb.fRtsControl     = RTS_CONTROL_HANDSHAKE;
        tp->dcb.newDcb.fOutxDsrFlow    = TRUE;
        tp->dcb.newDcb.fDtrControl     = DTR_CONTROL_HANDSHAKE;
        break;
      case TTYIO_FLOW_SOFT:
        tp->dcb.newDcb.fInX            = TRUE;
        tp->dcb.newDcb.fOutX           = TRUE;
        tp->dcb.newDcb.XonLim          = 50;
        tp->dcb.newDcb.XoffLim         = 200;
        tp->dcb.newDcb.XonChar         = 17;
        tp->dcb.newDcb.XoffChar        = 19;
        tp->dcb.newDcb.fOutxDsrFlow    = FALSE;
        tp->dcb.newDcb.fDtrControl     = DTR_CONTROL_DISABLE;
        tp->dcb.newDcb.fOutxCtsFlow    = FALSE;
        tp->dcb.newDcb.fRtsControl     = RTS_CONTROL_DISABLE;
        break;
      case TTYIO_FLOW_NONE:
        tp->dcb.newDcb.fInX            = FALSE;
        tp->dcb.newDcb.fOutX           = FALSE;
        tp->dcb.newDcb.fOutxDsrFlow    = FALSE;
        tp->dcb.newDcb.fDtrControl     = DTR_CONTROL_DISABLE;
        tp->dcb.newDcb.fOutxCtsFlow    = FALSE;
        tp->dcb.newDcb.fRtsControl     = RTS_CONTROL_DISABLE;
        break;
      default:
        errno = EINVAL;
        logioMsg(lp, LOG_ERR, "%s: unsupported at_flow=%d", fid, tp->attr.at_flow);
        return ttyioClose(tp);
    }

    /* parity */

    switch (tp->attr.at_parity) {
      case TTYIO_PARITY_NONE:
        tp->dcb.newDcb.fParity = FALSE;
        tp->dcb.newDcb.Parity  = NOPARITY;
        break;
      case TTYIO_PARITY_ODD:
        tp->dcb.newDcb.fParity = TRUE;
        tp->dcb.newDcb.Parity  = ODDPARITY;
        break;
      case TTYIO_PARITY_EVEN:
        tp->dcb.newDcb.fParity = TRUE;
        tp->dcb.newDcb.Parity  = EVENPARITY;
        break;
      case TTYIO_PARITY_MARK:
        tp->dcb.newDcb.fParity = TRUE;
        tp->dcb.newDcb.Parity  = MARKPARITY;
        break;
      case TTYIO_PARITY_SPACE:
        tp->dcb.newDcb.fParity = TRUE;
        tp->dcb.newDcb.Parity  = SPACEPARITY;
        break;
      default:
        errno = EINVAL;
        logioMsg(lp, LOG_ERR, "%s: unsupported at_parity=%d", fid, tp->attr.at_parity);
        return ttyioClose(tp);
    }

    /* load the attributes */

    if (!SetCommState(tp->fd, &tp->dcb.newDcb)) {
        logioMsg(lp, LOG_ERR, "%s: SetCommState: %s", fid, strerror(errno));
        return ttyioClose(tp);
    }

    /* Set timeout interval */

    ttyioSetTimeout(tp, tp->attr.at_to);

    /* Set internal i/o buffer lengths */

    SetupComm(tp->fd, 32768, 32768);

    /* EscapeCommFunction(tp->fd, CLRDTR); */

/* Create pipe and fire up read thread if non-zero pipe length specified */

    tp->pipe.active = tp->pipe.failed = FALSE;
    if (tp->attr.at_pipe > 0 && (!InitPipe(tp) || !ttyioStartReadThread(tp))) {
        logioMsg(lp, LOG_ERR, "%s: failed to create pipe and start read thread", fid);
        return ttyioClose(tp);
    }

/* Return handle */

    return tp;
}

/* close device */

TTYIO *ttyioClose(TTYIO *tp)
{
    if (tp == NULL) return NULL;
    MUTEX_LOCK(&tp->mutex);
    if(tp->fd != INVALID_HANDLE_VALUE) {
        PurgeComm(tp->fd,
            PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR
        );
        SetCommState(tp->fd, &tp->dcb.oldDcb);
        CloseHandle(tp->fd);
    }
    free(tp);
    return NULL;
}

/* read/write */

INT32 ttyioRead(TTYIO *tp, UINT8 *buf, INT32 want)
{
COMSTAT ComStat;
DWORD dwErrFlags;
DWORD rcvd;

    if (tp == NULL || tp->fd == INVALID_HANDLE_VALUE) {
        errno = EINVAL;
        return -1;
    }

    MUTEX_LOCK(&tp->mutex);
    if (tp->pipe.active) {
        MUTEX_UNLOCK(&tp->mutex);
        if (!ReadFile(tp->pipe.out, (char *) buf, (DWORD) want, &rcvd, 0)) {
            return -1;
        } else {
            return (INT32) rcvd;
        }
    } else if (tp->pipe.failed) {
        MUTEX_UNLOCK(&tp->mutex);
        return -1;
    } else {
        MUTEX_UNLOCK(&tp->mutex);
        ClearCommError(tp->fd, &dwErrFlags, &ComStat);
        if (!ReadFile(tp->fd, (char *) buf, (DWORD) want, &rcvd, 0)) {
            return -1;
        } else {
            return (INT32) rcvd;
        }
    }
}

INT32 ttyioWrite(TTYIO *tp, UINT8 *buf, INT32 len)
{
INT32 i;
DWORD sent;

    for (i = 0; i < len; i++) {
        if (!WriteFile(tp->fd, (char *) &buf[i], 1, &sent, 0)) {
            return -1;
        }
    }
    return len;
}

VOID ttyioSetLog(TTYIO *tp, LOGIO *lp)
{
    MUTEX_LOCK(&tp->mutex);
        tp->lp = lp;
    MUTEX_UNLOCK(&tp->mutex);
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
 * $Log: win32.c,v $
 * Revision 1.6  2006/05/17 23:23:10  dechavez
 * added copyright notice
 *
 * Revision 1.5  2005/11/03 23:22:15  dechavez
 * fixed ttyio:InitPipe fid declaration
 *
 * Revision 1.4  2005/10/11 17:06:18  dechavez
 * added ttyioSetLog (10-11-2005-aap)
 *
 * Revision 1.3  2005/10/06 22:07:28  dechavez
 * added LOGIO argument to ttyioOpen(), replaced obsolete utilLog calls with logioMsg equivalents
 *
 * Revision 1.2  2005/06/10 15:25:43  dechavez
 * removed ibuf and obuf from handle
 *
 * Revision 1.1  2005/05/26 23:18:05  dechavez
 * initial release of new Unix/Windows compatible version
 *
 */
