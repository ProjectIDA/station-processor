#pragma ident "$Id: ida9.c,v 1.7 2007/03/26 21:51:32 dechavez Exp $"
/*======================================================================
 *
 * Fill IDA9 packets with barometer data and write to disk loop
 *
 * CompleteIda9Header() included here is also used by the ARS thread
 * for filling in the IDA9 specific stuff in the headers.
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_IDA9

#define REV9_TAG_OFFSET     2 /* offest to previously unused field for tag */
#define REV9_SNAME_OFFSET  26 /* offset to station name in raw packet header */
#define REV9_SEQNO_OFFSET  30 /* offset to IDA sequence number in raw packet header */
#define REV9_TSTAMP_OFFSET 60 /* offset to creation time stamp */
#define REV9_TAGLEN 4
#define REV9_SNAMELEN 4

static void InitStaticPayloadFields(BAROMETER *bp)
{
UINT8 *pkt;

    bp->local.raw.hdr.seqno = ISI_UNDEFINED_SEQNO;
    pkt = bp->local.raw.payload;

    pkt[0] = 'A'; pkt[1] = ~pkt[0];                 /* record type */
    utilPackUINT16(&pkt[8], 1000);                  /* sytem time multiplier */
    utilPackUINT16(&pkt[24], 3);                    /* Unit ID */
    utilPackUINT16(&pkt[34], 0);                    /* format (uncompressed) */
    utilPackUINT16(&pkt[36], bp->format.ida9.chan); /* channel code */
    utilPackUINT16(&pkt[38], bp->format.ida9.filt); /* filter code */
    utilPackUINT16(&pkt[56], bp->format.ida9.strm); /* stream code */
}

static INT32 *StartPacket(ISI_RAW_PACKET *raw, ISIDL_TIMESTAMP *ttag)
{
UINT8 *pkt;
UINT32 tstamp;
int yr, da, hr, mn, sc, ms;

    pkt = raw->payload;

    utilTsplit((double) ttag->sys, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[10], tstamp);     /* data start sys time */
    utilPackUINT16(&pkt[6], (UINT16) yr); /* year */

    utilTsplit((double) ttag->ext, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[14], tstamp);     /* data start ext time */

    utilTsplit((double) ttag->pps, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[18], tstamp);     /* data start pps time */

    pkt[22] = ttag->code;                 /* data start time quality code */

    return (INT32 *) (&pkt[64]);
}

static void FinishPacket(ISI_RAW_PACKET *raw, UINT16 nsamp, ISIDL_TIMESTAMP *ttag)
{
UINT8 *pkt;
UINT32 tstamp;
int yr, da, hr, mn, sc, ms;

    pkt = raw->payload;

    utilPackUINT16(&pkt[40], nsamp);      /* number of samples */

    utilTsplit((double) ttag->sys, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[42], tstamp);     /* data start sys time */

    utilTsplit((double) ttag->ext, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[46], tstamp);     /* data start ext time */

    utilTsplit((double) ttag->pps, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    utilPackUINT32(&pkt[50], tstamp);     /* data start pps time */

    pkt[54] = ttag->code;                 /* data start time quality code */
}

static void StuffSample(BAROMETER *bp, ISIDL_TIMESTAMP *ttag, INT32 value)
{
char timestring[32];
static char *fid = "ida9:StuffSample";

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
        InitStaticPayloadFields(bp);
        bp->out = StartPacket(&bp->local.raw, ttag);
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

void CompleteIda9Header(ISI_RAW_PACKET *raw, ISI_DL *dl, BOOL rt593)
{
int i;
UINT8 *ptr;
UINT32 seqno;
time_t tstamp;
static char *tag = IDA_REV9_TAG_0;

/* insert the correct, upper case, station name */

    ptr = &raw->payload[REV9_SNAME_OFFSET];
    memcpy(ptr, dl->sys->site, REV9_SNAMELEN);
    for (i = 0; i < REV9_SNAMELEN; i++) ptr[i] = toupper(ptr[i]);

/* insert the low order bits of the sequence number */

    seqno = (UINT32) dl->sys->seqno.counter;
    utilPackUINT32(&raw->payload[REV9_SEQNO_OFFSET], seqno);

/* insert the creation time stamp into packets that take them */

    if (ida_rtype(raw->payload, 9) == IDA_DATA) {
        tstamp = time(NULL) - SAN_EPOCH_TO_1970_EPOCH;
        utilPackUINT32(&raw->payload[REV9_TSTAMP_OFFSET], tstamp);
    }

/* tag this as an IDA9 packet */

    memcpy(&raw->payload[REV9_TAG_OFFSET], tag, strlen(tag));

/* apply RT593 patch, if applicable */

    if (rt593) RT593PatchTest(raw, dl);
}

THREAD_FUNC Ida9BarometerThread(void *argptr)
{
INT32 i, value, missed;
ISIDL_TIMESTAMP prev, crnt;
BAROMETER *bp;

    bp = (BAROMETER *) argptr;

    bp->maxsamp = IDA_MAXDLEN / sizeof(UINT32);
    bp->nsamp = 0;

/* Time tag the current sample and stuff it into the buffer */

    GetTimeStamp(&prev);
    while (1) {
        GetTimeStamp(&crnt);
        if (crnt.sys > prev.sys) {
            value = GetSample(bp);
            missed = crnt.sys - prev.sys - 1;
            for (i = 0; i < missed; i++) {
                ++prev.sys;
                StuffSample(bp, &prev, MISSED_BAROMETER_SAMPLE);
            }
            StuffSample(bp, &crnt, value);
            prev = crnt;
        }
        utilDelayMsec(250);
    }
}

/* Revision History
 *
 * $Log: ida9.c,v $
 * Revision 1.7  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.6  2007/02/08 22:55:39  dechavez
 * use LOCALPKT handle
 *
 * Revision 1.5  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.4  2006/06/12 21:42:07  dechavez
 * initialize seqno to unitialized in InitStaticPayloadFields (cosmetic change
 * to make debugging easier... seqno gets set for real in isiWriteToDiskLoop)
 *
 * Revision 1.3  2006/06/08 00:15:15  dechavez
 * fixed upper case of station names
 *
 * Revision 1.2  2006/06/02 21:05:17  dechavez
 * changed input to CompleteIda9Header() to be ISI_RAW_PACKET instead of its payload
 *
 * Revision 1.1  2006/03/30 22:08:53  dechavez
 * initial release
 *
 */
