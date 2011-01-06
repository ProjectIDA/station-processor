#pragma ident "$Id: unix.c,v 1.4 2006/05/17 23:23:10 dechavez Exp $"
/*======================================================================
 *
 *  Unix (Solaris) implementation of the ttyio library
 *
 *====================================================================*/
#define INCLUDE_TTYIO_DEFAULT_ATTR
#include "ttyio.h"
#include "util.h"

/* Read into a pipe */

static BOOL InitPipe(TTYIO *tp)
{
int fildes[2], flags;

    if (SEM_INIT(&tp->pipe.semaphore, 0, 1) != 0) return FALSE;
    tp->pipe.buf = (UINT8 *) malloc(tp->attr.at_pipe * 2);
    if (tp->pipe.buf == NULL) return FALSE;

    if (pipe(fildes) == 0) {
        tp->pipe.active = TRUE;
        tp->pipe.in = fildes[0];
        tp->pipe.out = fildes[1];
        return TRUE;
    } else {
        return FALSE;
    }
}

THREAD_FUNC ttyioReadThread(void *argptr)
{
TTYIO *tp;
ssize_t got, wrote;
BOOL done = FALSE;
static char *fid = "ttyioReadThread";

    tp = (TTYIO *) argptr;
    SEM_POST(&tp->pipe.semaphore);

    while (1) {
        got = read(tp->fd, tp->pipe.buf, tp->attr.at_pipe);
        if (got > 0) {
            wrote =write(tp->pipe.in, tp->pipe.buf, got);
            if (wrote != got) {
                MUTEX_LOCK(&tp->mutex);
                    done = TRUE;
                MUTEX_UNLOCK(&tp->mutex);
                logioMsg(tp->lp, LOG_INFO, "%s: write failed: %s", fid, strerror(errno));
            }
        } else {
            errno = EIO;
            logioMsg(tp->lp, LOG_INFO, "%s: read failed: %s", fid, strerror(errno));
            MUTEX_LOCK(&tp->mutex);
                done = TRUE;
            MUTEX_UNLOCK(&tp->mutex);
        }

        if (done) {
            MUTEX_LOCK(&tp->mutex);
                tp->pipe.active = tp->pipe.failed = FALSE;
            MUTEX_UNLOCK(&tp->mutex);
            SEM_POST(&tp->pipe.semaphore);
            logioMsg(tp->lp, LOG_INFO, "%s: thread exits", fid);
            THREAD_EXIT(0);
        }
    }
}

/* Set exclusive use */

static BOOL SetExclusiveUse(TTYIO *tp)
{
#ifdef HAVE_TIOCEXCL
    return (ioctl(tp->fd, TIOCEXCL, NULL) < 0) ? FALSE : TRUE;
#else
    return TRUE;
#endif
}

/* Covert from integer speed to symbolic value */

static speed_t EncodeBaud(INT32 IntergerSpeed)
{
    switch (IntergerSpeed) {
#ifdef B0
      case      0: return B0;
#endif

#ifdef B50
      case     50: return B50;
#endif

#ifdef B75
      case     75: return B75;
#endif

#ifdef B110
      case    110: return B110;
#endif

#ifdef B134
      case    134: return B134;
#endif

#ifdef B150
      case    150: return B150;
#endif

#ifdef B200
      case    200: return B200;
#endif

#ifdef B300
      case    300: return B300;
#endif

#ifdef B600
      case    600: return B600;
#endif

#ifdef B1200
      case   1200: return B1200;
#endif

#ifdef B2400
      case   2400: return B2400;
#endif

#ifdef B4800
      case   4800: return B4800;
#endif

#ifdef B9600
      case   9600: return B9600;
#endif

#ifdef B19200
      case  19200: return B19200;
#endif

#ifdef B38400
      case  38400: return B38400;
#endif

#ifdef B57600
      case  57600: return B57600;
#endif

#ifdef B76800
      case  76800: return B76800;
#endif

#ifdef B115200
      case 115200: return B115200;
#endif

#ifdef B153600
      case 153600: return B153600;
#endif

#ifdef B230400
      case 230400: return B230400;
#endif

#ifdef B307200
      case 307200: return B307200;
#endif

#ifdef B460800
      case 460800: return B460800;
#endif
      default: 
        errno = EINVAL;
        return -1;
    }
}

