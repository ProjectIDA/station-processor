#pragma ident "$Id: tokens.h,v 1.3 2007/05/17 22:27:44 dechavez Exp $"
/*======================================================================
 *
 * Q330 DP Tokens
 *
 *====================================================================*/
#ifndef qdp_tokens_h_included
#define qdp_tokens_h_included

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QDP_NNAME_LEN 2 /* network name */
#define QDP_SNAME_LEN 5 /* station name */
#define QDP_CNAME_LEN 3 /* channel name */
#define QDP_LNAME_LEN 2 /* location code */
#define QDP_PASCALSTRING_LEN 0xff

/* type 1 - Token Version Number */

typedef struct {
    UINT8 version;
    BOOL valid;
} QDP_TOKEN_VERSION;

/* type 2 - Network and Station ID */

typedef struct {
    char nname[QDP_NNAME_LEN+1];
    char sname[QDP_SNAME_LEN+1];
    BOOL valid;
} QDP_TOKEN_SITE;

/* type 3, 5, 9 - server ports */

typedef struct {
    UINT16 port;
    BOOL valid;
} QDP_TOKEN_SERVER;

typedef struct {
    QDP_TOKEN_SERVER net;
    QDP_TOKEN_SERVER web;
    QDP_TOKEN_SERVER data;
} QDP_TOKEN_SERVICES;

/* type 4 - Data Subscription Server Parameters */

typedef struct {
    struct {
        char hi[8];
        char mi[8];
        char lo[8];
    } passwd;
    UINT32 timeout;
    UINT32 maxbps;
    UINT8 verbosity;
    UINT8 maxcpu;
    UINT16 port;
    UINT16 maxmem;
    BOOL valid;
} QDP_TOKEN_DSS;

/* type 6 - Clock Processing Parameters */

typedef struct {
    INT32 offset;
    UINT16 maxlim;
    struct {
        UINT8 locked;
        UINT8 track;
        UINT8 hold;
        UINT8 off;
        UINT8 spare;
    } pll;
    UINT8 maxhbl;
    UINT8 minhbl;
    UINT8 nbl;
    UINT16 clkqflt;
    BOOL valid;
} QDP_TOKEN_CLOCK;

/* type 7 - Log and Timing Identification */

typedef struct {
    struct {
        char loc[QDP_LNAME_LEN+1];
        char chn[QDP_CNAME_LEN+1];
    } mesg, time;
    BOOL valid;
} QDP_TOKEN_LOGID;

/* type 8 - Configuration Identification */

typedef struct {
    char loc[QDP_LNAME_LEN+1];
    char chn[QDP_CNAME_LEN+1];
    UINT8 flags;
    UINT16 interval;
    BOOL valid;
} QDP_TOKEN_CNFID;

/* type 128 - Logical Channel Queue */

typedef struct {
    UINT32 len; /* number of samples between reports */
    UINT8  filt; /* optional IIR filter to use or QDP_LCQ_AVE_FILT_NONE */
#define QDP_LCQ_AVE_FILT_NONE 0xff
} QDP_LCQ_AVEPAR;

typedef struct {
    UINT8 src; /* src LCQ number */
    UINT8 fir; /* decimation FIR filter number */
} QDP_LCQ_DECIM;

typedef struct {
    UINT8 base;    /* detector number to use as base */
    UINT8 use;     /* detector number for this invocation */
    UINT8 options; /* option bitmap (see QDP_LCQ_DETECT_x descriptions below) */
    BOOL  set;     /* TRUE when contents have been set */
} QDP_LCQ_DETECT;

#define QDP_LCQ_DETECT_RUN            0x01 /* bit 0 */
#define QDP_LCQ_DETECT_LOG_ENABLED    0x02 /* bit 1 */
#define QDP_LCQ_DETECT_BIT_2_RESERVED 0x04 /* bit 2 */
#define QDP_LCQ_DETECT_LOG_MESSAGE    0x08 /* bit 2 */
#define QDP_LCQ_DETECT_BIT_4_RESERVED 0x10 /* bit 4 */
#define QDP_LCQ_DETECT_BIT_5_RESERVED 0x20 /* bit 5 */
#define QDP_LCQ_DETECT_BIT_6_RESERVED 0x40 /* bit 6 */
#define QDP_LCQ_DETECT_BIT_7_RESERVED 0x80 /* bit 7 */

