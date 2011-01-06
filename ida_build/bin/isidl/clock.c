#pragma ident "$Id: clock.c,v 1.4 2007/02/08 22:56:49 dechavez Exp $"
/*======================================================================
 *
 *  Generate time stamps with 1-sec precision.
 *
 *  The time stamp consists of 3 values.  The "sys" time is the host
 *  computer time at the moment of the event.  The "ext" time is the
 *  GPS time at some arbitrary time in the past.  The "pps" time is
 *  the host computer time at the moment the GSP clock was read.
 *
 *  The difference between the "ext" and "pps" time can be used to
 *  infer what the true time is for the corresponding "sys" value.
 *
 *  Actual support for an GPS clock is deferred pending determination
 *  of a need for this.  At the moment this code is only planned for
 *  use at BFO, where the host time is disciplined via NTP so there is
 *  no need for a GPS reality check.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_CLOCK

MUTEX mutex;
static ISIDL_TIMESTAMP CrntTstamp;

/* Thread to fake external clock updates */

static THREAD_FUNC FakeClockThread(void *unused)
{
time_t now, next;
static char *fid = "FakeClockThread";

    LogMsg(LOG_INFO, "generating aux device time stamps from internal clock");

    now = time(NULL);

    while (1) {
        MUTEX_LOCK(&mutex);
            CrntTstamp.ext = now;
            CrntTstamp.pps = now;
            CrntTstamp.code = 'N';
        MUTEX_UNLOCK(&mutex);
        next = now + 1;
        while ((now = time(NULL)) < next) utilDelayMsec(250);
    }
}

void GetTimeStamp(ISIDL_TIMESTAMP *dest)
{
static char *fid = "GetTimeStamp";

    if (dest == NULL) {
        errno = EINVAL;
        LogMsg(LOG_INFO, "%s: unexpected NULL input", fid);
        Exit(MY_MOD_ID + 1);
    }

    MUTEX_LOCK(&mutex);
        *dest = CrntTstamp;
    MUTEX_UNLOCK(&mutex);
    dest->sys = time(NULL);
}

void StartTimingSubsystem(char *GPSstring)
{
THREAD tid;
static char *fid = "StartTimingSubsystem";

    MUTEX_INIT(&mutex);

    if (GPSstring != NULL) LogMsg(LOG_INFO, "GPS timing support not implemented");

    if (!THREAD_CREATE(&tid, FakeClockThread, NULL)) {
        util_log(1, "%s: THREAD_CREATE: FakeClockThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: clock.c,v $
 * Revision 1.4  2007/02/08 22:56:49  dechavez
 * LOG_ERR, LOG_WARN to LOG_INFO
 *
 * Revision 1.3  2006/06/02 21:06:08  dechavez
 * change clock code to N when no GPS is attached
 *
 * Revision 1.2  2006/04/03 21:38:55  dechavez
 * flag faked time stamps as locked
 *
 * Revision 1.1  2006/03/30 22:01:28  dechavez
 * initial release (internal clock only)
 *
 */
