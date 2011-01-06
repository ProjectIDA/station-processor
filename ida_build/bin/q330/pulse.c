#pragma ident "$Id: pulse.c,v 1.4 2009/02/04 17:51:04 dechavez Exp $"
/*======================================================================
 *
 *  Pulse sensor control line(s)
 *
 *====================================================================*/
#include "q330cc.h"

#define DEFAULT_DURATION 100

static BOOL PulseHelp(Q330 *q330)
{
    fprintf(stderr, "usage: %s line [dur=duration]\n", q330->cmd.name);
    fprintf(stderr, "line should be one of: a=calib a=mp a=lock a=unlock b=calib b=mp b=lock b=unlock\n");
    fprintf(stderr, "default duration is %.2f seconds\n", (float) DEFAULT_DURATION / 100.0);
    return FALSE;
}

static char *LineString(int line)
{
int i;
static struct {int line; char *string;} map[] = {
    {QDP_SC_SENSOR_A_CALIB,  "sensor A calibration enable"},
    {QDP_SC_SENSOR_A_CENTER, "sensor A mass recenter"},
    {QDP_SC_SENSOR_A_LOCK,   "sensor A lock"},
    {QDP_SC_SENSOR_A_UNLOCK, "sensor A unlock"},
    {QDP_SC_SENSOR_A_CALIB,  "sensor B calibration enable"},
    {QDP_SC_SENSOR_A_CENTER, "sensor B mass recenter"},
    {QDP_SC_SENSOR_A_LOCK,   "sensor B lock"},
    {QDP_SC_SENSOR_A_UNLOCK, "sensor B unlock"},
    {-1, NULL}
};
static char *undefined = "<LOGIC ERROR IN LineString!!!!>";

    for (i = 0; map[i].string != NULL; i++) if (map[i].line == line) return map[i].string;

    return undefined;
}

static BOOL UpdateBitmap(UINT16 *bitmap, UINT32 *sc, int line)
{
int i;

    for (i = 0; i < QDP_NSC; i++) {
        if ((sc[i] & QDP_SC_MASK) == line) {
            if (sc[i] & QDP_SC_ACTIVE_HIGH_BIT) {
                *bitmap |= (1 << i);
            } else {
                *bitmap &= ~(1 << i);
            }
            return TRUE;
        }
    }

    fprintf(stderr, "ERROR: requested %s line is not defined.\n", LineString(line));
    fprintf(stderr, "Verify the sensor control map ('sc' command).\n");
    return FALSE;
}

BOOL VerifyPulse(Q330 *q330)
{
int i;
char *arg;

    q330->cmd.code = Q330_CMD_PULSE;

    q330->cmd.pulse.duration = DEFAULT_DURATION;
    for (q330->cmd.pulse.line = -1, i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];
        if (strcasecmp(arg, "help") == 0) {
            return PulseHelp(q330);

        } else if (strcasecmp(arg, "a=calib") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_CALIB;
        } else if (strcasecmp(arg, "a=mp") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_CENTER;
        } else if (strcasecmp(arg, "a=lock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_LOCK;
        } else if (strcasecmp(arg, "a=unlock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_A_UNLOCK;

        } else if (strcasecmp(arg, "b=calib") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_CALIB;
        } else if (strcasecmp(arg, "b=mp") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_CENTER;
        } else if (strcasecmp(arg, "b=lock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_LOCK;
        } else if (strcasecmp(arg, "b=unlock") == 0) {
            q330->cmd.pulse.line = QDP_SC_SENSOR_B_UNLOCK;

        } else if (strncasecmp(arg, "dur=", strlen("dur=")) == 0) {
            q330->cmd.pulse.duration = (UINT16) (atof(arg+strlen("dur=")) * 100);
        } else {
            fprintf(stderr, "ERROR: unrecognized %s argument '%s'\n",  q330-> cmd.name, arg);
            return PulseHelp(q330);
        }
    }

    if (q330->cmd.pulse.line < 0) return PulseHelp(q330);
    if (q330->cmd.pulse.duration < 1) {
        fprintf(stderr, "ERROR: illegal duration\n",  arg);
        return PulseHelp(q330);
    }

    return TRUE;
}

void pulse(Q330 *q330)
{
QDP_PKT pkt;
UINT16 bitmap;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_GLOB glob;

    if (!qdp_C1_RQSC(q330->qdp, sc)) {
        fprintf(stderr, "ERROR: unable to retrieve sensor control mapping!\n");
        return;
    }
    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) {
        fprintf(stderr, "ERROR: unable to retrieve global programming!\n");
        return;
    }
    bitmap = glob.sensor_map;

    if (!UpdateBitmap(&bitmap, sc, q330->cmd.pulse.line)) return;


    qdpEncode_C1_PULSE(&pkt, bitmap, q330->cmd.pulse.duration);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("pulse sensor control lines command failed\n");
    } else {
        printf("Pulsing %s line ", LineString(q330->cmd.pulse.line));
        printf("for %.2f seconds\n", (float) q330->cmd.pulse.duration / 100.0);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: pulse.c,v $
 * Revision 1.4  2009/02/04 17:51:04  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.3  2009/01/26 22:24:29  dechavez
 * Fixed bug setting pulse duration, changed default pulse width to one
 * second, fixed bug specifying sensor b recentering, changed flag for
 * recenter line from mass to mp.
 *
 * Revision 1.2  2009/01/26 21:21:52  dechavez
 * fixed bug in setting pulse duration
 *
 * Revision 1.1  2009/01/24 00:13:10  dechavez
 * initial release
 *
 */
