#pragma ident "$Id: read.c,v 1.4 2008/02/03 21:12:15 dechavez Exp $"
/*======================================================================
 *
 *  Read a LISS mini-seed record, and decode it a bit.  We only support
 *  uncompressed 16 and 32 bit data plus Steim 1 and Steim 2
 *  compressed formats only.
 *
 *====================================================================*/
#include <math.h>
#include "liss.h"
#include "util.h"

static int read_to(LISS *liss)
{
size_t remain;
size_t got;
UINT8 *ptr, *buf;
char *ident;
LOGIO *lp;
int  sd, want, to, error, width;
fd_set readfds;
struct timeval timeout;
static char *fid = "read_to";

    sd = liss->sd;
    lp = liss->lp;
    buf = liss->buf;
    want = liss->blksiz;
    to = liss->to;
    ident = liss->server;

    if (want <= 0) {
        errno = EINVAL;
        logioMsg(lp, LOG_INFO, "%s: illegal blksiz = %d ", ident, want);
        return -1;
    }

    error           = 0;
    timeout.tv_sec  = to;
    timeout.tv_usec = 0;

/* Mask out our file descriptor as the only one to look at */

#ifndef WINNT
    width = getdtablesize(); /* This is ignored in Windows sockets...*/
#else
    width = 0;
#endif
    FD_ZERO(&readfds);
    FD_SET(sd, &readfds);
    
/*  Read from socket until desired number of bytes acquired  */

    remain = want;
    ptr    = buf;

    while (remain) {
        error = select(width, &readfds, NULL, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            logioMsg(lp, LOG_INFO, "%s timed out", ident);
            return -1;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                logioMsg(lp, LOG_ERR, "%s select: %s", ident, strerror(errno)
                );
                return -1;
            }
        } else {
#ifdef WINNT
            got = recv(sd, ptr, remain, 0);
            if (got == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAECONNRESET) {
                    got = 0;
                } else {
                    got = -1;
                }
            }
#else
            got = read(sd, (void *) ptr, (size_t) remain);
#endif
            if (got > 0) {
                remain -= got;
                ptr    += got;
            } else if (got == 0) {
                errno = ECONNRESET; /* actually it's EOF */
                logioMsg(lp, LOG_INFO, "%s read: %s", ident, strerror(errno));
                return -1;
#ifdef WINNT
            } else if (WSAGetLastError() != 0) {
                logioMsg(lp, LOG_INFO, "%s recv: %d", ident, WSAGetLastError());
                return -1;
            } else if (WSAGetLastError() == 0) {
                logioMsg(lp, LOG_INFO, "%s recv: %d (ignored)", ident, WSAGetLastError());
#endif
            } else if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                logioMsg(lp, LOG_INFO, "%s read: error %d", ident, errno);
                return -1;
            } else {
                logioMsg(lp, LOG_INFO, "%s read: error %d (ignored)", ident, errno);
            }
        }
    }

    return want;
}

BOOL lissRead(LISS *liss, LISS_PKT *pkt, UINT32 flags)
{
int nread;
static char *fid = "lissRead";

    if (liss->to > 0) {
        nread = read_to(liss);
        if (nread < 0) {
            return FALSE;
        } else if (nread != liss->blksiz) {
            return FALSE;
        }
    } else {
        if ((nread = read(liss->sd, liss->buf, liss->blksiz)) != liss->blksiz) {
            logioMsg(liss->lp, LOG_INFO, "%s: read: expect %d, got %d", fid, liss->blksiz, nread);
            return FALSE;
        }
    }

    lissUnpackMiniSeed(pkt, liss->buf, flags);
    return TRUE;
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.4  2008/02/03 21:12:15  dechavez
 * lissRead() changed to receive all packets of the expected size, regardless
 * of contents (let lissUnpackMiniSeed() decide if it can be interpreted)
 *
 * Revision 1.3  2008/01/07 21:51:21  dechavez
 * Remove selection tests and data decode operations from lissRead() (they were moved to lissUnpackMiniSeed())
 *
 * Revision 1.2  2007/05/03 19:54:56  dechavez
 * fixed uninitialized "ident" bug
 *
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
