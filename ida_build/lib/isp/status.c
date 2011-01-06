#pragma ident "$Id"
/*======================================================================
 *
 *  Routines to deal with ISPD status messages
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include "util.h"
#include "isp.h"

static int PackPreamble(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->digitizer);
    ptr += utilPackINT32(ptr, (UINT32) src->pid);
    ptr += utilPackINT32(ptr, (UINT32) src->start);
    ptr += utilPackINT32(ptr, (UINT32) src->tstamp);

    return (int) (ptr - dest);
}

static int UnpackPreamble(UINT8 *src, struct isp_status *dest)
{
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->digitizer = (int) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->pid = (pid_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->start = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->tstamp = (time_t) ltmp;

    return (int) (ptr - src);
}

static int PackHeapStats(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr  = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->heap.barometer.nfree);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.barometer.lowat);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.dpm.nfree);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.dpm.lowat);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.commands.nfree);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.commands.lowat);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.packets.nfree);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.packets.lowat);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.obuf.nfree);
    ptr += utilPackINT16(ptr, (UINT16) src->heap.obuf.lowat);

    return (int) (ptr - dest);
}

static int UnpackHeapStats(UINT8 *src, struct isp_status *dest)
{
UINT16  stmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.barometer.nfree = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.barometer.lowat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.dpm.nfree = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.dpm.lowat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.commands.nfree = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.commands.lowat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.packets.nfree = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.packets.lowat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.obuf.nfree = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->heap.obuf.lowat = (int) stmp;

    return (int) (ptr - src);
}

static int PackQueueStats(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->queue.baro.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.baro.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.dpm.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.dpm.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_read.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_read.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_write.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_write.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_process.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.das_process.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.massio.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.massio.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.nrts.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.nrts.hiwat);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.obuf.npend);
    ptr += utilPackINT16(ptr, (UINT16) src->queue.obuf.hiwat);

    return (int) (ptr - dest);
}

static int UnpackQueueStats(UINT8 *src, struct isp_status *dest)
{
UINT16  stmp;
UINT8 *ptr;

    ptr = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.baro.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.baro.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.dpm.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.dpm.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_read.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_read.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_write.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_write.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_process.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.das_process.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.massio.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.massio.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.nrts.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.nrts.hiwat = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.obuf.npend = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->queue.obuf.hiwat = (int) stmp;

    return (int) (ptr - src);
}

static int PackCounters(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->count.drec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.lrec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.irec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.krec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.crec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.srec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.erec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.orec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.arec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.urec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.nrec);
    ptr += utilPackINT32(ptr, (UINT32) src->count.badr);
    ptr += utilPackINT32(ptr, (UINT32) src->count.sync);
    ptr += utilPackINT32(ptr, (UINT32) src->count.shrt);
    ptr += utilPackINT32(ptr, (UINT32) src->count.naks);
    ptr += utilPackINT32(ptr, (UINT32) src->count.dups);
    ptr += utilPackINT32(ptr, (UINT32) src->count.njec);

    return (int) (ptr - dest);
}

static int UnpackCounters(UINT8 *src, struct isp_status *dest)
{
UINT32  ltmp;
UINT8 *ptr;

    ptr = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.drec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.lrec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.irec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.krec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.crec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.srec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.erec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.orec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.arec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.urec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.nrec = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.badr = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.sync = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.shrt = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.naks = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.dups = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->count.njec = (long) ltmp;

    return (int) (ptr - src);
}

static int PackAlarms(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.code);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.das);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.oper);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.ioerr);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.offline);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.oqfull);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.bqfull);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.rqfull);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.aux);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.dasdbg);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.dasclk);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.auxclk);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.nrts);
    ptr += utilPackINT32(ptr, (UINT32) src->alarm.adc);

    return (int) (ptr - dest);
}

static int UnpackAlarms(UINT8 *src, struct isp_status *dest)
{
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.code = (u_long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.das = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.oper = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.ioerr = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.offline = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.oqfull = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.bqfull = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.rqfull = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.aux = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.dasdbg = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.dasclk = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.auxclk = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.nrts = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->alarm.adc  = (time_t) ltmp;

    return (int) (ptr - src);
}

static int PackFlags(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->flag.event);
    ptr += utilPackINT16(ptr, (UINT16) src->flag.calib);
    ptr += utilPackINT16(ptr, (UINT16) src->flag.config);
    ptr += utilPackINT16(ptr, (UINT16) src->flag.operator);

    return (int) (ptr - dest);
}

static int UnpackFlags(UINT8 *src, struct isp_status *dest)
{
UINT16 stmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->flag.event = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->flag.calib = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->flag.config = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->flag.operator = (int) stmp;

    return (int) (ptr - src);
}

static int PackMassio(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->buffer.nrec);
    ptr += utilPackINT16(ptr, (UINT16) src->buffer.capacity);
    ptr += utilPackBytes(ptr, (UINT8 *) src->output.device, ISP_DEVNAMELEN);
    ptr += utilPackINT32(ptr, (UINT32) src->output.bfact);
    ptr += utilPackINT16(ptr, (UINT16) src->output.type);
    ptr += utilPackINT16(ptr, (UINT16) src->output.state);
    ptr += utilPackINT32(ptr, (UINT32) src->output.nrec);
    ptr += utilPackINT16(ptr, (UINT16) src->output.capacity);
    ptr += utilPackINT32(ptr, (UINT32) src->output.err);
    ptr += utilPackINT16(ptr, (UINT16) src->output.file);
    ptr += utilPackINT32(ptr, (UINT32) src->output.lost);

    return (int) (ptr - dest);
}

static int UnpackMassio(UINT8 *src, struct isp_status *dest)
{
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->buffer.nrec = (long) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->buffer.capacity = (int) stmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->output.device, ISP_DEVNAMELEN);
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->output.bfact = (long) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->output.type = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->output.state = (int) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->output.nrec = (unsigned long) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->output.capacity = (int) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->output.err = (long) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->output.file = (int) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->output.lost = (unsigned long) ltmp;

    return (int) (ptr - src);
}

static int PackAuxStats(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->baro.enabled);
    ptr += utilPackINT16(ptr, (UINT16) src->dpm.enabled);
    ptr += utilPackINT16(ptr, (UINT16) src->extclock.enabled);
    if (src->baro.enabled) {
        ptr += utilPackINT32(ptr, (UINT32) src->baro.datum);
        ptr += utilPackINT32(ptr, (UINT32) src->baro.nrec);
        ptr += utilPackINT32(ptr, (UINT32) src->baro.rerr);
        ptr += utilPackINT32(ptr, (UINT32) src->baro.drop);
        ptr += utilPackINT32(ptr, (UINT32) src->baro.miss);
    }
    if (src->dpm.enabled) {
        ptr += utilPackINT32(ptr, (UINT32) src->dpm.datum);
        ptr += utilPackINT32(ptr, (UINT32) src->dpm.nrec);
        ptr += utilPackINT32(ptr, (UINT32) src->dpm.rerr);
        ptr += utilPackINT32(ptr, (UINT32) src->dpm.drop);
        ptr += utilPackINT32(ptr, (UINT32) src->dpm.miss);
    }
    if (src->extclock.enabled) {
        ptr += utilPackINT32(ptr, (UINT32) src->extclock.datum);
        ptr += utilPackINT32(ptr, (UINT32) src->extclock.err);
        ptr += utilPackINT16(ptr, (UINT16) src->extclock.qual);
        ptr += utilPackBytes(ptr, (UINT8 *) &src->extclock.code, 1);
    }

    return (int) (ptr - dest);
}

static int UnpackAuxStats(UINT8 *src, struct isp_status *dest)
{
UINT16 stmp;
UINT32 ltmp;
struct ident_rec *idrec;
struct config_rec *cnf;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->baro.enabled = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->dpm.enabled = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->extclock.enabled = (int) stmp;
    if (dest->baro.enabled) {
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->baro.datum = (long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->baro.nrec = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->baro.rerr = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->baro.drop = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->baro.miss = (unsigned long) ltmp;
    }
    if (dest->dpm.enabled) {
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->dpm.datum = (long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->dpm.nrec = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->dpm.rerr = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->dpm.drop = (unsigned long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->dpm.miss = (unsigned long) ltmp;
    }
    if (dest->extclock.enabled) {
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->extclock.datum = (time_t) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->extclock.err = (unsigned long) ltmp;
        ptr += utilUnpackUINT16(ptr, &stmp); dest->extclock.qual = (short) stmp;
        ptr += utilUnpackBytes(ptr, (UINT8 *) &dest->extclock.code, 1);
    }

    return (int) (ptr - src);
}

static int PackDasTimers(UINT8 *dest, struct isp_status *src)
{
int i;
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->clock);
    ptr += utilPackINT16(ptr, (UINT16) src->clock_qual);
    ptr += utilPackINT32(ptr, (UINT32) src->last.read);
    ptr += utilPackINT32(ptr, (UINT32) src->last.write);
    ptr += utilPackINT32(ptr, (UINT32) src->last.pkt);
    ptr += utilPackINT32(ptr, (UINT32) src->last.commerr);
    ptr += utilPackINT32(ptr, (UINT32) src->last.dasstat);
    ptr += utilPackINT32(ptr, (UINT32) src->last.change);
    for (i = 0; i < DAS_MAXSTREAM; i++) {
        ptr += utilPackINT32(ptr, (UINT32) src->last.stream[i]);
    }
    ptr += utilPackINT32(ptr, (UINT32) src->cleared.pkt);
    ptr += utilPackINT32(ptr, (UINT32) src->cleared.comm);
    ptr += utilPackINT32(ptr, (UINT32) src->cleared.aux);

    return (int) (ptr - dest);
}

static int UnpackDasTimers(UINT8 *src, struct isp_status *dest)
{
int i;
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->clock = (time_t) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->clock_qual = (int) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.read = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.write = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.pkt = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.commerr = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.dasstat = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.change = (time_t) ltmp;
    for (i = 0; i < DAS_MAXSTREAM; i++) {
        ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.stream[i] = (time_t) ltmp;
    }
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->cleared.pkt = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->cleared.comm = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->cleared.aux = (time_t) ltmp;

    return (int) (ptr - src);
}

static int PackDasParams(UINT8 *dest, struct isp_status *src)
{
IDA_IDENT_REC *idrec;
IDA_CONFIG_REC *cnf;
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT16(ptr, (UINT16) src->have_iddata);
    ptr += utilPackINT16(ptr, (UINT16) src->have_config);
    if (src->have_iddata) {
        idrec = &src->iddata;
        ptr += utilPackBytes(ptr, (UINT8 *) &idrec->sname, IDA_SNAMLEN);
        ptr += utilPackINT16(ptr, (UINT16) idrec->atod_rev);
        ptr += utilPackINT16(ptr, (UINT16) idrec->dsp_rev);
        ptr += utilPackINT16(ptr, (UINT16) idrec->atod_id);
    }
    if (src->have_config) {
        cnf = &src->config;
        ptr += utilPackINT16(ptr, (UINT16) cnf->jumpers);
        ptr += utilPackINT16(ptr, (UINT16) cnf->detect.chan);
    }

    return (int) (ptr - dest);
}

static int UnpackDasParams(UINT8 *src, struct isp_status *dest)
{
UINT16 stmp;
IDA_IDENT_REC *idrec;
IDA_CONFIG_REC *cnf;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->have_iddata = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->have_config = (int) stmp;
    if (dest->have_iddata) {
        idrec = &dest->iddata;
        ptr += utilUnpackBytes(ptr, (UINT8 *) idrec->sname, IDA_SNAMLEN);
        ptr += utilUnpackUINT16(ptr, &stmp); idrec->atod_rev = (short) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); idrec->dsp_rev = (short) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); idrec->atod_id = (short) stmp;
    }
    if (dest->have_config) {
        cnf = &dest->config;
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->jumpers = (short) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.chan = (short) stmp;
    }
    return (int) (ptr - src);
}

static int PackDasStats(UINT8 *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->das.reboots);
    ptr += utilPackINT32(ptr, (UINT32) src->das.start);
    ptr += utilPackINT32(ptr, (UINT32) src->das.naks);
    ptr += utilPackINT32(ptr, (UINT32) src->das.acks);
    ptr += utilPackINT32(ptr, (UINT32) src->das.dropped);
    ptr += utilPackINT32(ptr, (UINT32) src->das.timeouts);
    ptr += utilPackINT32(ptr, (UINT32) src->das.triggers);
    ptr += PackDasTimers(ptr, src);
    ptr += PackDasParams(ptr, src);

    return (int) (ptr - dest);
}

static int UnpackDasStats(UINT8 *src, struct isp_status *dest)
{
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.reboots = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.start = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.naks = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.acks = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.dropped = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.timeouts = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->das.triggers = (long) ltmp;
    ptr += UnpackDasTimers(ptr, dest);
    ptr += UnpackDasParams(ptr, dest);

    return (int) (ptr - src);
}

static int PackSanConnStat(UINT8 *dest, SAN_CONNSTAT *src)
{
UINT8 *ptr;

    ptr = dest;
    ptr += utilPackINT32(ptr, (UINT32) src->connected);
    ptr += utilPackINT32(ptr, (UINT32) src->lastRead);
    ptr += utilPackINT32(ptr, (UINT32) src->count.p1);
    ptr += utilPackINT32(ptr, (UINT32) src->count.p2);
    ptr += utilPackINT32(ptr, (UINT32) src->count.p3);
    ptr += utilPackINT32(ptr, (UINT32) src->count.reconn);

    return (int) (ptr - dest);
}

static int UnpackSanConnStat(UINT8 *src, SAN_CONNSTAT *dest)
{
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;

    ptr  = src;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->connected = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->lastRead = (time_t) ltmp;
    ptr += utilUnpackUINT32(ptr, &dest->count.p1);
    ptr += utilUnpackUINT32(ptr, &dest->count.p2);
    ptr += utilUnpackUINT32(ptr, &dest->count.p3);
    ptr += utilUnpackUINT32(ptr, &dest->count.reconn);

    return (int) (ptr - src);
}

static int PackSanStats(UINT8 *dest, struct isp_status *src)
{
int i;
UINT8 *ptr;

    ptr = dest;
    ptr += PackSanConnStat(ptr, &src->san.data);
    ptr += PackSanConnStat(ptr, &src->san.cmnd);
    ptr += utilPackINT16(ptr, (UINT16) src->san.cal);
    ptr += utilPackINT16(ptr, (UINT16) src->san.evt);
    ptr += utilPackINT16(ptr, (UINT16) src->san.oper);
    ptr += utilPackINT32(ptr, (UINT32) src->san.boottime);
    ptr += utilPackINT16(ptr, (UINT16) src->san.gps.status);
    ptr += utilPackINT16(ptr, (UINT16) src->san.gps.phase);
    ptr += utilPackINT16(ptr, (UINT16) src->san.gps.pll);
    ptr += utilPackBytes(ptr, (UINT8 *) &src->san.din, 1);
    ptr += utilPackREAL32(ptr, (REAL32) src->san.temp);
    ptr += utilPackINT16(ptr, (UINT16) src->san.nstream);
    for (i = 0; i < src->san.nstream; i++) {
        ptr += utilPackBytes(ptr, (UINT8 *) src->san.stream[i].name, SAN_STREAM_NAMLEN+1);
        ptr += utilPackREAL32(ptr, (REAL32) src->san.stream[i].rms);
        ptr += utilPackREAL32(ptr, (REAL32) src->san.stream[i].ave);
    }
    ptr += utilPackINT32(ptr, (UINT32) src->last.change);

    return (int) (ptr - dest);
}

static int UnpackSanStats(UINT8 *src, struct isp_status *dest)
{
int i;
UINT16 stmp;
UINT32 ltmp;
REAL32 rtmp;
UINT8 *ptr;

    ptr  = src;
    ptr += UnpackSanConnStat(ptr, &dest->san.data);
    ptr += UnpackSanConnStat(ptr, &dest->san.cmnd);
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.cal = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.evt = (BOOL) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.oper= (BOOL) stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->san.boottime = (int) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.gps.status = (UINT16) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.gps.phase = (UINT16) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.gps.pll = (UINT16) stmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) &dest->san.din, 1);
    ptr += utilUnpackREAL32(ptr, &rtmp); dest->san.temp = (float) rtmp;
    ptr += utilUnpackUINT16(ptr, &stmp); dest->san.nstream = (int) stmp;
    if (dest->san.nstream > SAN_MAX_STREAMS) {
        util_log(1, "FORCE dest->san.nstream = SAN_MAX_STREAMS!");
        dest->san.nstream = SAN_MAX_STREAMS;
    }
    for (i = 0; i < dest->san.nstream; i++) {
        ptr += utilUnpackBytes(ptr, (UINT8 *) dest->san.stream[i].name, SAN_STREAM_NAMLEN+1);
        ptr += utilUnpackREAL32(ptr, &rtmp); dest->san.stream[i].rms = (float) rtmp;
        ptr += utilUnpackREAL32(ptr, &rtmp); dest->san.stream[i].ave = (float) rtmp;
    }
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->last.change = (time_t) ltmp;

    return (int) (ptr - src);
}

/* Encode an ISP state of health message */

