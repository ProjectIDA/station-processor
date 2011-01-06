#pragma ident "$Id: par.c,v 1.13 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * Manipulate the parameter structure
 *
 *====================================================================*/
#include "qdp.h"

/* Initilize the structure */

void qdpInitPar(QDP_PAR *par, int linkport)
{
    if (par == NULL) return;

    MUTEX_INIT(&par->mutex);
    memset(par->host, 0, MAXPATHLEN+1);
    par->port.base = QDP_DEFAULT_BASEPORT;
    par->port.link = linkport;
    par->port.ctrl.base = par->port.ctrl.value = QDP_DEFAULT_CTRL_BASEPORT;
    par->port.data.base = par->port.data.value = QDP_DEFAULT_DATA_BASEPORT;
    par->authcode = 0;
    par->serialno = 0;
    par->timeout.ctrl = QDP_DEFAULT_CTRL_TIMEOUT;
    par->timeout.data = QDP_DEFAULT_DATA_TIMEOUT;
    par->interval.retry = QDP_DEFAULT_RETRY_INTERVAL;
    par->interval.open = QDP_DEFAULT_OPEN_INTERVAL;
    par->hbeat.bitmap = QDP_DEFAULT_HBEAT_BITMAP;
    par->hbeat.forward = QDP_DEFAULT_HBEAT_FORWARD;
    par->debug = QDP_DEFAULT_DEBUG;
    par->trash.pkts = QDP_DEFAULT_TRASH_PKTS;
    par->trash.link = QDP_DEFAULT_TRASH_LINK;
    par->trash.interval = QDP_DEFAULT_TRASH_INTERVAL;
    par->user.arg = QDP_DEFAULT_USER_ARG;
    par->user.func = QDP_DEFAULT_USER_FUNC;
    par->meta.arg = QDP_DEFAULT_META_ARG;
    par->meta.func = QDP_DEFAULT_META_FUNC;

    switch (par->port.link) {
      case QDP_CFG_PORT:
      case QDP_SFN_PORT:
        par->hbeat.interval = QDP_DEFAULT_CMD_HBEAT_INTERVAL;
        par->nbuf = QDP_DEFAULT_CMD_NBUF;
        break;
      default:
        par->hbeat.interval = QDP_DEFAULT_DATA_HBEAT_INTERVAL;
        par->nbuf = QDP_DEFAULT_DATA_NBUF;
    }
}

/* Extract QDP_PAR from strings of the form host:serial:auth:sport:cport:debug */

BOOL qdpParseArgString(QDP_PAR *par, char *argstr)
{
LNKLST *TokenList = NULL;

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) return FALSE;
    if (!listSetArrayView(TokenList)) return FALSE;
    if (TokenList->count != 6) return FALSE;
    qdpInitPar(par, atoi((char *) TokenList->array[3]));
    qdpSetHost(par, (char *) TokenList->array[0]);
    qdpSetSerialno(par, (UINT64) strtoll((char *) TokenList->array[1], NULL, 16));
    qdpSetAuthcode(par, (UINT64) strtoll((char *) TokenList->array[2], NULL, 16));
    qdpSetMyCtrlPort(par, atoi((char *) TokenList->array[4]));
    qdpSetMyDataPort(par, atoi((char *) TokenList->array[4]));
    qdpSetDebug(par, atoi((char *) TokenList->array[5]));

    listDestroy(TokenList);

    return TRUE;
}

/* Set various fields */

BOOL qdpSetHost(QDP_PAR *par, char *host)
{
    if (par == NULL || host == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    strncpy(par->host, host, MAXPATHLEN);
    par->host[MAXPATHLEN] = 0;
    return TRUE;
}

BOOL qdpSetBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.base = value;
    return TRUE;
}

BOOL qdpSetCtrlBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.ctrl.base = value;
    return TRUE;
}

BOOL qdpSetMyCtrlPort(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.ctrl.value = par->port.ctrl.base + value;
    return TRUE;
}

BOOL qdpSetDataBaseport(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.data.base = value;
    return TRUE;
}

BOOL qdpSetMyDataPort(QDP_PAR *par, int value)
{
    if (par == NULL) return FALSE;
    par->port.data.value = par->port.data.base + value;
    return TRUE;
}

BOOL qdpSetAuthcode(QDP_PAR *par, UINT64 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->authcode = value;
    return TRUE;
}

BOOL qdpSetSerialno(QDP_PAR *par, UINT64 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->serialno = value;
    return TRUE;
}

BOOL qdpSetCtrlTimeout(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->timeout.ctrl = value;
    return TRUE;
}

BOOL qdpSetDataTimeout(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->timeout.data = value;
    return TRUE;
}

BOOL qdpSetRetryInterval(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->interval.retry = value;
    return TRUE;
}

BOOL qdpSetOpenInterval(QDP_PAR *par, UINT32 value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->interval.open = value;
    return TRUE;
}

BOOL qdpSetHeartbeat(QDP_PAR *par, UINT32 interval, UINT32 bitmap, BOOL forward)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->hbeat.interval = interval;
    par->hbeat.bitmap = bitmap;
    par->hbeat.forward = forward;
    return TRUE;
}

BOOL qdpSetNbuf(QDP_PAR *par, int value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->nbuf = value;
    return TRUE;
}

BOOL qdpSetTrash(QDP_PAR *par, BOOL pkts, BOOL link)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->trash.pkts = pkts;
    par->trash.link = link;
    return TRUE;
}

