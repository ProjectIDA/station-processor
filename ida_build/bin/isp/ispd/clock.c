#pragma ident "$Id: clock.c,v 1.7 2007/01/11 22:02:30 dechavez Exp $"
/*======================================================================
 *
 *  Support for external clock.  Generates external and "1-Hz" pairs
 *  with one second precision.
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
#include <sys/types.h>
#include <netinet/in.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_CLOCK

extern ISP_PARAMS  *Params;
extern struct isp_status  *Status;
extern struct isp_reftime *Reftime;

static SCLK *cp;

static MUTEX DebugMutex = MUTEX_INITIALIZER;
#define OFF 0
#define ON  1
static int DebugState = OFF;

void clockDebugToggle()
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
    util_log(1, "Clock debug messages %s", newval == ON ? "ON" : "OFF");
}

static int debugClock()
{
int retval;

    MUTEX_LOCK(&DebugMutex);
        retval = DebugState;
    MUTEX_UNLOCK(&DebugMutex);
    return retval;
}

/* Thread to continuously read data from external clock */

static THREAD_FUNC ReadClockThread(void *dummy)
{
int iqual, status, error = 0;
char crnt_qual = 0x0f;
char prev_qual = 0x0f;
time_t internal, offset;
struct sclk_tstamp external;
static char *fid = "ReadClockThread";

    util_log(1, "ISP time stamps from clock@%s,%d",
        Params->clock.port, Params->clock.baud
    );
    MUTEX_LOCK(&Status->lock);
        Status->extclock.enabled = 1;
    MUTEX_UNLOCK(&Status->lock);

    set_alarm(ISP_ALARM_AUXCLK);

    while (1) {
        if ((status = sclk_gettime(cp, &external)) != 0) {
            if (++error == 1) {
                set_alarm(ISP_ALARM_AUXCLK);
                util_log(1, "aux clock error: %s", syserrmsg(errno));
            }
            MUTEX_LOCK(&Status->lock);
                ++Status->extclock.err;
            MUTEX_UNLOCK(&Status->lock);
            crnt_qual = 0x0f;
        } else {
            internal = time(NULL);
#ifdef INCLUDE_GPS_SANITY_CHECKS
            if (labs(internal - external.time) > Params->clock.tolerance) {
                util_log(2, "%s: aux clock garbage?: %s",
                    fid, util_lttostr(external.time, 0)
                );
                MUTEX_LOCK(&Status->lock);
                    ++Status->extclock.err;
                MUTEX_UNLOCK(&Status->lock);
            } else {
#else
                MUTEX_LOCK(&Reftime->mutex);
                    Reftime->ext  = external.time;
                    Reftime->qual = external.qual;
                    Reftime->hz   = internal;
                    crnt_qual     = Reftime->qual;
                MUTEX_UNLOCK(&Reftime->mutex);
                if (debugClock()) {
                    util_log(1, "%s:    ext = %10ld = %s (qual=0x%02x)", fid,
                        Reftime->ext, util_lttostr(Reftime->ext, 0), Reftime->qual
                    );
                    util_log(1, "%s:     hz = %10ld = %s", fid,
                        Reftime->hz, util_lttostr(Reftime->hz, 0)
                    );
                    offset = Reftime->ext - Reftime->hz;
                    util_log(1, "%s: offset = %10ld = %s", fid,
                        offset, util_lttostr(offset, 8)
                    );
                }
                MUTEX_LOCK(&Status->lock);
                    isidbLookupTqual(Params->glob.db, external.qual, &iqual);
                    Status->extclock.datum = external.time;
                    Status->extclock.code  = external.qual;
                    Status->extclock.qual  = iqual;
                MUTEX_UNLOCK(&Status->lock);
#endif /* !INCLUDE_GPS_SANITY_CHECKS */
#ifdef INCLUDE_GPS_SANITY_CHECKS
            }
#endif /* INCLUDE_GPS_SANITY_CHECKS */
            if (prev_qual != crnt_qual) {
                if (crnt_qual == 'M') {
                    clear_alarm(ISP_ALARM_AUXCLK);
                } else if (prev_qual == 'M') {
                    set_alarm(ISP_ALARM_AUXCLK);
                }
            }
        }
        prev_qual = crnt_qual;
    }
}

/* Thread to fake external clock updates */

static THREAD_FUNC FakeClockThread(void *dummy)
{
time_t now, next;
static char *fid = "FakeClockThread";

    clear_alarm(ISP_ALARM_AUXCLK);

    util_log(1, "ISP time stamps generated using internal clock");
    MUTEX_LOCK(&Status->lock);
        Status->extclock.enabled = 0;
    MUTEX_UNLOCK(&Status->lock);

    now = time(NULL);

    while (1) {
        MUTEX_LOCK(&Reftime->mutex);
            Reftime->ext  = now;
            Reftime->hz   = now;
            Reftime->qual = 'Z';
        MUTEX_UNLOCK(&Reftime->mutex);
        next = now + 1;
        while ((now = time(NULL)) < next) usleep(250000);
    }
}

void InitClock(void)
{
THREAD tid;
int fake_it;
struct sclk_param sclk_param;
static char *fid = "InitClock";

    MUTEX_LOCK(&Status->lock);
        Status->extclock.enabled = 0;
        Status->extclock.err     = 0;
    MUTEX_UNLOCK(&Status->lock);

/* Use the external clock, if we have one and can read it */
 
    if (Params->clock.port != (char *) NULL) {
        cp = sclk_open(Params->clock.port, Params->clock.baud, &sclk_param);
        if (cp == (SCLK *) NULL) {
            util_log(1, "%s: sclk_open: %s", fid, syserrmsg(errno));
            util_log(1, "external clock disabled");
            fake_it = 1;
        }
        fake_it = 0;
    } else {
        fake_it = 1;
    }

/* Start the appropriate thread */

    if (fake_it) {
        if (!THREAD_CREATE(&tid, FakeClockThread, NULL)) {
            util_log(1, "%s: failed to create FakeClockThread", fid);
            ispd_die(MY_MOD_ID + 1);
        }
    } else {
        if (!THREAD_CREATE(&tid, ReadClockThread, NULL)) {
            util_log(1, "%s: failed to create ReadClockThread", fid);
            ispd_die(MY_MOD_ID + 2);
        }
    }
}

/* Revision History
 *
 * $Log: clock.c,v $
 * Revision 1.7  2007/01/11 22:02:30  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.6  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.5  2002/03/15 22:51:36  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2001/04/19 22:45:57  dec
 * fixed unitilized Hz bug, added ISP_DEBUG_CLOCK support
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
