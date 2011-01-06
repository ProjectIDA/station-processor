#pragma ident "$Id: sanio.h,v 1.18 2006/11/10 05:46:45 dechavez Exp $"
/*======================================================================
 *
 *  SANIO library include file.
 *
 *  Constants with names of the form SAN_xxx cannot be changed.  They
 *  reflect definitions implemented in the SAN.  Constants with names
 *  of the form SANIO_xxx may be changed.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef sanio_h_included
#define sanio_h_included

#include "platform.h"

/* Library defaults */

#define SANIO_DEFAULT_TO      10
#define SANIO_BUFLEN          8192
#define SANIO_PREAMBLE_LEN    4
#define SANIO_MAX_TIMESTR_LEN 31

/* SAN Constants - DO NOT MODIFY! */

#define SAN_EPOCH_YEAR 1999
#define SAN_EPOCH_TO_1970_EPOCH 915148800

#define SAN_OBSERVER 0
#define SAN_OPERATOR 1

#define SAN_PEER_NAMLEN   31
#define SAN_STREAM_NAMLEN 7
#define SAN_MAX_STREAMS   64
#define SAN_MAX_CASCADES  16
#define SAN_MAX_CONN      64
#define SAN_MAX_IPADDRESS_LEN 15
#define SAN_MAX_FILTER_COEFF 251
#define SAN_MAX_FILTERS   5

#define SAN_CAL_OFF     0
#define SAN_CAL_PENDING 1
#define SAN_CAL_ON      2

#define ttagStimeUndefined(ptr)    \
(                                  \
    ((ptr)->sec  == 0xffffffff) && \
    ((ptr)->msc  ==     0xffff) && \
    ((ptr)->sign ==      0xff)     \
)

/* Clock status bits */

#define SANIO_TT_STAT_EXTPPS_AVAIL    0x01 /* external PPS present */
#define SANIO_TT_STAT_PLL_ENABLE      0x02 /* PLL is enabled (1) or not */
#define SANIO_TT_STAT_EXTTIME_AVAIL   0x04 /* external time available (1) */
#define SANIO_TT_STAT_EXTTIME_QUALITY 0x08 /* 0 = < 4 SV's (or unlocked) */
#define SANIO_TT_STAT_EXTTIME_NOINIT  0x10 /* set if good time string has been seen */
#define SANIO_TT_STAT_RTT_FAILURE     0x20 /* RTT counter failure if set */
#define SANIO_TT_STAT_SUSPICIOUS_GPS 0x100 /* GPS time may be bogus */
#define SANIO_TT_STAT_TYPEMASK        0xc0
#define SANIO_TT_STAT_EXTTIME_TYPE(s) (((s) & SANIO_TT_STAT_TYPEMASK) >> 6)
#define SANIO_TT_STAT_SETTYPE(t)     (((t) << 6) & SANIO_TT_STAT_TYPEMASK)
#define SANIO_TT_STAT_RCVR_SPECIFIC(s) ((s) << 9)

#define SANIO_TT_TYPE_NMEA 0
#define SANIO_TT_TYPE_TSIP 1

/* Cause codes.  These must be unique from the message types
 * defined in the next section.  That way, we can have ccRecv
 * (or the equivalent) return either the message type or a
 * cause code (in case of failure).  These cause codes are also
 * used in certain messages (eg, acks and disconnects).
 */

#define SAN_OK           0 /* success, or normal            */
#define SAN_ERROR        1 /* failure, or general fault     */
#define SAN_TIMEDOUT     2 /* timeout                       */
#define SAN_SIGNAL       3 /* interrupted by signal         */
#define SAN_NOPERM       4 /* no permission (not operator)  */
#define SAN_EINVAL       5 /* invalid argument              */
#define SAN_NOSUCH       6 /* unsupported feature           */
#define SAN_NOMEM        7 /* insufficient memory           */
#define SAN_BUSY         8 /* busy (or resource locked)     */
#define SAN_QUIT         9 /* normal client disconnect      */
#define SAN_CONNRESET   10 /* abrupt client disconnect      */

/************* SAN COMMAND & CONTROL MESSAGE TYPES **************/

/* Dataless message type codes (the type is the message) */

