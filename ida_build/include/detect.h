#pragma ident "$Id: detect.h,v 1.3 2007/09/26 23:00:09 dechavez Exp $"
#ifndef detect_h_included
#define detect_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Detector types */

#define DETECTOR_TYPE_UNDEFINED 0
#define DETECTOR_TYPE_STALTA    1

/* Detector states */

#define DETECTOR_STATE_OFF       0
#define DETECTOR_STATE_ON        1
#define DETECTOR_STATE_NOCHANGE -100

/* STA/LTA ratio event detector (type DETECTOR_TYPE_STALTA) */

typedef struct {
    struct {
        UINT32  sta;  /* STA length (samples) */
        UINT32  lta;  /* LTA length (samples) */
    } len;
    struct {
        REAL32 on;    /* event ON threshold (STA/LTA) */
        REAL32 off;   /* event OFF threshold (STA/LTA) */
    } ratio;
} DETECTOR_STALTA_CONFIG;

typedef struct {
    UINT32 sta;
    UINT32 lta;
    REAL32 ratio;
    INT32  state;
} DETECTOR_STALTA_TRIGGER;

typedef struct {
    INT32 *buffer;
    INT32 sum;
    INT32 len;
    UINT32 index;
} DETECTOR_STALTA_WINDOW;

typedef struct {
    INT32 diff; /* filtered sample */
    INT32 prev; /* previous datum */
    DETECTOR_STALTA_WINDOW sta; /* used to compute current STA */
    DETECTOR_STALTA_WINDOW lta; /* used to compute current LTA */
    DETECTOR_STALTA_TRIGGER crnt; /* current values of trigger data */
    UINT32 initializing; /* non-zero while LTA window has not been filled */
} DETECTOR_STALTA_WORK;

typedef struct {
    BOOL enabled;
    UINT32 nsamp;
    INT32  *diff;
    INT32  *sta;
    INT32  *lta;
    INT32  *tla;
    REAL32 *ratio;
    INT32  *state;
} DETECTOR_STALTA_DEBUG;
    
typedef struct {
    DETECTOR_STALTA_CONFIG config;   /* static configuration */
    DETECTOR_STALTA_WORK work;       /* workspace */
    DETECTOR_STALTA_DEBUG debug;     /* debug space */
    DETECTOR_STALTA_TRIGGER trigger; /* values that caused state to change */
    INT32 state;                     /* detector state after processing one block of data */
} DETECTOR_STALTA;

/* Generic event detector */

typedef struct {
    int type;     /* DETECTOR_TYPE_x */
    BOOL debug;   /* TRUE to enable saving of internal results */
    DETECTOR_STALTA stalta;
    int state;    /* DETECTOR_STATE_x */
} DETECTOR;

/* Handle for doing detections */

/* Function prototypes */

/* init.c */
BOOL detectInit(DETECTOR *engine);

/* process.c */
int detectProcessINT32(DETECTOR *detector, INT32 *data, UINT32 nsamp);

/* stalta.c */
char *detectStaLtaTriggerString(DETECTOR_STALTA *stalta, char *buf);
BOOL detectInitStaLta(DETECTOR_STALTA *stalta);
int detectStaLtaINT32(DETECTOR_STALTA *stalta, INT32 *data, UINT32 nsamp);

/* string.c */
char *detectTriggerString(DETECTOR *engine, char *buf);

/* version.c */
char *detectVersionString(VOID);
VERSION *detectVersion(VOID);

#ifdef __cplusplus
}
#endif

#endif /* detect_h_included */

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
 * $Log: detect.h,v $
 * Revision 1.3  2007/09/26 23:00:09  dechavez
 * libdetect 1.0.1
 *
 * Revision 1.2  2007/09/25 20:56:20  dechavez
 * completed STALTA support (libdetect 1.0.0)
 *
 * Revision 1.1  2007/09/22 02:38:39  dechavez
 * initial release
 *
 */
