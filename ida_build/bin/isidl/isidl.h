#pragma ident "$Id: isidl.h,v 1.28 2009/05/14 16:40:54 dechavez Exp $"
#ifndef isidl_h_included
#define isidl_h_included

#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"
#include "ida.h"
#include "logio.h"
#include "ttyio.h"
#include "paro.h"
#include "msgq.h"
#ifdef INCLUDE_Q330
#include "q330.h"
#include "qdp.h"
#include "qdplus.h"
#endif /* INCLUDE_Q330 */

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2005,2006 - Regents of the University of California.";

/* Module Id's for generating meaningful exit codes */

#define ISIDL_MOD_MAIN      ((INT32)  100)
#define ISIDL_MOD_INIT      ((INT32)  200)
#define ISIDL_MOD_LOG       ((INT32)  300)
#define ISIDL_MOD_SIGNALS   ((INT32)  400)
#define ISIDL_MOD_ARS       ((INT32)  500)
#define ISIDL_MOD_ISI       ((INT32)  600)
#define ISIDL_MOD_BARO      ((INT32)  700)
#define ISIDL_MOD_CLOCK     ((INT32)  800)
#define ISIDL_MOD_IDA9      ((INT32)  900)
#define ISIDL_MOD_IDA10     ((INT32) 1000)
#define ISIDL_MOD_MSEED     ((INT32) 1100)
#define ISIDL_MOD_Q330      ((INT32) 1200)
#define ISIDL_MOD_QDP       ((INT32) 1300)
#define ISIDL_MOD_QDPLUS    ((INT32) 1400)
#define ISIDL_MOD_LOCAL     ((INT32) 1500)
#define ISIDL_MOD_IDA5      ((INT32) 1600)
#define ISIDL_MOD_META      ((INT32) 1700)
#define ISIDL_MOD_LISS      ((INT32) 1800)
#define ISIDL_MOD_INTERCEPT ((INT32) 1900)

/* Data source enums */

#define SOURCE_UNDEFINED  0
#define SOURCE_ISI        1
#define SOURCE_ARS        2
#define SOURCE_Q330       3
#define SOURCE_LISS       4

/* time tag for 1 Hz aux data streams */

typedef struct {
    UINT32 sys;
    UINT32 pps;
    UINT32 ext;
    char   code;
} ISIDL_TIMESTAMP;

/* For handling locally acquired packets */

typedef struct {
    MUTEX mutex;
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
    BOOL rt593;
} LOCALPKT;

/* ARS support */

typedef struct {
    TTYIO *tp;
    char *port;
    int baud;
    int parity;
    int flow;
    int sbits;
    int pktrev;
    LOCALPKT local;
} ARS;

/* Paroscientific barometer support */

typedef struct {
    MUTEX mutex;
    INT32 sample;
    PARO_PARAM param;
    PARO *pp;
    int nsamp;
    int maxsamp;
    LOGIO *lp;
    char *port;
    int baud;
    struct {
        int type;
        struct {
            int filt;
            int chan;
            int strm;
        } ida9;
        struct {
            char *chname;
        } ida10;
        struct {
            char *chn;
            char *loc;
        } mseed;
        struct {
            char *chn;
            char *loc;
            UINT16 seqno;
            UINT64 serialno;
        } qdplus;
    } format;
    INT32 *out;
    BOOL ValidSensorSeen;
    BOOL ValidSampleSeen;
    LOCALPKT local;
} BAROMETER;

#define MISSED_BAROMETER_SAMPLE (INT32) 0xFFFFFFFF

/* Quanterra Q330 support */

#ifdef INCLUDE_Q330

typedef struct {
    Q330_CFG *cfg;
    QDP_PAR par;
    LOGIO *lp;
    QDP *qp;
    BOOL first;
    LOCALPKT local;
} Q330;

#endif /* INCLUDE_Q330 */

typedef struct {
#ifdef INCLUDE_Q330
    QDP_LCQ lcq;
#endif /* INCLUDE_Q330 */
    int dummy;
} INTERCEPT_BUFFER;

/* Run time parameters */

