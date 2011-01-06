#pragma ident "$Id: flushcf.c,v 1.7 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Take a SAN configuration and inject into the massio packet buffer
 *  as a sequence of 1024 byte CF records (IDA10.1)
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"
#include "sanio.h"

#define MY_MOD_ID ISPD_MOD_FLUSHCF

extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;

#define TESTLEN 4
#define CF_BUFLEN (sizeof(SANIO_CONFIG)+TESTLEN)
#define CF_DATA_LEN 970
#define TESTCHAR 'e'
static UINT8 CommonHeader[IDA10_CMNHEADLEN];
static UINT8 CFbuf[CF_BUFLEN];
static int   TotalSegments;
static BOOL  initialized = FALSE;

static int LoadTimeTag(UINT8 *start, time_t tstamp)
{
UINT32 epoch;
UINT8 *ptr;

    ptr = start;

    epoch = tstamp - SAN_EPOCH_TO_1970_EPOCH;

    ptr += utilPackINT32(ptr, 0);     /* external time     */
    ptr += utilPackINT32(ptr, 0);     /* system time, secs */
    ptr += utilPackINT16(ptr, 0);     /* system time, msc  */
    ptr += utilPackINT16(ptr, 0);     /* clock status      */
    ptr += utilPackINT16(ptr, 0);     /* PLL               */
    ptr += utilPackINT16(ptr, 0);     /* phase             */
    ptr += utilPackINT32(ptr, epoch); /* epoch time, secs  */
    ptr += utilPackINT16(ptr, 0);     /* epoch time, msc   */

    return (int) (ptr - start);
}

static BOOL OverRun()
{
static char tbuf[TESTLEN] = {TESTCHAR, TESTCHAR, TESTCHAR, TESTCHAR};

    if (memcmp(
        (void *) &CFbuf[CF_BUFLEN-TESTLEN-1],
        (void *) tbuf,
        (size_t) TESTLEN
    ) != 0) return TRUE;

    return FALSE;
}

static int LoadFtable(UINT8 *start, SANIO_CONFIG *cnf)
{
int i, j;
UINT8 *ptr;
static char *fid = "BuildSanCF:LoadFtable";

    ptr = start;

    ptr += utilPackINT16(ptr, (UINT16) cnf->nfilter);
    if (cnf->nfilter > SAN_MAX_FILTERS) {
        util_log(1, "FATAL ERROR: %s: SAN_MAX_FILTERS (%d) < %d!",
            fid, SAN_MAX_FILTERS, cnf->nfilter
        );
        ispd_die(MY_MOD_ID + 1);
    }
    for (i = 0; i < cnf->nfilter; i++) {
        ptr += utilPackINT16(ptr, (UINT16) cnf->filter[i].fid);
        ptr += utilPackINT16(ptr, (UINT16) cnf->filter[i].ntaps);
        ptr += utilPackINT16(ptr, (UINT16) cnf->filter[i].decim);
        if (cnf->filter[i].ntaps > SAN_MAX_FILTER_COEFF) {
            util_log(1, "FATAL ERROR: %s: SAN_MAX_FILTER_COEFF (%d) < %d!",
                fid, SAN_MAX_FILTER_COEFF, cnf->filter[i].ntaps
            );
            ispd_die(MY_MOD_ID + 2);
        }
        for (j = 0; j < cnf->filter[i].ntaps; j++) {
            ptr += utilPackREAL32(ptr, cnf->filter[i].coeff[j]);
        }
    }

    if (OverRun()) {
        util_log(1, "FATAL ERROR: %s: buffer overrun!", fid);
        ispd_die(MY_MOD_ID + 3);
    }

    return (int) (ptr - start);
}

static int LoadDetector(UINT8 *start, SANIO_CONFIG *cnf)
{
UINT8 *ptr;
static char *fid = "BuildSanCF:LoadDetector";

    ptr = start;

    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.stalen);
    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.ltalen);
    ptr += utilPackREAL32(ptr, (REAL32) cnf->detector.thresh);
    ptr += utilPackREAL32(ptr, (REAL32) cnf->detector.ethresh);
    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.vthresh);
    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.chans);
    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.pretrig);
    ptr += utilPackINT32(ptr, (UINT32) cnf->detector.posttrig);

    if (OverRun()) {
        util_log(1, "FATAL ERROR: %s: buffer overrun!", fid);
        ispd_die(MY_MOD_ID + 4);
    }

    return (int) (ptr - start);
}

