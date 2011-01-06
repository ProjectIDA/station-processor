#pragma ident "$Id: sc.c,v 1.3 2009/02/23 22:13:06 dechavez Exp $"
/*======================================================================
 *
 *  Sensor Control Mapping
 *
 *====================================================================*/
#include "q330cc.h"

#define UNDEFINED_INST -1
#define NONE      0
#define STS1      1
#define STS2      2
#define KS54K     3
#define GURHI     4
#define GURLO     5
#define EPISENSOR 6
#define FBA23     7
#define OTHER     8

static struct {
    char *name;
    int code;
} instmap[] = {
    {"sts1", STS1},
    {"sts2", STS2},
    {"gurhi", GURHI},
    {"gurlo", GURLO},
    {"ks54k", KS54K},
    {"epi", EPISENSOR},
    {"fba23", FBA23},
    {"none", NONE},
    {"other", OTHER},
    {NULL, UNDEFINED_INST}
};

static BOOL SCHelp(Q330 *q330)
{
int i;

    fprintf(stderr, "usage: %s [ a=sensor ] [ b=sensor ]\n", q330->cmd.name);
    fprintf(stderr, "sensor should be one of:");
    for (i = 0; instmap[i].name != NULL; i++) fprintf(stderr, " %s", instmap[i].name);
    fprintf(stderr, "\n");
    return FALSE;
}

static int instype(char *want)
{
int i;

    for (i = 0; instmap[i].name != NULL; i++) {
        if (strcasecmp(want, instmap[i].name) == 0) return instmap[i].code;
    }
    return UNDEFINED_INST;
}

static BOOL SetInst(Q330 *q330, char *arg)
{
int type;
char *want;

    if (strcasecmp(arg, "help") == 0) return SCHelp(q330);

    if (strncasecmp(arg, "a=", strlen("a="))== 0) {
        want = arg + strlen("a=");
        if ((q330->cmd.sc.sensorA = instype(want)) == UNDEFINED_INST) {
            fprintf(stderr, "sensor A type '%s' unrecognized\n", want);
            return SCHelp(q330);
        }
    } else if (strncasecmp(arg, "b=", strlen("b=")) == 0) {
        want = arg + strlen("b=");
        if ((q330->cmd.sc.sensorB = instype(want)) == UNDEFINED_INST) {
            fprintf(stderr, "sensor B type '%s' unrecognized\n", want);
            return SCHelp(q330);
        }
    } else {
        fprintf(stderr, "unrecognized argument '%s'\n", arg);
        return SCHelp(q330);
    }

    q330->cmd.update = TRUE;
    return TRUE;
}

BOOL VerifySC(Q330 *q330)
{
    q330->cmd.update = FALSE;
    q330->cmd.sc.sensorA = q330->cmd.sc.sensorB = UNDEFINED_INST;
    q330->cmd.code = Q330_CMD_SC;

    switch (q330->cmd.arglst->count) {
      case 0:
        return TRUE;
      case 1:
        return SetInst(q330, (char *) q330->cmd.arglst->array[0]);
      case 2:
        if (SetInst(q330, (char *) q330->cmd.arglst->array[0])) {
            return SetInst(q330, (char *) q330->cmd.arglst->array[1]);
        }
        break;
    }

    return SCHelp(q330);
}

static void updateA(UINT32 *sc, int type)
{
int i;

    switch (type) {
      case STS2:
        sc[0] = QDP_SC_SENSOR_A_CENTER | QDP_SC_ACTIVE_HIGH_BIT;
        sc[1] = QDP_SC_IDLE;
        sc[2] = QDP_SC_IDLE;
        sc[3] = QDP_SC_SENSOR_A_CALIB  | QDP_SC_ACTIVE_HIGH_BIT;
        break;
      case GURHI:
        sc[0] = QDP_SC_SENSOR_A_CENTER | QDP_SC_ACTIVE_HIGH_BIT;
        sc[1] = QDP_SC_SENSOR_A_UNLOCK | QDP_SC_ACTIVE_HIGH_BIT;
        sc[2] = QDP_SC_SENSOR_A_LOCK   | QDP_SC_ACTIVE_HIGH_BIT;
        sc[3] = QDP_SC_SENSOR_A_CALIB  | QDP_SC_ACTIVE_HIGH_BIT;
        break;
      case GURLO:
        sc[0] = QDP_SC_SENSOR_A_CENTER;
        sc[1] = QDP_SC_SENSOR_A_UNLOCK;
        sc[2] = QDP_SC_SENSOR_A_LOCK;
        sc[3] = QDP_SC_SENSOR_A_CALIB;
        break;
      default:
        for (i = 0; i < 4; i++) sc[i] = 0;
    }
}

