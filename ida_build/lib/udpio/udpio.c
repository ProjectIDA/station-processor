#pragma ident "$Id: udpio.c,v 1.10 2006/06/26 23:49:11 dechavez Exp $"
/*======================================================================
 *
 *  BOOL udpioInit(UDPIO *up, int port, int to, LPIO *lp)
 *  Creates a socket used for subsequent I/O operations.  Port number is
 *  the port which the socket will listen on for incoming data, to is the
 *  read timeout interval in msec (use 0 for blocking I/O). 
 * 
 *  int udpioSend(UDPIO *up, UINT32 ip, int port, char *buf, int len)
 *  Sends "len" bytes of data in the buffer pointed to by "buf" to 
 *  port number "port" at IP address "ip" where "ip" is the 32 bit
 *  IP address in host byte order. "up" is the handle initialized
 *  with a prior call to udpioInit().  Returns the number of bytes sent
 *  (which will always be "len") or a negative number in case of error.
 * 
 *  int udpioRecv(UDPIO *up, char *buf, int buflen)
 *  Reads the next UDP packet at the port given by "up", which was
 *  initialized with a prior call to udpioInit().  The packet is stored 
 *  in the buffer pointed to by "buf", which is "buflen" bytes long.  If
 *  the incoming message is too large (ie, is more than "buflen" bytes
 *  long, then the message will be truncated.  Returns the number of bytes
 *  read or a negative number in case of error.
 *
 *======================================================================*/
#include "udpio.h"
#include "util.h"

static void InitStats(UDPIO_STATS *stats)
{
    MUTEX_INIT(&stats->mutex);
    stats->total.bytes = stats->total.pkts = (UINT64) 0;
    stats->len.min = stats->len.max = 0;
    stats->tstamp = utilTimeStamp();
    stats->error.count = stats->error.tstamp = 0;
}

static void UpdateStats(UDPIO_STATS *stats, int nbytes, UINT64 tstamp)
{
    MUTEX_LOCK(&stats->mutex);
        if (nbytes > 0) {
            stats->total.bytes += nbytes;
            ++stats->total.pkts;
            if (stats->len.min == 0 || nbytes < stats->len.min) stats->len.min = nbytes;
            if (nbytes > stats->len.max) stats->len.max = nbytes;
            stats->tstamp = tstamp;
        } else {
            ++stats->error.count;
            stats->error.tstamp = tstamp;
        }
    MUTEX_UNLOCK(&stats->mutex);
}

void udpioGetStats(UDPIO *up, UDPIO_STATS *xmit, UDPIO_STATS *recv)
{
    if (up == NULL) return;

    if (xmit != NULL) {
        MUTEX_LOCK(&up->stats.xmit.mutex);
            *xmit = up->stats.xmit;
        MUTEX_UNLOCK(&up->stats.xmit.mutex);
    }

    if (recv != NULL) {
        MUTEX_LOCK(&up->stats.recv.mutex);
            *recv = up->stats.recv;
        MUTEX_UNLOCK(&up->stats.recv.mutex);
    }
}