typedef struct {
    int len;                   /* length of this token */
    char loc[QDP_LNAME_LEN+1]; /* location code */
    char chn[QDP_CNAME_LEN+1]; /* channel code (seed name) */
    UINT8 ref;                 /* LCQ reference number */
#define QDP_LCQSRC_LEN 2
    UINT8 src[QDP_LCQSRC_LEN]; /* DT_DATA channel, filter code for 24-bit data, parameter no. for other */
    UINT32 options;            /* option bitmap (see bit descriptions below) */
    INT16 rate;                /* sample rate (pos => Hz, neg => 1/-rate (Hz)) */
    REAL64 frate;              /* derived sample rate in seconds */
    REAL64 sint;               /* derived sample interval in seconds */
    /* the following are defined only if their corresponding option bit is set */
    UINT16 pebuf;              /* number of pre-event buffers */
    REAL32 gapthresh;          /* gap threshold */
    UINT16 caldly;             /* calibration delay in seconds */
    UINT8 comfr;               /* maximum fram count */
    REAL32 firfix;             /* FIR multiplier */
    UINT8 cntrl;               /* control detector number */
    QDP_LCQ_AVEPAR ave;        /* averaging parameters */
    QDP_LCQ_DECIM decim;       /* decimation entry */
#define QDP_LCQ_NUM_DETECT 8
    QDP_LCQ_DETECT detect[QDP_LCQ_NUM_DETECT]; /* detector to run */
} QDP_TOKEN_LCQ;

/* LCQ option bits */

#define QDP_LCQ_TRIGGERED_MODE         0x00000001 /* bit  0 */
#define QDP_LCQ_WRITE_DETECT_PKTS      0x00000002 /* bit  1 */
#define QDP_LCQ_WRITE_CALIB_PKTS       0x00000004 /* bit  2 */
#define QDP_LCQ_HAVE_PRE_EVENT_BUFFERS 0x00000008 /* bit  3 */
#define QDP_LCQ_HAVE_GAP_THRESHOLD     0x00000010 /* bit  4 */
#define QDP_LCQ_HAVE_CALIB_DELAY       0x00000020 /* bit  5 */
#define QDP_LCQ_HAVE_FRAME_COUNT       0x00000040 /* bit  6 */
#define QDP_LCQ_HAVE_FIR_MULTIPLIER    0x00000080 /* bit  7 */
#define QDP_LCQ_HAVE_AVEPAR            0x00000100 /* bit  8 */
#define QDP_LCQ_HAVE_CNTRL_DETECTOR    0x00000200 /* bit  9 */
#define QDP_LCQ_HAVE_DECIM_ENTRY       0x00000400 /* bit 10 */
#define QDP_LCQ_DO_NOT_OUTPUT          0x00000800 /* bit 11 */
#define QDP_LCQ_HAVE_DETECTOR_1        0x00001000 /* bit 12 */
#define QDP_LCQ_HAVE_DETECTOR_2        0x00002000 /* bit 13 */
#define QDP_LCQ_HAVE_DETECTOR_3        0x00004000 /* bit 14 */
#define QDP_LCQ_HAVE_DETECTOR_4        0x00008000 /* bit 15 */
#define QDP_LCQ_HAVE_DETECTOR_5        0x00010000 /* bit 16 */
#define QDP_LCQ_HAVE_DETECTOR_6        0x00020000 /* bit 17 */
#define QDP_LCQ_HAVE_DETECTOR_7        0x00040000 /* bit 18 */
#define QDP_LCQ_HAVE_DETECTOR_8        0x00080000 /* bit 19 */
#define QDP_LCQ_BIT_20_UNUSED          0x00100000 /* bit 20 */
#define QDP_LCQ_BIT_21_UNUSED          0x00200000 /* bit 21 */
#define QDP_LCQ_BIT_22_UNUSED          0x00400000 /* bit 22 */
#define QDP_LCQ_BIT_23_UNUSED          0x00800000 /* bit 23 */
#define QDP_LCQ_BIT_24_UNUSED          0x01000000 /* bit 24 */
#define QDP_LCQ_BIT_25_UNUSED          0x02000000 /* bit 25 */
#define QDP_LCQ_BIT_26_UNUSED          0x04000000 /* bit 26 */
#define QDP_LCQ_BIT_27_UNUSED          0x08000000 /* bit 27 */
#define QDP_LCQ_BIT_28_UNUSED          0x10000000 /* bit 28 */
#define QDP_LCQ_SEND_TO_DATA_SERVER    0x20000000 /* bit 29 */
#define QDP_LCQ_SEND_TO_NET_SERVER     0x40000000 /* bit 30 */
#define QDP_LCQ_FORCE_CNP_BLOCKETTTES  0x80000000 /* bit 31 */

