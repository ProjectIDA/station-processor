#pragma ident "$Id: calib.c,v 1.4 2009/05/14 18:13:44 dechavez Exp $"
/*======================================================================
 *
 *  Calibration control
 *
 *====================================================================*/
#include "q330cc.h"

static void CalibHelp(Q330 *q330)
{
    fprintf(stderr, "usage: %s { start [ now | yyyy:ddd-hh:mm:ss ] | stop }\n", q330->cmd.name);
    exit(1);
}

static void CalibStartHelp(Q330 *q330)
{
static char *details =
"     wf = waveform (0=sine 1=red noise 2=white noise 3=step 4=random binary)\n"
"    amp = amplitude bit shift (0=-6db 1=-12db etc)\n"
"    dur = duration in seconds\n"
" settle = settling time in seconds\n"
"trailer = trailer time in seconds\n"
"  chans = calibration channel bitmap (eg. 7 = chans 1, 2, 3)\n"
"    mon = monitor channel bitmap (eg. 8 = chan 4)\n"
"divisor = frequency divisor (for sine, freq = 1/divisor, all others, freq = 125/divisor, in Hz)\n";

    fprintf(stderr, "usage: %s  start [ now | yyyy:ddd-hh:mm:ss ] wf amp dur settle trailer chans mon divisor\n", q330->cmd.name);
    fprintf(stderr, "\n%s\n", details);
    exit(1);
}

typedef struct {
    char *text;
    char *description;
    UINT16 code;
} TEXT_MAP;

static TEXT_MAP WaveformMap[] = {
    {"sine",  "sine wave",     QDP_QCAL_SINE},
    {"red",   "red noise",     QDP_QCAL_RED},
    {"white", "white noise",   QDP_QCAL_WHITE},
    {"step",  "step",          QDP_QCAL_STEP},
    {"rb",    "random binary", QDP_QCAL_RB},
    {NULL, NULL, -1}
};

static char *WaveformString(UINT16 value)
{
int i;
static char *undefined = "UNDEFINED";

    for (i = 0; WaveformMap[i].text != NULL; i++) {
        if (value == WaveformMap[i].code) return WaveformMap[i].description;
    }
    return undefined;
}

static char *ChannelString(UINT16 bitmap)
{
int i;
static char none[] = " none";
static char string[64];

    if (bitmap == 0) return none;

    string[0] = 0;
    for (i = 0; i < QDP_NCHAN; i++) {
        if (bitmap & (1 << i)) sprintf(string+strlen(string), " %d", i+1);
    }
    return string;
}

static char *StartTimeString(UINT32 tstamp)
{
static char *now = "immediately";
static char string[64];

    return (tstamp == 0) ? now : utilLttostr(tstamp + QDP_EPOCH_TO_1970_EPOCH, 0, string);
}

static float FrequencyValue(QDP_TYPE_C1_QCAL *calib)
{
float base, result;

    base = calib->waveform == QDP_QCAL_SINE ? 1.0 : 125.0;
    result = base / calib->divisor;

    return result;
}

static void PrintCalib(QDP_TYPE_C1_QCAL *calib)
{
float freq;

    freq = FrequencyValue(calib);

    printf("        start time = %s\n", StartTimeString(calib->starttime));
    printf("          waveform = %s\n", WaveformString(calib->waveform));
    printf("         amplitude = -%ddb\n", 6 * (calib->amplitude + 1));
    printf("          duration = %hu seconds\n", calib->duration);
    printf("     settling time = %hu seconds\n", calib->settle);
    printf("      trailer time = %hu seconds\n", calib->trailer);
    printf("   calibrate chans = %s\n", ChannelString(calib->chans));
    printf("     monitor chans = %s\n", ChannelString(calib->monitor));
    printf("         frequency = %.4f Hz (%.4f sec period)\n", freq, 1.0 / freq);
}

static BOOL StartAtSpecifiedTime(Q330 *q330)
{
char *tstring;

    tstring = (char *) q330->cmd.arglst->array[1];

    if (strcasecmp(tstring, "now") == 0 || strcasecmp(tstring, "0") == 0) {
        q330->cfg->calib.starttime = 0;
    } else {
        q330->cfg->calib.starttime = utilAttodt(tstring) - QDP_EPOCH_TO_1970_EPOCH;
    }

    return TRUE;
}

static BOOL StartCustom(Q330 *q330)
{
    q330->cfg->calib.waveform = (UINT16) atoi(q330->cmd.arglst->array[2]);
    q330->cfg->calib.amplitude = (UINT16) atoi(q330->cmd.arglst->array[3]);
    q330->cfg->calib.duration = (UINT16) atoi(q330->cmd.arglst->array[4]);
    q330->cfg->calib.settle = (UINT16) atoi(q330->cmd.arglst->array[5]);
    q330->cfg->calib.trailer = (UINT16) atoi(q330->cmd.arglst->array[6]);
    q330->cfg->calib.chans = (UINT16) atoi(q330->cmd.arglst->array[7]);
    q330->cfg->calib.monitor = (UINT16) atoi(q330->cmd.arglst->array[8]);
    q330->cfg->calib.divisor = (UINT16) atoi(q330->cmd.arglst->array[9]);

    return StartAtSpecifiedTime(q330);
}

BOOL VerifyCalib(Q330 *q330)
{
char *arg;

    if (q330->cmd.arglst->count == 0) CalibHelp(q330);

    arg = (char *) q330->cmd.arglst->array[0];

    if (strcasecmp(arg, "start") == 0) {
        q330->cmd.code = Q330_CMD_CAL_START;
        switch (q330->cmd.arglst->count) {
          case 1:
            q330->cfg->calib.starttime = 0;
            return TRUE;
          case 2:
            arg = (char *) q330->cmd.arglst->array[1];
            if (strcasecmp(arg, "help") == 0 || strcasecmp(arg, "?") == 0) CalibStartHelp(q330);
            return StartAtSpecifiedTime(q330);
          case 10:
            return StartCustom(q330);
          default:
            CalibStartHelp(q330);
            return FALSE;
        }
    } else if (strcasecmp(arg, "stop") == 0) {
        if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);
        q330->cmd.code = Q330_CMD_CAL_STOP;
    } else {
        return UnexpectedArg(q330, arg);
    }
    return TRUE;
}

void calStart(Q330 *q330)
{
QDP_PKT pkt;

    qdpEncode_C1_QCAL(&pkt, &q330->cfg->calib);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("calibration start command failed\n");
    } else {
        printf("calibration start command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
        PrintCalib(&q330->cfg->calib);
    }
}

void calStop(Q330 *q330)
{
QDP_PKT pkt;

    qdpInitPkt(&pkt, QDP_C1_STOP, 0, 0);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("calibration stop command failed\n");
    } else {
        printf("calibration stop command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: calib.c,v $
 * Revision 1.4  2009/05/14 18:13:44  dechavez
 * Fixed bug in reporting calibration amplitude
 *
 * Revision 1.3  2008/01/04 19:24:21  dechavez
 * added missing return value in FrequencyValue()
 * (this worked without return under Solaris... scary!!!)
 *
 * Revision 1.2  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
