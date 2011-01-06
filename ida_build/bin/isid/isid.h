#pragma ident "$Id: isid.h,v 1.25 2008/10/10 22:47:24 dechavez Exp $"
#ifndef isid_h_included
#define isid_h_included

#include "isi/dl.h"
#include "nrts/dl.h"
#include "iacp.h"
#include "msgq.h"
#include "util.h"
#include "logio.h"
#include "list.h"

extern char *VersionIdentString;
static char *Copyright = "Copyright (C) 2003-2008 - Regents of the University of California.";

/* various default parameters */

#ifndef HAVE_MYSQL
#define USE_FLATFILE_DB
#endif

#define DEFAULT_USER           "nrts"
#define DEFAULT_PORT           ISI_DEFAULT_PORT
#define DEFAULT_STATUS         0
#define DEFAULT_MAXCLIENT      128
#define DEFAULT_BUFLEN         32768
#define DEFAULT_RECVBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_SENDBUFLEN     DEFAULT_BUFLEN
#define DEFAULT_BACKGROUND_LOG "syslogd:local3"
#define DEFAULT_FOREGROUND_LOG "-"
#define DEFAULT_TIMEOUT        IACP_MINTIMEO
#ifdef USE_FLATFILE_DB
#define DEFAULT_DB             NRTS_DEFHOME
#else
#define DEFAULT_DB             "localhost:nrts:NULL:isi:0"
#endif
#define DEFAULT_HOME           NRTS_DEFHOME
#define DEFAULT_POLL_INTERVAL  1000
#ifdef WIN32
#define DEFAULT_PROCESS_PRIORITY NORMAL_PRIORITY_CLASS
#define DEFAULT_THREAD_PRIORITY  THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_PROCESS_PRIORITY 0 /* ignored if not windows */
#define DEFAULT_THREAD_PRIORITY  0 /* ignored if not windows */
#endif
//#define DEFAULT_NRTS_PORT      14002
#define DEFAULT_NRTS_PORT      0 /* disabled until actually implemented */

#define ISID_INI_NAME       "isid.ini"
#define ISID_ENV_INI        "ISID_INI"

#ifdef WIN32
#define ISID_WIN32_INI_NAME "ISID.INI"
#endif /* WIN32 */

/* Module Id's for generating meaningful exit codes */

#define ISID_MOD_MAIN     ((INT32)  100)
#define ISID_MOD_APP      ((INT32)  200)
#define ISID_MOD_CLIENT   ((INT32)  300)
#define ISID_MOD_CONVERT  ((INT32)  400)
#define ISID_MOD_DATA     ((INT32)  500)
#define ISID_MOD_INIT     ((INT32)  600)
#define ISID_MOD_LOG      ((INT32)  700)
#define ISID_MOD_LOADINI  ((INT32)  900)
#define ISID_MOD_NRTS     ((INT32)  000)
#define ISID_MOD_REPORT   ((INT32) 1000)
#define ISID_MOD_SIGNALS  ((INT32) 1100)
#define ISID_MOD_SEQNO    ((INT32) 1200)
#define ISID_MOD_STATUS   ((INT32) 1300)
#define ISID_MOD_EDES     ((INT32) 1400)

/* Status flags for servicing data requests */

#define ISID_DONE              0
#define ISID_WAITING_FOR_DATA  1
#define ISID_DATA_READY        2
#define ISID_CORRUPT_DISK_LOOP 3
#define ISID_ABORT_REQUEST     4

/* server specific versions of data request -
 * I tried to define the isi.h data request with the server specific
 * stuff conditionally included, thus avoiding having to copy into
 * server side format, but kept getting hung up with cross dependencies
 * between isi.h and nrts.h.
 */

typedef struct {
    char site[ISI_SITELEN+1];
    ISI_SEQNO beg;
    ISI_SEQNO end;
    int status; /* internal use only, never sent over the wire */
    ISI_DL *dl;      /* disk loop for this site */
    BOOL continuous; /* window or feed flag */
    UINT32 type;     /* IACP output time series type flag */
    UINT32 begndx;   /* dl index of first record to transfer */
    UINT32 endndx;   /* dl index of last  record to transfer */
    UINT32 nxtndx;   /* dl index of next  record to transfer */
    UINT64 count;    /* disk loop counter at time indices were determined */
} ISID_SEQNO_REQUEST;

