#pragma ident "$Id: flags.c,v 1.4 2001/05/20 17:43:46 dec Exp $"
/*======================================================================
 *
 *  Module to handle flag operations.
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

extern struct isp_status *Status;

/* Set when DAS initialization sequence is in progress */

static MUTEX initializing_lock = MUTEX_INITIALIZER;
static int     initializing_flag = 0;

/* Set when a DAS status request has been sent, clear when received */

static MUTEX sreqsent_lock = MUTEX_INITIALIZER;
static int     sreqsent_flag = 0;

/* Set when we have a configuration record from the DAS */

static MUTEX have_dascnf_lock = MUTEX_INITIALIZER;
static int     have_dascnf_flag = 0;

/* Describes configuration status */

static MUTEX config_lock = MUTEX_INITIALIZER;
static int     config_flag = ISP_CONFIG_UNKNOWN;

/* Set when DAS is desired */

static MUTEX dasreboot_lock = MUTEX_INITIALIZER;
static int     dasreboot_flag = 0;

/* Set when the DAS is triggered */

static MUTEX event_lock = MUTEX_INITIALIZER;
static int     event_flag = 0;

/* Describes calibration status */

static MUTEX calib_lock = MUTEX_INITIALIZER;
static int     calib_flag = ISP_CALIB_OFF;

/* Set when output to the public ring is enabled */

static MUTEX output_enabled_lock = MUTEX_INITIALIZER;
static int     output_enabled_flag = 0;

/* Set when system shutdown is in progress */

static MUTEX shutting_down_lock = MUTEX_INITIALIZER;
static int     shutting_down_flag = 0;

/* Set flags */

void set_initializing()
{
    MUTEX_LOCK(&initializing_lock);
        initializing_flag = 1;
    MUTEX_UNLOCK(&initializing_lock);
}

void set_sreqsent()
{
    MUTEX_LOCK(&sreqsent_lock);
        sreqsent_flag = 1;
    MUTEX_UNLOCK(&sreqsent_lock);
}

void set_have_dascnf()
{
    MUTEX_LOCK(&have_dascnf_lock);
        have_dascnf_flag = 1;
    MUTEX_UNLOCK(&have_dascnf_lock);
}

void set_config(int flag)
{
    MUTEX_LOCK(&config_lock);
        config_flag = flag;
    MUTEX_UNLOCK(&config_lock);
}

void set_dasreboot()
{
    MUTEX_LOCK(&dasreboot_lock);
        dasreboot_flag = 1;
    MUTEX_UNLOCK(&dasreboot_lock);
}

void set_event(int new_state)
{
int transition;

    MUTEX_LOCK(&event_lock);
        transition = (event_flag != new_state);
        event_flag = new_state;
    MUTEX_UNLOCK(&event_lock);

    if (transition) {
        if (event_flag) {
            util_log(1, "trigger ON");
        } else {
            util_log(1, "trigger OFF");
        }
    }
}

void set_calib(int new_state)
{
int transition;

    MUTEX_LOCK(&calib_lock);
        transition = (calib_flag != new_state);
        calib_flag = new_state;
    MUTEX_UNLOCK(&calib_lock);

    if (transition) {
        switch (calib_flag) {
          case ISP_CALIB_ON:
            util_log(1, "calibration ON");
            break;
          case ISP_CALIB_PENDING:
            util_log(1, "calibration PENDING");
            break;
          case ISP_CALIB_OFF:
            util_log(1, "calibration OFF");
            break;
        }
    }
}

void set_shutdown(int state)
{
    MUTEX_LOCK(&shutting_down_lock);
        shutting_down_flag = state;
    MUTEX_UNLOCK(&shutting_down_lock);
}

void enable_output(int loglevel)
{
    if (!output_enabled()) {
        MUTEX_LOCK(&output_enabled_lock);
            output_enabled_flag = 1;
        MUTEX_UNLOCK(&output_enabled_lock);
        util_log(loglevel, "public data output enabled");
    }
    das_statreq(3);
}

/* Clear flags */

void clear_initializing()
{
    MUTEX_LOCK(&initializing_lock);
        initializing_flag = 0;
    MUTEX_UNLOCK(&initializing_lock);
}

void clear_sreqsent()
{
    MUTEX_LOCK(&sreqsent_lock);
        sreqsent_flag = 0;
    MUTEX_UNLOCK(&sreqsent_lock);
}

void clear_have_dascnf()
{
    MUTEX_LOCK(&have_dascnf_lock);
        have_dascnf_flag = 0;
    MUTEX_UNLOCK(&have_dascnf_lock);
}

void clear_dasrebootflag()
{
    MUTEX_LOCK(&dasreboot_lock);
        dasreboot_flag = 0;
    MUTEX_UNLOCK(&dasreboot_lock);
}

void disable_output(int loglevel)
{
    if (output_enabled()) {
        MUTEX_LOCK(&output_enabled_lock);
            output_enabled_flag = 0;
        MUTEX_UNLOCK(&output_enabled_lock);
        util_log(loglevel, "public data output disabled");
    }
}

/* Test flags */

int initializing()
{
int status;

    MUTEX_LOCK(&initializing_lock);
        status = initializing_flag;
    MUTEX_UNLOCK(&initializing_lock);

    return status;
}

