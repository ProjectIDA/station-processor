#pragma ident "$Id: common_r.c,v 1.9 2007/01/04 23:34:27 dechavez Exp $"
/*======================================================================
 *
 * More or less MT-safe versions of the things in common.c
 *
 *====================================================================*/
#include "platform.h"
#include "xfer.h"
#include "util.h"

static MUTEX mutex = MUTEX_INITIALIZER;

#define FATAL_ERROR    -2 /* must be negative */
#define NONFATAL_ERROR -1 /* must be negative */
#define TRANSIENT_ERROR 0 /* must be 0 */

void XferLibInit()
{
    MUTEX_INIT(&mutex);
}

#define FIXED_TIMEOUT (XFER_MINTO * 2)

/* Local function for doing timeout enabled reads.  This assumes that
 * the file descripter has already been set for non-blocking I/O,
 * which is done as part of eqioOpen() and eqioAccept().
 */

static BOOL xfer_Read(int sd, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t got;
UINT8 *ptr;
int  error, width;
fd_set readfds;
struct timeval timeout;
static char *fid = "xfer_Read";

    if (want == 0) return TRUE;

    error           = 0;
    timeout.tv_sec  = FIXED_TIMEOUT * 4; /* have to hard-code since no way to pass */
    timeout.tv_usec = 0;

/* Mask out our file descriptor as the only one to look at */

#ifndef WIN32
    width = FD_SETSIZE;
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
            xfer_errno = XFER_ETIMEDOUT;
            return FALSE;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                xfer_errno = XFER_EIO;
                return FALSE;
            }
        } else {
#ifdef WIN32
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
                xfer_errno = XFER_ECONNECT;
                return FALSE;
#ifdef WIN32
            } else if (WSAGetLastError() != 0) {
                xfer_errno = XFER_EIO;
                return FALSE;
#endif
            } else if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
                xfer_errno = XFER_EIO;
                return FALSE;
            }
        }
    }

    return TRUE;
}