int ispPackStatus(char *dest, struct isp_status *src)
{
UINT8 *ptr;

    ptr = (UINT8 *) dest;
    ptr += PackPreamble(ptr, src);
    ptr += PackHeapStats(ptr, src);
    ptr += PackQueueStats(ptr, src);
    ptr += PackCounters(ptr, src);
    ptr += PackAlarms(ptr, src);
    ptr += PackFlags(ptr, src);
    ptr += PackMassio(ptr, src);
    ptr += PackAuxStats(ptr, src);
    if (src->digitizer == ISP_DAS) {
        ptr += PackDasStats(ptr, src);
    } else if (src->digitizer == ISP_SAN) {
        ptr += PackSanStats(ptr, src);
    }

    return (int) ((char *) ptr - dest);
}

int ispUnpackStatus(char *src, struct isp_status *dest)
{
UINT16 stmp;
IDA_IDENT_REC *idrec;
IDA_CONFIG_REC *cnf;
UINT8 *ptr;

    ptr  = (UINT8 *) src;
    ptr += UnpackPreamble(ptr, dest);
    ptr += UnpackHeapStats(ptr, dest);
    ptr += UnpackQueueStats(ptr, dest);
    ptr += UnpackCounters(ptr, dest);
    ptr += UnpackAlarms(ptr, dest);
    ptr += UnpackFlags(ptr, dest);
    ptr += UnpackMassio(ptr, dest);
    ptr += UnpackAuxStats(ptr, dest);
    if (dest->digitizer == ISP_DAS) {
        ptr += UnpackDasStats(ptr, dest);
    } else if (dest->digitizer == ISP_SAN) {
        ptr += UnpackSanStats(ptr, dest);
    }

    return (int) ((char *) ptr - src);
}

/* Revision History
 *
 * $Log: status.c,v $
 * Revision 1.5  2006/02/09 20:05:14  dechavez
 * support for libida 4.0.0 and libisidb 1.0.0
 *
 * Revision 1.4  2003/12/10 05:59:43  dechavez
 * various casts and the like to calm solaris cc
 *
 * Revision 1.3  2003/05/23 19:46:48  dechavez
 * changed pack/unpack calls to follow new util naming style
 *
 * Revision 1.2  2002/09/09 21:42:27  dec
 * added dpm soh
 *
 * Revision 1.1  2000/11/02 20:22:14  dec
 * initial release
 *
 */
