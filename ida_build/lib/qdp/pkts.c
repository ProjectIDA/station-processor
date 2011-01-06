#pragma ident "$Id: pkts.c,v 1.10 2009/02/03 23:04:03 dechavez Exp $"
/*======================================================================
 * 
 * Simple packet utilities
 *
 *====================================================================*/
#include "qdp.h"

int qdpErrorCode(QDP_PKT *pkt)
{
UINT16 code;

    if (pkt == NULL || pkt->hdr.cmd != QDP_C1_CERR) return QDP_CERR_NOERR;
    utilUnpackUINT16(pkt->payload, &code);

    return (int) code;
}

void qdpInitPkt(QDP_PKT *pkt, UINT8 cmd, UINT16 seqno, UINT16 ack)
{
    memset(pkt->raw, 0, QDP_MAX_MTU);
    pkt->payload = &pkt->raw[QDP_CMNHDR_LEN];
    pkt->hdr.cmd = cmd;
    pkt->hdr.dlen = 0;
    pkt->hdr.seqno = seqno;
    pkt->hdr.ack = ack;
    pkt->hdr.ver = QDP_VERSION;
    pkt->len = QDP_CMNHDR_LEN;
}

void qdpCopyPkt(QDP_PKT *dest, QDP_PKT *src)
{
static char *fid = "qdpCopyPkt";

    memcpy(dest, src, sizeof(QDP_PKT));
    dest->payload = &dest->raw[QDP_CMNHDR_LEN];
}

int qdpReadPkt(FILE *fp, QDP_PKT *pkt)
{
    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (fread(pkt->raw, 1, QDP_CMNHDR_LEN, fp) != QDP_CMNHDR_LEN) {
        if (feof(fp)) {
            return 0;
        } else {
            return -2;
        }
    }

    qdpNetToHost(pkt);

    if (pkt->hdr.dlen > QDP_MAX_PAYLOAD) {
        errno = EFAULT;
        return -3;
    }

    if (fread(pkt->raw + QDP_CMNHDR_LEN, 1, pkt->hdr.dlen, fp) != pkt->hdr.dlen) return -4;

    return QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

int qdpWritePkt(FILE *fp, QDP_PKT *pkt)
{
    if (fp == NULL || pkt == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (fwrite(pkt->raw, 1, QDP_CMNHDR_LEN, fp) != QDP_CMNHDR_LEN) return -2;
    if (fwrite(pkt->raw + QDP_CMNHDR_LEN, 1, pkt->hdr.dlen, fp) != pkt->hdr.dlen) return -3;

    return QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

BOOL qdp_C1_RQGLOB(QDP *qdp, QDP_TYPE_C1_GLOB *glob)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQGLOB, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_GLOB) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_GLOB(pkt.payload, glob);

    return TRUE;
}

BOOL qdp_C1_RQSC(QDP *qdp, UINT32 *sc)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQSC, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_SC) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_SC(pkt.payload, sc);

    return TRUE;
}

BOOL qdp_C1_RQPHY(QDP *qdp, QDP_TYPE_C1_PHY *phy)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQPHY, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_PHY) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_PHY(pkt.payload, phy);

    return TRUE;
}

BOOL qdp_C1_RQFIX(QDP *qdp, QDP_TYPE_C1_FIX *fix)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQFIX, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_FIX) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_FIX(pkt.payload, fix);

    return TRUE;
}

BOOL qdp_C1_RQGID(QDP *qdp, QDP_TYPE_C1_GID *gid)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQGID, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_GID) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_GID(pkt.payload, gid);

    return TRUE;
}

BOOL qdp_C1_RQDCP(QDP *qdp, QDP_TYPE_C1_DCP *dcp)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQDCP, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;

    if (pkt.hdr.cmd != QDP_C1_DCP) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_DCP(pkt.payload, dcp);

    return TRUE;
}

BOOL qdp_C1_RQSPP(QDP *qdp, QDP_TYPE_C1_SPP *spp)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQSPP, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;

    if (pkt.hdr.cmd != QDP_C1_SPP) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_SPP(pkt.payload, spp);

    return TRUE;
}

BOOL qdp_C1_RQMAN(QDP *qdp, QDP_TYPE_C1_MAN *man)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C1_RQMAN, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;

    if (pkt.hdr.cmd != QDP_C1_MAN) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_MAN(pkt.payload, man);

    return TRUE;
}

BOOL qdp_C2_RQAMASS(QDP *qdp, QDP_TYPE_C2_AMASS *amass)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQAMASS, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C2_AMASS) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C2_AMASS(pkt.payload, amass);

    return TRUE;
}

BOOL qdp_C2_RQGPS(QDP *qdp, QDP_TYPE_C2_GPS *gps)
{
QDP_PKT pkt;

    if (!qdpNoParCmd(qdp, QDP_C2_RQGPS, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C2_GPS) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C2_GPS(pkt.payload, gps);

    return TRUE;
}

BOOL qdp_C1_RQSTAT(QDP *qdp, UINT32 bitmap, QDP_TYPE_C1_STAT *stat)
{
QDP_PKT pkt;

    if (!qdpRqstat(qdp, bitmap, TRUE)) return FALSE;
    if (!qdpNextPkt(qdp, &pkt)) return FALSE;
    if (pkt.hdr.cmd != QDP_C1_STAT) {
        qdp->errcode = QDP_ERR_NOSUCH;
        return FALSE;
    }

    qdpDecode_C1_STAT(pkt.payload, stat);

    return TRUE;
}

BOOL qdpRequestCheckout(QDP *qdp, QDP_TYPE_CHECKOUT *co, char *ident)
{
static UINT32 bitmap = QDP_SRB_BOOM;

    strncpy(co->ident, ident, QDP_IDENTLEN);
    if (!qdp_C1_RQFIX(qdp, &co->fix)) return FALSE;
    if (!qdp_C1_RQGLOB(qdp, &co->glob)) return FALSE;
    if (!qdp_C1_RQDCP(qdp, &co->dcp)) return FALSE;
    if (!qdp_C1_RQSC(qdp,  co->sc)) return FALSE;
    if (!qdp_C1_RQGID(qdp, co->gid)) return FALSE;
    if (!qdp_C2_RQGPS(qdp, &co->gps)) return FALSE;
    if (!qdp_C2_RQAMASS(qdp, &co->amass)) return FALSE;
    if (!qdp_C1_RQSTAT(qdp, bitmap, &co->status)) return FALSE;
    if (!qdp_C1_RQPHY(qdp, &co->phy)) return FALSE;
    if (!qdp_C1_RQSPP(qdp, &co->spp)) return FALSE;
    if (!qdp_C1_RQMAN(qdp, &co->man)) return FALSE;
    return TRUE;
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
 * $Log: pkts.c,v $
 * Revision 1.10  2009/02/03 23:04:03  dechavez
 * added qdpRequestCheckout(), qdp_C1_RQSTAT(), qdp_C2_RQGPS(), qdp_C1_RQMAN,
 * qdp_C1_RQSPP(), qdp_C1_RQDCP(), qdp_C1_RQGID(), qdp_C1_RQFIX(), qdp_C1_RQPHY(),
 * qdp_C1_RQSC(), qdp_C1_RQGLOB()
 *
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