#define SAN_NOP        100 /* do nothing (heartbeat)             */
#define SAN_DEFCONFIG  101 /* load (and activate) default config */
#define SAN_ABORTCAL   102 /* calibration abort request          */
#define SAN_STOPACQ    103 /* stop data acquisition              */
#define SAN_STARTACQ   104 /* start data acquisition             */
#define SAN_SNDCONFIG  105 /* request active configuration       */
#define SAN_SNDSOH     106 /* request SOH snapshot               */
#define SAN_LOGON      107 /* turn log message forwarding on     */
#define SAN_LOGOFF     108 /* turn log message forwarding off    */
#define SAN_SNDKEY     109 /* send active public key parameters  */
#define SAN_REKEYABORT 110 /* abort pending rekey operation      */
#define SAN_SNDSOH_A   111 /* request SOH subset A               */
#define SAN_SNDSOH_B   112 /* request SOH subset B               */
#define SAN_SNDSOH_C   113 /* request SOH subset C               */
#define SAN_SNDSOH_D   114 /* request SOH subset D               */
#define SAN_CLEARAB    115 /* clear latched security byte        */
#define SAN_SAVE       116 /* save current config in NV ram      */
#define SAN_CLEARSB    117 /* clear latched security byte        */
#define SAN_CLEARVB    118 /* clear latched security byte        */
#define SAN_REBOOT     198 /* system reboot                      */
#define SAN_HALT       199 /* system halt                        */

/* Messages with non-zero data fields */

#define SAN_VERSION        200 /* protocol version to use             */
#define SAN_PID            201 /* process id                          */
#define SAN_DISCONNECT     202 /* peer disconnect in progress         */
#define SAN_SETTO          203 /* set I/O timeout interval            */
#define SAN_STATE          204 /* operator/observer state             */
#define SAN_SOH            205 /* SOH snapshot to follow              */
#define SAN_CONFIG         206 /* configuration table                 */
#define SAN_CONFIGACK      207 /* config accept/deny reply            */
#define SAN_CALIB          208 /* calibration parameters              */
#define SAN_CALIBACK       209 /* calibration accept/deny reply       */
#define SAN_LOGMSG         210 /* log message                         */
#define SAN_FLASH          211 /* calibration parameters              */
#define SAN_FLASHACK       212 /* calibration accept/deny reply       */
#define SAN_RCM            213 /* mass recenter (2001/3 only)         */
#define SAN_KEYGEN         214 /* generate new key pair               */
#define SAN_KEYGENACK      215 /* key generation ack                  */
#define SAN_REKEY          216 /* implement new key pair              */
#define SAN_REKEYACK       217 /* re-key ack                          */
#define SAN_PUBLICKEY      218 /* active public key parameters        */
#define SAN_NEWKEY         219 /* new public key announcement         */
#define SAN_SIGN_Y         221 /* sign public key Y request and data  */
#define SAN_SIGNED_Y       222 /* encapsulated Y, signed by old & new */
#define SAN_CHATREQ        223 /* chat request ("doorbell")           */
#define SAN_CALIBCALPER    224 /* calib/calper data                   */
#define SAN_CALIBCALPERACK 225 /* calib/calper data accept/deny reply */
#define SAN_SITENAME       226 /* site name data                      */
#define SAN_SITENAMEACK    227 /* site name data accept/deny replu    */
#define SAN_SETIDLETO      228 /* set idle timeout interval           */
#define SAN_OPERATOR_PW    229 /* operator request (with password)    */
#define SAN_PASSWORD       230 /* new operator password               */
#define SAN_PASSWORDACK    231 /* new operator password accept/deny   */

#define SAN_IDENT      299 /* localhost ident (SAN only)          */

/* Subfields in the configuration table */

#define SAN_CONFIG_DONE         0
#define SAN_CONFIG_ADDRESS      1
#define SAN_CONFIG_NETMASK      2
#define SAN_CONFIG_GATEWAY      3
#define SAN_CONFIG_BOOTFLAGS    4
#define SAN_CONFIG_TTMD         5
#define SAN_CONFIG_BACKOFF      6
#define SAN_CONFIG_UNUSED_TAG_7 7
#define SAN_CONFIG_SITENAME     8
#define SAN_CONFIG_SITECOORDS   9
#define SAN_CONFIG_DBUFLEN      10
#define SAN_CONFIG_NICETIMES    11
#define SAN_CONFIG_ADSINT       12
#define SAN_CONFIG_AUXDEV       13
#define SAN_CONFIG_FILTERS      14
#define SAN_CONFIG_STREAMS      15
#define SAN_CONFIG_DETECTOR     16
#define SAN_CONFIG_SERVICES     17
#define SAN_CONFIG_LIMITS       18
#define SAN_CONFIG_CLOCKTYPE    19
#define SAN_CONFIG_AUXSINT      20
#define SAN_CONFIG_DAFLAGS      21
#define SAN_CONFIG_AUTH         22
#define SAN_CONFIG_CD11CSFF1SB  23
#define SAN_CONFIG_STREAMSPLUS  24
#define SAN_CONFIG_ASNPARAMS    25
#define SAN_CONFIG_ACS          26
#define SAN_CONFIG_AB_MAP       27
#define SAN_CONFIG_VB_MAP       28
#define SAN_CONFIG_PLLNMEAN     29

