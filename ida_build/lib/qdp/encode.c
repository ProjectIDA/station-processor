#pragma ident "$Id: encode.c,v 1.13 2009/02/23 21:56:30 dechavez Exp $"
/*======================================================================
 *
 * Pack various data structures into QDP packets
 *
 *====================================================================*/
#include "qdp.h"

static int Encode_DP_IPR(UINT8 *start, QDP_DP_IPR *src)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, src->ip);
    ptr += utilPackUINT16(ptr, src->port);
    ptr += utilPackUINT16(ptr, src->registration);

    return (int) (ptr - start);
}

void qdpEncode_C1_SRVRSP(QDP_PKT *pkt, QDP_TYPE_C1_SRVRSP *src)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SRVRSP, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(ptr, src->serialno);
    ptr += utilPackUINT64(ptr, src->challenge);
    ptr += Encode_DP_IPR(ptr, &src->dp);
    ptr += utilPackUINT64(ptr, src->random);
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, src->md5[i]);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_DSRV(QDP_PKT *pkt, UINT64 serialno)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_DSRV, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(pkt->payload, serialno);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQSRV(QDP_PKT *pkt, UINT64 serialno)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQSRV, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT64(pkt->payload, serialno);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQFLGS(QDP_PKT *pkt, UINT16 dataport)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQFLGS, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT16(pkt->payload, dataport);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQSTAT(QDP_PKT *pkt, UINT32 bitmap)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQSTAT, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(pkt->payload, bitmap);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_CTRL(QDP_PKT *pkt, UINT16 flags)
{
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_CTRL, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT16(pkt->payload, flags);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_RQMEM(QDP_PKT *pkt, QDP_TYPE_C1_RQMEM *src)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_RQMEM, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(ptr, src->offset);
    ptr += utilPackUINT16(ptr, src->nbyte);
    ptr += utilPackUINT16(ptr, src->type);
    for (i = 0; i < 4; i++) ptr += utilPackUINT32(ptr, src->passwd[i]);
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

}

void qdpEncode_C1_QCAL(QDP_PKT *pkt, QDP_TYPE_C1_QCAL *src)
{
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_QCAL, 0, 0);

    ptr = pkt->payload;
    ptr += utilPackUINT32(ptr, src->starttime);
    ptr += utilPackUINT16(ptr, src->waveform);
    ptr += utilPackUINT16(ptr, src->amplitude);
    ptr += utilPackUINT16(ptr, src->duration);
    ptr += utilPackUINT16(ptr, src->settle);
    ptr += utilPackUINT16(ptr, src->chans);
    ptr += utilPackUINT16(ptr, src->trailer);
    ptr += utilPackUINT16(ptr, src->sensor);
    ptr += utilPackUINT16(ptr, src->monitor);
    ptr += utilPackUINT16(ptr, src->divisor);
    ptr += 2; /* skip over spare */
    ptr += 4; /* skip over coupling bytes 0 -  3 */
    ptr += 4; /* skip over coupling bytes 4 -  7 */
    ptr += 4; /* skip over coupling bytes 8 - 11 */
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_NoParCmd(QDP_PKT *pkt, int command)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, command, 0, 0);

    ptr = pkt->payload;
    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;

}