BOOL udpioInit(UDPIO *up, int port, int to, LOGIO *lp)
{
static struct sockaddr_in addr;
static char *fid = "udpioInit";
#ifdef WIN32
WSADATA lpWSAData;
#endif /* WIN32 */

    if (up == NULL) {
        errno = EINVAL;
        return FALSE;
    }

#ifdef WIN32
    if (WSAStartup(MAKEWORD(2,2), &lpWSAData) != 0){ 
        logioMsg(up->lp, LOG_ERR, "%s: WSAStartup", fid);
        return FALSE;
    }
#endif /* WIN32 */

    up->lp = lp;

    if ((up->sd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        logioMsg(up->lp, LOG_ERR, "%s: socket: %s", fid, strerror(errno));
        return FALSE;
    }

    if (to > 0) {
        up->to.tv_sec = (time_t) to / MSEC_PER_SEC;;
        up->to.tv_usec = (suseconds_t) (to - up->to.tv_sec * MSEC_PER_SEC) * USEC_PER_MSEC;
        utilSetNonBlockingSocket(up->sd);
    } else {
        up->to.tv_sec = 0;
        up->to.tv_usec = 0;
    }

    memset((void *) &addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((short) port);

    if (bind(up->sd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        logioMsg(up->lp, LOG_ERR, "%s: bind %s", fid, strerror(errno));
        return FALSE;
    }

    InitStats(&up->stats.xmit);
    InitStats(&up->stats.recv);

    up->start = up->stats.xmit.tstamp = up->stats.recv.tstamp = utilTimeStamp();
    return TRUE;
}

int udpioRecv(UDPIO *up, char *buf, int buflen)
{
LOGIO *lp;
int error, got;
INT32 width;
fd_set fds;
static int len = sizeof(struct sockaddr_in);
static char *fid = "udpioRecv";

    if (up == NULL || buf == NULL) {
        errno = EINVAL;
        lp = (up == NULL) ? NULL : up->lp;
        logioMsg(lp, LOG_ERR, "%s: NULL input not allowed", fid);
        return -1;
    }

    if (buflen < 1) {
        logioMsg(up->lp, LOG_ERR, "%s: illegal buflen '%d'", fid, buflen);
        return -1;
    }

    if (up->sd == INVALID_SOCKET) {
        logioMsg(up->lp, LOG_ERR, "%s: invalid socket", fid, buflen);
        errno = EINVAL;
        return -1;
    }

    width = getdtablesize();
    FD_ZERO(&fds);
    FD_SET(up->sd, &fds);
    got = 0;

    while (got == 0) {
        error = select(width, &fds, NULL, NULL, &up->to);
        if (error > 0) {
            got = RECVFROM(up->sd, buf, buflen, 0, &up->peer.addr, &len);
            if (got > 0) {
                inet_ntop(AF_INET, &up->peer.addr, up->peer.ident, MAXPATHLEN);
            } else {
                logioMsg(up->lp, LOG_INFO, "%s: RECVFROM: %s", fid, strerror(errno));
                if (errno != EAGAIN && errno != EINTR) return -1;
            }
        } else if (error == 0) {
            errno = ETIMEDOUT;
            return 0;
        } else if (errno != EAGAIN && errno != EINTR) {
            logioMsg(up->lp, LOG_INFO, "%s: select: %s", fid, strerror(errno));
            return -1;
        }
    }
    UpdateStats(&up->stats.recv, got, utilTimeStamp());

    return got;
}

int udpioSend(UDPIO *up, UINT32 ip, int port, char *buf, int buflen)
{
int sent;
LOGIO *lp;
struct sockaddr_in addr;
static int alen = sizeof(struct sockaddr_in);
static char *fid = "udpioSend";

    if (up == NULL || buf == NULL) {
        errno = EINVAL;
        lp = (up == NULL) ? NULL : up->lp;
        logioMsg(lp, LOG_ERR, "%s: NULL input not allowed", fid);
        return -1;
    }

    memset((void *) &addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port        = htons((short) port);

    sent = SENDTO(up->sd, buf, buflen, 0, &addr, alen);
    UpdateStats(&up->stats.xmit, sent, utilTimeStamp());

    return sent;
}

#ifdef READ_TEST

#define MYBUFLEN 2048

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [-v] PortNumber\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int i, len, port = -1;
char buf[MYBUFLEN];
UDPIO up;
BOOL verbose = FALSE;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (port == -1) {
            port = atoi(argv[i]);
        } else {
            help(argv[0]);
        }
    }

    if (port == -1) {
        help(argv[0]);
    } else {
        fprintf(stderr, "Listening for connections at port %d\n", port);
    }

    if ((!udpioInit(&up, port, NULL)) == INVALID_SOCKET) {
        perror("udioInit failed");
        exit(1);
    }

    while (1) {
        if ((len = udpioRecv(&up, buf, MYBUFLEN)) > 0) {
            if (verbose) {
                fprintf(stderr, "Recv'd %d bytes:\n", len);
                utilPrintHexDump(stderr, buf, len);
            }
            fwrite(buf, 1, len, stdout);
            fflush(stdout);
        } else {
            perror("udpioRecv failed");
        }
    }
}

#endif /* READ_TEST */

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
 * $Log: udpio.c,v $
 * Revision 1.10  2006/06/26 23:49:11  dechavez
 * fixed log message with unitialized log handle bug
 *
 * Revision 1.9  2006/06/26 22:22:47  dechavez
 * removed unfererenced local variables
 *
 * Revision 1.8  2006/06/17 23:37:40  dechavez
 * added missing bracket for win32 builds (aap)
 *
 * Revision 1.7  2006/05/20 01:37:23  dechavez
 * fixed incorrect return value in udpioInit()
 *
 * Revision 1.6  2006/05/17 23:21:47  dechavez
 * added copyright notice
 *
 * Revision 1.5  2006/05/17 18:10:29  dechavez
 * updated comments
 *
 * Revision 1.4  2006/05/06 02:24:01  dechavez
 * initial production release
 *
 */
