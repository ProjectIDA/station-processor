#pragma ident "$Id: q330cc.h,v 1.8 2009/02/23 22:12:38 dechavez Exp $"
#ifndef q330cc_h_included
#define q330cc_h_included
#include "platform.h"
#include "stdtypes.h"
#include "qdp.h"
#include "q330.h"

#include "defaults.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2007 - Regents of the University of California.";

typedef struct {
    char *name;
    int code;
    LNKLST *arglst;
    UINT32 p32;
    BOOL update;
    struct {
        int line;
        UINT16 duration;
    } pulse;
    struct {
        UINT32 map[QDP_NSC];
        int sensorA;
        int sensorB;
    } sc;
    QDP_TYPE_C1_SPP spp;
    QDP_TYPE_C2_GPS gps;
} Q330_CMD;

typedef struct {
    Q330_CFG *cfg;
    Q330_ADDR addr;
    int port;
    Q330_CMD cmd;
    int debug;
    LOGIO *lp;
    QDP *qdp;
    BOOL NeedRegistration;
} Q330;

#define Q330_CMD_UNDEFINED 0
#define Q330_CMD_SAVE      1
#define Q330_CMD_REBOOT    2
#define Q330_CMD_RESYNC    3
#define Q330_CMD_GPS_ON    4
#define Q330_CMD_GPS_OFF   5
#define Q330_CMD_GPS_COLD  6
#define Q330_CMD_GPS_CNF   7
#define Q330_CMD_GPS_ID    8
#define Q330_CMD_CAL_START 9
#define Q330_CMD_CAL_STOP 10
#define Q330_CMD_IFCONFIG 11
#define Q330_CMD_STATUS   12
#define Q330_CMD_FIX      13
#define Q330_CMD_GLOB     14
#define Q330_CMD_SC       15
#define Q330_CMD_PULSE    16
#define Q330_CMD_AMASS    17
#define Q330_CMD_DCP      18
#define Q330_CMD_CO       19
#define Q330_CMD_SPP      20
#define Q330_CMD_MAN      21
#define Q330_CMD_SAVEBOOT 22

/* Function prototypes */

/* main.c */
int main (int argc, char **argv);

/* amass.c */
BOOL VerifyAmass(Q330 *q330);
void amass(Q330 *q330);

/* calib.c */
BOOL VerifyCalib(Q330 *q330);
void calStart(Q330 *q330);
void calStop(Q330 *q330);

/* cmd.c */
BOOL BadArgLstCount(Q330 *q330);
BOOL UnexpectedArg(Q330 *q330, char *arg);
BOOL VerifyQ330Cmd(Q330 *q330);

/* dcp.c */
BOOL VerifyDcp(Q330 *q330);
void dcp(Q330 *q330);

/* fixvar.c */
BOOL VerifyFixvar(Q330 *q330);
void fixvar(Q330 *q330);

/* glob.c */
BOOL VerifyGlob(Q330 *q330);
BOOL glob(Q330 *q330);

/* gps.c */
BOOL VerifyGps(Q330 *q330);
void gpsON(Q330 *q330);
void gpsOFF(Q330 *q330);
void gpsColdStart(Q330 *q330);
void gpsCnf(Q330 *q330);
void gpsId(Q330 *q330);

/* ifconfig.c */
BOOL VerifyIfconfig(Q330 *q330);
void ifconfig(Q330 *q330);

/* init.c */
void init(int argc, char **argv, Q330 *q330);

/* man.c */
BOOL VerifyMan(Q330 *q330);
void man(Q330 *q330);

/* pulse.c */
BOOL VerifyPulse(Q330 *q330);
void pulse(Q330 *q330);

/* reboot.c */
BOOL VerifyReboot(Q330 *q330);
void boot(Q330 *q330);

/* resync.c */
BOOL VerifyResync(Q330 *q330);
void resync(Q330 *q330);

/* save.c */
BOOL VerifySave(Q330 *q330);
void save(Q330 *q330);
void saveboot(Q330 *q330);

/* sc.c */
BOOL rqsc(Q330 *q330, UINT32 *sc);
BOOL VerifySC(Q330 *q330);
void sc(Q330 *q330);

/* spp.c */
BOOL VerifySpp(Q330 *q330);
void spp(Q330 *q330);

/* status.c */
BOOL VerifyStatus(Q330 *q330);
void status(Q330 *q330);

/* trigger.c */
BOOL VerifyTrigger(Q330 *q330);
void triggerON(Q330 *q330);
void triggerOFF(Q330 *q330);

#endif /* q330cc_h_included */

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
 * $Log: q330cc.h,v $
 * Revision 1.8  2009/02/23 22:12:38  dechavez
 * added spp and gps fields to handle
 *
 * Revision 1.7  2009/02/04 17:49:56  dechavez
 * support for spp, dcp, man, gps cnf, gps id, save (re)boot
 *
 * Revision 1.6  2009/01/24 00:13:42  dechavez
 * support for amass, pulse, and sc commands
 *
 * Revision 1.5  2009/01/05 22:49:06  dechavez
 * added save command
 *
 * Revision 1.4  2008/10/02 23:02:46  dechavez
 * updated prototypes, added new command codes
 *
 * Revision 1.3  2007/12/14 22:09:58  dechavez
 * 0.0.2 checkpoint
 *
 * Revision 1.2  2007/10/31 17:55:37  dechavez
 * checkpoint, gps, reboot, resync added
 *
 * Revision 1.1  2007/10/16 22:01:07  dechavez
 * checkpoint
 *
 */
