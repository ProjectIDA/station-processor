#pragma ident "$Id: gps.c,v 1.5 2009/02/23 22:08:34 dechavez Exp $"
/*======================================================================
 *
 *  Q330 GPS control
 *
 *====================================================================*/
#include "q330cc.h"

static void GpsHelp(Q330 *q330)
{
    fprintf(stderr, "usage: %s { on | off | cold | id | cnf [ internal | exernal ] }\n", q330->cmd.name);
    exit(1);
}

BOOL VerifyGps(Q330 *q330)
{
char *arg;
static QDP_TYPE_C2_GPS DefaultInternal = QDP_DEFAULT_TYPE_C2_GPS_INTERNAL;
static QDP_TYPE_C2_GPS DefaultExternal = QDP_DEFAULT_TYPE_C2_GPS_EXTERNAL;

    switch (q330->cmd.arglst->count) {
      case 1:
        arg = (char *) q330->cmd.arglst->array[0];
        if (strcasecmp(arg, "on") == 0) {
            q330->cmd.code = Q330_CMD_GPS_ON;
        } else if (strcasecmp(arg, "off") == 0) {
            q330->cmd.code = Q330_CMD_GPS_OFF;
        } else if (strcasecmp(arg, "cold") == 0) {
            q330->cmd.code = Q330_CMD_GPS_COLD;
        } else if (strcasecmp(arg, "cnf") == 0) {
            q330->cmd.code = Q330_CMD_GPS_CNF;
            q330->cmd.update = FALSE;
        } else if (strcasecmp(arg, "id") == 0) {
            q330->cmd.code = Q330_CMD_GPS_ID;
        } else {
            return UnexpectedArg(q330, arg);
        }
        return TRUE;

      case 2:
        arg = (char *) q330->cmd.arglst->array[0];
        if (strcasecmp(arg, "cnf") != 0)  return UnexpectedArg(q330, arg);
        q330->cmd.code = Q330_CMD_GPS_CNF;
        arg = (char *) q330->cmd.arglst->array[1];
        if (strncasecmp(arg, "int", strlen("int")) == 0) {
            q330->cmd.gps = DefaultInternal;
            q330->cmd.update = TRUE;
        } else if (strncasecmp(arg, "ext", strlen("ext")) == 0) {
            q330->cmd.gps = DefaultExternal;
            q330->cmd.update = TRUE;
        } else {
            return UnexpectedArg(q330, arg);
        }
        return TRUE;
    }

    GpsHelp(q330);
}

void gpsON(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_ON, TRUE)) {
        printf("gps ON command failed\n");
    } else {
        printf("gps ON command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

void gpsOFF(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_OFF, TRUE)) {
        printf("gps OFF command failed\n");
    } else {
        printf("gps OFF command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

void gpsColdStart(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_GPS_COLDSTART, TRUE)) {
        printf("gps cold start command failed\n");
    } else {
        printf("gps cold start command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

void gpsCnf(Q330 *q330)
{
QDP_PKT pkt;
QDP_TYPE_C2_GPS gps;

    if (q330->cmd.update) {
        qdpEncode_C2_GPS(&pkt, &q330->cmd.gps);
        if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
            fprintf(stderr, "qdp_C1_SGPS failed\n");
            return;
        }
    }

    if (!qdp_C2_RQGPS(q330->qdp, &gps)) {
        fprintf(stderr, "qdp_C2_RQGPS failed\n");
    } else {
        qdpPrint_C2_GPS(stdout, &gps);
    }
}

void gpsId(Q330 *q330)
{
QDP_TYPE_C1_GID gid[QDP_NGID];

    if (!qdp_C1_RQGID(q330->qdp, gid)) {
        fprintf(stderr, "qdp_C1_RQGID failed\n");
    } else {
        qdpPrint_C1_GID(stdout, gid);
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
 * $Log: gps.c,v $
 * Revision 1.5  2009/02/23 22:08:34  dechavez
 * permit setting GPS parameters via internal/external option to gps cnf
 *
 * Revision 1.4  2009/02/04 17:48:33  dechavez
 * added cnf and id subcommands
 *
 * Revision 1.3  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