static int LoadStreams(UINT8 *start, SANIO_CONFIG *cnf)
{
int i, j;
UINT8 *ptr, byte;
static char *fid = "BuildSanCF:LoadStreams";

    ptr = start;

    ptr += utilPackINT16(ptr, (UINT16) cnf->nstream);
    if (cnf->nstream > SAN_MAX_STREAMS) {
        util_log(1, "FATAL ERROR: %s: SAN_MAX_STREAMS (%d) < %d!",
            fid, SAN_MAX_STREAMS, cnf->nstream
        );
        ispd_die(MY_MOD_ID + 5);
    }
    for (i = 0; i < cnf->nstream; i++) {
        ptr += utilPackBytes(ptr, (UINT8 *) cnf->stream[i].name, 6);
        byte = (UINT8) (cnf->stream[i].dsp.chan & 0xff);
        ptr += utilPackBytes(ptr, &byte, 1);
        byte = (UINT8) (cnf->stream[i].dsp.flags & 0xff);
        ptr += utilPackBytes(ptr, &byte, 1);
        for (j = 0; j < SAN_MAX_CASCADES; j++) {
            byte = (UINT8) (cnf->stream[i].dsp.cascade[j] & 0xff);
            ptr += utilPackBytes(ptr, &byte, 1);
        }
    }

    if (OverRun()) {
        util_log(1, "FATAL ERROR: %s: buffer overrun!", fid);
        ispd_die(MY_MOD_ID + 6);
    }

    return (int) (ptr - start);
}

void BuildSanCF(SANIO_CONFIG *cnf, time_t tstamp, UINT16 boxId)
{
int len;
UINT16 seqno = 0;
UINT8 *ptr, *start;
OLD_MSGQ_MSG *msg;
UINT8 ten = 10, one = 1;
static char *fid = "BuildSanCF";

/* Build the common header */

    ptr = CommonHeader;

    ptr += utilPackBytes(ptr, (UINT8 *) "CF", 2);
    ptr += utilPackBytes(ptr, &ten, 1); /* format 10    */
    ptr += utilPackBytes(ptr, &one, 1); /* sub-format 1 */
    ptr += utilPackINT16(ptr, boxId);
    ptr += LoadTimeTag(ptr, tstamp);  /* beg */
    ptr += utilPackBytes(ptr, "** IDA10.1 Format **", 20);
    ptr += utilPackINT16(ptr, CF_DATA_LEN + sizeof(UINT16) + sizeof(UINT16));

/* Build the complete configuration data section */

    memset(CFbuf, 'e', CF_BUFLEN);
    ptr = CFbuf;

    ptr += LoadFtable(ptr, cnf);
    ptr += LoadDetector(ptr, cnf);
    ptr += LoadStreams(ptr, cnf);

/* Figure out how many CF packets this will require */

    len = (int) (ptr - CFbuf);
    TotalSegments = len / CF_DATA_LEN;
    if (TotalSegments * CF_DATA_LEN < len) ++TotalSegments;

    initialized = TRUE;
}

/* Break the configuration data into multiple CF packets */

void FlushSanCF()
{
UINT16 i, hdrlen, seqno = 0;
UINT8 *ptr, *src;
OLD_MSGQ_MSG *msg;
char ten = 10, zero = 0;
static char *fid = "FlushSanCF";

    if (!initialized) return;

    for (src=CFbuf, i=1; i <= TotalSegments; i++, src += CF_DATA_LEN) {
        msg = msgq_get(&Heap->packets, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received!", fid);
            ispd_die(MY_MOD_ID + 7);
        }
        ptr = (UINT8 *) msg->data;
        ptr += utilPackBytes(ptr, CommonHeader, IDA10_CMNHEADLEN);
        ptr += utilPackINT16(ptr, i);
        if (i < TotalSegments) {
            ptr += utilPackINT16(ptr, 0);
        } else {
            ptr += utilPackINT16(ptr, i);
        }
        hdrlen = (int) (ptr - (UINT8 *) msg->data);
        memcpy(ptr, src, CF_DATA_LEN);
        msg->len = hdrlen + CF_DATA_LEN;
        msgq_put(&Q->massio, msg);
    }
}

/* Revision History
 *
 * $Log: flushcf.c,v $
 * Revision 1.7  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.6  2005/09/30 22:08:52  dechavez
 * fixed problem of incorrect value in CF nbytes field
 *
 * Revision 1.5  2005/09/13 00:34:14  dechavez
 * build 10.1 packets
 *
 * Revision 1.4  2003/05/23 19:52:19  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.3  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.2  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.1  2000/11/08 01:57:23  dec
 * Release 2.1 (Support SAN CF records).
 *
 */