int sreqsent()
{
int status;

    MUTEX_LOCK(&sreqsent_lock);
        status = sreqsent_flag;
    MUTEX_UNLOCK(&sreqsent_lock);

    return status;
}

int have_dascnf()
{
int status;

    MUTEX_LOCK(&have_dascnf_lock);
        status = have_dascnf_flag;
    MUTEX_UNLOCK(&have_dascnf_lock);

    return status;
}

int event_in_progress()
{
int status;

    MUTEX_LOCK(&event_lock);
        status = event_flag;
    MUTEX_UNLOCK(&event_lock);

    return status;
}

int calib_status()
{
int status;

    MUTEX_LOCK(&calib_lock);
        status = calib_flag;
    MUTEX_UNLOCK(&calib_lock);

    return status;
}

int reboot_pending()
{
int status;

    MUTEX_LOCK(&dasreboot_lock);
        status = dasreboot_flag;
    MUTEX_UNLOCK(&dasreboot_lock);

    return status;
}

int config_status()
{
int status;

    MUTEX_LOCK(&config_lock);
        status = config_flag;
    MUTEX_UNLOCK(&config_lock);

    return status;
}

int output_enabled()
{
int status;

    MUTEX_LOCK(&output_enabled_lock);
        status = output_enabled_flag;
    MUTEX_UNLOCK(&output_enabled_lock);

    return status;
}

int shutting_down()
{
int status;

    MUTEX_LOCK(&shutting_down_lock);
        status = shutting_down_flag;
    MUTEX_UNLOCK(&shutting_down_lock);

    return status;
}

/* The following manipulate the alarm bitmap */

void set_alarm(int alarm)
{
time_t now;
int already;
static char *fid = "set_alarm";

    MUTEX_LOCK(&Status->lock);
        already = (Status->alarm.code & alarm);
    MUTEX_UNLOCK(&Status->lock);

    if (already) return;

    now = time(NULL);

    MUTEX_LOCK(&Status->lock);
        switch (alarm) {
          case ISP_ALARM_DAS:
            if (Status->digitizer == ISP_DAS) {
                util_log(1, "set DAS alarm");
            } else {
                util_log(1, "set SAN alarm");
            }
            Status->alarm.das = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_IOERR:
            util_log(1, "set IOERR alarm");
            Status->alarm.ioerr = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_OPER:
            util_log(1, "set OPER alarm");
            Status->alarm.oper = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_OFFLINE:
            util_log(1, "set OFFLINE alarm");
            Status->alarm.offline = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_OQFULL:
            util_log(1, "set OQFULL alarm");
            Status->alarm.oqfull = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_BQFULL:
            util_log(1, "set BQFULL alarm");
            Status->alarm.bqfull = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_RQFULL:
            util_log(1, "set RQFULL alarm");
            Status->alarm.rqfull = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_AUX:
            util_log(1, "set AUX alarm");
            Status->alarm.aux = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_DASDBG:
            util_log(1, "set DASDBG alarm");
            Status->alarm.dasdbg = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_DASCLK:
            util_log(2, "set DASCLK alarm");
            Status->alarm.dasclk = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_AUXCLK:
            util_log(2, "set AUXCLK alarm");
            Status->alarm.auxclk = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_NRTS:
            util_log(1, "set NRTS alarm");
            Status->alarm.nrts = now;
            Status->alarm.code |= alarm;
            break;

          case ISP_ALARM_ADC:
            util_log(1, "set ADC alarm");
            Status->alarm.adc = now;
            Status->alarm.code |= alarm;
            break;

          default:
            util_log(1, "%s: unrecognized alarm 0x%0x ignored", fid, alarm);
        }
    MUTEX_UNLOCK(&Status->lock);
}

void clear_alarm(int alarm)
{
int already;
static char *fid = "clear_alarm";

    MUTEX_LOCK(&Status->lock);
        already = ((Status->alarm.code & alarm) == 0);
    MUTEX_UNLOCK(&Status->lock);

    if (already) return;

    MUTEX_LOCK(&Status->lock);
        switch (alarm) {
          case ISP_ALARM_DAS:
            if (Status->digitizer == ISP_DAS) {
                util_log(1, "clear DAS alarm");
            } else {
                util_log(1, "clear SAN alarm");
            }
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_OPER:
            util_log(1, "clear OPER alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_IOERR:
            util_log(1, "clear IOERR alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_OFFLINE:
            util_log(1, "clear OFFLINE alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_OQFULL:
            util_log(1, "clear OQFULL alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_BQFULL:
            util_log(1, "clear BQFULL alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_RQFULL:
            util_log(1, "clear RQFULL alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_DASDBG:
            util_log(1, "clear DASDBG alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_DASCLK:
            util_log(2, "clear DASCLK alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_AUXCLK:
            util_log(2, "clear AUXCLK alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_AUX:
            util_log(1, "clear AUX alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_NRTS:
            util_log(1, "clear NRTS alarm");
            Status->alarm.code &= ~alarm;
            break;

          case ISP_ALARM_ADC:
            util_log(1, "clear ADC alarm");
            Status->alarm.code &= ~alarm;
            break;

          default:
            util_log(1, "%s: unrecognized alarm 0x%0x ignored", fid, alarm);
        }
    MUTEX_UNLOCK(&Status->lock);
}

/* Revision History
 *
 * $Log: flags.c,v $
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/08 01:57:23  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.2  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
