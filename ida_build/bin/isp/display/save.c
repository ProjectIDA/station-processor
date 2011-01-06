#pragma ident "$Id: save.c,v 1.5 2006/02/10 02:23:42 dechavez Exp $"
/*======================================================================
 *
 * Decode and save various things.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "isp_console.h"

extern int localhost;

static struct {
    MUTEX status;
    MUTEX params;
    MUTEX dascnf;
    MUTEX alarms;
    MUTEX state;
    MUTEX priv;
    MUTEX queue;
    MUTEX dastat;
    MUTEX nrts;
    MUTEX pid;
    MUTEX chnmap;
    MUTEX intervals;
    MUTEX needoper;
} mutex = {
    MUTEX_INITIALIZER, MUTEX_INITIALIZER, MUTEX_INITIALIZER,
    MUTEX_INITIALIZER, MUTEX_INITIALIZER, MUTEX_INITIALIZER,
    MUTEX_INITIALIZER, MUTEX_INITIALIZER, MUTEX_INITIALIZER,
    MUTEX_INITIALIZER, MUTEX_INITIALIZER, MUTEX_INITIALIZER,
    MUTEX_INITIALIZER
};

static struct {
    int status;
    int params;
    int dascnf;
    int alarms;
    int state;
    int chnmap;
} have = {0, 0, 0, 0, 0, 0};

static struct {
    int status;
    int params;
    int dascnf;
    int alarms;
    int nrts;
    int chnmap;
} want = {0, 0, 0, 0, 0, 0};

static pid_t ispd_pid = -1;
static int needoper_flag = 0;
static int dastat_flag = 1;
static int queue_flag = 0;
static int intervals_flag = 0;
static int priv_flag = 0;
static int nrts_flag = 0;
static int chnmap_flag = 0;
static int state = ISP_OPERATOR;
static int state_changed = 0;
static struct isp_alarm  alarms;
static time_t onset;
static struct isp_status status;
static struct isp_params params;
static struct isp_dascnf dascnf;
static int digitizer = ISP_UNKNOWN;
static IDA Ida;

void InitChanMap()
{
    idaInitHandle(&Ida);
}

/* ISPD process id */

void SetPid(pid_t value)
{
    MUTEX_LOCK(&mutex.pid);
        ispd_pid = value;
    MUTEX_UNLOCK(&mutex.pid);
}

pid_t Pid()
{
pid_t retval;

    MUTEX_LOCK(&mutex.pid);
        retval = ispd_pid;
    MUTEX_UNLOCK(&mutex.pid);

    return retval;
}

/* NRTS view */

void SetWantNrts(int value)
{
    MUTEX_LOCK(&mutex.nrts);
        nrts_flag = value;
    MUTEX_UNLOCK(&mutex.nrts);
}

int WantNrts()
{
int retval;

    MUTEX_LOCK(&mutex.nrts);
        retval = nrts_flag;
    MUTEX_UNLOCK(&mutex.nrts);

    return retval;
}

/* Aux inputs */

void SetDasStat(int value)
{
    MUTEX_LOCK(&mutex.dastat);
        dastat_flag = value;
    MUTEX_UNLOCK(&mutex.dastat);
}

int WantDasStat()
{
int retval;

    MUTEX_LOCK(&mutex.dastat);
        retval = dastat_flag;
    MUTEX_UNLOCK(&mutex.dastat);

    return retval;
}

/* Heap stats */

void SetQueueStats(int value)
{
    MUTEX_LOCK(&mutex.queue);
        queue_flag = value;
    MUTEX_UNLOCK(&mutex.queue);
}

int WantQueue()
{
int retval;

    MUTEX_LOCK(&mutex.queue);
        retval = queue_flag;
    MUTEX_UNLOCK(&mutex.queue);

    return retval;
}

/* Counter intervals */

void SetIntervals(int value)
{
    MUTEX_LOCK(&mutex.intervals);
        intervals_flag = value;
    MUTEX_UNLOCK(&mutex.intervals);
}

