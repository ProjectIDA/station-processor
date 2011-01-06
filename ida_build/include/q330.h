#pragma ident "$Id: q330.h,v 1.6 2008/03/11 20:50:28 dechavez Exp $"
/*======================================================================
 *
 *  Q330 utilities (NOT QDP communication stuff)
 *
 *====================================================================*/
#ifndef q330_h_included
#define q330_h_included

#include "platform.h"
#include "isi.h" /* for string lengths */
#include "detect.h"
#include "util.h"
#include "logio.h"
#include "qdp.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#ifndef Q330_DEFAULT_CFG_PATH
#define Q330_DEFAULT_CFG_PATH "/usr/nrts/etc/q330.cfg"
#endif /* !Q330_DEFAULT_CFG_PATH */

#ifndef Q330_CFG_ENV_STRING
#define Q330_CFG_ENV_STRING "Q330_CFG"
#endif /* !Q330_CFG_ENV_STRING */

#define Q330_MAX_CAL_DURATION 16383
#define Q330_MAX_CAL_AMPLTUDE     0
#define Q330_MAX_CAL_DIVISOR    255

#define Q330_DEFAULT_STARTTIME                     0 /* now */
#define Q330_DEFAULT_WAVEFORM            QDP_QCAL_RB /* random binary */
#define Q330_DEFAULT_AMPLITUDE Q330_MAX_CAL_AMPLTUDE /* -6db */
#define Q330_DEFAULT_DURATION  Q330_MAX_CAL_AMPLTUDE /* 4.5 hours */
#define Q330_DEFAULT_SETTLE                       60 /* 1 minute */
#define Q330_DEFAULT_TRAILER                      60 /* 1 minute */
#define Q330_DEFAULT_CHANS                      0x07 /* chans 1, 2, 3 */
#define Q330_DEFAULT_SENSOR                        0 /* do nothing */
#define Q330_DEFAULT_MONITOR                    0x08 /* chan 4 */
#define Q330_DEFAULT_DIVISOR    Q330_MAX_CAL_DIVISOR /* 2.04 seconds period */

#define Q330_DEFAULT_CALIB { \
    Q330_DEFAULT_STARTTIME,  \
    Q330_DEFAULT_WAVEFORM,   \
    Q330_DEFAULT_AMPLITUDE,  \
    Q330_DEFAULT_DURATION,   \
    Q330_DEFAULT_SETTLE,     \
    Q330_DEFAULT_TRAILER,    \
    Q330_DEFAULT_CHANS,      \
    Q330_DEFAULT_SENSOR,     \
    Q330_DEFAULT_MONITOR,    \
    Q330_DEFAULT_DIVISOR     \
}

/* Structures */

typedef struct {
    char name[MAXPATHLEN+1]; /* digitizer name (or IP address) */
    UINT64 serialno;         /* serial number */
    UINT64 authcode;         /* authorization code */
} Q330_ADDR;

#define Q330_DEFAULT_SITE_NAME "default"

typedef struct {
    BOOL enabled;                /* TRUE if we have something here */
    char name[ISI_STALEN+1];     /* site name for which this applies */
    char channels[MAXPATHLEN+1]; /* comma delimited list of names of all triggered channels */
    int votes;                   /* mininum number of "on" channels required to declare an event */
    UINT32 pre;                  /* pre-event memory duration, in seconds */
    UINT32 pst;                  /* post-event memory duration, in seconds */
    DETECTOR engine;             /* event detector parameters */
} Q330_DETECTOR;

typedef struct {
    char fname[MAXPATHLEN+1]; /* name of cfg file that was read */
    int naddr;                /* number of entries to follow */
    Q330_ADDR *addr;          /* array of digitizer addresses */
    int ndetector;            /* number of entries to follow */
    Q330_DETECTOR *detector;  /* array of event detectors */
    QDP_TYPE_C1_QCAL calib;   /* default calibration parameters */
} Q330_CFG;

/* function prototypes */

/* cfg.c */
void q330DestroyCfg(Q330_CFG *cfg);
Q330_CFG *q330ReadCfg(char *name);
BOOL q330LookupAddr(char *name, Q330_CFG *cfg, Q330_ADDR *addr);
BOOL q330LookupDetector(char *name, Q330_CFG *cfg, Q330_DETECTOR *detector);

/* register.c */
QDP *q330Register(Q330_ADDR *addr, int port, int debug, LOGIO *lp);

/* version.c */
char *q330VersionString(void);
VERSION *q330Version(void);

#ifdef __cplusplus
}
#endif

#endif /* q330_h_included */

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
 * $Log: q330.h,v $
 * Revision 1.6  2008/03/11 20:50:28  dechavez
 * defined Q330_MAX_CAL_DURATION, Q330_MAX_CAL_AMPLTUDE, and Q330_MAX_CAL_DIVISOR
 * changed default duration to Q330_MAX_CAL_DURATION
 *
 * Revision 1.5  2007/12/14 21:34:29  dechavez
 * added "calib" support
 *
 * Revision 1.4  2007/10/31 17:20:22  dechavez
 * updated prototypes
 *
 * Revision 1.3  2007/09/25 20:55:30  dechavez
 * added enabled field to detector
 *
 * Revision 1.2  2007/09/22 02:38:17  dechavez
 * added detector
 *
 * Revision 1.1  2007/09/06 18:24:39  dechavez
 * created
 *
 */