typedef struct {
    ISI_STREAM_NAME name;
    REAL64 beg;
    REAL64 end;
    int status; /* internal use only, never sent over the wire */
    char ident[ISI_STREAM_NAME_LEN+1]; /* sta:chnloc */
    NRTS_DL *dl; /* disk loop which contains this stream */
    NRTS_STREAM nrts; /* NRTS dl specifics for this stream */
    BOOL continuous;
    long begndx; /* dl index of first record to transfer */
    long endndx; /* dl index of last  record to transfer */
    long nxtndx; /* dl index of next  record to transfer */
    long count;  /* dl counter at time indices were determined */
} ISID_TWIND_REQUEST;

typedef struct {
    UINT32 type;       /* request type (seqno, stream) */
    UINT32 policy;     /* reconnect policy */
    UINT32 format;     /* delivery format */
    UINT32 compress;   /* telemetry compression */
    UINT32 options;    /* request options */
    UINT32 nreq;       /* number of request entries to follow */
    LNKLST *slist;     /* optional list of desired streams, as ISI_STREAM_NAME (ignored by twind requests) */
    union {
        ISID_TWIND_REQUEST *twind;
        ISID_SEQNO_REQUEST *seqno;
    } req;
} ISID_DATA_REQUEST;

/* For managing client connections */

typedef struct {
    MUTEX  mutex;              /* for protection */
    UINT32 index;              /* index number for debugging use */
    IACP   *iacp;              /* client handle */
    char   *ident;             /* client id string */
    BOOL finished;             /* TRUE when all done with this client */
    UINT32 result;             /* what to tell the client when finished */
    BOOL brkrcvd;              /* TRUE when client has sent us a break */
    BOOL dataready;            /* TRUE when disk loop has data available to send */
    struct {
        UINT32 poll;           /* data ready poll interval, msec */
        UINT32 hbeat;          /* heartbeat interval, sec */
    } interval;
    ISI_INCOMING incoming;     /* for tracking incoming multi-part messages */
    ISI_DATA_REQUEST datreq;   /* incoming data request */
    ISI_DL_MASTER *master;     /* all the ISI and NRTS disk loops */
    struct {
        IACP_FRAME frame;
        UINT8 *buf;
        UINT32 buflen;
    } send, recv, temp;        /* output, input and compressed data */
    struct {
        IACP_PAYLOAD msg;         /* last command received from peer */
        ISI_DATA_REQUEST *datreq; /* last complete data request */
    } last;
    NRTS_PKT pkt;
    LNKLST history;
} CLIENT;

/* Run time parameters */

typedef struct param {
    BOOL daemon;           /* if TRUE, run in the background */
    UINT16 port;           /* TCP/IP connection port (cmnd and data) */
    UINT16 status;         /* status server TCP/IP port */
    UINT16 nrts;           /* NRTS port (cmnd and data) */
    UINT32 maxclient;      /* max number of simultaneous clients */
    int priority;          /* process priority */
    ISI_GLOB glob;         /* global ISI parameters */
    char *log;             /* log specifier */
    char *user;            /* run as user */
    LOGIO lp;              /* log handle */
    BOOL debug;            /* debug flag */
    IACP_ATTR  attr;       /* connection attributes */
    ISI_DL_MASTER *master; /* all the ISI and NRTS disk loops */
    struct {
        UINT32 send;
        UINT32 recv;
    } buflen;               /* maximum send/receive buffer lengths */
    BOOL logtt;             /* TRUE for debugging disk loop race condition */
} PARAM;

/* For passing command line to MainThread */

typedef struct thread_params {
    int argc;
    char **argv;
    char *myname;
} MainThreadParams;

/* Function prototypes */

/* client.c */
VOID CloseClientConnection(CLIENT *client);
CLIENT *NextAvailableClient(IACP *iacp);
int PrintActiveClientReport(FILE *fp, long tstamp);
VOID InitClientList(PARAM *par);

/* convert.c */
BOOL ConvertData(CLIENT *client, ISID_DATA_REQUEST *req, NRTS_PKT *pkt);

/* copy.c */
BOOL CopyIntoServerSideFormat(ISID_DATA_REQUEST *dest, ISI_DATA_REQUEST *src);
VOID InitDataRequest(ISID_DATA_REQUEST *req);
VOID ClearDataRequest(ISID_DATA_REQUEST *req);

