#pragma ident "$Id: dasops.c,v 1.5 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Convenience routines for communicating with the DAS.  They don't
 *  interact with the DAS directly, but do stick messages in the
 *  command queue for processing by the das_write thread.
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
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_DASOPS

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;

static MUTEX calib_mutex = MUTEX_INITIALIZER;

struct calib_info {
    char cmd[44];
    unsigned int wait;
} calib;

/* Load a das command into the message queue */

void das_command(char *cmd, int loglevel)
{
OLD_MSGQ_MSG *msg;
static char *fid = "das_command";

    msg = msgq_get(&Heap->commands, OLD_MSGQ_NOBLOCK);
    if (msg == (OLD_MSGQ_MSG *) NULL) {
        util_log(1, "DAS command queue full, force reset");
        set_dasreboot();
    } else {
        if (cmd[0] > ISPD_MAX_CMDLEN) {
            util_log(1, "FATAL ERROR - DAS command too long!");
            ispd_die(MY_MOD_ID + 1);
        }
        msg->data[0] = (char) loglevel;
        memcpy(msg->data+1, cmd, cmd[0]+1);
        msgq_put(&Q->das_write, msg);
    }
}

/* Reboot DAS */

void das_reboot()
{
static char cmd[3] = {2, DAS_RESET_CMD, DAS_CONFIRM_CMD};

    set_dasreboot();
    das_command(cmd, 1);
}

/* Packet acknowledgment */

void das_ack(int loglevel)
{
static char cmd[2] = {1, DAS_ACK};

    das_command(cmd, loglevel);
}

/* Packet reject */

void das_nak(int loglevel)
{
static char cmd[2] = {1, DAS_NAK};

    das_command(cmd, loglevel);
}

/* DAS IDDATA request */

void das_iddata(int loglevel)
{
static char cmd[2] = {1, DAS_SEND_IDDATA_CMD};

    das_command(cmd, loglevel);
}

void das_statreq(int loglevel)
{
static char cmd[2] = {1, DAS_SEND_STATS_CMD};

    das_command(cmd, loglevel);
    set_sreqsent();
}

/* Flush DAS logs */

void das_flushlog(int loglevel)
{
static char cmd[2] = {1, DAS_SENDLOG_CMD};

    das_command(cmd, loglevel);
}

/* Pause for calibration start time */
 
static THREAD_FUNC StartCalThread(void *dummy)
{
static char *fid = "StartCalThread";
 
    if (calib.wait) {
        MUTEX_LOCK(&calib_mutex);
            set_calib(ISP_CALIB_PENDING_AT_ISP);
        MUTEX_UNLOCK(&calib_mutex);
        sleep(calib.wait);
    }
    MUTEX_LOCK(&calib_mutex);
        if (calib_status() == ISP_CALIB_PENDING_AT_ISP) {
            das_command(calib.cmd, 1);
            set_calib(ISP_CALIB_PENDING);
        }
    MUTEX_UNLOCK(&calib_mutex);
    THREAD_EXIT(0);
}

/* Set up calibration */

void das_calibrate(int loglevel, struct isp_dascal *cal)
{
short stmp;
long  ltmp;
time_t wait;
int len, status, quit;
char *ptr;
THREAD tid;
static char *fid = "das_calibrate";

    if ((status = calib_status()) != ISP_CALIB_OFF) {
        if (status == ISP_CALIB_ON) {
            util_log(1, "cal request rejected: calibration already on");
        } else {
            util_log(1, "cal request rejected: calibration pending");
        }
        return;
    }

    assert(sizeof(short) == 2);
    assert(sizeof(long)  == 4);

    wait = cal->on - time(NULL);
    calib.wait = (wait < 0) ? 0 : (unsigned int) wait;
    set_calib(ISP_CALIB_PENDING_AT_ISP);
    cal->on = 0;

/* Package and deliver the calibration request */

    if (cal->preset == 1) {

        calib.cmd[0] = 1;
        calib.cmd[1] = DAS_CALIB1;

    } else if (cal->preset == 2) {

        calib.cmd[0] = 1;
        calib.cmd[1] = DAS_CALIB2;

    } else if (cal->preset == 3) {

        calib.cmd[0] = 1;
        calib.cmd[1] = DAS_CALIB3;

    } else if (cal->preset == 4) {

        calib.cmd[0] = 1;
        calib.cmd[1] = DAS_CALIB4;

    } else if (cal->preset == 5) {

        calib.cmd[0] = 1;
        calib.cmd[1] = DAS_CALIB5;

    } else if (cal->preset == 0) {

        calib.cmd[0] = 43;
        calib.cmd[1] = DAS_START_CALIBRATE_CMD;
        ptr    = calib.cmd + 2;

        stmp = cal->fnc; SSWAB(&stmp, 1);
        memcpy(ptr, &stmp, 2);
        ptr += 2;

        stmp = cal->amp; SSWAB(&stmp, 1);
        memcpy(ptr, &stmp, 2);
        ptr += 2;

        stmp = cal->wid; SSWAB(&stmp, 1);
        memcpy(ptr, &stmp, 2);
        ptr += 2;

        stmp = cal->ntr; SSWAB(&stmp, 1);
        memcpy(ptr, &stmp, 2);
        ptr += 2;

        ptr += 26; /* to skip over channel & gain fields */

        ltmp = cal->on; LSWAB(&ltmp, 1);
        memcpy(ptr, &ltmp, 4);
        ptr += 4;

        ltmp = cal->dur; LSWAB(&ltmp, 1);
        memcpy(ptr, &ltmp, 4);

    } else {
        util_log(1, "Unknown DAS calibration sequence (%d) ignored",
            cal->preset
        );
        return;
    }

/* Leave a thread to start the calib at the appropriate time */

    MUTEX_LOCK(&calib_mutex);
        if (!THREAD_CREATE(&tid, StartCalThread, NULL)) {
            util_log(1, "%s: failed to start StartCalThread", fid);
            util_log(1, "calibration request failed");
        }
    MUTEX_UNLOCK(&calib_mutex);
}