static BOOL xfer_Write(int sd, UINT8 *buf, UINT32 want)
{
size_t remain;
ssize_t got;
UINT8 *ptr;
int  error, width;
fd_set fds;
struct timeval timeout;

    error           = 0;
    timeout.tv_sec  = FIXED_TIMEOUT; /* have to hard-code since no way to pass */
    timeout.tv_usec = 0;

/* Mask out our file descriptor as the only one to look at */

#ifndef WIN32
    width = FD_SETSIZE;
#else
    width = 0;
#endif
    FD_ZERO(&fds);
    FD_SET(sd, &fds);
    
/*  Write to socket until desired number of bytes sent */

    remain = want;
    ptr    = buf;

    while (remain) {
        error = select(width, NULL, &fds, NULL, &timeout);
        if (error == 0) {
            errno = ETIMEDOUT;
            return FALSE;
        } else if (error < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                return FALSE;
            }
        } else {
#ifdef WIN32
            got = send(sd, ptr, remain, 0);
            if (got == SOCKET_ERROR) got = -1;
#else
            got = write(sd, (void *) ptr, remain);
#endif
            if (got >= 0) {
                remain -= got;
                ptr    += got;
            } else if (
                errno != EWOULDBLOCK &&
                errno != EAGAIN      &&
                errno != EINTR
            ) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/* Send a message */

int xfer_SendMsg(int sd, char *message_plus, long msglen)
{
unsigned long lval;
long len;
static char *fid = "xfer_SendMsg";

/* Stuff the first 4 bytes with the message length */

    lval = htonl(msglen);
    memcpy(message_plus, &lval, 4);

/* Do a single write to send the byte count and message */

    len = msglen + 4;
    if (xfer_Write(sd, (UINT8 *) message_plus, (UINT32) len)) return XFER_OK;

/* Must be an error at this point */

    return XFER_ERROR;
}

/* Receive a message */

int xfer_RecvMsg(int sd, char *message, long maxlen, long *len)
{
unsigned long lval;
int msglen;
static char *fid = "xfer_RecvMsg";

/* Get the message length */

    if (maxlen < sizeof(unsigned long)) {
        util_log(1, "%s: message buffer too small (%d < %d)",
            fid, maxlen, sizeof(unsigned long)
        );
        xfer_errno = XFER_ELIMIT;
        return XFER_ERROR;
    }

    if (!xfer_Read(sd, (UINT8 *) &lval, (UINT32) sizeof(unsigned long))) {
        return XFER_ERROR;
    }

    *len = msglen = (size_t) ntohl(lval);

/* Zero length messages are OK */

    if (msglen == 0) {
        util_log(3, "BREAK received");
        return XFER_OK;
    }

/* Otherwise make sure we have enough buffer space to hold the message */

    if (msglen > maxlen) {
        util_log(1, "%s: incoming message too big (msglen=%d, maxlen=%d)",
            fid, msglen, maxlen
        );
        xfer_errno = XFER_ETOOBIG;
        return XFER_ERROR;
    }

/* Read the message */

    if (xfer_Read(sd, (UINT8 *) message, (UINT32) msglen)) return XFER_OK;

/* Must be an error at this point */

    return XFER_ERROR;
}

/* Convert from epoch time to xfer_time */

struct xfer_time *xfer_time(dtime)
double dtime;
{
static struct xfer_time ntime;
static char *fid = "xfer_time";

    ntime.sec =  (long) dtime;
    ntime.usec = (u_long) ((dtime - (double) ntime.sec)*(double) 1000000.0);

    return &ntime;
}

/* Convert from xfer_time to epoch time */

double xfer_dtime(ntime)
struct xfer_time *ntime;
{
static char *fid = "xfer_dtime";

    return (double) ntime->sec + ((double) ntime->usec/(double) 1000000.0);
}

/*============================ External use ==========================*/

char *Xfer_ErrStr()
{
static char defmsg[] = "unknown error 0x00000000 plus some extra";
static char *fid = "Xfer_ErrStr";

    if (xfer_errno == XFER_EPROTOCOL) {
        return "unsupported protocol";
    } else if (xfer_errno == XFER_EREQUEST) {
        return "unrecognized request code";
    } else if (xfer_errno == XFER_EFORMAT) {
        return "unsupported format";
    } else if (xfer_errno == XFER_EREFUSED) {
        return "unauthorized connection refused by server";
    } else if (xfer_errno == XFER_ELIMIT) {
        return "implementation defined limit exceeded";
    } else if (xfer_errno == XFER_EINVAL) {
        return "illegal data received";
    } else if (xfer_errno == XFER_ETOOBIG) {
        return "message too large to receive";
    } else if (xfer_errno == XFER_ETIMEDOUT) {
        return "connection timed out";
    } else if (xfer_errno == XFER_ECONNECT) {
        return "no connection with peer";
    } else if (xfer_errno == XFER_EIO) {
        return "I/O error";
    } else if (xfer_errno == XFER_EPIPE) {
        return "no peer";
    } else if (xfer_errno == XFER_EREJECT) {
        return "request rejected by server";
    } else if (xfer_errno == XFER_EHANDLER) {
        return "unable to install signal handler";
    } else if (xfer_errno == XFER_ENOSUCH) {
        return "none of the requested stations/channels are available";
    } else if (xfer_errno == XFER_EBUSY) {
        return "server busy";
    } else if (xfer_errno == XFER_EFAULT) {
        return "server fault";
    } else if (xfer_errno == XFER_EHOSTNAME) {
        return "can't resolve hostname/address";
    }

    sprintf(defmsg, "unknown error 0x%x", xfer_errno);
    return defmsg;
}

/* Shutdown and exit */

void Xfer_Exit(int sd, int status)
{
static char *fid = "Xfer_Exit";

    if (sd > 0) close(sd);
    util_log(1, "exit %d", status);
    exit(status);
}

static int GetPortNumber(char *service, int port)
{
#ifndef WIN32
struct servent sp, *ptrsp;
char buffer[sizeof(struct servent)];
int buflen = sizeof(struct servent);

    if (service != NULL) {
#if defined (LINUX) || defined (BSD)
        if ((ptrsp = getservbyname(service, "tcp")) == NULL) {
            return -1;
        }
        return (int) ntohs(ptrsp->s_port);
#else
        if (getservbyname_r(service, "tcp", &sp, buffer, buflen) == NULL) {
            return -1;
        }
        return (int) ntohs(sp.s_port);
#endif /* !LINUX */
    }
#endif /* WIN32 */

    return port;
}

static BOOL SetHostAddr(struct sockaddr_in *in_addr, char *host, int port)
{
#ifndef in_addr_t
unsigned long addr;
#else
in_addr_t addr;
#endif
struct hostent *hp;
static char *fid = "SetHostAddr";

     MUTEX_LOCK(&mutex);
        hp = gethostbyname(host);
        if (hp == (struct hostent *) NULL) {
            if (h_errno != HOST_NOT_FOUND) {
                MUTEX_UNLOCK(&mutex); return FALSE;
            }

        /* try again assuming server name is in dot decimal form */

            if ((addr = inet_addr(host)) == -1) {
                MUTEX_UNLOCK(&mutex); return FALSE;
            } else {
                hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
            }
        }
        if (hp == (struct hostent *) NULL) {
            MUTEX_UNLOCK(&mutex); return FALSE;
        }

        memcpy(&in_addr->sin_addr, hp->h_addr, hp->h_length);
    MUTEX_UNLOCK(&mutex);

    in_addr->sin_family = AF_INET;
    in_addr->sin_port = ntohs((u_short) port);

    return TRUE;
}

static int ConnectToServer(char *server, int port, int sndbuf, int rcvbuf)
{
struct sockaddr_in addr_in;
struct sockaddr *peer;
int plen, ilen, val, sd;

    if (!SetHostAddr(&addr_in, server, port)) return FATAL_ERROR;
    peer = (struct sockaddr *) &addr_in;
    plen = (int) sizeof(struct sockaddr_in);
    val  = 1;
    ilen = sizeof(int);

/* Create socket */

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
perror("socket");
        return FATAL_ERROR;
    }

/* Set socket options.  These are really suggestions so we don't
 * care if they work or not and therefore don't test for failure.
 * Ignorance is bliss.
 */

    setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, (char *) &val, ilen);

    if (sndbuf > 0) {
        setsockopt(sd, SOL_SOCKET, SO_SNDBUF, (char *) &sndbuf, ilen);
    }
    if (rcvbuf > 0) {
        setsockopt(sd, SOL_SOCKET, SO_RCVBUF, (char *) &rcvbuf, ilen);
    }

/* Do the connection */

    if (connect(sd, peer, plen) != 0) {
perror("connect");
#ifndef WIN32
        close(sd);
#else
        closesocket(sd);
#endif /* WIN32 */
        switch (errno) {
          case EINTR:
          case ETIMEDOUT:
          case ECONNREFUSED:
          case ECONNRESET:
          case ENETDOWN:
          case ENETRESET:
          case ENETUNREACH:
          case EHOSTUNREACH:
          case EHOSTDOWN:
            sd = TRANSIENT_ERROR;
            break;
          case EACCES:
          case EAFNOSUPPORT:
          case EINVAL:
          case ELOOP:
          case ENOENT:
#ifdef ENOSR
          case ENOSR:
#endif
          case ENOTDIR:
          case ENOTSOCK:
          case EPROTOTYPE:
            sd = FATAL_ERROR;
            break;
          default:
            sd = NONFATAL_ERROR;
            break;
        }
    }
    return sd;
}

