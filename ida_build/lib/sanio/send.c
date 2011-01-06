#pragma ident "$Id: send.c,v 1.2 2003/05/23 19:48:39 dechavez Exp $"
/*======================================================================
 *
 *  Send a message to the SAN
 *
 *====================================================================*/
#include <errno.h>
#include <unistd.h>
#include "sanio.h"
#include "util.h"

/* Local function for doing timeout enabled writes.  This assumes that
 * the file descripter has already been set for non-blocking I/O.
 */

static BOOL write_to(SAN_HANDLE *san, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t got;
UINT8 *ptr;
int  error, width, sd;
fd_set writefds;
struct timeval timeout;
static char *fid = "sanioSend:write_to";

    error           = 0;
    timeout.tv_sec  = san->to < SANIO_DEFAULT_TO ? SANIO_DEFAULT_TO : san->to;
    timeout.tv_usec = 0;

/* Mask out our file descriptor as the only one to look at */

    width = getdtablesize();
    FD_ZERO(&writefds);
    if ((sd = sanioGetSD(san)) < 0) {
        errno = ECONNABORTED;
        util_log(1, "%s:%d %s", san->peer, san->port, strerror(errno));
        return FALSE;
    }
    FD_SET(sd, &writefds);
    
/*  Write to socket until desired number of bytes sent */

    remain = want;
    ptr    = buf;

    while (remain) {
        error = select(width, NULL, &writefds, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            util_log(1, "%s:%d select: %s", 
                san->peer, san->port, strerror(errno)
            );
            return FALSE;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                util_log(1, "%s:%d select: %s", 
                    san->peer, san->port, strerror(errno)
                );
                return FALSE;
            }
        } else {
            if ((sd = sanioGetSD(san)) < 0) {
                errno = ECONNABORTED;
                util_log(1, "%s:%d %s", san->peer, san->port, strerror(errno));
                return FALSE;
            }
            got = write(sd, (void *) ptr, remain);
            if (got >= 0) {
                remain -= got;
                ptr    += got;
            } else if (
                errno != EWOULDBLOCK &&
                errno != EAGAIN      &&
                errno != EINTR
            ) {
                util_log(1, "%s:%d write: %s", 
                    san->peer, san->port, strerror(errno)
                );
                return FALSE;
            }
        }
    }

    return TRUE;
}

BOOL sanioSend(SAN_HANDLE *san, int type, UINT8 *msg, int msglen)
{
UINT8 *ptr;
static char *fid = "sanioSend";

    if (!sanioCheckHandle(san)) {
        util_log(1, "%s: sanioCheckHandle FAILED", fid);
        return FALSE;
    }

/* Add the preamble */

    ptr = msg;
    ptr += utilPackINT16(ptr, (UINT16) msglen + 2);
    ptr += utilPackINT16(ptr, (UINT16) type);
    msglen += 4;

/* Send the message */

    if (!write_to(san, msg, msglen)) {
        MUTEX_LOCK(&san->mutex);
            if (san->sd > -1) {
                shutdown(san->sd, 2);
                close(san->sd);
                san->sd = -1;
            }
        MUTEX_UNLOCK(&san->mutex);
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 * $Log: send.c,v $
 * Revision 1.2  2003/05/23 19:48:39  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.1  2001/05/20 16:17:48  dec
 * introduced
 *
 */
