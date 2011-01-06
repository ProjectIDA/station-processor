#pragma ident "$Id: connect.c,v 1.10 2007/10/31 23:12:36 dechavez Exp $"
/*======================================================================
 *
 *  Establish a C&C connection with a SAN.
 *  Returns SANCC descriptor if successful, a negative value if not.
 *  Calls util_connect(), which is MT-unsafe.
 *
 *====================================================================*/
#include <unistd.h>
#include <errno.h>
#include "sanio.h"
#include "util.h"

static SAN_HANDLE *cleanup(SAN_HANDLE *san)
{
    if (san == NULL) return NULL;
    free(san);

    return NULL;
}

static BOOL fail(SAN_HANDLE *san)
{
    if (san != NULL) {
        if (san->sd > -1) {
            MUTEX_LOCK(&san->mutex);
                shutdown(san->sd, 2);
                close(san->sd);
                san->sd = -1;
            MUTEX_UNLOCK(&san->mutex);
        }
    }
    return FALSE;
}

static BOOL doConnect(SAN_HANDLE *san)
{
int sd, type;
UINT8 buf[SANIO_BUFLEN], *dptr;
static char *fid = "sanioConnect";

    if (!sanioCheckHandle(san)) {
        util_log(1, "%s: illegal handle", fid);
        return FALSE;
    }

/* Establish socket connection */

    sd = util_connect(san->peer, NULL, san->port, "tcp", 0, 0);
    if (sd < 0) {
        util_log(1, "%s: util_connect failed", fid);
        return FALSE;
    }
    if (sd == 0) return FALSE;
    MUTEX_LOCK(&san->mutex);
        san->sd = sd;
    MUTEX_UNLOCK(&san->mutex);
    util_log(2, "%s: connected to %s:%d", fid, san->peer, san->port);

/* Send over process ID */

    if (!sanioSendPid(san, buf)) {
        util_log(1, "%s: sanioSendPid failed", fid);
        return fail(san);
    }

/* Read back the connection state */

    type = sanioRecv(san, &dptr);
    if (type == SAN_ERROR) {
        return fail(san);
    } else if (type != SAN_STATE) {
        if (errno == 0) util_log(1, "%s: protocol error: %d != %d", fid, type, SAN_STATE);
        return fail(san);
    }
    utilUnpackUINT16(dptr, &san->state);

/* Set the timeout */

    if (!sanioSendTimeout(san, san->to, buf)) {
        util_log(1, "%s: sanioSendTimeout failed", fid);
        return fail(san);
    }

    return TRUE;
    
}

BOOL sanioConnect(SAN_HANDLE *san)
{
int count = 0;

    while (!doConnect(san)) {
        MUTEX_LOCK(&san->mutex);
            if (san->sd > -1) {
                shutdown(san->sd, 2);
                close(san->sd);
                san->sd = -1;
            }
        MUTEX_UNLOCK(&san->mutex);
        if (!san->retry) return FALSE;
        if (++count == 1) {
            util_log(1, "SAN@%s:%d not responding", san->peer, san->port);
        }
        sleep(30);
    }
    if (count > 0) util_log(1, "SAN@%s:%d OK", san->peer, san->port);
    return TRUE;
}

SAN_HANDLE *sanioInit(char *peer, int port, int to, BOOL retry)
{
SAN_HANDLE *san;
static char *fid = "sanioInit";

    if ((san = malloc(sizeof(SAN_HANDLE))) == NULL) {
        util_log(1, "%s: malloc: %s", fid, strerror(errno));
        return cleanup(san);
    }
    if (strlen(peer) > MAXPATHLEN) {
        util_log(1, "%s: peer name too long!", fid);
        return cleanup(san);
    }
    strcpy(san->peer, peer);
    san->port  = port;
    san->sd    = -1;
    san->to    = (to > 0) ? to : SANIO_DEFAULT_TO;
    san->retry = TRUE;

    MUTEX_INIT(&san->mutex);

    return san;
}

/* Revision History
 * $Log: connect.c,v $
 * Revision 1.10  2007/10/31 23:12:36  dechavez
 * fixed return value error on NULL san in fail()
 *
 * Revision 1.9  2003/12/10 06:11:29  dechavez
 * use sign/unsigned specific utilUnpack...s
 *
 * Revision 1.8  2003/05/23 19:48:38  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.7  2001/09/18 23:21:31  dec
 * interpret util_connect retval of 0 to mean "gentle" failure
 *
 * Revision 1.6  2001/05/20 16:19:51  dec
 * accomodate SAN_HANDLE changes, MUTEX protected sd
 *
 * Revision 1.5  2000/11/08 18:38:06  dec
 * initialize send/recieve mutexen
 *
 * Revision 1.4  2000/11/02 20:27:33  dec
 * Initial working release
 *
 * Revision 1.3  2000/09/20 00:49:45  dec
 * checkpoint save
 *
 * Revision 1.2  2000/08/03 21:14:58  dec
 * checkpoint save... still not tested
 *
 * Revision 1.1  2000/08/02 16:22:56  dec
 * initial release, based on SAN VxWorks sources (untested)
 *
 */
