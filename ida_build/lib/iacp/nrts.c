#pragma ident "$Id: nrts.c,v 1.1 2008/08/20 18:14:14 dechavez Exp $"
/*======================================================================
 * 
 * NRTS I/O
 *
 *====================================================================*/
#include "iacp.h"
#include "util.h"

#define HANDSHAKE_BUFLEN 2048

/* Client side of the handshake */

BOOL iacpNrtsClientHandshake(IACP *iacp)
{
int LogLevel;
IACP_FRAME frame;
UINT8 buf[HANDSHAKE_BUFLEN];
static char *fid = "iacpNrtsClientHandshake";

    if (iacp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    LogLevel = iacpGetDebug(iacp) ? LOG_INFO : LOG_DEBUG;

    errno = ENOTSUP;
    return FALSE;
}

/* Server side of the handshake */

BOOL iacpNrtsServerHandshake(IACP *iacp)
{
int LogLevel;
IACP_FRAME frame;
UINT8 buf[HANDSHAKE_BUFLEN];
static char *fid = "iacpNrtsServerHandshake";

    if (iacp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    LogLevel = iacpGetDebug(iacp) ? LOG_INFO : LOG_DEBUG;

    errno = ENOTSUP;
    return FALSE;
}

/* Revision History
 *
 * $Log: nrts.c,v $
 * Revision 1.1  2008/08/20 18:14:14  dechavez
 * preliminary version with just stubs for iacpNrtsClientHandshake() and iacpNrtsServerHandshake()
 *
 */
