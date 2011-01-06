#pragma ident "$Id: connect.c,v 1.6 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * High level intialization
 *
 *====================================================================*/
#include "qdp.h"

QDP *qdpConnectWithPar(QDP_PAR *par, LOGIO *lp)
{
QDP *qp;
static char *fid = "qdpConnectWithPar";

    if (par == NULL) {
        logioMsg(lp, LOG_ERR, "%s: NULL par not allowed", fid);
        return NULL;
    }

    if ((qp = (QDP *) malloc(sizeof(QDP))) == NULL) {
        logioMsg(lp, LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
        return NULL;
    }
    if (!qdpInit(qp, par, lp)) {
        logioMsg(lp, LOG_ERR, "%s: qdpInit failed", fid);
        free(qp);
        return NULL;
    }

    qdpLogPar(qp);

    if (!qdpStartFSA(qp)) {
        qdpError(qp, "%s: qdpStartFSA failed", fid);
        free(qp);
        return NULL;
    }

    return qp;

}

QDP *qdpConnect(char *host, UINT64 serialno, UINT64 authcode, int port, int debug, LOGIO *lp)
{
QDP_PAR par;
static char *fid = "qdpConnect";

    qdpInitPar(&par, port);
    if (!qdpSetHost(&par, host)) {
        logioMsg(lp, LOG_ERR, "%s: qdpSetHost failed", fid);
        return NULL;
    }
    if (!qdpSetSerialno(&par, serialno)) {
        logioMsg(lp, LOG_ERR, "%s: qdpSetSerialno failed", fid);
        return NULL;
    }
    if (!qdpSetAuthcode(&par, authcode)) {
        logioMsg(lp, LOG_ERR, "%s: qdpSetAuthcode failed", fid);
        return NULL;
    }
    if (!qdpSetDebug(&par, debug)) {
        logioMsg(lp, LOG_ERR, "%s: qdpSetDebug failed", fid);
        return NULL;
    }

    return qdpConnectWithPar(&par, lp);
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
 * $Log: connect.c,v $
 * Revision 1.6  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
