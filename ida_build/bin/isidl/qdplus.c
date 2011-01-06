#pragma ident "$Id: qdplus.c,v 1.7 2007/02/08 22:52:46 dechavez Exp $"
/*======================================================================
 *
 * QDPLUS packet support
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_QDPLUS

/* Offsets to QDPLUS_PKT header fields */

#define SERIALNO_OFFSET 0
#define SEQNO_OFFSET    8
#define PACKET_OFFSET   QDPLUS_HDRLEN

/* Add digitizer serial to QDPLUS payload */

void InsertQdplusSerialno(ISI_RAW_PACKET *raw, UINT64 serialno)
{
    utilPackUINT64(&raw->payload[SERIALNO_OFFSET], serialno);
}

/* Add sequence number to QDPLUS payload */

void CompleteQdplusHeader(ISI_RAW_PACKET *raw, ISI_DL *dl)
{
UINT8 *ptr;

    ptr = &raw->payload[SEQNO_OFFSET];
    ptr += utilPackUINT32(ptr, dl->sys->seqno.signature);
    ptr += utilPackUINT64(ptr, dl->sys->seqno.counter);
}

/* Copy QDP packet into QDPLUS payload */

void CopyQDPToQDPlus(ISI_RAW_PACKET *raw, QDP_PKT *src)
{
static char *fid = "CopyQDPToQDPlus";

    memcpy(&raw->payload[QDPLUS_HDRLEN], src->raw, src->len);
    raw->hdr.len.used = src->len + QDPLUS_HDRLEN;
}

/* Start a new packet */

static INT32 *StartPacket(ISI_RAW_PACKET *raw, ISIDL_TIMESTAMP *ttag, BAROMETER *bp)
{
UINT8 *qdp, *aux, *ptr;
static char *fid = "StartPacket";

/* Find the start of the QDP packet */

    qdp = &raw->payload[QDPLUS_HDRLEN];
    memset(qdp, 0, QDP_MAX_MTU);

/* Find the start of the QDP packet payload */

    aux = qdp + QDP_CMNHDR_LEN;

/* Stuff in the QDP common header (less payload length) **/

    ptr = qdp;
    ptr += sizeof(UINT32); /* skip over crc */
    *ptr++ = QDP_DT_USER;
    *ptr++ = QDP_VERSION;
    ptr += sizeof(UINT16); /* skip over dlen */
    ptr += utilPackUINT16(ptr, ++bp->format.qdplus.seqno);
    ptr += sizeof(UINT16); /* skip over ack */

/* ptr points to start of QDP packet payload area.  It begins with the QDPLUS_DT_USER_AUX header. */

    ptr = aux;
    *ptr++ = QDPLUS_DT_USER_AUX;                                  /* user defined aux data */
    ptr++;                                                        /* skip over reserved */             
    *ptr++ = ISI_TYPE_INT32;                                      /* datum type */
    ptr += utilPackBytes(ptr, bp->format.qdplus.chn, ISI_CHNLEN); /* channel name */
    ptr += utilPackBytes(ptr, bp->format.qdplus.loc, ISI_LOCLEN); /* location code */
    ptr += utilPackUINT16(ptr, 1000);                             /* hard 1 sample per second */
    ptr += utilPackUINT32(ptr, ttag->sys);                        /* beg sys time */
    ptr += utilPackUINT32(ptr, ttag->pps);                        /* beg pps time */
    ptr += utilPackUINT32(ptr, ttag->ext);                        /* beg ext time */
    *ptr++ = ttag->code;                                          /* beg time qual code */

/* Find the start of the sample data */

#define DT_USER_AUX_HEADER_LEN 40
    return (INT32 *) aux + DT_USER_AUX_HEADER_LEN;
}

