#pragma ident "$Id: stalta.c,v 1.2 2007/09/26 23:03:17 dechavez Exp $"
/*======================================================================
 * 
 * Run a block of data through the stalta engine
 *
 *====================================================================*/
#include "detect.h"

char *detectStaLtaTriggerString(DETECTOR_STALTA *stalta, char *buf)
{
static char *BadInput = "ERROR: detectStaLtaTriggerString(stalta == NULL)";
static char MtUnsafeSpace[1000];

    if (buf == NULL) buf = MtUnsafeSpace;
    if (stalta == NULL) return BadInput;

    if (stalta->trigger.state == DETECTOR_STATE_ON) {
        sprintf(buf, "ON:  ");
    } else {
        sprintf(buf, "off: ");
    }
    sprintf(buf, " STA=%lu LTA=%lu STA/LTA=%.2f", stalta->trigger.sta, stalta->trigger.lta, stalta->trigger.ratio);
    return buf;
}

/* Initialize workspace */

static BOOL InitWindow(DETECTOR_STALTA_WINDOW *window, UINT32 len)
{
    if ((window->buffer = (UINT32 *) calloc(len, sizeof(INT32))) == NULL) return FALSE;
    window->len = len;
    window->sum = 0;
    window->index = 0;

    return TRUE;
}

BOOL detectInitStaLta(DETECTOR_STALTA *stalta)
{
    stalta->work.crnt.state = stalta->trigger.state = DETECTOR_STATE_OFF;
    stalta->work.prev = 0;
    if (stalta->config.len.lta < stalta->config.len.sta) {
        errno = EINVAL;
        return FALSE;
    }
    if (stalta->config.ratio.on <= stalta->config.ratio.off) {
        errno = EINVAL;
        return FALSE;
    }
    if (!InitWindow(&stalta->work.sta, stalta->config.len.sta)) return FALSE;
    if (!InitWindow(&stalta->work.lta, stalta->config.len.lta)) return FALSE;
    stalta->work.initializing = stalta->config.len.lta;
    stalta->work.crnt.sta = 0;
    stalta->work.crnt.lta = 0;
    stalta->work.crnt.ratio = 0.0;
    stalta->work.crnt.state = DETECTOR_STATE_OFF;

    stalta->trigger = stalta->work.crnt;

    stalta->debug.enabled = FALSE;
    stalta->debug.nsamp = 0;
    stalta->debug.diff = NULL;
    stalta->debug.sta = NULL;
    stalta->debug.lta = NULL;
    stalta->debug.tla = NULL;
    stalta->debug.ratio = NULL;
    stalta->debug.state = NULL;

    return TRUE;
}

/* Update moving window average */

static UINT32 UpdateAverage(DETECTOR_STALTA_WINDOW *window, INT32 datum)
{
UINT32 result;

    window->sum -= window->buffer[window->index];
    window->buffer[window->index] = datum;
    window->sum += window->buffer[window->index];
    window->index = ++window->index % window->len;

    result = window->sum / window->len;

    return result;

}

/* Update STA/LTA ratio, look for detections */

static void UpdateRatio(DETECTOR_STALTA *stalta, INT32 datum)
{

/* First difference filter to remove the DC offset */

    if (datum > stalta->work.prev) {
        stalta->work.diff = datum - stalta->work.prev;
    } else {
        stalta->work.diff = stalta->work.prev - datum;
    }
    stalta->work.prev = datum;

/* Update STA and LTA */

    stalta->work.crnt.sta = UpdateAverage(&stalta->work.sta, stalta->work.diff);
    stalta->work.crnt.lta = UpdateAverage(&stalta->work.lta, stalta->work.diff);
    if (stalta->work.crnt.lta == 0) stalta->work.crnt.lta = 1;

/* Update the comparison LTA when the trigger is off */

    if (stalta->trigger.state == DETECTOR_STATE_OFF) stalta->trigger.lta = stalta->work.crnt.lta;

/* STA/LTA ratio */

    stalta->work.crnt.ratio = (REAL32) stalta->work.crnt.sta / (REAL32) stalta->trigger.lta;
}

/* Look for state transitions */

static int CheckState(DETECTOR_STALTA *stalta)
{
int result = DETECTOR_STATE_NOCHANGE;

    if (stalta->work.initializing) {
        --stalta->work.initializing;
    } else if (stalta->state == DETECTOR_STATE_OFF) {
        if (stalta->work.crnt.ratio >= stalta->config.ratio.on) {
            result = stalta->work.crnt.state = DETECTOR_STATE_ON;
            stalta->trigger = stalta->work.crnt;
        }
    } else if (stalta->state == DETECTOR_STATE_ON) {
        if (stalta->work.crnt.ratio <= stalta->config.ratio.off) {
            result = stalta->work.crnt.state = DETECTOR_STATE_OFF;
            stalta->trigger = stalta->work.crnt;
        }
    }

    return result;
}

int detectStaLtaINT32(DETECTOR_STALTA *stalta, INT32 *data, UINT32 nsamp)
{
BOOL SeenDetection = FALSE; /* TRUE if at least one sample results in a detection ON */
UINT32 i;

    if (stalta->debug.enabled && stalta->debug.nsamp < nsamp) {
        stalta->debug.diff = (INT32 *) realloc(stalta->debug.diff, nsamp * sizeof(INT32));
        stalta->debug.sta = (INT32 *) realloc(stalta->debug.sta, nsamp * sizeof(INT32));
        stalta->debug.lta = (INT32 *) realloc(stalta->debug.lta, nsamp * sizeof(INT32));
        stalta->debug.tla = (INT32 *) realloc(stalta->debug.tla, nsamp * sizeof(INT32));
        stalta->debug.ratio = (REAL32 *) realloc(stalta->debug.ratio, nsamp * sizeof(REAL32));
        stalta->debug.state = (INT32 *) realloc(stalta->debug.state, nsamp * sizeof(INT32));
        stalta->debug.nsamp = nsamp;
    }

/* Update and test for detection sample by sample */

    for (i = 0; i < nsamp; i++) {
        UpdateRatio(stalta, data[i]);
        CheckState(stalta);
        if (!SeenDetection && stalta->work.crnt.state == DETECTOR_STATE_ON) SeenDetection = TRUE;
        if (stalta->debug.enabled) {
            stalta->debug.diff[i] = stalta->work.diff;
            stalta->debug.sta[i] = stalta->work.crnt.sta;
            stalta->debug.lta[i] = stalta->work.crnt.lta;
            stalta->debug.tla[i] = stalta->trigger.lta;
            stalta->debug.ratio[i] = stalta->work.crnt.ratio;
            stalta->debug.state[i] = stalta->work.crnt.state;
        }
    }

    stalta->state = SeenDetection ? DETECTOR_STATE_ON : DETECTOR_STATE_OFF;

    return stalta->state;
}

/* Revision History
 *
 * $Log: stalta.c,v $
 * Revision 1.2  2007/09/26 23:03:17  dechavez
 * update LTA constantly, but retain frozen LTA for use in computing detrigger ratio
 *
 * Revision 1.1  2007/09/25 20:52:29  dechavez
 * initial production release
 *
 */