int Xfer_Connect2(
    char *host,
    char *service,
    int port,
    char *unused1,
    struct xfer_req *req,
    struct xfer_cnf *cnf,
    int retry,
    int unused2
) {
int sd;
unsigned long count = 0;
static char *fid = "Xfer_Connect2";


/* Set the global socket I/O timeout */

    req->timeout = FIXED_TIMEOUT;

/* If socket buffer lengths are not explicity given, use our defaults */

    if (req->sndbuf <= 0) req->sndbuf = XFER_SO_SNDBUF;
    if (req->rcvbuf <= 0) req->rcvbuf = XFER_SO_RCVBUF;

/* Try to connect once or forever, depending on state of retry flag.
 * Note, a retry of -1 means try forever, regardless of error code.
 * If we do retry, we will sleep for 1, then 2, then 3 times the
 * the timeout interval before attempting again.  After 3 trys we
 * reset the sleep interval back to zero and repeat.
 */

    port = GetPortNumber(service, port);
    do {

        sleep((count++ % 4) * FIXED_TIMEOUT);

        sd = ConnectToServer(host, port, req->sndbuf, req->rcvbuf);

        if (sd == FATAL_ERROR) return -1;
        if (sd == NONFATAL_ERROR && retry != -1) return -1;

    /* Positive return means we connected... send the request */

        if (sd > 0) {
            util_log(1, "sending request");
            if (xfer_SendReq(sd, req) != XFER_OK) {
                util_log(2, "%s: xfer_SendReq failed: %s",
                    fid, Xfer_ErrStr()
                );
                close(sd);
                if (xfer_errno == XFER_ETIMEDOUT || retry < 0) {
                    sd = 0;
                } else {
                    retry = 0;
                    sd = -1;
                }
            } else {
                util_log(1, "waiting for acknowledgment");
                if (xfer_RecvCnf(sd, cnf) != XFER_OK) {
                    util_log(2, "%s: xfer_RecvCnf failed: %s",
                        fid, Xfer_ErrStr()
                    );
                    close(sd);
                    if (xfer_errno == XFER_ETIMEDOUT || retry < 0) {
                        sd = 0;
                    } else {
                        retry = 0;
                        sd = -1;
                    }
                } else {
                    util_log(1, "request accepted");
                }
            }
        }
    } while (retry && sd == 0);

/* Return the socket descriptor of the connection */

    return sd ? sd : -1;
}

/* Revision History
 *
 * $Log: common_r.c,v $
 * Revision 1.9  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.8  2006/06/15 00:00:07  dechavez
 * fixed linux builds (by using getservbyname instead of getservbyname_r)
 *
 * Revision 1.7  2005/05/23 21:02:24  dechavez
 * cleaned up includes (05-23 update AAP)
 *
 * Revision 1.6  2004/09/28 22:49:48  dechavez
 * improved error handling
 *
 * Revision 1.5  2004/01/29 18:19:04  dechavez
 * use FD_SETSIZE instead of getdtablesize()
 *
 * Revision 1.4  2003/12/23 21:24:26  dechavez
 * use unsigned long if in_addr_t isn't defined
 *
 * Revision 1.3  2003/12/23 21:07:48  dechavez
 * removed debugging print statements
 *
 * Revision 1.2  2003/12/10 06:05:03  dechavez
 * various casts, includes, and the like to calm solaris cc
 *
 * Revision 1.1  2002/02/21 22:40:09  dec
 * created
 *
 */