/* Subfields in the state of health report (MUST be non-zero) */

#define SAN_SOH_GPS         1
#define SAN_SOH_OFFSET      2
#define SAN_SOH_BOOTTIME    3
#define SAN_SOH_BOXID       4
#define SAN_SOH_ACQFLAG     5
#define SAN_SOH_DI          6
#define SAN_SOH_BOXTEMP     7
#define SAN_SOH_CAL         8
#define SAN_SOH_STREAMS     9
#define SAN_SOH_DCONN      10
#define SAN_SOH_CCONN      11
#define SAN_SOH_AUTH       12
#define SAN_SOH_CS1        13
#define SAN_SOH_TRGFLAG    14
#define SAN_SOH_COUNT      15
#define SAN_SOH_FLAGS      16
#define SAN_SOH_LATCHED_AB 17
#define SAN_SOH_CURRENT_AB 18
#define SAN_SOH_AUX        19

/* Flash codes */

#define SAN_FLASH_SAN        0
#define SAN_FLASH_SAN_BACKUP 1
#define SAN_FLASH_BOOT       2
#define SAN_FLASH_DSP        3

/* Handle for C&C socket I/O */

typedef struct san_handle {
    int sd;
    MUTEX mutex;
    char peer[MAXPATHLEN+1];
    int  port;
    INT16 to;
    BOOL retry;
    UINT16 state;
    UINT8 rcvbuf[SANIO_BUFLEN];
} SAN_HANDLE;

/* Time tag on the comm side of the system */

typedef struct san_stime {
    UINT32 sec;
    UINT16 msc;
    INT16  sign;
} SAN_STIME;

/* Optional user supplied GSE 2.0 per stream parameters */

typedef struct {
    REAL32 calib;
    REAL32 calper;
} SAN_GSEDB;

/* A derived data stream */

typedef struct san_dsp {
    INT32 sid;
    INT32 chan;
    INT32 flags;
    INT32 nmean;
    INT32 cascade[SAN_MAX_CASCADES];
} SAN_DSP;

typedef struct {
    char    name[SAN_STREAM_NAMLEN+1]; /* user supplied identifer      */
    SAN_DSP dsp;                       /* DSP parameters               */
    int     npak;                      /* number of packets to buffer  */
    int     hide;                      /* number of packets to "hide"  */
    SAN_GSEDB gse;                     /* optional GSE parameters      */
} SAN_DASTREAM;

/* A SAN filter */

typedef struct san_filter {
    UINT32    fid;
    UINT32  ntaps;
    UINT32  decim;
    REAL32  coeff[SAN_MAX_FILTER_COEFF];
} SAN_FILTER;

/* The event detector */

typedef struct san_detector {
    INT32  stalen;    /* STA length (samples)                */
    INT32  ltalen;    /* LTA length (samples)                */
    REAL32 thresh;    /* trigger threshold (STA/LTA)         */
    REAL32 ethresh;   /* end trigger threshold (STA/LTA)     */
    UINT32 vthresh;   /* minimum # of votes to cause trigger */
    UINT32 chans;     /* detection channels, bitmapped       */
    INT32  pretrig;   /* pre-event memory (samples)          */
    INT32  posttrig;  /* post-event memory (samples)         */
} SAN_DETECTOR;

/* SAN configuration table */

typedef struct san_config {

    INT32        nfilter;
    SAN_FILTER   filter[SAN_MAX_FILTERS];
    INT32        nstream;
    SAN_DASTREAM stream[SAN_MAX_STREAMS];
    INT32        auxdev;
    SAN_DETECTOR detector;

} SANIO_CONFIG;

/* State of Health */

typedef struct {
    UINT32 value; /* counter value */
    UINT32 roll;  /* number of times value has rolled over */
} SAN_COUNTER;

