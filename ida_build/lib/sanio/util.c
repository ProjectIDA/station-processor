#pragma ident "$Id: util.c,v 1.4 2003/05/23 19:48:39 dechavez Exp $"
/*======================================================================
 *
 *  SAN C&C utilities
 *
 *====================================================================*/
#include <errno.h>
#include <unistd.h>
#include "sanio.h"
#include "util.h"

int sanioGetSD(SAN_HANDLE *san)
{
int retval;

    MUTEX_LOCK(&san->mutex);
        retval = san->sd;
    MUTEX_UNLOCK(&san->mutex);
    return retval;
}

BOOL sanioCheckHandle(SAN_HANDLE *san)
{
    if (san == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    return TRUE;
}

VOID sanioDisconnect(SAN_HANDLE *san, int code, UINT8 *buf)
{
int len;

    len = utilPackINT16(buf+SANIO_PREAMBLE_LEN, (UINT16) code);
    sanioSend(san, SAN_DISCONNECT, buf, len);

    MUTEX_LOCK(&san->mutex);
        if (san->sd > -1) {
            shutdown(san->sd, 2);
            close(san->sd);
            san->sd = -1;
        }
    MUTEX_UNLOCK(&san->mutex);
}

BOOL sanioRequestState(SAN_HANDLE *san, int state, UINT8 *buf)
{
int len;

    len = utilPackINT16(buf+SANIO_PREAMBLE_LEN, (UINT16) state);
    return sanioSend(san, SAN_STATE, buf, len);
}

BOOL sanioReboot(SAN_HANDLE *san, BOOL force, UINT8 *buf)
{

    if (!sanioCheckHandle(san)) return FALSE;

    if (san->state != SAN_OPERATOR) {
        if (!force) {
            errno = EPERM;
            return FALSE;
        } else {
            sanioRequestState(san, SAN_OPERATOR, buf);
        }
    }

    return sanioSend(san, SAN_REBOOT, buf, 0);
}

BOOL sanioSendConfig(SAN_HANDLE *san, SANIO_CONFIG *config, BOOL force, UINT8 *buf)
{
int len;
BOOL retval;

    if (!sanioCheckHandle(san)) return FALSE;

    if (san->state != SAN_OPERATOR) {
        if (!force) {
            errno = EPERM;
            return FALSE;
        } else {
            sanioRequestState(san, SAN_OPERATOR, buf);
        }
    }

    len = sanioPackConfig(buf+SANIO_PREAMBLE_LEN, config);
    return sanioSend(san, SAN_CONFIG, buf, len);
}

BOOL sanioSendPid(SAN_HANDLE *san, UINT8 *buf)
{
UINT32 pid;
int len;

    pid = (UINT32) getpid();
    len = utilPackINT32(buf+SANIO_PREAMBLE_LEN, pid);
    return sanioSend(san, SAN_PID, buf, len);
}

BOOL sanioSendTimeout(SAN_HANDLE *san, int timeout, UINT8 *buf)
{
int len;

    len = utilPackINT16(buf+SANIO_PREAMBLE_LEN, (UINT16) timeout);
    return sanioSend(san, SAN_SETTO, buf, len);
}

/* Revision History
 * $Log: util.c,v $
 * Revision 1.4  2003/05/23 19:48:39  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.3  2001/05/20 16:17:29  dec
 * (re)introduced
 *
 */
