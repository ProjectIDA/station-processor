#pragma ident "$Id: beep.c,v 1.1 2000/10/19 22:24:52 dec Exp $"
/*======================================================================
 *
 * Beep the beeper if not beeped lately.
 *
 *====================================================================*/
#include "isp_console.h"

void SoundBeep(int flag)
{
static time_t last_beep = 0;
time_t now, elapsed;
static int status = BEEP_ENABLE;

/* Can explicitly turn on the beeps at anytime */

    if (flag == BEEP_ENABLE) {
        status = flag;
        return;
    }

/* Only operators hear beeps, and not even they if we are shut up */

    if (!Operator() || status == BEEP_SHUTUP) return;

/* If this is not a beep request, then it must be a modification flag */

    if (flag != BEEP_REQUEST) {
        status = flag;
        return;
    }

/* See when the last time we sounded a beep was */

    now = time(NULL);
    elapsed = now - last_beep;

/* If it has been more than an hour, then force enable */

    if (elapsed > 3600) status = BEEP_ENABLE;

/* Beep every 10 seconds */

    if (status == BEEP_ENABLE && elapsed > 10) {
        last_beep = now;
        beep();
    }
}

/* Revision History
 *
 * $Log: beep.c,v $
 * Revision 1.1  2000/10/19 22:24:52  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 */