BOOL qdpSetDebug(QDP_PAR *par, int value)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    if (value < QDP_TERSE) value = QDP_TERSE;
    if (value > QDP_DEBUG) value = QDP_DEBUG;
    MUTEX_LOCK(&par->mutex);
        par->debug = value;
    MUTEX_UNLOCK(&par->mutex);
    return TRUE;
}

BOOL qdpSetUser(QDP_PAR *par, void *arg, QDP_USER_FUNC func)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->user.arg = arg;
    par->user.func = func;
    return TRUE;
}

BOOL qdpSetMeta(QDP_PAR *par, void *arg, QDP_META_FUNC func)
{
    if (par == NULL) {
        errno = EINVAL;
        return FALSE;
    }
    par->meta.arg = arg;
    par->meta.func = func;
    return TRUE;
}

/* Log contents */

void qdpLogPar(QDP *qp)
{
char *forward;

    if (qp == NULL) return;


    qdpInfo(qp, "          host = %s", qp->par.host);
    qdpInfo(qp, "      serialno = %016llX%", qp->par.serialno);
    qdpInfo(qp, "      authcode = %016llX%", qp->par.authcode);
    qdpInfo(qp, "     base port = %d", qp->par.port.base);
    qdpInfo(qp, "          port = %s", qdpPortString(qp->par.port.link));
    qdpInfo(qp, "     ctrl port = %d", qp->par.port.ctrl.value);
    qdpInfo(qp, "  ctrl timeout = %lu", qp->par.timeout.ctrl / MSEC_PER_SEC);
    qdpInfo(qp, "     data port = %d", qp->par.port.data.value);
    qdpInfo(qp, "  data timeout = %lu", qp->par.timeout.data / MSEC_PER_SEC);
    qdpInfo(qp, "retry interval = %lu", qp->par.interval.retry / MSEC_PER_SEC);
    qdpInfo(qp, " open interval = %lu", qp->par.interval.open / MSEC_PER_SEC);
    if (qp->par.hbeat.interval) {
        qdpInfo(qp, "hbeat interval = %lu", qp->par.hbeat.interval / MSEC_PER_SEC);
        forward = qp->par.hbeat.forward ? "forwarded" : "not forwarded";
        qdpInfo(qp, " status bitmap = 0x%04x, %s", qp->par.hbeat.bitmap, forward);
    } else {
        qdpInfo(qp, "hbeat interval = n/a (no automatic heartbeat)");
    }
    qdpInfo(qp, "          nbuf = %d", qp->par.nbuf);
    qdpInfo(qp, "         debug = %s", qdpDebugString(qp->par.debug));
    qdpInfo(qp, "  pkt function = %s", qp->par.user.func == NULL ? "disabled" : "enabled");
    qdpInfo(qp, " meta function = %s", qp->par.meta.func == NULL ? "disabled" : "enabled");
    if (qp->par.trash.pkts && qp->par.trash.pkts) {
        qdpInfo(qp, "         trash = pkts, link");
    } else if (qp->par.trash.pkts) {
        qdpInfo(qp, "         trash = pkts");
    } else if (qp->par.trash.link) {
        qdpInfo(qp, "         trash = link");
    }
}

/* Print contents */

void qdpPrintPar(FILE *fp, QDP *qp)
{
char *forward;

    if (qp == NULL) return;

    fprintf(fp, "          host = %s\n", qp->par.host);
    fprintf(fp, "      serialno = %016llX%\n", qp->par.serialno);
    fprintf(fp, "      authcode = %016llX%\n", qp->par.authcode);
    fprintf(fp, "     base port = %d\n", qp->par.port.base);
    fprintf(fp, "          port = %s\n", qdpPortString(qp->par.port.link));
    fprintf(fp, "     ctrl port = %d\n", qp->par.port.ctrl.value);
    fprintf(fp, "  ctrl timeout = %lu\n", qp->par.timeout.ctrl / MSEC_PER_SEC);
    fprintf(fp, "     data port = %d\n", qp->par.port.data.value);
    fprintf(fp, "  data timeout = %lu\n", qp->par.timeout.data / MSEC_PER_SEC);
    fprintf(fp, "retry interval = %lu\n", qp->par.interval.retry / MSEC_PER_SEC);
    fprintf(fp, " open interval = %lu\n", qp->par.interval.open / MSEC_PER_SEC);
    if (qp->par.hbeat.interval) {
        fprintf(fp, "hbeat interval = %lu\n", qp->par.hbeat.interval / MSEC_PER_SEC);
        forward = qp->par.hbeat.forward ? "forwarded" : "not forwarded";
        fprintf(fp, " status bitmap = 0x%04x, %s\n", qp->par.hbeat.bitmap, forward);
    } else {
        fprintf(fp, "hbeat interval = n/a (no automatic heartbeat)\n");
    }
    fprintf(fp, "          nbuf = %d\n", qp->par.nbuf);
    fprintf(fp, "         debug = %s\n", qdpDebugString(qp->par.debug));
    fprintf(fp, "  pkt function = %s\n", qp->par.user.func == NULL ? "disabled" : "enabled");
    fprintf(fp, " meta function = %s\n", qp->par.meta.func == NULL ? "disabled" : "enabled");
    if (qp->par.trash.pkts && qp->par.trash.pkts) {
        fprintf(fp, "         trash = pkts, link\n");
    } else if (qp->par.trash.pkts) {
        fprintf(fp, "         trash = pkts\n");
    } else if (qp->par.trash.link) {
        fprintf(fp, "         trash = link\n");
    }
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
 * $Log: par.c,v $
 * Revision 1.13  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
