#pragma ident "$Id: ida8.c,v 1.3 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Build IDA8 packets
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

#define MY_MOD_ID ISPD_MOD_IDA8

extern ISP_PARAMS *Params;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;
extern struct isp_status *Status;

#define IDA_HLEN  60
#define NUMBYTE   960
#define MAXOFFSET (NUMBYTE - 1)
#define MAXSAMP   (NUMBYTE / 4)

/* Start a new IDA8 data packet */

static char *StartIDA8Packet(
    ISPD_AUX_HANDLE *ap, ISPD_AUX_DATUM *sample, char *buf
){
u_short sval;
u_long  lval, tstamp;
int yr, da, hr, mn, sc, ms;
static char *fid = "StartIDA8Packet";

    buf[IDA_BUFSIZ-1] = 1; /* output always enabled */ /* IDA10 OK */

/* Record type */

    buf[0] = 'A';
    buf[1] = ~buf[0];

/* System time multiplier */

    sval = htons(1000);
    memcpy(buf + 8, &sval, 2);

/* Data start system time */

    util_tsplit((double) sample->sys_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 10, &lval, 4);

/* Year */

    sval = htons(yr);
    memcpy(buf + 6, &sval, 2);

/* Data start external time */

    util_tsplit((double) sample->ext_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 14, &lval, 4);

/* Data start 1-Hz time */

    util_tsplit((double) sample->hz_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 18, &lval, 4);

/* Data start time quality code */

    buf[22] = sample->qual;

/* Unit ID (3 => ISP) */

    sval = htons(3);
    memcpy(buf + 24, &sval, 2);

/* Station name */

    memcpy(buf + 26, Params->sta, 8);

/* Data format */

#ifdef INCLUDE_IDA8_COMPRESSION_SUPPORT
#error "IDA8 compression is not really supported!"
    if (ap->par->comp) {
        sval = htons(2);
    } else {
        sval = 0;
    }
#else
    sval = 0;
#endif /* INCLUDE_IDA8_COMPRESSION_SUPPORT */

    memcpy(buf + 34, &sval, 2);

/* Channel */

    sval = htons(ap->par->chan);
    memcpy(buf + 36, &sval, 2);

/* Filter */

    sval = htons(ap->par->filt);
    memcpy(buf + 38, &sval, 2);

/* Stream ID */

    sval = htons(ap->par->strm);
    memcpy(buf + 56, &sval, 2);

/* Return pointer to start of data */

    return buf + IDA_HLEN;
}

/* Complete a newly filled IDA8 packet */

static void FinishIDA8Packet(
    ISPD_AUX_HANDLE *ap, int nsamp, ISPD_AUX_DATUM *sample, char *buf
){
u_short sval;
u_long  lval, tstamp;
int yr, da, hr, mn, sc, ms;
static char *fid = "FinishIDA8Packet";

/* Nsamp */

    sval = htons(nsamp);
    memcpy(buf + 40, &sval, 2);

/* Data end system time */

    util_tsplit((double) sample->sys_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 42, &lval, 4);

/* Data end external time */

    util_tsplit((double) sample->ext_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 46, &lval, 4);

/* Data end 1-Hz time */

    util_tsplit((double) sample->hz_time, &yr, &da, &hr, &mn, &sc, &ms);
    tstamp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    lval = htonl(tstamp);
    memcpy(buf + 50, &lval, 4);

/* Data end time quality code */

    buf[54] = sample->qual;
}

/* Generate IDA8 packets */

THREAD_FUNC AuxDataToIDA8Thread(void *argptr)
{
long *out;
int  nsamp;
OLD_MSGQ_MSG *SampleMsg, *BufferMsg;
ISPD_AUX_DATUM sample;
ISPD_AUX_HANDLE *ap;
static char *fid = "AuxDataToIDA8Thread";

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
            out = (long *) StartIDA8Packet(ap, &sample, BufferMsg->data);
        }

    /* Append uncompressed samples to packet until done */

        out[nsamp++] = htonl(sample.value);
        if (nsamp == MAXSAMP) {
            FinishIDA8Packet(ap, nsamp, &sample, BufferMsg->data);
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
 * $Log: ida8.c,v $
 * Revision 1.3  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.2  2002/09/10 17:48:23  dec
 * first working rev
 *
 * Revision 1.1  2002/09/09 21:57:32  dec
 * created
 *
 */
