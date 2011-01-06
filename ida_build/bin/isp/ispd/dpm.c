#pragma ident "$Id: dpm.c,v 1.3 2007/01/05 02:35:49 dechavez Exp $"
/*======================================================================
 *
 *  Support for digital power monitor.
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
#ifdef INCLUDE_DPM

#include "dpm.h"

#define MY_MOD_ID ISPD_MOD_DPM

extern ISP_PARAMS *Params;
extern struct isp_status   *Status;
extern struct isp_reftime  *Reftime;
extern struct ispd_queue   *Q;
extern struct ispd_heap    *Heap;

static DPM *dp = NULL;

static MUTEX DebugMutex = MUTEX_INITIALIZER;
#define OFF 0
#define ON  1
static int DebugState = OFF;

void dpmDebugToggle()
{
int newval;

    MUTEX_LOCK(&DebugMutex);
        if (DebugState == ON) {
            newval = OFF;
        } else {
            newval = ON;
        }
        DebugState = newval;
    MUTEX_UNLOCK(&DebugMutex);
    util_log(1, "DPM debug messages %s", newval == ON ? "ON" : "OFF");
}

static int dpmDebug()
{
int retval;

    MUTEX_LOCK(&DebugMutex);
        retval = DebugState;
    MUTEX_UNLOCK(&DebugMutex);
    return retval;
}

/* Thread to continuously read data from DPM */

static THREAD_FUNC ReadDPMThread(void *argptr)
{
time_t testval;
u_long nodata, loopCount = 0;
time_t now, then;
static char *dpmString;
REAL32 rvalue;
ISPD_AUX_DATUM sample;
ISPD_AUX_HANDLE *ap;
static char *fid = "dpm:ReadDPMThread";
REAL32 ActualValue;

    util_log(2, "DPM thread started, tid = %d", THREAD_SELF());

    ap = (ISPD_AUX_HANDLE *) argptr;
    nodata = 1;

/* Wait until clock is producing valid time stamps */

#ifndef NO_CLOCK_AVAIL
    testval = -1;
    while (testval < 0) {
        if (++loopCount == 10) {
            loopCount = 0;
            if (dpmDebug()) {
                util_log(1, "%s: waiting for local GPS clock to init", fid);
            }
        }
        sleep(1);
        MUTEX_LOCK(&Reftime->mutex);
            testval = (long) Reftime->ext;
        MUTEX_UNLOCK(&Reftime->mutex);
    }
#endif

/* Loop forever, reading and saving readings */

    then = 0;
    while (1) {
        if ((dpmString = dpmRead(dp, FALSE)) == (char *) NULL) {
            if (++nodata == 1) {
                set_alarm(ISP_ALARM_AUX);
                util_log(1, "DPM read error: %s, ", syserrmsg(errno));
            }
            MUTEX_LOCK(&Status->lock);
                ++Status->dpm.rerr;
            MUTEX_UNLOCK(&Status->lock);
        } else {
            if (nodata) {
                util_log(1, "DPM OK");
                clear_alarm(ISP_ALARM_AUX);
                nodata = 0;
            }
            now = time(NULL);
            if (now != then) {
                sample.sys_time = now;
                ActualValue     = atof(dpmString);
                if (ActualValue < ap->par->thresh) {
                    util_log(1, "LOW POWER ALERT (%.2fV)", ActualValue);
                    util_log(1, "INITIATING SYSTEM SHUTDOWN NOW");
                    if (!SendBootMgrMsg(0)) util_log(1, "SHUTDOWN ATTEMPT FAILED");
                    ispd_die(MY_MOD_ID + 1);
                }
                sample.value    = (int) (ActualValue * ap->par->multiplier);
                if (dpmDebug()) {
                    util_log(1, "DPM: '%s' = %ld", dpmString, sample.value);
                }
#ifdef NO_CLOCK_AVAIL
                sample.hz_time  = now;
                sample.ext_time = now;
                sample.qual     = 0;
#else
                MUTEX_LOCK(&Reftime->mutex);
                    sample.hz_time  = Reftime->hz;
                    sample.ext_time = Reftime->ext;
                    sample.qual     = Reftime->qual;
                MUTEX_UNLOCK(&Reftime->mutex);
#endif
                MUTEX_LOCK(&Status->lock);
                    Status->dpm.datum = sample.value;
                MUTEX_UNLOCK(&Status->lock);
                SaveLocalDatum(ap, &sample);
            }
            then = now;
        }
    }
}

void InitDPM(void)
{
char qual;
THREAD tid;
static ISPD_AUX_HANDLE handle;
static char *fid = "InitDPM";

    if (Params->dpm.port == (char *) NULL) return;

    MUTEX_LOCK(&Status->lock);
        Status->dpm.enabled = 0;
    MUTEX_UNLOCK(&Status->lock);

/* Connect to device */

    dp = dpmOpen(Params->dpm.port, Params->dpm.baud, 10);
    if (dp == (DPM *) NULL) {
        util_log(1, "%s: dpmOpen: %s", fid, syserrmsg(errno));
        util_log(1, "DPM disabled.");
        return;
    }

    util_log(1, "DPM input from %s enabled.", Params->dpm.port);

/* Initialize handle for reading and formatting threads */

    initAuxHandle(&handle, ISPD_DPM);

/* Start reading device */

    if (!THREAD_CREATE(&tid, ReadDPMThread, (void *) &handle)) {
        util_log(1, "%s: failed to create ReadBaroThread", fid);
        ispd_die(MY_MOD_ID + 2);
    }

/* Start filling packets */

    if (Params->digitizer == ISP_DAS) {
        if (!THREAD_CREATE(&tid, AuxDataToIDA8Thread, (void *) &handle)) {
            util_log(1, "%s: failed to create AuxDataToIDA8Thread,", fid);
            ispd_die(MY_MOD_ID + 3);
        }
    } else {
        if (!THREAD_CREATE(&tid, AuxDataToIDA10Thread, (void *) &handle)) {
            util_log(1, "%s: failed to create AuxDataToIDA10Thread,", fid);
            ispd_die(MY_MOD_ID + 4);
        }
    }
}

#else

void dpmDebugToggle()
{
    return;
}

void InitDPM(void)
{
    return;
}

#endif /* INCLUDE_DPM */

/* Revision History
 *
 * $Log: dpm.c,v $
 * Revision 1.3  2007/01/05 02:35:49  dechavez
 * removed digital power monitor and tape support
 *
 * Revision 1.2  2002/09/10 17:50:21  dec
 * undef'd NO_CLOCK_AVAIL
 *
 * Revision 1.1  2002/09/09 21:54:01  dec
 * created
 *
 */
