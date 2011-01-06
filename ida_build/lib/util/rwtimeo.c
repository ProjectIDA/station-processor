#pragma ident "$Id: rwtimeo.c,v 1.8 2005/05/27 00:15:26 dechavez Exp $"
/*======================================================================
 *
 *  Unbuffered read/write routines with timeout detection.
 *
 *  Usage: util_write(fd, buffer, n, timeout) timeout in seconds
 *          util_read(fd, buffer, n, timeout) timeout in seconds
 *
 *  Usage: utilWrite(fd, buffer, n, timeout) timeout in msec
 *          utilRead(fd, buffer, n, timeout) timeout in msec
 *
 *====================================================================*/
#include "util.h"

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif

#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC 1000
#endif

static void SetTimeout(struct timeval *timeout, INT32 to)
{
time_t secs;
suseconds_t usec;

    secs = (time_t) to / MSEC_PER_SEC;
    usec = (suseconds_t) (to - secs * MSEC_PER_SEC) * USEC_PER_MSEC;

    timeout->tv_sec  = secs;
    timeout->tv_usec = usec;
}

INT32 utilWrite(int fd, UINT8 *buffer, UINT32 n, INT32 to)
{
INT32 nrem, nw, width, nout;
UINT8 *ptr;
int  error;
fd_set writefds;
struct timeval timeout;

    width = getdtablesize();
    SetTimeout(&timeout, to);

/*  Write to fd until no more can be written  */

    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    
    nout = 0;
    nrem = n;
    ptr  = buffer;
    while (nrem > 0) {
        error = select(width, NULL, &writefds, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            return n - nrem;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                return -1;
            }
        } else {
            if ((nw = write(fd, ptr, nrem)) <= 0) {
                if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                    return -1;
                }
            }  else {
                nrem -= nw;
                ptr  += nw;
            }
        }
    }

    return n;
}

INT32 utilRead(int fd, UINT8 *buffer, INT32 n, INT32 to)
{
INT32 nrem, nr, width;
UINT8 *ptr;
int  error;
fd_set readfds;
struct timeval timeout;

    errno = 0;

    width = getdtablesize();
    SetTimeout(&timeout, to);

/*  Read from fd until desired number of bytes acquired  */

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    
    nrem = n;
    ptr  = buffer;
    while (nrem > 0) {
        error = select(width, &readfds, NULL, NULL, &timeout);
        if (error > 0) {
            nr = read(fd, ptr, nrem);
            if (nr > 0) {
                nrem -= nr;
                ptr += nr;
            } else if (nr == 0) {
                errno = ECONNRESET;
                return -3;
            } else if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                return -2;
            }
        } else if (error == 0) {
            errno = ETIMEDOUT;
            return n - nrem;
        } else if (errno != EINTR && errno != EAGAIN) {
            return -1;
        }
    }

    return n;
}

long util_write(int fd, void *buffer, long n, int to)
{
long nrem, nw, width, nout;
char *ptr;
int  error;
fd_set writefds;
struct timeval timeout;

#ifdef WIN32
    width = 512;
#else
    width = FD_SETSIZE;
#endif

    timeout.tv_sec = to;
    timeout.tv_usec = 0;

/*  Set fd to non-blocking  */

    /*if (util_noblock(fd) != 0) return -1;*/

/*  Write to fd until no more can be written  */

    FD_ZERO(&writefds);
    FD_SET(fd, &writefds);
    
    nout = 0;
    nrem = n;
    ptr  = buffer;
    while (nrem > 0) {
        error = select(width, NULL, &writefds, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            return n - nrem;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                return -1;
            }
        } else {
/**************** AAP ******************************/
#ifdef WIN32
            if ((nw = send(fd, ptr, nrem, 0)) < 0) {
#else
            if ((nw = write(fd, ptr, nrem)) < 0) {
#endif
/**************** END AAP ***************************/
                if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                    return -1;
                }
            }  else {
                nrem -= nw;
                ptr  += nw;
            }
        }
    }

    return n;
}

long util_read(int fd, void *buffer, long n, int to)
{
long nrem, nr, width;
char *ptr;
int  error;
fd_set readfds;
struct timeval timeout;
static char *fid = "util_read";

    util_log(3, "%s: trace 0, n=%d, to=%d", fid, n, to);
    errno = 0;

#ifdef WIN32
    width = 512;
#else
    width = FD_SETSIZE;
#endif
    timeout.tv_sec = to;
    timeout.tv_usec = 0;

/*  Read from fd until desired number of bytes acquired  */

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    
    nrem = n;
    ptr  = buffer;
    while (nrem > 0) {
        util_log(3, "%s: trace 1, nrem = %d", fid, nrem);
        error = select(width, &readfds, NULL, NULL, &timeout);
        util_log(3, "%s: trace 2, select returns %d", fid, error);
        if (error > 0) {
            util_log(3, "%s: trace 3", fid);
/**************** AAP ***************************/
#ifdef WIN32
            nr = recv(fd, ptr, nrem, 0);
#else
            nr = read(fd, ptr, nrem);
#endif
/****************END AAP**************************/
            util_log(3, "%s: trace 4, read returns %d", fid, nr);
            if (nr > 0) {
                nrem -= nr;
                ptr += nr;
            } else if (nr == 0) {
                errno = ECONNRESET;
                util_log(3, "%s: return -3", fid);
                return -3;
            } else if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                util_log(3, "%s: return -2", fid);
                return -2;
            }
        } else if (error == 0) {
            util_log(3, "%s: set errno = ETIMEDOUT, return %d", fid, n-nrem);
            errno = ETIMEDOUT;
            return n - nrem;
        } else if (errno != EINTR && errno != EAGAIN) {
            util_log(3, "%s: set errno = %d, return -1", fid, errno);
            return -1;
        }
    }

    util_log(3, "%s: return %d", fid, n);
    return n;
}

/* Revision History
 *
 * $Log: rwtimeo.c,v $
 * Revision 1.8  2005/05/27 00:15:26  dechavez
 * added utilRead() and utilWrite() (msec timeouts)
 *
 * Revision 1.7  2005/05/25 22:58:35  dechavez
 * cleared tabs
 *
 * Revision 1.6  2005/05/25 22:41:46  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.5  2004/12/21 00:30:37  dechavez
 * aap win32 mods
 *
 * Revision 1.4  2004/07/26 23:18:22  dechavez
 * removed uneccesary includes, hard-code width instead of flakey (or non-existant getdtablesize())
 *
 * Revision 1.3  2000/10/06 19:56:34  dec
 * added debugging trace statements to util_read
 *
 * Revision 1.2  2000/09/22 21:07:42  nobody
 * cleaned up util_read
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
