#pragma ident "$Id: status.c,v 1.4 2009/02/04 17:47:20 dechavez Exp $"
/*======================================================================
 *
 *  Request Status
 *
 *====================================================================*/
#include "q330cc.h"

static void StatusHelp(Q330 *q330)
{
    fprintf(stderr, "usage: %s [ full | { global ps mp gps gsv pll arp dp1 dp2 dp3 dp4 enet sp1 sp2 sp3 } ]\n", q330->cmd.name);
}

BOOL VerifyStatus(Q330 *q330)
{
int i;
char *arg;
UINT32 bitmap = QDP_STATUS_DEFAULT_BITMAP |
                QDP_STATUS_DATA_PORT_1    |
                QDP_STATUS_DATA_PORT_2    |
                QDP_STATUS_DATA_PORT_3    |
                QDP_STATUS_DATA_PORT_4;

    q330->cmd.code = Q330_CMD_STATUS;
    if (q330->cmd.arglst->count == 0) {
        q330->cmd.p32 = bitmap;
        return TRUE;
    }

    q330->cmd.p32 = 0;
    for (i = 0; i < q330->cmd.arglst->count; i++) {
        arg = (char *) q330->cmd.arglst->array[i];

        if (strcasecmp(arg, "full") == 0) {
            q330->cmd.p32 |= bitmap;
        } else if (strcasecmp(arg, "help") == 0) {
            StatusHelp(q330);
            return FALSE;
        } else if (strcasecmp(arg, "?") == 0) {
            StatusHelp(q330);
            return FALSE;
        } else if (strcasecmp(arg, "global") == 0) {
            q330->cmd.p32 |= QDP_STATUS_GLOBAL;
        } else if (strcasecmp(arg, "ps") == 0) {
            q330->cmd.p32 |= QDP_STATUS_POWER_SUPPLY;
        } else if (strcasecmp(arg, "mp") == 0) {
            q330->cmd.p32 |= QDP_STATUS_BOOM_ETC;
        } else if (strcasecmp(arg, "gps") == 0) {
            q330->cmd.p32 |= QDP_STATUS_GPS;
        } else if (strcasecmp(arg, "gsv") == 0) {
            q330->cmd.p32 |= QDP_STATUS_SATELLITES;
        } else if (strcasecmp(arg, "pll") == 0) {
            q330->cmd.p32 |= QDP_STATUS_PLL;
        } else if (strcasecmp(arg, "arp") == 0) {
            q330->cmd.p32 |= QDP_STATUS_ARP;
        } else if (strcasecmp(arg, "dp1") == 0) {
            q330->cmd.p32 |= QDP_STATUS_DATA_PORT_1;
        } else if (strcasecmp(arg, "dp2") == 0) {
            q330->cmd.p32 |= QDP_STATUS_DATA_PORT_2;
        } else if (strcasecmp(arg, "dp3") == 0) {
            q330->cmd.p32 |= QDP_STATUS_DATA_PORT_3;
        } else if (strcasecmp(arg, "dp4") == 0) {
            q330->cmd.p32 |= QDP_STATUS_DATA_PORT_4;
        } else if (strcasecmp(arg, "sp1") == 0) {
            q330->cmd.p32 |= QDP_STATUS_SERIAL_1;
        } else if (strcasecmp(arg, "sp2") == 0) {
            q330->cmd.p32 |= QDP_STATUS_SERIAL_2;
        } else if (strcasecmp(arg, "sp3") == 0) {
            q330->cmd.p32 |= QDP_STATUS_SERIAL_3;
        } else if (strcasecmp(arg, "enet") == 0) {
            q330->cmd.p32 |= QDP_STATUS_ETHERNET;
        } else {
            fprintf(stderr, "ERROR: unrecognized %s argument '%s'\n", q330->cmd.name, arg);
            StatusHelp(q330);
            return FALSE;
        }
    }

    return q330->cmd.p32 ? TRUE : FALSE;
}

void status(Q330 *q330)
{
int i;
QDP_TYPE_C1_STAT stat;

    if (!qdp_C1_RQSTAT(q330->qdp, q330->cmd.p32, &stat)) {
        fprintf(stderr, "qdp_C1_RQSTAT failed\n");
        return;
    } else {
        qdpPrint_C1_STAT(stdout, &stat);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
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
 * $Log: status.c,v $
 * Revision 1.4  2009/02/04 17:47:20  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.3  2008/10/03 19:41:41  dechavez
 * Allow specifying an arbitrary number of status selectors
 *
 * Revision 1.2  2008/10/03 17:13:57  dechavez
 * Added gsv command, removed satellites from gps selection
 *
 * Revision 1.1  2008/10/02 23:04:26  dechavez
 * created
 *
 */