static void FinishPacket(ISI_RAW_PACKET *raw, UINT16 nsamp, ISIDL_TIMESTAMP *ttag)
{
UINT32 crc;
UINT16 dlen;
UINT8 *qdp, *aux, *ptr;
static char *fid = "FinishPacket";

/* Find the start of the QDP packet */

    qdp = &raw->payload[QDPLUS_HDRLEN];

/* Find the start of the QDP packet payload */

    aux = qdp + QDP_CMNHDR_LEN;

/* Stuff in the end time and total number of samples */

#define OFFSET_TO_DT_AUX_END_TIME 24

    ptr = aux + OFFSET_TO_DT_AUX_END_TIME;
    ptr += utilPackUINT32(ptr, ttag->sys);   /* end sys time */
    ptr += utilPackUINT32(ptr, ttag->pps);   /* end pps time */
    ptr += utilPackUINT32(ptr, ttag->ext);   /* end ext time */
    *ptr++ = ttag->code;                     /* end time qual code */
    ptr++;                                   /* skip over reserved */
    ptr += utilPackUINT16(ptr, nsamp);       /* nsamp */

/* Figure out total QDP payload length and stuff it in the QDP common header */

    dlen = DT_USER_AUX_HEADER_LEN + (nsamp * sizeof(UINT32));

#define OFFSET_TO_QDP_DLEN 6

    ptr = qdp + OFFSET_TO_QDP_DLEN;
    ptr += utilPackUINT16(ptr, dlen);

/* Compute and insert the CRC */

    crc = qdpCRC(qdp + 4, dlen + QDP_CMNHDR_LEN - 4);
    utilPackUINT32(qdp, crc);

/* Figure out the total length of the packet */

    raw->hdr.len.used = QDPLUS_HDRLEN + QDP_CMNHDR_LEN + dlen;
}

static void StuffSample(BAROMETER *bp, ISIDL_TIMESTAMP *ttag, INT32 value)
{
char timestring[32];
static char *fid = "qdplus:StuffSample";

/* Since there are some startup delays in the barometer reading thread,
 * we will discard all "missed" samples until we've seen one valid
 * sample come through here.  That will prevent the first packet from
 * having a couple of unwarrented "missed" samples at the front.
 */

    if (!bp->ValidSampleSeen) {
        if (value == MISSED_BAROMETER_SAMPLE) {
            return;
        } else {
            bp->ValidSampleSeen = TRUE;
        }
    }

    if (bp->nsamp == 0) {
        InsertQdplusSerialno(&bp->local.raw, bp->format.qdplus.serialno);
        bp->out = StartPacket(&bp->local.raw, ttag, bp);
    }

    bp->out[bp->nsamp++] = htonl(value);

    if (BarometerDebugEnabled()) {
        utilLttostr(ttag->sys, 0, timestring);
        LogMsg(LOG_INFO, "%s %3d/%d: %s %ld\n", bp->param.sn, bp->nsamp, bp->maxsamp, timestring, value);
    }

    if (bp->nsamp == bp->maxsamp) {
        FinishPacket(&bp->local.raw, (UINT16) bp->nsamp, ttag);
        bp->nsamp = 0;
        ProcessLocalData(&bp->local);
    }
}

THREAD_FUNC QdplusBarometerThread(void *argptr)
{
INT32 i, value, missed;
struct {
    ISIDL_TIMESTAMP prev;
    ISIDL_TIMESTAMP crnt;
} ttag;
BAROMETER *bp;
static char *fid = "QdplusBarometerThread";

    LogMsg(LOG_DEBUG, "%s started", fid);

    bp = (BAROMETER *) argptr;

    bp->nsamp = 0;

/* Time tag the current sample and stuff it into the buffer */

    GetTimeStamp(&ttag.prev);
    while (1) {
        GetTimeStamp(&ttag.crnt);
        if (ttag.crnt.sys > ttag.prev.sys) {
            value = GetSample(bp);
            missed = ttag.crnt.sys - ttag.prev.sys - 1;
            for (i = 0; i < missed; i++) {
                ++ttag.prev.sys;
                StuffSample(bp, &ttag.prev, MISSED_BAROMETER_SAMPLE);
            }
            StuffSample(bp, &ttag.crnt, value);
            ttag.prev = ttag.crnt;
        }
        utilDelayMsec(250);
    }
}

/* Revision History
 *
 * $Log: qdplus.c,v $
 * Revision 1.7  2007/02/08 22:52:46  dechavez
 * use LOCALPKT handle
 *
 * Revision 1.6  2006/12/12 23:12:39  dechavez
 * removed all the misguided metadata stuff
 *
 * Revision 1.5  2006/12/08 17:36:48  dechavez
 * support for streamlined QDPLUS_PKT
 *
 * Revision 1.4  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.3  2006/06/23 18:29:40  dechavez
 * changed thread initialization message to debug
 *
 * Revision 1.2  2006/06/07 22:37:58  dechavez
 * fixed incorrect maxsamp assignment and a pointer problem in FinishPacket
 *
 * Revision 1.1  2006/06/02 21:09:07  dechavez
 * initial release
 *
 */