/* Convert from symbolic speed to integer value */

static INT32 DecodeBaud(speed_t SymbolicSpeed)
{
    switch (SymbolicSpeed) {
#ifdef B0
      case      B0: return      0;
#endif

#ifdef B50
      case     B50: return     50;
#endif

#ifdef B75
      case     B75: return     75;
#endif

#ifdef B110
      case    B110: return    110;
#endif

#ifdef B134
      case    B134: return    134;
#endif

#ifdef B150
      case    B150: return    150;
#endif

#ifdef B200
      case    B200: return    200;
#endif

#ifdef B300
      case    B300: return    300;
#endif

#ifdef B600
      case    B600: return    600;
#endif

#ifdef B1200
      case   B1200: return   1200;
#endif

#ifdef B2400
      case   B2400: return   2400;
#endif

#ifdef B4800
      case   B4800: return   4800;
#endif

#ifdef B9600
      case   B9600: return   9600;
#endif

#ifdef B19200
      case  B19200: return  19200;
#endif

#ifdef B38400
      case  B38400: return  38400;
#endif

#ifdef B57600
      case  B57600: return  56700;
#endif

#ifdef B76800
      case  B76800: return  76800;
#endif

#ifdef B115200
      case B115200: return 115200;
#endif

#ifdef B153600
      case B153600: return 153600;
#endif

#ifdef B230400
      case B230400: return 230400;
#endif

#ifdef B307200
      case B307200: return 307200;
#endif

#ifdef B460800
      case B460800: return 460800;
#endif

      default: 
        errno = EINVAL;
        return -1;
    }
}

/* Recover I/O speeds */

INT32 ttyioGetOutputSpeed(TTYIO *tp)
{
INT32 retval;

    MUTEX_LOCK(&tp->mutex);
        retval = DecodeBaud(cfgetospeed(&tp->termio.new));
    MUTEX_UNLOCK(&tp->mutex);

    return retval;
}

INT32 ttyioGetInputSpeed(TTYIO *tp)
{
INT32 retval;

    MUTEX_LOCK(&tp->mutex);
        retval = DecodeBaud(cfgetispeed(&tp->termio.new));
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
    MUTEX_UNLOCK(&tp->mutex);
}