int WantIntervals()
{
int retval;

    MUTEX_LOCK(&mutex.intervals);
        retval = intervals_flag;
    MUTEX_UNLOCK(&mutex.intervals);

    return retval;
}

/* Privilege level */

void SetPrivilege(int value)
{
    MUTEX_LOCK(&mutex.priv);
        priv_flag = value;
    MUTEX_UNLOCK(&mutex.priv);
}

int Privileged()
{
int retval;

    MUTEX_LOCK(&mutex.priv);
        retval = priv_flag ? PRIVILEGED : 0;
    MUTEX_UNLOCK(&mutex.priv);

    return retval;
}

/* Client operator/observer state */

void SaveState(void *data)
{
short stmp;
int newstate;

    MUTEX_LOCK(&mutex.state);
        memcpy(&stmp, data, 2);
        newstate = (int) ntohs(stmp);
        if (newstate == ISP_OBSERVER || newstate == ISP_OPERATOR) {
            if (newstate != state) state_changed = 1;
            state = newstate;
        }
    MUTEX_UNLOCK(&mutex.state);
}

void SetNeedOper(int value)
{
    MUTEX_LOCK(&mutex.needoper);
        needoper_flag = value;
    MUTEX_UNLOCK(&mutex.needoper);
}

int NeedOperator()
{
int retval;

    MUTEX_LOCK(&mutex.needoper);
        retval = needoper_flag;
    MUTEX_UNLOCK(&mutex.needoper);

    return retval;
}

/* Stream map */

void SaveStreamMap(void *data)
{
IDA_CHNLOCMAP map;

    MUTEX_LOCK(&mutex.chnmap);
        ispDecodeChnLocMapEntry(&map, (UINT8 *) data);
        listAppend(Ida.chnlocmap, (void *) &map, sizeof(IDA_CHNLOCMAP));
    MUTEX_UNLOCK(&mutex.chnmap);
}

/* Channel map */

void SaveChanMap(void *data)
{
    MUTEX_LOCK(&mutex.chnmap);
        ispDecodeChnLocMap(&Ida, (UINT8 *) data);
    MUTEX_UNLOCK(&mutex.chnmap);
}

IDA *GetIdaHandle(void)
{
    return &Ida;
}

int GetState()
{
int retval;

    MUTEX_LOCK(&mutex.state);
        retval = state;
    MUTEX_UNLOCK(&mutex.state);

    return retval;
}

int Operator()
{
    return (GetState() == ISP_OPERATOR);
}

/* Current state of health */

void SaveStatus(void *data)
{
    MUTEX_LOCK(&mutex.status);
        ispUnpackStatus((char *) data, &status);
        have.status = 1;
        MUTEX_LOCK(&mutex.alarms);
            alarms = status.alarm;
            onset  = status.tstamp;
            have.alarms = 1;
        MUTEX_UNLOCK(&mutex.alarms);
    MUTEX_UNLOCK(&mutex.status);
}

int ChangedState()
{
int retval;

    MUTEX_LOCK(&mutex.state);
        retval = state_changed;
        state_changed = 0;
    MUTEX_UNLOCK(&mutex.state);

    return retval;
}

int GetStatus(struct isp_status *output, int clear)
{
int retval;

    MUTEX_LOCK(&mutex.status);
        if (have.status) {
            *output = status;
            retval = 0;
            if (clear) have.status = 0;
        } else {
            retval = 1;
            output->have_iddata = 0;
            output->have_config = 0;
        }
    MUTEX_UNLOCK(&mutex.status);

    return retval;
}

void ClearStatus()
{
    MUTEX_LOCK(&mutex.status);
        have.status = 0;
    MUTEX_UNLOCK(&mutex.status);
}

/* DAS configuration */