/* die.c */
VOID BlockOnShutdown(VOID);
BOOL ShutdownInProgress(VOID);
VOID GracefulExit(INT32 status);
VOID InitGracefulExit(VOID);

/* edes.c */
VOID StartNrtsServer(PARAM *par);

/* hbeat.c */
VOID MaintainHeartbeat(CLIENT *client);
VOID PauseForNewData(CLIENT *client);

/* init.c */
IACP *Init(char *myname, PARAM *par);

/* isi.c */
ISI_DL *LocateDiskLoopBySite(ISI_DL_MASTER *master, char *target);

/* log.c */
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
BOOL InitLogging(char *myname, PARAM *par);

/* main.c */
VOID DisableNewConnections(VOID);

/* nrts.c */
VOID ProcessNrtsSohReq(CLIENT *client);
VOID ProcessNrtsCnfReq(CLIENT *client);
VOID ProcessNrtsWfdiscReq(CLIENT *client);

/* par.c */
PARAM *LoadPar(char *prog, int argc, char **argv);
VOID LogPar(PARAM *par);

/* report.c */
VOID InitReportGenerator(PARAM *par);
VOID ClientReport(FILE *fp, CLIENT *client, time_t now);
VOID PrintStatusReport(FILE *fp);

/* respond.c */
VOID RespondToClientMessage(CLIENT *client);

/* seqno.c */
VOID ProcessSeqnoRequest(CLIENT *client);

/* service.c */
VOID ServiceConnection(IACP *iacp);

/* signals.c */
VOID StartSignalHandler(VOID);

/* status.c */
VOID StartStatusServer(PARAM *par);

/* twind.c */
VOID ProcessTwindRequest(CLIENT *client);

#endif /* isid_h_included */

/* Revision History
 *
 * $Log: isid.h,v $
 * Revision 1.25  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.24  2008/02/07 20:06:51  dechavez
 * updated prototypes
 *
 * Revision 1.23  2008/01/25 22:10:52  dechavez
 * moved options from CLIENT to ISID_DATA_REQUEST and added slist
 *
 * Revision 1.22  2008/01/17 19:34:50  dechavez
 * removed unused option constants
 *
 * Revision 1.21  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.20  2007/06/14 21:57:31  dechavez
 * Changed history list to instance instead of pointer
 *
 * Revision 1.19  2007/05/17 22:22:31  dechavez
 * 3.1.0 (beta 1)
 *
 * Revision 1.18  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.17  2006/09/29 19:58:33  dechavez
 * added NeedHeartbeat flag to CLIENT
 *
 * Revision 1.16  2006/02/09 20:22:22  dechavez
 * added pollint client parameter
 *
 * Revision 1.15  2005/10/17 21:11:40  dechavez
 * set default status port to 0 (ie, disabled by default)
 *
 * Revision 1.14  2005/09/10 21:51:25  dechavez
 * replaced DEFAULT_LOG with DEFAULT_BACKGROUND_LOG and DEFAULT_FOREGROUND_LOG
 *
 * Revision 1.13  2005/08/26 20:20:57  dechavez
 * Changed default log spec to "syslogd:local3"
 *
 * Revision 1.12  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.11  2005/07/06 15:52:27  dechavez
 * addes support for new "user" parameter
 *
 * Revision 1.10  2005/06/30 01:43:05  dechavez
 * first working seqno (ISI_DL) version, no compression
 *
 * Revision 1.9  2005/06/24 21:54:07  dechavez
 * checkpoint, additional seqno support in place, but not yet ready
 *
 * Revision 1.8  2005/01/28 02:03:18  dechavez
 * updated copyright
 *
 * Revision 1.7  2004/06/21 19:56:49  dechavez
 * Set default database server to localhost (for MYSQL builds)
 *
 * Revision 1.6  2004/06/10 20:43:01  dechavez
 * added home directory to parameter structure
 * removed SearchDiskloop prototype (moved function to NRTS library)
 *
 * Revision 1.5  2004/04/26 21:18:36  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.4  2003/12/10 06:18:37  dechavez
 * updated prototypes
 *
 * Revision 1.3  2003/11/26 19:35:29  dechavez
 * updated prototypes
 *
 * Revision 1.2  2003/11/04 00:24:36  dechavez
 * replaced INCOMING structure with ISI_INCOMING, added cleint index to handle
 *
 * Revision 1.1  2003/10/16 18:07:23  dechavez
 * initial release
 *
 */