static void updateB(UINT32 *sc, int type)
{
int i;

    switch (type) {
      case STS2:
        sc[4] = QDP_SC_SENSOR_B_CENTER | QDP_SC_ACTIVE_HIGH_BIT;
        sc[5] = QDP_SC_IDLE;
        sc[6] = QDP_SC_IDLE;
        sc[7] = QDP_SC_SENSOR_B_CALIB  | QDP_SC_ACTIVE_HIGH_BIT;
        break;
      case GURHI:
        sc[4] = QDP_SC_SENSOR_B_CENTER | QDP_SC_ACTIVE_HIGH_BIT;
        sc[5] = QDP_SC_SENSOR_B_UNLOCK | QDP_SC_ACTIVE_HIGH_BIT;
        sc[6] = QDP_SC_SENSOR_B_LOCK   | QDP_SC_ACTIVE_HIGH_BIT;
        sc[7] = QDP_SC_SENSOR_B_CALIB  | QDP_SC_ACTIVE_HIGH_BIT;
        break;
      case GURLO:
        sc[4] = QDP_SC_SENSOR_B_CENTER;
        sc[5] = QDP_SC_SENSOR_B_UNLOCK;
        sc[6] = QDP_SC_SENSOR_B_LOCK;
        sc[7] = QDP_SC_SENSOR_B_CALIB;
        break;
      default:
        for (i = 4; i < 8; i++) sc[i] = 0;
    }
}

void sc(Q330 *q330)
{
int i;
BOOL IsIdle, IsActiveHigh;
QDP_PKT pkt;
UINT32 sc[QDP_NSC];
QDP_TYPE_C1_GLOB glob;

/* Get current mapping, plus global data (sensor bitmap lives there) */

    if (!qdp_C1_RQSC(q330->qdp, sc)) return;
    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

/* Print current values */

    if (q330->cmd.update) printf("Current ");
    qdpPrint_C1_SC(stdout, sc);
    printf("C1_GLOB SC bitmap = "); utilPrintBinUINT16(stdout, glob.sensor_map); printf("\n");

/* All done if we aren't changing anything */

    if (!q330->cmd.update) return;

/* Define and install new sensor control line definitions */

    if (q330->cmd.sc.sensorA != UNDEFINED_INST) updateA(sc, q330->cmd.sc.sensorA);
    if (q330->cmd.sc.sensorB != UNDEFINED_INST) updateB(sc, q330->cmd.sc.sensorB);

    qdpEncode_C1_SC(&pkt, sc);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Physical Interfaces command failed\n");
        return;
    }

/* Define and install sensor control bitmap based on the above */

    for (glob.sensor_map = 0, i = 0; i < QDP_NSC; i++) {
        IsIdle       = ((UINT32) sc[i] & QDP_SC_MASK) == QDP_SC_IDLE  ? TRUE : FALSE;
        IsActiveHigh = ((UINT32) sc[i] & QDP_SC_ACTIVE_HIGH_BIT) != 0 ? TRUE : FALSE;
        if ((IsIdle && IsActiveHigh) || (!IsIdle && !IsActiveHigh)) glob.sensor_map |=  (1 << i);
    }

    qdpEncode_C1_GLOB(&pkt, &glob);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Global Programming command failed\n");
        return;
    }

/* Retrieve the freshly installed values, and print them out */

    if (!qdp_C1_RQSC(q330->qdp, sc)) return;
    if (!qdp_C1_RQGLOB(q330->qdp, &glob)) return;

    printf("\nUpdated ");
    qdpPrint_C1_SC(stdout, sc);
    printf("C1_GLOB SC bitmap = "); utilPrintBinUINT16(stdout, glob.sensor_map); printf("\n");
    printf("Changes will not take effect until configuration is saved and the Q330 is rebooted.\n");
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
 * $Log: sc.c,v $
 * Revision 1.3  2009/02/23 22:13:06  dechavez
 * permit setting individual or default values
 *
 * Revision 1.2  2009/02/04 17:47:20  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.1  2009/01/24 00:13:09  dechavez
 * initial release
 *
 */