typedef struct {
    SAN_STIME   tstamp;   /* system time last sample was produced        */
    REAL32      rms;      /* RMS amplitude                               */
    REAL32      ave;      /* average amplitude                           */
    SAN_COUNTER count;    /* sample counts                               */
    SAN_STIME   oldest;   /* system time of oldest datum in packet pool  */
    SAN_STIME   yngest;   /* system time of yngest datum in packet pool  */
    SAN_COUNTER dropped;  /* number of dropped samples                   */
    SAN_STIME   lastdrop; /* system time of most recently dropped sample */
    SAN_COUNTER gaps;     /* number of gaps in packet pool               */
} SAN_STREAMSOH;

typedef struct {
    char peer[SAN_PEER_NAMLEN+1];
    SAN_STIME connect;     /* system time connection was established */
    SAN_STIME tstamp;      /* system time last command received      */
    INT8  isOperator;      /* true if operator                       */
    INT32 tid;             /* task ID of server for this conn        */
} SAN_CCSOH;

typedef struct {
    INT8 peer[SAN_PEER_NAMLEN+1];
    INT32 port;
    SAN_STIME connect;   /* system time connection was established */
    struct {
        SAN_STIME in;    /* time of last data from peer */
        SAN_STIME out;   /* time of last data to   peer */
    } tstamp;
    struct {
        SAN_COUNTER in;  /* number of bytes from peer */
        SAN_COUNTER out; /* number of bytes to   peer */     
    } bytes;
} SAN_DCSOH;

typedef struct {
    REAL32 lat;   /* latitude  */
    REAL32 lon;   /* longitude */
    REAL32 elev;  /* elev      */
    REAL32 depth; /* depth     */
} SAN_COORDS;

typedef struct {
    SAN_STIME  tstamp;
    UINT16 status;
    UINT16 pll;
    UINT16 phase;
    SAN_COORDS coords;
} SAN_GPSSOH;

typedef struct {
    INT16 state;     /* off, on, pending              */
    SAN_STIME start; /* start time (if pending or on) */
    SAN_STIME end;   /* end time (if pending or off)  */
} SAN_CALSOH;

typedef struct {
    UINT32 trig;      /* number of triggers since boot */
    UINT32 authReset; /* number of times authentication token reset */
    UINT32 reboot;    /* number of reboots since counter cleared */
    UINT32 panic;     /* number of panics since counter cleared */
} SAN_COUNTSOH;

typedef struct {
    SAN_GPSSOH gps;                /* GPS soh (includes snapshot time)   */
    INT16 auth;                    /* authentication status              */
    SAN_STIME offset;              /* current time offset                */
    SAN_STIME boottime;            /* system time at boot                */
    UINT16 boxID;                  /* box id                             */
    INT8  acqOn;                   /* TRUE when acquisition is ON        */
    INT8  trgOn;                   /* TRUE when trigger is ON            */
    INT8  di;                      /* snapshot of digitial inputs        */
    UINT32 flags;                  /* various flags                      */
    INT16 boxtemp;                 /* on-board temperature, deg C x 10   */
    SAN_CALSOH cal;                /* calibration state                  */
    SAN_COUNTSOH count;            /* various counters                   */
    INT16 nstream;                 /* number of valid entries to follow  */
    SAN_STREAMSOH stream[SAN_MAX_STREAMS]; /* per-stream stats           */
    INT16 ncc;                     /* number of valid entries to follow  */
    SAN_CCSOH cc[SAN_MAX_CONN];    /* command & control connection stats */
    INT16 ndc;                     /* number of valid entries to follow  */
    SAN_DCSOH dc[SAN_MAX_CONN];    /* data connection stats              */
} SANIO_SOH;

/* Function Prototypes */

/* connect.c */
BOOL sanioConnect(SAN_HANDLE *san);
SAN_HANDLE *sanioInit(char *host, int port, int to, BOOL retry);

/* pack.c */
int  sanioPackConfig(UINT8 *start, SANIO_CONFIG *src);
void sanioUnpackConfig(UINT8 *start, SANIO_CONFIG *out);
void sanioUnpackSOH(UINT8 *start, SANIO_SOH *out);

/* recv.c */
UINT16 sanioRecv(SAN_HANDLE *san, UINT8 **data);

/* send.c */
BOOL sanioSend(SAN_HANDLE *san, int type, UINT8 *msg, int msglen);

/* string.c */
char *sanioMessageTypeToString(int message);

