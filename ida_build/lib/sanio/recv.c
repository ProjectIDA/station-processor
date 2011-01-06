#pragma ident "$Id: recv.c,v 1.1 2001/05/20 16:17:48 dec Exp $"
/*======================================================================
 *
 *  Read a message from the SAN
 *
 *====================================================================*/
#include <errno.h>
#include <unistd.h>
#include "sanio.h"
#include "util.h"

/* Local function for doing timeout enabled reads.  This assumes that
 * the file descripter has already been set for non-blocking I/O.
 */

static BOOL read_to(SAN_HANDLE *san, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t got;
UINT8 *ptr;
int  error, width, sd;
fd_set readfds;
struct timeval timeout;
static char *fid = "sanioRecv:read_to";

    if (want == 0) return TRUE;

    error           = 0;
    timeout.tv_sec  = san->to < SANIO_DEFAULT_TO ? SANIO_DEFAULT_TO : san->to;
    timeout.tv_usec = 0;

/* Mask out our file descriptor as the only one to look at */

    width = getdtablesize();
    FD_ZERO(&readfds);
    if ((sd = sanioGetSD(san)) < 0) {
        errno = ECONNABORTED;
        util_log(1, "%s:%d %s", san->peer, san->port, strerror(errno));
        return FALSE;
    }
    FD_SET(sd, &readfds);
    
/*  Read from socket until desired number of bytes acquired  */

    remain = want;
    ptr    = buf;

    while (remain) {
        error = select(width, &readfds, NULL, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            util_log(1, "%s:%d timed out", san->peer, san->port);
            return FALSE;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                util_log(1, "%s:%d select error: %s", 
                    san->peer, san->port, strerror(errno)
                );
                return FALSE;
            }
        } else {
            if ((sd = sanioGetSD(san)) < 0) {
                errno = ECONNABORTED;
                return FALSE;
            }
            got = read(sd, (void *) ptr, (size_t) remain);
            if (got > 0) {
                remain -= got;
                ptr    += got;
            } else if (got == 0) {
                errno = ECONNRESET; /* actually it's EOF */
                util_log(1, "%s:%d read error: %s",
                    san->peer, san->port, strerror(errno)
                );
                return FALSE;
            } else if (
                errno != EWOULDBLOCK &&
                errno != EAGAIN &&
                errno != EINTR
            ) {
                util_log(1, "%s:%d read error: %s",
                    san->peer, san->port, strerror(errno)
                );
                return FALSE;
            } else {
                util_log(1, "%s:%d read error %d (ignored)",
                    san->peer, san->port, errno
                );
            }
        }
    }

    return TRUE;
}

static UINT16 CloseOnError(SAN_HANDLE *san)
{

    if (san == NULL) {
        errno = EINVAL;
        return SAN_ERROR;
    }

    MUTEX_LOCK(&san->mutex);
        if (san->sd > -1) {
            shutdown(san->sd, 2);
            close(san->sd);
            san->sd = -1;
        }
    MUTEX_UNLOCK(&san->mutex);
    
    return SAN_ERROR;
}

UINT16 sanioRecv(SAN_HANDLE *san, UINT8 **data)
{
UINT8 dummy;
UINT16 stmp, rem, msglen, readlen, type;
static char *fid = "sanioRecv";

/* Get the length */

    if (!read_to(san, (UINT8 *) &stmp, 2)) return CloseOnError(san);

    msglen = (int) ntohs(stmp);
    if (msglen > SANIO_BUFLEN) {
        util_log(1, "%s: message length (%d) exceeds buffer size (%d)!",
            fid, msglen, SANIO_BUFLEN
        );
        readlen = SANIO_BUFLEN;
    } else {
        readlen = msglen;
    }

/* Read the message */

    if (!read_to(san, san->rcvbuf, readlen)) return CloseOnError(san);

    if (readlen < msglen) {
        rem = msglen - readlen;
        util_log(1, "%s: flush %d message bytes (no buffer space)",
            fid, rem
        );
        while (rem > 0) {
            if (!read_to(san, &dummy, 1)) return CloseOnError(san);
            --rem;
        }
        return SAN_NOMEM;
    }

/* Point to the data portion */

    *data = san->rcvbuf + 2;

/* Decode and return the message type */

    memcpy(&stmp, san->rcvbuf, 2);
    type = ntohs(stmp);
    if (type == SAN_ERROR) {
        util_log(1, "%s: %d byte message decodes to SAN_ERROR!",
            fid, msglen
        );
        return CloseOnError(san);
    }

    return (UINT16) type;
}

/* Revision History
 * $Log: recv.c,v $
 * Revision 1.1  2001/05/20 16:17:48  dec
 * introduced
 *
 */
