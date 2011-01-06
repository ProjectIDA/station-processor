#pragma ident "$Id: packet.c,v 1.2 2009/05/14 16:35:39 dechavez Exp $"
/*======================================================================
 *
 *  Build IDA10 packets and write to disk loop.
 *
 *====================================================================*/
#include "parodl.h"

#define MY_MOD_ID PARODL_MOD_PACKET

/* Fill in the IDA10.5 header */

static void StartNewPacket(BAROMETER *bp)
{
#define STRBUFLEN 256
char strbuf[STRBUFLEN];
UINT8 *ptr, descriptor;

    ptr = bp->pkt.payload;

    /* 10.5 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 5;  /* subformat 5 */
    ptr += utilPackBytes(ptr, bp->sname, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, bp->nname, IDA105_NNAME_LEN);
    ptr += ida10PackGenericTtag(ptr, &bp->clock);
    ptr += IDA105_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* stream name, a concatenation of channel and location codes */

    ptr += utilPackBytes(ptr, bp->cname, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    ptr += utilPackUINT16(ptr, bp->maxsamp);

    /* sample rate */

    ptr += utilPackINT16(ptr, bp->srfact);
    ptr += utilPackINT16(ptr, bp->srmult);

}

static void StuffSample(BAROMETER *bp, UINT64 sint, INT32 value)
{
int offset;
#define STRBUFLEN 256
char strbuf[STRBUFLEN];

    if (bp->nsamp == 0) StartNewPacket(bp);
    offset = IDA10_FIXEDHDRLEN + (bp->nsamp*sizeof(INT32));
    utilPackINT32(bp->pkt.payload + offset, value);

    if (BarometerDebugEnabled()) {
        if (value != PARODL_MISSED_SAMPLE) {
            LogMsg(LOG_INFO, "append %s value=%ld, nsamp = %d/%d", bp->cname, value, bp->nsamp+1, bp->maxsamp);
        } else {
            LogMsg(LOG_INFO, "append %s value=**MISSED**, nsamp = %d/%d", bp->cname, value, bp->nsamp+1, bp->maxsamp);
        }
    }

    bp->clock.tstamp += sint;
    if (++bp->nsamp == bp->maxsamp) {
        WriteToDiskLoop(&bp->pkt);
        bp->nsamp = 0;
    }
}

THREAD_FUNC PacketThread(void *argptr)
{
BAROMETER *bp;
BOOL ready;
INT32 value, missed, i;
UINT64 now, NanoSint, EpochConversionFactor;
UINT32 ElapsedMsec, PollInterval;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "building %d sample IDA10.5 packets, sample interval = %lu msec", bp->maxsamp, bp->MsecSint);

    PollInterval = (bp->MsecSint / 2) / NANOSEC_PER_MSEC;
    NanoSint = bp->MsecSint * NANOSEC_PER_MSEC;
    EpochConversionFactor = SAN_EPOCH_TO_1970_EPOCH * NANOSEC_PER_SEC;

/* Loop forever, reading and saving readings */

    bp->clock.tstamp = utilTimeStamp() - EpochConversionFactor;
    bp->clock.status.receiver = 0;
    bp->clock.status.generic = IDA10_GENTAG_LOCKED | IDA10_GENTAG_DERIVED;

    while (1) {
        now = utilTimeStamp() - EpochConversionFactor;
        ElapsedMsec = (UINT32) ((now - bp->clock.tstamp) / NANOSEC_PER_MSEC);
        if (ElapsedMsec >= bp->MsecSint) {
            if ((value = GetSample(bp)) == PARODL_NO_SENSOR) {
                bp->nsamp = 0;
                bp->clock.tstamp += NanoSint;
            } else {
                missed = (ElapsedMsec / bp->MsecSint) - 1;
                for (i = 0; i < missed; i++) StuffSample(bp, NanoSint, PARODL_MISSED_SAMPLE);
                StuffSample(bp, NanoSint, value);
            }
        } else {
            utilDelayMsec(PollInterval);
        }
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: packet.c,v $
 * Revision 1.2  2009/05/14 16:35:39  dechavez
 * reduced poll interval by half
 *
 * Revision 1.1  2009/03/24 21:11:40  dechavez
 * initial release
 *
 */
