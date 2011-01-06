#pragma ident "$Id: status.c,v 1.8 2002/09/09 21:58:26 dec Exp $"
/*======================================================================
 *
 *  ISP state of health report routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
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

extern struct isp_status *Status;
extern struct ispd_heap  *Heap;
extern ISP_PARAMS *Params;

void clear_packetcounters(int loglevel)
{
    MUTEX_LOCK(&Status->lock);
        Status->count.drec  = 0;
        Status->count.lrec  = 0;
        Status->count.irec  = 0;
        Status->count.krec  = 0;
        Status->count.crec  = 0;
        Status->count.srec  = 0;
        Status->count.erec  = 0;
        Status->count.orec  = 0;
        Status->count.arec  = 0;
        Status->count.nrec  = 0;
        Status->count.urec  = 0;
        Status->count.njec  = 0;
        Status->san.data.count.p1 = 0;
        Status->san.data.count.p2 = 0;
        Status->san.data.count.p3 = 0;
        Status->san.cmnd.count.p1 = 0;
        Status->san.cmnd.count.p2 = 0;
        Status->san.cmnd.count.p3 = 0;
        Status->cleared.pkt = time(NULL);
    MUTEX_UNLOCK(&Status->lock);

    util_log(loglevel, "packet counters cleared");
}

void clear_commstats(int loglevel)
{
    MUTEX_LOCK(&Status->lock);
        Status->count.badr = 0;
        Status->count.sync = 0;
        Status->count.shrt = 0;
        Status->count.naks = 0;
        Status->count.dups = 0;
        Status->san.data.count.reconn = 0;
        Status->san.cmnd.count.reconn = 0;
        Status->last.commerr = -1;
        Status->cleared.comm = time(NULL);
    MUTEX_UNLOCK(&Status->lock);

    util_log(loglevel, "comm stats cleared");
}

void clear_auxstats(int loglevel)
{
    MUTEX_LOCK(&Status->lock);
        Status->extclock.err = 0;
        Status->baro.nrec = 0;
        Status->baro.rerr = 0;
        Status->baro.drop = 0;
        Status->baro.miss = 0;
        Status->dpm.nrec = 0;
        Status->dpm.rerr = 0;
        Status->dpm.drop = 0;
        Status->dpm.miss = 0;
        Status->cleared.aux = time(NULL);
    MUTEX_UNLOCK(&Status->lock);

    util_log(loglevel, "aux stats cleared");
}

BOOL SanCmndConn()
{
BOOL retval;

    MUTEX_LOCK(&Status->lock);
        retval = (Status->san.cmnd.connected != 0) ? TRUE : FALSE;
    MUTEX_UNLOCK(&Status->lock);

    return retval;
}

void SetSanCalState(BOOL state)
{
    MUTEX_LOCK(&Status->lock);
        Status->san.cal = state;
    MUTEX_UNLOCK(&Status->lock);
}

void SetSanEvtState(BOOL state)
{
    MUTEX_LOCK(&Status->lock);
        Status->san.evt = state;
    MUTEX_UNLOCK(&Status->lock);
}

void UpdateSanSoh(SAN_HANDLE *san, SANIO_SOH *soh)
{
int i;
static BOOL adc_timeout = FALSE, locked = TRUE;
BOOL no_data;
SAN_STIME uptime;
SANIO_CONFIG cnf;
static time_t SanBootTime = -1;

    MUTEX_LOCK(&Status->lock);
        if (Status->san.boottime < 0) {
            ttagStimeSub(&soh->gps.tstamp, &soh->boottime, &uptime);
            SanBootTime = (time_t) time(NULL) - uptime.sec;
        }
        Status->san.oper       = san->state == SAN_OPERATOR ? TRUE : FALSE;
        Status->san.evt        = soh->trgOn;
        Status->san.cal        = soh->cal.state;
        Status->san.boottime   = SanBootTime;
        Status->san.gps.status = soh->gps.status;
        Status->san.gps.phase  = soh->gps.phase;
        Status->san.gps.pll    = soh->gps.pll;
        Status->san.din        = (char) soh->di;
        Status->san.temp       = (float)soh->boxtemp/10.0;
        Status->san.nstream    = (int)  soh->nstream;
        no_data = Status->san.nstream > 1 ? TRUE : FALSE;
        for (i = 0; i < Status->san.nstream; i++) {
            strcpy(Status->san.stream[i].name, SanStreamName(i));
            Status->san.stream[i].rms = (float) soh->stream[i].rms;
            Status->san.stream[i].ave = (float) soh->stream[i].ave;
            if (i && soh->stream[i].count.value != 0) no_data = FALSE;
        }
    MUTEX_UNLOCK(&Status->lock);

    if (soh->gps.status & SANIO_TT_STAT_EXTTIME_QUALITY) {
        if (!locked) clear_alarm(ISP_ALARM_DASCLK);
        locked = TRUE;
    } else {
        if (locked) set_alarm(ISP_ALARM_DASCLK);
        locked = FALSE;
    }

    if (no_data) {
        if (!adc_timeout) {
            util_log(1, "No data from live SAN, ADC timeout suspected!");
            set_alarm(ISP_ALARM_ADC);
            adc_timeout = TRUE;
        }
    } else {
        if (adc_timeout) clear_alarm(ISP_ALARM_ADC);
        adc_timeout = FALSE;
    }
}

void status_init()
{
int i;
time_t now;

    now = time(NULL);

    memset(Status, 0, sizeof(struct isp_status));
    clear_packetcounters(2);
    clear_commstats(2);
    clear_auxstats(2);

    MUTEX_INIT(&Status->lock);
    Status->start        = now;
    Status->cleared.comm = now;
    Status->cleared.pkt  = now;
    Status->pid          = getpid();
    Status->digitizer    = Params->digitizer;

    Status->heap.barometer.nfree = msgq_count(&Heap->barometer);
    Status->heap.barometer.lowat = msgq_lowat(&Heap->barometer);
    Status->heap.commands.nfree  = msgq_count(&Heap->commands);
    Status->heap.commands.lowat  = msgq_lowat(&Heap->commands);
    Status->heap.packets.nfree   = msgq_count(&Heap->packets);
    Status->heap.packets.lowat   = msgq_lowat(&Heap->packets);
    Status->heap.obuf.nfree      = msgq_count(&Heap->obuf);
    Status->heap.obuf.lowat      = msgq_lowat(&Heap->obuf);
    Status->heap.dpm.nfree       = msgq_count(&Heap->dpm);
    Status->heap.dpm.lowat       = msgq_lowat(&Heap->dpm);

    Status->last.read    = -1;
    Status->last.write   = -1;
    Status->last.pkt     = -1;
    Status->last.commerr = -1;
    Status->last.dasstat = -1;
    Status->last.change  = -1;
    for (i = 0; i < DAS_MAXSTREAM; i++) Status->last.stream[i] = -1;

    Status->buffer.nrec     = 0;
    Status->buffer.capacity = 0;

    Status->output.type     = ISP_OUTPUT_NONE;
    Status->output.state    = ISP_OUTPUT_UNKNOWN;
    Status->output.nrec     = 0;
    Status->output.capacity = 0;
    Status->output.err      = 0;
    Status->output.file     = 0;

    Status->alarm.code    = ISP_ALARM_NONE;
    Status->alarm.das     = -1;
    Status->alarm.ioerr   = -1;
    Status->alarm.oper    = -1;
    Status->alarm.offline = -1;
    Status->alarm.oqfull  = -1;
    Status->alarm.bqfull  = -1;
    Status->alarm.rqfull  = -1;
    Status->alarm.aux     = -1;
    Status->alarm.dasdbg  = -1;
    Status->alarm.dasclk  = -1;
    Status->alarm.auxclk  = -1;
    Status->alarm.nrts    = -1;
    Status->alarm.adc     = -1;

    Status->san.data.connected = 0;
    Status->san.cmnd.connected = 0;
    Status->san.boottime       = -1;
}

/* Revision History
 *
 * $Log: status.c,v $
 * Revision 1.8  2002/09/09 21:58:26  dec
 * added dpm parameters
 *
 * Revision 1.7  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.6  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.5  2000/11/15 19:56:50  dec
 * fix bug in updating san boottime
 *
 * Revision 1.4  2000/11/08 01:57:24  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.3  2000/11/06 23:11:40  dec
 * added trigger flag to SAN soh
 *
 * Revision 1.2  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
