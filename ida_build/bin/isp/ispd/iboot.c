#pragma ident "$Id: iboot.c,v 1.1 2008/08/21 21:28:43 dechavez Exp $"
/*======================================================================
 *
 *  iboot watchdog
 *
 *====================================================================*/
#include "ispd.h"
#include "iboot.h"

#define MY_MOD_ID ISPD_MOD_IBOOT

extern ISP_PARAMS *Params;
extern struct isp_status *Status;

static TotalPowerCycles = 0;

static BOOL AlarmConditionExists()
{
time_t duration;
struct isp_alarm alarm;

/* Get snapshot of current alarms */

    MUTEX_LOCK(&Status->lock);
        alarm = Status->alarm;
    MUTEX_UNLOCK(&Status->lock);

/* Check for extended DAS alarm */

    if (alarm.code & ISP_ALARM_DAS) {
        if ((duration = time(NULL) - alarm.das) >= Params->iboot.interval) {
            LogMsg(LOG_INFO, "warning: DAS alarm has been set for %d seconds", duration);
            return TRUE;
        }
    }

/* Check for extended ADC alarm */

    if (alarm.code & ISP_ALARM_ADC) {
        if ((duration = time(NULL) - alarm.adc) >= Params->iboot.interval) {
            LogMsg(LOG_INFO, "warning: ADC alarm has been set for %d seconds", duration);
            return TRUE;
        }
    }

/* No extended alarms */

    return FALSE;
}

static void CyclePower()
{
    ibootCommand(Params->iboot.server, Params->iboot.port, Params->iboot.passwd, IBOOT_CYCLE);
    ++TotalPowerCycles;
    LogMsg(LOG_INFO, "iboot CYCLE command sent to %s, port %d, count=%d/%d", Params->iboot.server, Params->iboot.port, TotalPowerCycles, Params->iboot.maxcycle);
    if (TotalPowerCycles == Params->iboot.maxcycle) LogMsg(LOG_INFO, "warning: maximum iboot cycle count reached, iboot support suspended");
}

static void ResetCycleCounter()
{
    if (TotalPowerCycles == Params->iboot.maxcycle) LogMsg(LOG_INFO, "no existing data alarms, iboot support resumed");
    TotalPowerCycles = 0;
}

static THREAD_FUNC IbootThread(void *dummy)
{
int LoopInterval;
static char *fid = "IbootThread";

    util_log(2, "iboot watchdog thread started, tid = %d", THREAD_SELF());

    LogMsg(LOG_INFO, "iboot watchdog started, server=%s, port=%d, passwd=%s, interval=%d, maxcycle=%d",
        Params->iboot.server, Params->iboot.port, Params->iboot.passwd, Params->iboot.interval, Params->iboot.maxcycle
    );

    while (1) {
        if (AlarmConditionExists()) {
            if (TotalPowerCycles < Params->iboot.maxcycle) CyclePower();
           LoopInterval = Params->iboot.interval;
        } else {
            if (TotalPowerCycles != 0) ResetCycleCounter();
            LoopInterval = 1;
        }
        sleep(LoopInterval);
    }
}

void StartIbootWatchdog()
{
THREAD tid;
static char *fid = "StartIbootWatchdog";

    if (Params->iboot.server == NULL) return;
    if (Params->iboot.port < 1) return;
    if (Params->iboot.passwd == NULL) return;
    if (Params->iboot.interval < 1) return;
    if (Params->iboot.maxcycle < 1) return;

    if (!THREAD_CREATE(&tid, IbootThread, NULL)) {
        util_log(1, "%s: failed to create IbootThread", fid);
        ispd_die(MY_MOD_ID + 1);
    }
}

/* Revision History
 *
 * $Log: iboot.c,v $
 * Revision 1.1  2008/08/21 21:28:43  dechavez
 * initial release
 *
 */