typedef struct {
    int  source;        /* source identifier */
    char *input;        /* input descriptor string */
    int timeout;        /* I/O timeout, seconds */
    LOGIO *lp;          /* logging handle */
    char *site;         /* site string */
    char *begstr;       /* optional beg= string */
    char *endstr;       /* optional end= string */
    int nsite;          /* number of disk loop handles to follow */
    ISI_DL **dl;        /* disk loop handle(s) */
    BOOL *first;        /* to track incoming packets */
    NRTS_TOLERANCE tol; /* time tag tolerances */
    char *dbgpath;      /* IACP debug dump path */
    ARS  ars;           /* up to one ARS */
    LNKLST baro;        /* zero or more barometers */
#ifdef INCLUDE_Q330
    char *cfgpath;      /* Q330 configuration file */
    LNKLST q330;        /* zero or more Q330's */
#endif /* INCLUDE_Q330 */
    int sndbuf;         /* ISI/IACP socket send buffer length */
    int rcvbuf;         /* ISI/IACP socket recv buffer length */
    UINT16 flags;       /* option flags */
#define ISIDL_FLAG_PATCH_RT593     0x0001
#define ISIDL_FLAG_INTERCEPT       0x0002
#define ISIDL_FLAG_LEAP_YEAR_CHECK 0x0004
    INTERCEPT_BUFFER ib; /* used when ISIDL_FLAG_INTERCEPT bit set */
    struct {
#define SCL_UNSPECIFIED 0
#define SCL_FROM_NRTS   1
#define SCL_FROM_DB     2
#define SCL_FROM_FILE   3
        int source;      /* source type */
        char *spec;      /* specifier for loading ISI stream control list */
        LNKLST *list;    /* stream control list */
    } scl;
} ISIDL_PAR;

/* default parameters */

#define DEFAULT_USER           "nrts"
#define DEFAULT_DAEMON         FALSE
#define DEFAULT_SOURCE         SOURCE_UNDEFINED
#define DEFAULT_HOME           "/usr/nrts"
#define DEFAULT_TIMEOUT        30
#define DEFAULT_BACKGROUND_LOG "syslogd:user"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_DEBUG          FALSE
#define DEFAULT_DBGPATH        NULL
#define DEFAULT_QDEPTH         10
#ifndef HAVE_MYSQL
#define DEFAULT_DB             "/usr/nrts"
#else
#define DEFAULT_DB             "localhost:nrts:NULL:ida:0"
#endif
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif

#define DEFAULT_BAUD   19200
#define DEFAULT_PARITY TTYIO_PARITY_NONE
#define DEFAULT_FLOW   TTYIO_FLOW_SOFT
#define DEFAULT_SBITS  2

#define DEFAULT_SNDBUF 65536
#define DEFAULT_RCVBUF 65536

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/*  Function prototypes  */

/* ars.c */
BOOL ParseARSArgs(ARS *ars, char *argstr);
void StartArsReader(ISIDL_PAR *par);

/* baro.c */
void ToggleBarometerDebugState(void);
BOOL BarometerDebugEnabled(void);
INT32 GetSample(BAROMETER *bp);
BOOL AddBarometer(ISIDL_PAR *par, char *string);
void StartBarometerReader(BAROMETER *bp);

/* bground.c */
BOOL BackGround(ISIDL_PAR *par);

/* clock.c */
void StartTimingSubsystem(char *GPSstring);
void GetTimeStamp(ISIDL_TIMESTAMP *dest);

/* exit.c */
void BlockShutdown(char *fid);
void UnblockShutdown(char *fid);
void Exit(INT32 status);
VOID InitExit(ISIDL_PAR *ptr);

/* ida5.c */
void CompleteIda5Header(ISI_RAW_PACKET *raw, ISI_DL *dl);

/* ida9.c */
void CompleteIda9Header(ISI_RAW_PACKET *raw, ISI_DL *dl, BOOL rt593);
THREAD_FUNC Ida9BarometerThread(void *argptr);

/* ida10.c */
void CompleteIda10Header(ISI_RAW_PACKET *raw, ISI_DL *dl);
THREAD_FUNC Ida10BarometerThread(void *argptr);

/* init.c */
ISIDL_PAR *init(char *myname, int argc, char **argv);

/* intercept.c */
void InterceptProcessor(ISI_RAW_PACKET *raw, ISI_DL *dl, INTERCEPT_BUFFER *ib);
BOOL InitInterceptBuffer(INTERCEPT_BUFFER *ib);

/* isi.c */
void ToggleISIDebugState(void);
void ReadFromISI(ISIDL_PAR *par);

/* liss.c */
BOOL ParseLissArgs(char *string);
void StartLissReader(ISIDL_PAR *par);

/* local.c */
void ProcessLocalData(LOCALPKT *local);
BOOL InitLocalProcessor(ISIDL_PAR *par);

