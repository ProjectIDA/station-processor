#pragma ident "$Id: baro.c,v 1.6 2002/09/09 21:58:12 dec Exp $"
/*======================================================================
 *
 *  Support for microbarometer input.
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
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_BARO

#define ISPD_DEF_PARO_PR 119
#define ISPD_DEF_PARO_UN 0
#define ISPD_DEF_PARO_UF "68947.57"
#define ISPD_DEF_PARO_PA 0
#define ISPD_DEF_PARO_PM 1

extern ISP_PARAMS *Params;
extern struct isp_status   *Status;
extern struct isp_reftime  *Reftime;
extern struct ispd_queue   *Q;
extern struct ispd_heap    *Heap;

static PARO *pp = (PARO *) NULL; /* Paroscientific object */

static MUTEX DebugMutex = MUTEX_INITIALIZER;
#define OFF 0
#define ON  1
static int DebugState = OFF;

void baroDebugToggle()
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
    util_log(1, "Barometer debug messages %s", newval == ON ? "ON" : "OFF");
}

static int baroDebug()
{
int retval;

    MUTEX_LOCK(&DebugMutex);
        retval = DebugState;
    MUTEX_UNLOCK(&DebugMutex);
    return retval;
}

/* Thread to continuously read data from barometer */

static THREAD_FUNC ReadBaroThread(void *argptr)
{
time_t testval;
u_long nodata, loopCount = 0;
time_t now, then;
static char *reply;
ISPD_AUX_DATUM sample;
ISPD_AUX_HANDLE *ap;
static char *fid = "baro:ReadBaroThread";

    util_log(2, "BAROMETER thread started, tid = %d", THREAD_SELF());

    ap = (ISPD_AUX_HANDLE *) argptr;
    nodata = 1;

/* Wait until clock is producing valid time stamps */

    testval = -1;
    while (testval < 0) {
        if (++loopCount == 10) {
            loopCount = 0;
            if (baroDebug()) {
                util_log(1, "%s: waiting for local GPS clock to init", fid);
            }
        }
        sleep(1);
        MUTEX_LOCK(&Reftime->mutex);
            testval = (long) Reftime->ext;
        MUTEX_UNLOCK(&Reftime->mutex);
    }

/* Tell the device to start sending data continuously */

    paro_command(pp, PARO_P4);

/* Loop forever, reading and saving readings */

    then = 0;
    while (1) {
        if ((reply = paro_read(pp)) == (char *) NULL) {
            if (++nodata == 1) {
                set_alarm(ISP_ALARM_AUX);
                util_log(1, "barometer read error: %s, ", syserrmsg(errno));
            }
            MUTEX_LOCK(&Status->lock);
                ++Status->baro.rerr;
            MUTEX_UNLOCK(&Status->lock);
        } else {
            if (nodata) {
                util_log(1, "barometer OK");
                clear_alarm(ISP_ALARM_AUX);
                nodata = 0;
            }
            now = time(NULL);
            if (now != then) {
                sample.sys_time = now;
                sample.value    = atoi(reply);
                MUTEX_LOCK(&Reftime->mutex);
                    sample.hz_time  = Reftime->hz;
                    sample.ext_time = Reftime->ext;
                    sample.qual     = Reftime->qual;
                MUTEX_UNLOCK(&Reftime->mutex);
                MUTEX_LOCK(&Status->lock);
                    Status->baro.datum = sample.value;
                MUTEX_UNLOCK(&Status->lock);
                SaveLocalDatum(ap, &sample);
            }
            then = now;
        }
    }
}

static int verifyConstants(struct paro_param *param)
{
int verfied = 1; /* assume verified, for now */
char tmp[ISP_MAXPAROPARAMLEN], *ptr;

    if (param->pr != Params->baro.pr) {
        util_log(1, "Barometer has PR = %d, want %d",
            param->pr, Params->baro.pr);
        verfied = 0;
    }

    if (param->un != ISPD_DEF_PARO_UN) {
        util_log(1, "Barometer has UN = %d, want %d",
            param->un, ISPD_DEF_PARO_UN
        );
        verfied = 0;
    }

    if (param->pm != ISPD_DEF_PARO_PM) {
        util_log(1, "Barometer has PM = %d, want %d",
            param->pm, ISPD_DEF_PARO_PM
        );
        verfied = 0;
    }

    if (param->pa != ISPD_DEF_PARO_PA) {
        util_log(1, "Barometer has PA = %d, want %d",
            param->pa, ISPD_DEF_PARO_PA
        );
        verfied = 0;
    }

/* Do a string compare for UF, since it is not an integer */

    sprintf(tmp, "%9.7g", param->uf);
    ptr = tmp;
    while (*ptr == ' ') ++ptr;
    if (strcmp(ptr, Params->baro.uf) != 0) {
        util_log(1, "Barometer has UF = %s, want %s",
            ptr, Params->baro.uf
        );
        verfied = 0;
    }

    return verfied;
}