/* Abort calibration */

void das_abortcal(int loglevel)
{
static char cmd[2] = {1, DAS_END_CALIBRATE_CMD};

    MUTEX_LOCK(&calib_mutex);
        if (calib_status() == ISP_CALIB_PENDING_AT_ISP) {
            set_calib(ISP_CALIB_OFF);
            util_log(loglevel, "pending calibration aborted");
        } else {
            das_command(cmd, loglevel);
        }
    MUTEX_UNLOCK(&calib_mutex);
}

/* Set DAS log level to LOG_INFO */

void das_loginfo(int loglevel)
{
static char cmd[3] = {2, DAS_LOGLEVEL_CMD, DAS_LOG_INFO};

    das_command(cmd, loglevel);
    clear_alarm(ISP_ALARM_DASDBG);
}

/* Set DAS log level to LOG_DEBUG */

void das_logdebug(int loglevel)
{
static char cmd[3] = {2, DAS_LOGLEVEL_CMD, DAS_LOG_DEBUG};

    das_command(cmd, loglevel);

    set_alarm(ISP_ALARM_DASDBG);
}

/* Set default DAS configuration */

void das_setdfcf(int loglevel)
{
static char cmd[3] = {2, DAS_DEFAULT_CONFIG_CMD, DAS_CONFIRM_CMD};

    das_command(cmd, loglevel);
}

void das_setdftrig(int loglevel)
{
static char cmd[3] = {2, DAS_DEFAULT_TRIGCONFIG_CMD, DAS_CONFIRM_CMD};

    das_command(cmd, loglevel);
}

/* DAS initialization procedure */

static void do_das_init()
{
int count;

/* Turn of public output while we mess with the DAS */

    set_initializing();
    disable_output(1);

/* Ignore any earlier configurations we may have lying around */

    clear_have_dascnf();

/* Reboot the DAS to clear its queue and generate a fresh IDDATA record */ 
 
    das_reboot();
    count = 0;

    /* Wait until we get a current IDDATA record */
 
    while (!have_dascnf()) {
        if (count == Params->iddatato) {
            util_log(1, "no IDDATA from DAS");
            das_reboot();
            count = 0;
        }
        sleep(1);
        ++count;
    }
 
    /* Now that we have the current DAS configuration, make
     * sure that it agrees with what we want it to be.
     */
 
    ConfigureDas(1);
 
/* At this point the DAS is confirmed configured and we are done */
/* Clear all the uplink stats and re-enable public output        */
 
    das_loginfo(1);
    clear_commstats(2);
    clear_initializing();

}
static THREAD_FUNC DasInitThread(void *dummy)
{
    do_das_init();
    THREAD_EXIT(0);
}

void VerifyDasConfig(int wait)
{
THREAD tid;
static char *fid = "VerifyDasConfig";

    if (wait) {
        do_das_init();
        return;
    }

    if (!THREAD_CREATE(&tid, DasInitThread, NULL)) {
        util_log(1, "%s: failed to create DasInitThread", fid);
        ispd_die(MY_MOD_ID + 2);
    }

    return;
}

/* Revision History
 *
 * $Log: dasops.c,v $
 * Revision 1.5  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.4  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.3  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
