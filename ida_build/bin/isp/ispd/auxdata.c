#pragma ident "$Id: auxdata.c,v 1.2 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Support for locally (ISP) acquired data
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2002 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_AUX

extern ISP_PARAMS *Params;
extern struct isp_status   *Status;
extern struct ispd_queue   *Q;
extern struct ispd_heap    *Heap;

/* Insert a sample into the queue */

static void EnqueueSample(ISPD_AUX_HANDLE *ap, ISPD_AUX_DATUM *sample)
{
time_t diff;
OLD_MSGQ_MSG *msg;
static char *fid = "local:EnqueueSample";

    if (sample->sys_time < sample->hz_time) {
        diff = sample->hz_time - sample->sys_time;
        sample->hz_time  -= diff;
        sample->ext_time -= diff;
    }

    msg = msgq_get(ap->q.empty, OLD_MSGQ_NOBLOCK);
    if (msg == (OLD_MSGQ_MSG *) NULL) {
        if (++ap->dropped == 1) {
            util_log(1, "%s heap exhausted, sample dropped", ap->tag);
            set_alarm(ISP_ALARM_BQFULL);
        }
        MUTEX_LOCK(&Status->lock);
            ++ap->stat->drop;
        MUTEX_UNLOCK(&Status->lock);
    } else {
        memcpy(msg->data, sample, sizeof(ISPD_AUX_DATUM));
        msgq_put(ap->q.full, msg);
        if (ap->dropped) {
            clear_alarm(ISP_ALARM_BQFULL);
            ap->dropped = 0;
        }
    }
}

void SaveLocalDatum(ISPD_AUX_HANDLE *ap, ISPD_AUX_DATUM *sample)
{
ISPD_AUX_DATUM fake;
int i, new_sample_time;
static char *fid = "SaveLocalDatum";

/* First time here, just stick the sample in the queue */

    if (ap->first) {
        EnqueueSample(ap, sample);
        ap->first = FALSE;
        ap->prev = *sample;
        return;
    }

/* We are not yet interested if it is not time for the next sample */

    new_sample_time = ap->prev.sys_time + ap->par->sint;
    if (sample->sys_time < new_sample_time) return;

/* Ready for a new sample... see if we missed any earlier sample times */

    ap->missed = (sample->sys_time - new_sample_time) / ap->par->sint;

    if (ap->missed > 0) {
        util_log(1, "%s: missed %d %s samples",
            fid, ap->missed, ap->tag
        );
        util_log(1, "%s: previous tofs = %s",
            fid, util_lttostr(ap->prev.sys_time, 0)
        );
        util_log(1, "%s: expected tofs = %s",
            fid, util_lttostr(new_sample_time, 0)
        );
        util_log(1, "%s: actual   tofs = %s",
            fid, util_lttostr(sample->sys_time, 0)
        );
    } else {
        sample->sys_time = new_sample_time; /* handle round off errors */
    }

    if (ap->missed == 1) {

    /* Missing one sample is deemed OK by me... interpolate */

        fake = ap->prev;
        fake.value = (ap->prev.value + sample->value) / 2;
        fake.ext_time += ap->par->sint;
        fake.hz_time  += ap->par->sint;
        fake.sys_time += ap->par->sint;
        util_log(1, "%s: save interpolated sample: %ld %ld %ld",
            fid, ap->prev.value, fake.value, sample->value
        );
        EnqueueSample(ap, &fake);

   } else if (ap->missed > 1) {

    /* Missing more than one sample is bad... flag it with a zero */

        MUTEX_LOCK(&Status->lock);
            ap->stat->miss += ap->missed;
        MUTEX_UNLOCK(&Status->lock);
        util_log(1, "%s: missed %d %s readings",
            fid, ap->missed, ap->tag
        );
        fake = ap->prev;
        fake.value = 0;
        for (i = 0; i < ap->missed; i++) {
            fake.ext_time += ap->par->sint;
            fake.hz_time  += ap->par->sint;
            fake.sys_time += ap->par->sint;
            util_log(1, "%s: save fake sample %ld", fid, fake.value);
            EnqueueSample(ap, &fake);
        }
    }

/* Save the current sample */

    EnqueueSample(ap, sample);
    ap->prev = *sample;
}

/* Init a handle */

static void initBarometerHandle(ISPD_AUX_HANDLE *ap)
{
static char *tag = "barometer";

    ap->tag     = tag;
    ap->stat    = &Status->baro;
    ap->par     = &Params->baro;
    ap->q.empty = &Heap->barometer;
    ap->q.full  = &Q->baro;
}

static void initDPMHandle(ISPD_AUX_HANDLE *ap)
{
static char *tag = "DPM";

    ap->tag     = tag;
    ap->stat    = &Status->dpm;
    ap->par     = &Params->dpm;
    ap->q.empty = &Heap->dpm;
    ap->q.full  = &Q->dpm;
}

void initAuxHandle(ISPD_AUX_HANDLE *ap, int which)
{
static char *fid = "initAuxHandle";

    switch(which) {
      case ISPD_BAROMETER:
        initBarometerHandle(ap);
        break;
      case ISPD_DPM:
        initDPMHandle(ap);
        break;
    }

    ap->missed  = 0;
    ap->dropped = 0;
    ap->first   = TRUE;

    MUTEX_LOCK(&Status->lock);
        ap->stat->enabled = 1;
        ap->stat->datum   = 0;
        ap->stat->nrec    = 0;
        ap->stat->drop    = 0;
        ap->stat->miss    = 0;
    MUTEX_UNLOCK(&Status->lock);
}

/* Revision History
 *
 * $Log: auxdata.c,v $
 * Revision 1.2  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.1  2005/10/06 22:25:16  dechavez
 * renamed aux.c to auxdata.c
 *
 */