void qdpEncode_C1_PHY(QDP_PKT *pkt, QDP_TYPE_C1_PHY *src)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || src == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SPHY, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT64(ptr, src->serialno);
    for (i = 0; i < 3; i++) ptr += utilPackUINT32(ptr, src->serial[i].ip);
    ptr += utilPackUINT32(ptr, src->ethernet.ip);
    for (i = 0; i < 6; i++) *ptr++ = src->ethernet.mac[i];
    ptr += utilPackUINT16(ptr, src->baseport);
    for (i = 0; i < 3; i++) {
        ptr += utilPackUINT16(ptr, src->serial[i].baud);
        ptr += utilPackUINT16(ptr, src->serial[i].flags);
    }
    ptr += 2; /* skip over reserved */
    ptr += utilPackUINT16(ptr, src->ethernet.flags);
    for (i = 0; i < 3; i++) ptr += utilPackUINT16(ptr, src->serial[i].throttle);
    ptr += 2; /* skip over reserved */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_PULSE(QDP_PKT *pkt, UINT16 bitmap, UINT16 duration)
{
int i;
UINT8 *ptr;

    if (pkt == NULL) return;

    qdpInitPkt(pkt, QDP_C1_PULSE, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, bitmap);
    ptr += utilPackUINT16(ptr, duration);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_SC(QDP_PKT *pkt, UINT32 *sc)
{
int i;
UINT8 *ptr;

    if (pkt == NULL || sc == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SSC, 0, 0);

    ptr = pkt->payload;

    for (i = 0; i < QDP_NSC; i++) ptr += utilPackUINT32(ptr, sc[i]);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_GLOB(QDP_PKT *pkt, QDP_TYPE_C1_GLOB *glob)
{
int chan, freq;
UINT8 *ptr;

    if (pkt == NULL || glob == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SGLOB, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, glob->clock_to);
    ptr += utilPackUINT16(ptr, glob->initial_vco);
    ptr += utilPackUINT16(ptr, glob->gps_backup);
    ptr += utilPackUINT16(ptr, glob->samp_rates);
    ptr += utilPackUINT16(ptr, glob->gain_map);
    ptr += utilPackUINT16(ptr, glob->filter_map);
    ptr += utilPackUINT16(ptr, glob->input_map);
    ptr += utilPackUINT16(ptr, glob->web_port);
    ptr += utilPackUINT16(ptr, glob->server_to);
    ptr += utilPackUINT16(ptr, glob->drift_tol);
    ptr += utilPackUINT16(ptr, glob->jump_filt);
    ptr += utilPackUINT16(ptr, glob->jump_thresh);
    ptr += utilPackINT16(ptr, glob->cal_offset);
    ptr += utilPackUINT16(ptr, glob->sensor_map);
    ptr += utilPackUINT16(ptr, glob->sampling_phase);
    ptr += utilPackUINT16(ptr, glob->gps_cold);
    ptr += utilPackUINT32(ptr, glob->user_tag);
    for (chan = 0; chan < QDP_NCHAN; chan++) {
        for (freq = 0; freq < QDP_NFREQ; freq++) {
            ptr += utilPackUINT16(ptr, glob->scaling[chan][freq]);
        }
    }
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilPackUINT16(ptr, glob->offset[chan]);
    for (chan = 0; chan < QDP_NCHAN; chan++) ptr += utilPackUINT16(ptr, glob->gain[chan]);
    ptr += utilPackUINT32(ptr, glob->msg_map);

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C1_SPP(QDP_PKT *pkt, QDP_TYPE_C1_SPP *spp)
{
UINT8 *ptr;

    if (pkt == NULL || spp == NULL) return;

    qdpInitPkt(pkt, QDP_C1_SSPP, 0, 0);

    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, spp->max_main_current);
    ptr += utilPackUINT16(ptr, spp->min_off_time);
    ptr += utilPackUINT16(ptr, spp->min_ps_voltage);
    ptr += utilPackUINT16(ptr, spp->max_antenna_current);
    ptr += utilPackINT16( ptr, spp->min_temp);
    ptr += utilPackINT16( ptr, spp->max_temp);
    ptr += utilPackUINT16(ptr, spp->temp_hysteresis);
    ptr += utilPackUINT16(ptr, spp->volt_hysteresis);
    ptr += utilPackUINT16(ptr, spp->default_vco);
    ptr += 2; /* skip over spare */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
}

void qdpEncode_C2_GPS(QDP_PKT *pkt, QDP_TYPE_C2_GPS *gps)
{
UINT8 *ptr;

    if (pkt == NULL || gps == NULL) return;

    qdpInitPkt(pkt, QDP_C2_SGPS, 0, 0);
    ptr = pkt->payload;

    ptr += utilPackUINT16(ptr, gps->mode);
    ptr += utilPackUINT16(ptr, gps->flags);
    ptr += utilPackUINT16(ptr, gps->off_time);
    ptr += utilPackUINT16(ptr, gps->resync);
    ptr += utilPackUINT16(ptr, gps->max_on);
    ptr += utilPackUINT16(ptr, gps->lock_usec);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += utilPackUINT16(ptr, gps->interval);
    ptr += utilPackUINT16(ptr, gps->initial_pll);
    ptr += utilPackREAL32(ptr, gps->pfrac);
    ptr += utilPackREAL32(ptr, gps->vco_slope);
    ptr += utilPackREAL32(ptr, gps->vco_intercept);
    ptr += utilPackREAL32(ptr, gps->max_ikm_rms);
    ptr += utilPackREAL32(ptr, gps->ikm_weight);
    ptr += utilPackREAL32(ptr, gps->km_weight);
    ptr += utilPackREAL32(ptr, gps->best_weight);
    ptr += utilPackREAL32(ptr, gps->km_delta);
    ptr += sizeof(UINT32); /* skip over spare */
    ptr += sizeof(UINT32); /* skip over spare */

    pkt->hdr.dlen = (int) (ptr - pkt->payload);
    pkt->len = QDP_CMNHDR_LEN + pkt->hdr.dlen;
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
 * $Log: encode.c,v $
 * Revision 1.13  2009/02/23 21:56:30  dechavez
 * added qdpEncode_C1_SPP(), qdpEncode_C2_GPS()
 *
 * Revision 1.12  2009/01/24 00:08:09  dechavez
 * added qdpEncode_C1_PULSE(), qdpEncode_C1_SC(), qdpEncode_C1_GLOB()
 *
 * Revision 1.11  2009/01/06 20:46:00  dechavez
 * added qdpEncode_C1_PHY()
 *
 * Revision 1.10  2008/10/02 22:50:42  dechavez
 * added qdpEncode_NoParCmd
 *
 * Revision 1.9  2007/10/31 17:12:16  dechavez
 * added qdpEncode_C1_QCAL()
 *
 * Revision 1.8  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
