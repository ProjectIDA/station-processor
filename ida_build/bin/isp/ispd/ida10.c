#pragma ident "$Id: ida10.c,v 1.7 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Build (fixed length) IDA10 packets.  
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2002 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_IDA10

#define MAXSAMP 240
#define OVERHEAD 14
#define NBYTES (OVERHEAD + (MAXSAMP * 4))

extern ISP_PARAMS *Params;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;
extern struct isp_status *Status;

static void LoadTimeTag(UINT8 *start, ISPD_AUX_DATUM *sample)
{
UINT16 status;
UINT8  *ptr;
time_t revised_ext, epoch;

/* Feeble attempt to set the status word, not easy since we really
 * have no idea what the "qual" byte really means.
 */

    status  = 0;
    status |= SANIO_TT_STAT_EXTTIME_AVAIL;
    status |= (sample->qual << 9);

/* Adjust external time so that it would appear that the external time
 * was sampled at the system time, hence producing a zero offset between
 * 1Hz time and system time.
 */

    revised_ext = sample->ext_time + (sample->sys_time - sample->hz_time);
    epoch = revised_ext - SAN_EPOCH_TO_1970_EPOCH;

/* Stuff everything into the time tag */

    ptr = start;

    ptr += utilPackINT32(ptr, revised_ext);      /* external time     */
    ptr += utilPackINT32(ptr, sample->sys_time); /* system time, sec  */
    ptr += utilPackINT16(ptr, 0);                /* system time, msec */
    ptr += utilPackINT16(ptr, status);           /* status            */
    ptr += utilPackINT16(ptr, 0);                /* pll               */
    ptr += utilPackINT16(ptr, 0);                /* phase             */
    ptr += utilPackINT32(ptr, epoch);            /* epoch time, sec   */
    ptr += utilPackINT16(ptr, 0);                /* epoch time, msec  */
}

/* Start a new IDA10 data packet */

static long *StartIDA10Packet(
    ISPD_AUX_HANDLE *ap, ISPD_AUX_DATUM *sample, UINT8 *buf
){
INT16 fact, mult;
char ten = 10, one = 1;

/* Common header */

    utilPackBytes(buf+0, (UINT8 *) "TS", 2);
    utilPackBytes(buf+2, (UINT8 *) &ten, 1);  /* format 10    */
    utilPackBytes(buf+3, (UINT8 *) &one, 1);  /* sub-format 1 */
    utilPackINT16(buf+4, 0); /* box id n/a */
    LoadTimeTag(buf+6, sample);
    utilPackBytes(buf+28, (UINT8 *) "** IDA10.1 Format **", 20);
    utilPackINT16(buf+48, NBYTES);

/* Channel name */

    utilPackBytes(buf+50, (UINT8 *) ap->par->name, 6);

/* Data format/status descriptor */

    buf[56] = 0;
    buf[56] |= IDA10_COMP_NONE;
    buf[56] |= IDA10_A2D_24;
    buf[56] |= IDA10_DATA_INT32;
    buf[56] |= IDA10_CONT;

/* Conversion gain */

    buf[57] = ap->par->multiplier;

/* Number of samples */

    utilPackINT16(buf+58, MAXSAMP);

/* Nominal sample rate */

    ida10SintToFactMult((REAL64) ap->par->sint, &fact, &mult);
    utilPackINT16(buf+60, fact);
    utilPackINT16(buf+62, mult);

/* Return pointer to data portion */

    return (long *) (buf + 64);
}

/* Generate IDA10 packets */

THREAD_FUNC AuxDataToIDA10Thread(void *argptr)
{
long *out;
int  nsamp;
OLD_MSGQ_MSG *SampleMsg, *BufferMsg;
ISPD_AUX_DATUM sample;
ISPD_AUX_HANDLE *ap;
static char *fid = "AuxDataToIDA10Thread";

    util_log(2, "%s thread started, tid = %d", fid, THREAD_SELF());

    ap = (ISPD_AUX_HANDLE *) argptr;

    nsamp = 0;

    while (1) {

    /* Grap the next aux sample */

        SampleMsg = msgq_get(ap->q.full, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, SampleMsg)) {
            util_log(1, "%s: corrupt message received", fid);
            ispd_die(MY_MOD_ID + 4);
        }

    /* Copy it to local storage */

        sample = *((ISPD_AUX_DATUM *) SampleMsg->data);
        msgq_put(ap->q.empty, SampleMsg);

    /* If starting a new packet, grab empty buffer from heap and init */

        if (nsamp == 0) {
            BufferMsg = msgq_get(&Heap->packets, OLD_MSGQ_WAITFOREVER);
            if (!msgq_chkmsg2(fid, BufferMsg)) {
                util_log(1, "%s: corrupt message received", fid);
                ispd_die(MY_MOD_ID + 1);
            }
            out = StartIDA10Packet(ap, &sample, (UINT8 *) BufferMsg->data);
        }

    /* Append uncompressed samples to packet until done */

        out[nsamp++] = htonl(sample.value);
        if (nsamp == MAXSAMP) {
            BufferMsg->len = IDA10_FIXEDRECLEN;
            msgq_put(&Q->das_process, BufferMsg);
            MUTEX_LOCK(&Status->lock);
                ++ap->stat->nrec;
            MUTEX_UNLOCK(&Status->lock);
            nsamp = 0;
        }
    }
}

/* Revision History
 *
 * $Log: ida10.c,v $
 * Revision 1.7  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.6  2006/11/13 20:01:14  dechavez
 * use SANIO_TT_x macros instead of deprecated IDA10_TT
 *
 * Revision 1.5  2003/12/10 06:25:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.4  2003/10/02 20:10:44  dechavez
 * changed to IDA10_TYPE_INT32
 *
 * Revision 1.3  2003/05/23 19:52:19  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.2  2002/09/10 17:48:42  dec
 * removed uneeded debug code
 *
 * Revision 1.1  2002/09/09 21:57:16  dec
 * created
 *
 */