/* log.c */
LOGIO *GetLogHandle(void);
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* meta.c */
#ifdef INCLUDE_Q330
void ProcessMeta(Q330 *q330, QDP_META *meta);
BOOL InitMetaProcessor(ISIDL_PAR *par);
#endif /* INCLUDE_Q330 */

/* mseed.c */
void CompleteMseedHeader(UINT8 *start, ISI_DL *dl);
THREAD_FUNC MseedBarometerThread(void *argptr);

#ifdef INCLUDE_Q330

/* q330.c */
char *AddQ330(ISIDL_PAR *par, char *argstr, char *cfgpath);
void StartQ330Reader(Q330 *q330);

/* qdplus.c */
void CompleteQdplusHeader(ISI_RAW_PACKET *raw, ISI_DL *dl);
void InsertQdplusSerialno(ISI_RAW_PACKET *raw, UINT64 serialno);
void CopyQDPToQDPlus(ISI_RAW_PACKET *raw, QDP_PKT *pkt);
THREAD_FUNC QdplusBarometerThread(void *argptr);

#endif /* INCLUDE_Q330 */

/* rt593.c */
void RT593PatchTest(ISI_RAW_PACKET *raw, ISI_DL *dl);

/* signals.c */
VOID StartSignalHandler(VOID);

#endif /* isidl_h_included */

/* Revision History
 *
 * $Log: isidl.h,v $
 * Revision 1.28  2009/05/14 16:40:54  dechavez
 * Changed missed barometer sample from -2 to 0xFFFFFFFF
 *
 * Revision 1.27  2008/03/05 23:24:18  dechavez
 * added support intercept processor and ARS leap year bug check
 *
 * Revision 1.26  2008/01/25 22:02:12  dechavez
 * added scl field to ISIDL_PAR
 *
 * Revision 1.25  2007/09/07 20:06:20  dechavez
 * added q330.h Q330_CFG
 *
 * Revision 1.24  2007/05/03 20:27:34  dechavez
 * added LISS support
 *
 * Revision 1.23  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.22  2007/02/08 22:54:07  dechavez
 * define LOCALPKT handle
 *
 * Revision 1.21  2006/12/12 23:27:28  dechavez
 * simplified metadata stuff
 *
 * Revision 1.20  2006/09/29 19:48:57  dechavez
 * add socket send/receive buffers to ISIDL_PAR, with 64kbyte default
 *
 * Revision 1.19  2006/07/10 21:10:06  dechavez
 * added ida5 prototypes
 *
 * Revision 1.18  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.17  2006/06/19 19:14:36  dechavez
 * conditional Q330 support, define ARS handle for rev aware ARS input
 *
 * Revision 1.16  2006/06/07 22:40:50  dechavez
 * removed nsamp from qdplus barometer format handle
 *
 * Revision 1.15  2006/06/02 21:01:59  dechavez
 * added Q330/QDPLUS support
 *
 * Revision 1.14  2006/04/20 23:02:26  dechavez
 * updated prototypes
 *
 * Revision 1.13  2006/04/03 21:38:23  dechavez
 * added ValidSensorSeen to barometer handle
 *
 * Revision 1.12  2006/03/30 22:05:07  dechavez
 * barometer and station message queue support
 *
 * Revision 1.11  2006/03/14 20:31:48  dechavez
 * pass fid to block/unlock exit mutex calls, for debugging use
 *
 * Revision 1.10  2006/03/13 23:07:37  dechavez
 * updated prototypes
 *
 * Revision 1.9  2005/09/30 22:30:31  dechavez
 * updated prototypes
 *
 * Revision 1.8  2005/09/13 00:37:40  dechavez
 * added dbgpath support
 *
 * Revision 1.7  2005/09/10 22:06:34  dechavez
 * replaced DEFAULT_LOG with DEFAULT_BACKGROUND_LOG and DEFAULT_FOREGROUND_LOG
 *
 * Revision 1.6  2005/08/26 20:08:16  dechavez
 * Changed default log spec to syslogd:user
 *
 * Revision 1.5  2005/07/26 18:37:15  dechavez
 * prefix log messages with site or source tag
 *
 * Revision 1.4  2005/07/26 00:49:05  dechavez
 * initial release
 *
 * Revision 1.3  2005/07/06 15:49:36  dechavez
 * checkpoint, apparently with working isi input
 *
 * Revision 1.2  2005/06/24 21:50:36  dechavez
 * checkpoint 0.0.2, with apparently working tty input
 *
 * Revision 1.1  2005/06/10 16:04:06  dechavez
 * 0.0.1 checkpoint
 *
 */