static int setIntParoValue(int key, int value)
{
char tmp[ISP_MAXPAROPARAMLEN];

    sprintf(tmp, "%d", value);
    paro_set(pp, key, tmp);
    return value;
}

static int updateConstants(struct paro_param *param)
{
char tmp[ISP_MAXPAROPARAMLEN], *ptr;

    if (param->pr != Params->baro.pr) {
        util_log(1, "set barometer PR to %d",
            setIntParoValue(PARO_PR, Params->baro.pr)
        );
    }
    if (param->un != ISPD_DEF_PARO_UN) {
        util_log(1, "set barometer UN to %d",
            setIntParoValue(PARO_UN, ISPD_DEF_PARO_UN)
        );
    }
    if (param->pa != ISPD_DEF_PARO_PA) {
        util_log(1, "set barometer PA to %d",
            setIntParoValue(PARO_PA, ISPD_DEF_PARO_PA)
        );
    }
    if (param->pm != ISPD_DEF_PARO_PM) {
        util_log(1, "set barometer PM to %d",
            setIntParoValue(PARO_PM, ISPD_DEF_PARO_PM)
        );
    }

    sprintf(tmp, "%9.7g", param->uf);
    ptr = tmp;
    while (*ptr == ' ') ++ptr;
    if (strcmp(ptr, Params->baro.uf) != 0) {
        util_log(1, "set barometer UF to %s", Params->baro.uf);
        paro_set(pp, PARO_UF, Params->baro.uf);
    }

    if (paro_getcnf(pp, param) != 0) {
        util_log(1, "paro_getcnf: %s", syserrmsg(errno));
        util_log(1, "unable to read barometer constants after update!");
        return 0;
    }

    return verifyConstants(param);
}

void InitBaro(void)
{
char qual;
THREAD tid;
struct paro_param paro_param;
static ISPD_AUX_HANDLE handle;
static char *tag = "barometer";
static char *fid = "InitBaro";

    if (Params->baro.port == (char *) NULL) return;

    MUTEX_LOCK(&Status->lock);
        Status->baro.enabled = 0;
    MUTEX_UNLOCK(&Status->lock);

/* Connect to barometer */

    pp = paro_open(Params->baro.port, Params->baro.baud, &paro_param);
    if (pp == (PARO *) NULL) {
        util_log(1, "%s: paro_open: %s", fid, syserrmsg(errno));
        util_log(1, "Barometer disabled.");
        return;
    }

/* Verify calibration constants */

    if (!verifyConstants(&paro_param)) {
        if (!updateConstants(&paro_param)) {
            util_log(1, "Unable to update barometer constants!");
            util_log(1, "Barometer disabled.");
            return;
        } else {
            util_log(1, "Barometer constants updated OK");
        }
    } else {
        util_log(1, "Barometer constants OK");
    }

    util_log(1, "Barometer input from %s enabled.", Params->baro.port);

/* Initialize handle for reading and formatting threads */

    initAuxHandle(&handle, ISPD_BAROMETER);

/* Start reading barometer */

    if (!THREAD_CREATE(&tid, ReadBaroThread, (void *) &handle)) {
        util_log(1, "%s: failed to create ReadBaroThread", fid);
        ispd_die(MY_MOD_ID + 5);
    }

/* Start filling packets */

    if (Params->digitizer == ISP_DAS) {
        if (!THREAD_CREATE(&tid, AuxDataToIDA8Thread, (void *) &handle)) {
            util_log(1, "%s: failed to create AuxDataToIDA8Thread,", fid);
            ispd_die(MY_MOD_ID + 6);
        }
    } else {
        if (!THREAD_CREATE(&tid, AuxDataToIDA10Thread, (void *) &handle)) {
            util_log(1, "%s: failed to create AuxDataToIDA10Thread,", fid);
            ispd_die(MY_MOD_ID + 6);
        }
    }
}

/* Revision History
 *
 * $Log: baro.c,v $
 * Revision 1.6  2002/09/09 21:58:12  dec
 * rework to use general aux data interfaces (ie, sample and enqueue only,
 * leaving reformatting to somebody else)
 *
 * Revision 1.5  2002/03/15 22:51:36  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2001/04/19 22:46:11  dec
 * added ISP_DEBUG_BARO support
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