/* ttag.c */
VOID ttagStimeBuild(int year, int day, int hr, int mn, int sec, int ms, SAN_STIME *out);
VOID ttagStimeSplit(SAN_STIME *tag, int start, int *pyr, int *psec, int *pmsc);
char *ttagStimeString(SAN_STIME *stime, int code, char *buf);
int ttagStimeCompare(void *aptr, void *bptr);
BOOL ttagStimeAdd(SAN_STIME *a, SAN_STIME *b, SAN_STIME *c);
BOOL ttagStimeSub(SAN_STIME *a, SAN_STIME *b, SAN_STIME *c);

/* util.c */
int sanioGetSD(SAN_HANDLE *san);
BOOL sanioCheckHandle(SAN_HANDLE *san);
VOID sanioDisconnect(SAN_HANDLE *san, int code, UINT8 *buf);
BOOL sanioRequestState(SAN_HANDLE *san, int state, UINT8 *buf);
BOOL sanioReboot(SAN_HANDLE *san, BOOL force, UINT8 *buf);
BOOL sanioSendConfig(SAN_HANDLE *san, SANIO_CONFIG *config, BOOL force, UINT8 *buf);
BOOL sanioSendPid(SAN_HANDLE *san, UINT8 *buf);
BOOL sanioSendTimeout(SAN_HANDLE *san, int timeout, UINT8 *buf);

/* Macros */

#define sanioSendNop(san, buf)       sanioSend(san, SAN_NOP, buf, 0)
#define sanioRequestConfig(san, buf) sanioSend(san, SAN_SNDCONFIG, buf, 0)
#define sanioRequestSoh(san, buf)    sanioSend(san, SAN_SNDSOH, buf, 0)
#define sanioRequestSohA(san, buf)   sanioSend(san, SAN_SNDSOH_A, buf, 0)
#define sanioRequestSohB(san, buf)   sanioSend(san, SAN_SNDSOH_B, buf, 0)
#define sanioRequestSohC(san, buf)   sanioSend(san, SAN_SNDSOH_C, buf, 0)
#define sanioRequestSohD(san, buf)   sanioSend(san, SAN_SNDSOH_D, buf, 0)

#endif /* sanio_h_included */

/* Revision History
 *
 * $Log: sanio.h,v $
 * Revision 1.18  2006/11/10 05:46:45  dechavez
 * defined SANIO_TT_TYPE_NMEA and SANIO_TT_TYPE_TSIP constants
 *
 * Revision 1.17  2004/12/21 00:35:26  dechavez
 * define SAN_CONFIG_PLLNMEAN
 *
 * Revision 1.16  2004/06/24 18:09:00  dechavez
 * rename operator field to isOperator
 *
 * Revision 1.15  2002/09/03 20:11:05  dec
 * update constants to SAN 1.11.2
 *
 * Revision 1.14  2002/05/15 18:24:01  dec
 * fixed missing comment terminator errors
 *
 * Revision 1.13  2001/10/08 18:03:50  dec
 * updates to bring into sync with SAN rev 1.6.4 features
 *
 * Revision 1.12  2001/05/31 16:31:06  dec
 * added SAN_CONFIG_ASNPARAMS
 *
 * Revision 1.11  2001/05/20 16:03:02  dec
 * rework SAN_HANDLE to elminate send buffer, add various send macros
 *
 * Revision 1.10  2001/05/07 22:32:02  dec
 * replace stdtypes.h with platform.h
 *
 * Revision 1.9  2001/05/02 17:35:01  dec
 * SAN kernel 1.3.0 modifications and added sanioCmndToString()
 *
 * Revision 1.8  2001/02/23 22:49:41  dec
 * add count and flags to state of health
 *
 * Revision 1.7  2001/02/23 17:45:10  dec
 * added SAN_SOH_COUNT and SAN_SOH_FLAGS
 *
 * Revision 1.6  2000/11/08 01:55:58  dec
 * added filters to supported config
 *
 * Revision 1.5  2000/11/06 23:09:40  dec
 * added trigger state to SOH
 *
 * Revision 1.4  2000/11/02 20:19:44  dec
 * added timetag and other such stuff
 *
 * Revision 1.3  2000/09/20 00:57:22  dec
 * *** empty log message ***
 *
 * Revision 1.2  2000/08/03 21:13:59  dec
 * prototypes to reflect merged util.c and io.c
 *
 * Revision 1.1  2000/08/02 16:22:38  dec
 * initial release, based on SAN VxWorks sources
 *
 */