void SaveDasCnf(void *data)
{
static char *fid = "save_dascnf";
int i;

    MUTEX_LOCK(&mutex.dascnf);
        isp_decode_dascnf(&dascnf, (char *) data);
        have.dascnf = 1;
    MUTEX_UNLOCK(&mutex.dascnf);
}

int GetDasCnf(struct isp_dascnf *output, int clear)
{
int retval;
static char *fid = "get_dascnf";
int i;

util_log(1, "%s: begin, clear = %d", fid, clear);
    MUTEX_LOCK(&mutex.dascnf);
        if (have.dascnf) {
util_log(1, "%s: have.dascnf is TRUE", fid);
            *output = dascnf;
            if (clear) have.dascnf = 0;
            retval = 0;
        } else {
util_log(1, "%s: have.dascnf is FALSE", fid);
            retval = 1;
        }
    MUTEX_UNLOCK(&mutex.dascnf);

util_log(1, "%s: return %d", fid, retval);
    return retval;
}

int WantDasCnf()
{
int retval;

    MUTEX_LOCK(&mutex.dascnf);
        retval = want.dascnf;
    MUTEX_UNLOCK(&mutex.dascnf);

    return retval;
}

void NeedDasCnf(int flag)
{
    MUTEX_LOCK(&mutex.dascnf);
        want.dascnf = flag;
    MUTEX_UNLOCK(&mutex.dascnf);
}

/* Run time parameters */

int Digitizer()
{
int retval;

    MUTEX_LOCK(&mutex.params);
        retval = digitizer;
    MUTEX_UNLOCK(&mutex.params);

    return retval;
}

void SaveParams(void *data)
{
    MUTEX_LOCK(&mutex.params);
        isp_decode_params(&params, (char *) data);
        have.params = 1;
        digitizer = params.digitizer;
        SetWindowsFor(digitizer);
    MUTEX_UNLOCK(&mutex.params);
}

int GetParams(struct isp_params *output, int clear)
{
int retval;

    MUTEX_LOCK(&mutex.params);
        if (have.params) {
            *output = params;
            if (clear) have.params = 0;
            retval = 0;
        } else {
            retval = -1;
        }
    MUTEX_UNLOCK(&mutex.params);

    return retval;
}

void ClearParams()
{
    MUTEX_LOCK(&mutex.params);
        have.params = 0;
    MUTEX_UNLOCK(&mutex.params);
}

int WantParams()
{
int retval;

    MUTEX_LOCK(&mutex.params);
        retval = want.params;
    MUTEX_UNLOCK(&mutex.params);

    return retval;
}

void NeedParams(int flag)
{
    MUTEX_LOCK(&mutex.params);
        want.params = flag;
    MUTEX_UNLOCK(&mutex.params);
}

/* Alarms */

int GetAlarms(struct isp_alarm *output, time_t *tstamp, int clear)
{
int retval;

    MUTEX_LOCK(&mutex.alarms);
        if (have.alarms) {
            *output = alarms;
            *tstamp = onset;
            if (clear) have.alarms = 0;
            retval = 0;
        } else {
            retval = 1;
        }
    MUTEX_UNLOCK(&mutex.alarms);

    return retval;
}

int WantAlarms()
{
int retval;

    MUTEX_LOCK(&mutex.alarms);
        retval = want.alarms;
    MUTEX_UNLOCK(&mutex.alarms);

    return retval;
}

void NeedAlarms(int flag)
{
    MUTEX_LOCK(&mutex.alarms);
        want.alarms = flag;
    MUTEX_UNLOCK(&mutex.alarms);
}

/* Permission check */

int HavePerm(int test)
{
int perm = 0;

    if (test & PRIVILEGED) perm |= Privileged();
    if (test & CONNECTED)  perm |= Connected();
    if (test & OPERATOR)   perm |= Operator();
    if (test & LOCALHOST)  perm |= localhost;
    return ((test & perm) == test);
}

/* Revision History
 *
 * $Log: save.c,v $
 * Revision 1.5  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.4  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/02 20:30:44  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