VOID ttyioSetLog(TTYIO *tp, LOGIO *lp)
{
    MUTEX_LOCK(&tp->mutex);
        tp->lp = lp;
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
int OpenFlags = 0;
speed_t speed;
TTYIO *tp;
static char *fid = "ttyioOpen";

    if (port == NULL || strlen(port) > MAXPATHLEN) {
        logioMsg(lp, LOG_ERR, "%s: bad port arg", fid);
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

    OpenFlags = O_RDWR | O_NOCTTY;
    if (!tp->attr.at_pipe) OpenFlags |= O_NONBLOCK;
    if ((tp->fd = open(port, OpenFlags)) < 0) return ttyioClose(tp);

    if (tcgetattr(tp->fd, &tp->termio.old) < 0) return ttyioClose(tp);

/* Reconfigure port to desired settings */

    /* exclusive use */

    if (tp->attr.at_lock && !SetExclusiveUse(tp)) return ttyioClose(tp);

    memset(&tp->termio.new, 0, sizeof(struct termios));

    /* Raw 8-bit i/o */

    if (!tp->attr.at_pipe) {
        tp->termio.new.c_cc[VMIN]  = 0;
        tp->termio.new.c_cc[VTIME] = 0;
    } else {
        tp->termio.new.c_cc[VMIN]  = 1;
        tp->termio.new.c_cc[VTIME] = 0;
    }
    tp->termio.new.c_oflag     = 0;
    tp->termio.new.c_lflag     = 0;
    tp->termio.new.c_iflag     = IGNBRK | IGNPAR;
    tp->termio.new.c_cflag     = CS8 | CREAD;

    /* stop bits */

    if (tp->attr.at_sbits == 2) tp->termio.new.c_cflag |= CSTOPB;

    /* baud rate */

    if ((speed = EncodeBaud(tp->attr.at_ibaud)) < 0) {
        errno = EINVAL;
        return ttyioClose(tp);
    } else if (cfsetispeed(&tp->termio.new, speed) != 0) {
        return ttyioClose(tp);
    } else if ((speed = EncodeBaud(tp->attr.at_obaud)) < 0) {
        errno = EINVAL;
        return ttyioClose(tp);
    } else if (cfsetospeed(&tp->termio.new, speed) != 0) {
        return ttyioClose(tp);
    }

    /* flow control */

    switch (tp->attr.at_flow) {
      case TTYIO_FLOW_HARD:
#if defined (SOLARIS)
        tp->termio.new.c_cflag |= CRTSCTS | CRTSXOFF;
#elif defined (BSDOS)
        tp->termio.new.c_cflag |= CRTSCTS | CRTSXOFF;
#elif defined (SCO_SV)
        tp->termio.new.c_cflag |= CRTSCTS | CRTSXOFF;
#else
        tp->termio.new.c_cflag |= CRTSCTS;
#endif
        break;
      case TTYIO_FLOW_SOFT:
        tp->termio.new.c_iflag |= IXOFF | (IXON | IXANY) | CLOCAL;
        break;
      case TTYIO_FLOW_NONE:
        break;
      default:
        errno = EINVAL;
        return ttyioClose(tp);
    }

    /* parity */

    switch (tp->attr.at_parity) {
      case TTYIO_PARITY_ODD:
        tp->termio.new.c_iflag |= INPCK;
        tp->termio.new.c_cflag |= PARENB | PARODD;
        break;
      case TTYIO_PARITY_EVEN:
        tp->termio.new.c_iflag |= INPCK;
        tp->termio.new.c_cflag |= PARENB;
        break;
      case TTYIO_PARITY_NONE:
        break;
      default:
        errno = EINVAL;
        return ttyioClose(tp);
    }

    /* load the attributes */

    if (tcsetattr(tp->fd, TCSANOW, &tp->termio.new) < 0) return ttyioClose(tp);

/* Create pipe and fire up read thread if non-zero pipe length specified */

    tp->pipe.active = tp->pipe.failed = FALSE;
    if (
        tp->attr.at_pipe > 0 &&
        (!InitPipe(tp) || !ttyioStartReadThread(tp))
    ) return ttyioClose(tp);

/* Return handle */

    return tp;
}

/* close device */

TTYIO *ttyioClose(TTYIO *tp)
{
    if (tp == NULL) return NULL;
    MUTEX_LOCK(&tp->mutex);
    tcflush(tp->fd, TCIOFLUSH);
    tcsetattr(tp->fd, TCSANOW, &tp->termio.old);
    close(tp->fd);
    free(tp);
    return NULL;
}

/* read/write */

INT32 ttyioRead(TTYIO *tp, UINT8 *buf, INT32 want)
{
    MUTEX_LOCK(&tp->mutex);
    if (tp->pipe.active) {
        MUTEX_UNLOCK(&tp->mutex);
        return utilRead(tp->pipe.out, buf, want, ttyioGetTimeout(tp));
    } else if (tp->pipe.failed) {
        MUTEX_UNLOCK(&tp->mutex);
        return -1;
    } else {
        MUTEX_UNLOCK(&tp->mutex);
        return utilRead(tp->fd, buf, want, ttyioGetTimeout(tp));
    }
}

INT32 ttyioWrite(TTYIO *tp, UINT8 *buf, INT32 len)
{
    return utilWrite(tp->fd, buf, len, ttyioGetTimeout(tp));
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
 * $Log: unix.c,v $
 * Revision 1.4  2006/05/17 23:23:10  dechavez
 * added copyright notice
 *
 * Revision 1.3  2005/10/06 22:06:52  dechavez
 * added LOGIO argument to ttyioOpen()
 *
 * Revision 1.2  2005/06/10 15:26:28  dechavez
 * added ttyioSetLog, removed ibuf and obuf from handle
 *
 * Revision 1.1  2005/05/26 23:18:05  dechavez
 * initial release of new Unix/Windows compatible version
 *
 */