/* type 129 - IIR filter specification */

typedef struct {
    REAL32 ratio;
    UINT8 npole;
#define QDP_IIR_HIPASS 0
#define QDP_IIR_LOPASS 1
    int type;
} QDP_IIR_DATA;

typedef struct {
    int len;                   /* length of this token */
    UINT8 id;
    char name[QDP_PASCALSTRING_LEN+1];
    UINT8 nsection;
    REAL32 gain;
    REAL32 refreq;
    QDP_IIR_DATA data[0xff];
} QDP_TOKEN_IIR;

/* types 130 - FIR Filter specification */

typedef struct {
    int len;                   /* length of this token */
    UINT8 id;
    char name[QDP_PASCALSTRING_LEN+1];
} QDP_TOKEN_FIR;

/* type 131 - Control Detector Specification */

typedef struct {
    int len;                           /* length of this token */
    UINT8 id;                          /* control detector number */
    UINT8 options;                     /* control detector options */
    char name[QDP_PASCALSTRING_LEN+1]; /* control detector name */
    UINT8 equation[0xff];              /* detector equation */
    int nentry;                        /* number valid elements in the equation */
} QDP_TOKEN_CDS;

/* type 132 - Murdock Hutt Detector Specification */

typedef struct {
    int len;                           /* length of this token */
    UINT8 id;                          /* detector number */
    UINT8 detf;                        /* detector filter number */
    UINT8 iw;                          /* Iw parameter */
    UINT8 nht;                         /* Nht parameter */
    UINT32 fhi;                        /* Filhi parameter */
    UINT32 flo;                        /* Fillo parameter */
    UINT16 wa;                         /* Wa parameter */
    UINT16 spare;                      /* spare */
    UINT16 tc;                         /* Tc parameter */
    UINT8 x1;                          /* X1 parameter */
    UINT8 x2;                          /* X2 parameter */
    UINT8 x3;                          /* X3 parameter */
    UINT8 xx;                          /* Xx parameter */
    UINT8 av;                          /* Av parameter */
    char name[QDP_PASCALSTRING_LEN+1]; /* detector name */
} QDP_TOKEN_MHD;

/* type 133 - Threshold Detector Specification */

typedef struct {
    int len;                           /* length of this token */
    UINT8 id;
    UINT8 detf;
    UINT8 iw;
    UINT8 nht;
    UINT32 fhi;
    UINT32 flo;
    UINT16 wa;
    UINT16 spare;
    char name[QDP_PASCALSTRING_LEN+1]; /* detector name */
} QDP_TOKEN_TDS;

/* Everything */

typedef struct {
    QDP_TOKEN_VERSION ver;
    QDP_TOKEN_SITE site;
    QDP_TOKEN_DSS dss;
    QDP_TOKEN_CLOCK clock;
    QDP_TOKEN_LOGID logid;
    QDP_TOKEN_CNFID cnfid;
    QDP_TOKEN_SERVICES srvr;
    LNKLST lcq; /* list of QDP_TOKEN_LCQ */
    LNKLST iir; /* list of QDP_TOKEN_IIR */
    LNKLST fir; /* list of QDP_TOKEN_FIR */
    LNKLST cds; /* list of QDP_TOKEN_CDS */
    LNKLST mhd; /* list of QDP_TOKEN_MHD */
    LNKLST tds; /* list of QDP_TOKEN_TDS */
    int count;
#define QDP_DP_TOKEN_SIG_LEN 15
    char signature[QDP_DP_TOKEN_SIG_LEN+1]; /* typing monkeys validity flag */
} QDP_DP_TOKEN;

#ifdef __cplusplus
}
#endif

#endif /* qdp_tokens_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: tokens.h,v $
 * Revision 1.3  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
