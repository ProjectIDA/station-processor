#pragma ident "$Id: isp.h,v 1.21 2008/10/09 20:38:38 dechavez Exp $"
/*
 * Copyright (c) 1997, 1998, 1999 Regents of the University of California.
 * All rights reserved.
 */

#ifndef isp_h_defined
#define isp_h_defined

extern char *isp_release;

#include "platform.h"
#include "oldttyio.h"
#include "util.h" /* for util_log... defines */
#include "sanio.h"
#include "ida.h"
#include "isi/dl.h"
#include "isi/db.h"
#include "ida10.h"
#include "nrts.h"

#define ISP_NSTREAM 25
#define ISP_DEFAULT_PORT 10000
#define ISP_DEVNAMLEN 15
#define ISP_MAXDEVNAM (ISP_DEVNAMLEN + 1)
#define ISP_SNAMLEN   NRTS_SNAMLEN
#define ISP_MAXSYSNAM (ISP_SNAMLEN + 1)
#define ISP_ADDRNAMLEN 15
#define ISP_MAXADDRLEN (ISP_ADDRNAMLEN + 1)

#define ISP_PAROPARAMLEN 15
#define ISP_MAXPAROPARAMLEN (ISP_PAROPARAMLEN + 1)

#define ISP_SUBDIR "isp"

/* File names (relative to NRTS_HOME/SYSTEM/ISP_SUBDIR) */

#define ISP_RUN_FILE    "run"     /* run-time setup                   */
#define ISP_BUF_DIR     "buf/"    /* mmap'd massio buffer             */
#define ISP_TEE_DIR     "tee"     /* directory raw data packets tee   */
#define ISP_ISO_DIR     "iso"     /* directory iso images             */
#define ISP_CNF_FILE    "cnf"     /* DAS configuration                */
#define ISP_TMPCNF_FILE "cnf.tmp" /* temp file during reconfiguration */
#define ISP_DEFCNF_FILE "cnf.def" /* default DAS configuration        */
#define ISP_BAKCNF_FILE "cnf.bak" /* backup DAS configuration         */

/* Limits */

#define ISP_MIN_BAUD    1200 /* smallest supported baud rate          */
#define ISP_MAX_BAUD   38400 /* largest supported baud rate           */
#define ISP_MIN_STATINT   30 /* min DAS status request interval       */
#define ISP_MAX_CMDLEN    64 /* max length of a DAS command message   */
#define ISP_MAXCLIENTS    10 /* max number of simultaneous isp clients*/
#define ISP_MIN_SAN_TIMEOUT 30
#define ISP_MIN_SAN_SOHINT   5
#define ISP_MIN_NRTS_FLUSH_INTERVAL 30

/* Defaults */

#define ISP_DEFAULT_COMMAND_PORT 10000

/* Environment variable for default system */

#define ISP_SYSTEM "ISP_SYSTEM"

/* Packet injection service */

#ifndef ISP_INJECT
#define ISP_INJECT "isp_inject"
#endif

#ifndef ISP_INJECT_PORT
#define ISP_INJECT_PORT 9998
#endif

/* Boot mgr service */

#ifndef ISP_BOOTMGR
#define ISP_BOOTMGR "isp_bootmgr"
#endif

#ifndef ISP_BOOTMGR_PORT
#define ISP_BOOTMGR_PORT 9999
#endif

/* Acquisition modes */

#define ISP_CONT 0  /* continuous */
#define ISP_TRIG 1  /* triggered  */

/* Digitizer types */

#define ISP_UNKNOWN 0
#define ISP_DAS     1
#define ISP_SAN     2

/* DAS configuration file codes (must be negative) */

#define ISP_BACKUP_CNF  -1
#define ISP_NEW_CNF     -2

/* DAS command codes (taken from V8.2/include/tasks/dascmds.h) */

#define DAS_SYNCHRONIZE_CMD          0x01 /* not implemented */
#define DAS_CONFIRM_CMD              0x02
#define DAS_DEFAULT_TRIGCONFIG_CMD   0x41
#define DAS_RECONFIGURE_DETECTOR_CMD 0x42
#define DAS_START_CALIBRATE_CMD      0x43
#define DAS_DEFAULT_CONFIG_CMD       0x44
#define DAS_END_CALIBRATE_CMD        0x45
#define DAS_FLUSHQ_CMD               0x46 /* does not exist */
#define DAS_SEND_IDDATA_CMD          0x49
#define DAS_LOGLEVEL_CMD             0x4c
#define DAS_RECONFIGURE_CMD          0x52
#define DAS_SEND_STATS_CMD           0x53
#define DAS_SENDLOG_CMD              0x54
#define DAS_RESET_CMD                0x55
#define DAS_OFFLINE_CMD              0x2a /* not implemented */
#define DAS_SETYEAR_CMD              0x59 /* not implemented */
#define DAS_ACK                      0x06
#define DAS_NAK                      0x15

/* DAS preset calibs (taken from V8.2/include/tasks/dascmds.h) */

#define DAS_CALIB1 'a'   /* 5V,    20 sec, Random Binary, 12 hour */
#define DAS_CALIB2 'b'   /* 1.25V  20 sec, Random Binary, 12 hour */
#define DAS_CALIB3 'c'   /* 0.31V, 20 sec, Random Binary, 12 hour */
#define DAS_CALIB4 'd'   /* 1.25V   1 sec, Triangle,       1 hour */
#define DAS_CALIB5 'e'   /* 1.25V  10 sec, Triangle,       1 hour */

/* DAS log levels */

#define DAS_LOG_FATAL    0 /* unused */
#define DAS_LOG_ERROR    1 /* unused */
#define DAS_LOG_WARING   2 /* unused */
#define DAS_LOG_INFO     3
#define DAS_LOG_DEBUG    4
#define DAS_LOG_OPERATOR 5 /* unused */

/* Largest number of streams supported */

#define DAS_MAXSTREAM 25
#define SAN_MAXSTREAM 64
#define ISP_MAXSTREAM SAN_MAXSTREAM

/* Largest number of channels DAS supports */

#define DAS_MAXCHAN 24

/* Length of largest possible single message to the DAS */

#define ISP_MAXDASCMDLEN 256

/* Max length of output device name (to report) */

#define ISP_DEVNAMELEN 32

/* Calibration states */

#define DAS_CAL_NOT_UNDERWAY    0
#define DAS_CAL_AWAITING_START  1
#define DAS_CAL_UNDERWAY        2

#define ISP_CALIB_OFF     DAS_CAL_NOT_UNDERWAY
#define ISP_CALIB_PENDING DAS_CAL_AWAITING_START
#define ISP_CALIB_ON      DAS_CAL_UNDERWAY
#define ISP_CALIB_PENDING_AT_ISP 3

/* Configuration states */

#define ISP_CONFIG_UNKNOWN    1
#define ISP_CONFIG_INPROGRESS 2
#define ISP_CONFIG_VERIFIED   3

/* Output device states */

#define ISP_OUTPUT_UNKNOWN  0 /* have not attempted to open device */
#define ISP_OUTPUT_ONLINE   1 /* device is ready to record         */
#define ISP_OUTPUT_OFFLINE  2 /* device is off-line (no media)     */
#define ISP_OUTPUT_SKIP     3 /* skipping to end of data           */
#define ISP_OUTPUT_REWIND   4 /* rewinding                         */
#define ISP_OUTPUT_OPER     5 /* waiting for operator instructions */
#define ISP_OUTPUT_CHECK    6 /* checking newly inserted tape      */

/* Types of output devices */

#define ISP_OUTPUT_NONE  0 /* no mass storage device selected */
#define ISP_OUTPUT_TAPE  1 /* tape output                     */
#define ISP_OUTPUT_DISK  2 /* removable disk output           */
#define ISP_OUTPUT_CDROM 3 /* CD-rom output                   */

/* Commands for dialogs with ispd server thread */
/* Protocol permits values from 0 to 32768      */

#define ISP_OK                 0 /* success                           */
#define ISP_ERROR              1 /* failure                           */
#define ISP_NOP                2 /* do nothing (heartbeat)            */
#define ISP_DISCONNECT         3 /* disconnect                        */
#define ISP_NOPERM             4 /* no permission (not operator)      */
#define ISP_TIMEOUT            5 /* set I/O timeout interval          */

#define ISP_PARAM             10 /* run time parameters request/reply */
#define ISP_STATUS            11 /* state of health request/reply     */
#define ISP_STATE             12 /* operator/observer state           */
#define ISP_DASCNF            13 /* das configuration request/reply   */
#define ISP_DASCAL            14 /* das calibration request           */
#define ISP_CHNMAP            15 /* IDA channel map request/reply     */
#define ISP_STREAMMAP         16 /* IDA stream map entry              */

#define ISP_CLR_PACKSTAT      30 /* clear packet counters             */
#define ISP_CLR_COMMSTAT      31 /* clear uplink counters             */
#define ISP_CLR_AUXSTAT       32 /* clear auxillary counters          */
#define ISP_CLR_EVERYTHING    33 /* clear all counters                */
#define ISP_UPDATE_CDR_STATS  34 /* update stats (cd-r systems)       */

#define ISP_MEDIA_EJECT       40 /* eject media (no flush)            */
#define ISP_MEDIA_FLUSH       41 /* flush media                       */
#define ISP_MEDIA_CHANGE      42 /* change media (flush + eject)      */
#define ISP_MEDIA_ALERT       43 /* tape has data alert               */
#define ISP_MEDIA_OVERWRITE   44 /* permit overwrite                  */
#define ISP_MEDIA_APPEND      45 /* append to tape                    */

#define ISP_DAS_REBOOT        50 /* reboot DAS                        */
#define ISP_DAS_IDDATA        51 /* DAS iddata request                */
#define ISP_DAS_LOG_INFO      52 /* set DAS log level to LOG_INFO     */
#define ISP_DAS_LOG_DEBUG     53 /* set DAS log level to LOG_DEBUG    */
#define ISP_DAS_FLUSHLOG      54 /* flush DAS log                     */
#define ISP_DAS_ABORT_CAL     55 /* abort DAS calibration             */
#define ISP_DAS_ACK           56 /* force an immediate ACK            */
#define ISP_DAS_NAK           57 /* force an immediate NAK            */
#define ISP_DAS_SOH           58 /* request a DAS status report       */

#define ISP_NRTS_START        60 /* start NRTS subsystem              */
#define ISP_NRTS_STOP         61 /* stop  NRTS subsystem              */

#define ISP_DEBUG_BARO        70 /* toggle barometer debug mode       */
#define ISP_DEBUG_DPM         71 /* toggle DPM       debug mode       */
#define ISP_DEBUG_CLOCK       72 /* toggle clock     debug mode       */

#define ISP_HOST_REBOOT       90 /* reboot isp                        */
#define ISP_HOST_SHUTDOWN     91 /* shutdown isp                      */
#define ISP_ABORT_BOOTOP      92 /* abort reboot or shutdown          */
#define ISP_LOG_INCR          93 /* increase ispd log verbosity       */
#define ISP_LOG_RESET         94 /* default logging verbosity         */

#define ISP_LAST_COMMAND      94 /* required, for client convenience  */

/* Possible client states */

#define ISP_OBSERVER  0 /* client is an observer only     */
#define ISP_OPERATOR  1 /* client has operator privileges */
#define ISP_ROOT      2 /* client has full privileges     */

/* DAS configuration */

#define ISP_DASCNF_REQ 0 /* request active configuration */
#define ISP_DASCNF_SET 1 /* load new configuration       */
#define ISP_DASCNF_DEF 2 /* load default configuration   */
#define ISP_DASCNF_BAK 3 /* load backup configuration    */

struct isp_dascnf {
    MUTEX mutex;
    int flag;   /* for socket I/O use only */
    int preset; /* if non-zero, use a preset, else use below  */
    /*
    /* Stream definitions */
    struct {
        int active;   /* if set, the stream is active      */
        int channel;  /* DAS channel ident                 */
        int filter;   /* DAS filter code                   */
        int mode;     /* 0 for continuous, 1 for triggered */
        char name[NRTS_CNAMLEN+1]; /* channel name         */
    } stream[DAS_MAXSTREAM];
    /* Event detector */
    struct {
        int chan[DAS_MAXCHAN]; /* detector channels                 */
        int nchan;             /* number of defined entries in chan */
        short bitmap;          /* bitmap of above                   */
        int key;               /* key channel                       */
        int sta;               /* short term average, samples       */
        int lta;               /* long term average, samples        */
        int thresh;            /* lta/sta trigger ratio (x 10000)   */
        int voters;            /* minimum number of voters          */
        long maxtrig;          /* maximum trigger length, samples   */
        int preevent;          /* pre-event memory, records         */
    } detect;
};

#define ISP_CTSIZE (sizeof(struct isp_dascnf) * ISP_NSTREAM)

/* DAS calibration request */

#define DASCAL_IMPULSE  0
#define DASCAL_SQUARE   1
#define DASCAL_SINE     2
#define DASCAL_TRIANGLE 3
#define DASCAL_RANDOM   4
#define DASCAL_FNCMIN DASCAL_IMPULSE
#define DASCAL_FNCMAX DASCAL_RANDOM

#define DASCAL_AMP50000 0 /* 5 V     */
#define DASCAL_AMP12500 1 /* 1.25 V  */
#define DASCAL_AMP03100 2 /* 0.31 V  */
#define DASCAL_AMP00780 3 /* 78 mV   */
#define DASCAL_AMP00200 4 /* 20 mV   */
#define DASCAL_AMP00049 5 /* 4.9 mV  */
#define DASCAL_AMP00012 6 /* 1.2 mV  */
#define DASCAL_AMPMIN DASCAL_AMP50000
#define DASCAL_AMPMAX DASCAL_AMP00012

struct isp_dascal {
    time_t on;  /* start time                                 */
    long dur;   /* duration, seconds                          */
    int preset; /* if non-zero, use a preset, else use below  */
    int fnc;    /* function code, from above list of defines  */
    int amp;    /* amplitude code, from above list of defines */
    int wid;    /* cal signal period, 5 msec units            */
    int ntr;    /* signal interval (impulse & square only)    */
};

/* Alarms */

#define ISP_ALARM_NONE    0x0000 /* Everything is fine                   */
#define ISP_ALARM_DAS     0x0001 /* No data or bad data from DAS         */
#define ISP_ALARM_OPER    0x0002 /* Operator attentions required         */
#define ISP_ALARM_IOERR   0x0004 /* Mass store I/O error on last attempt */
#define ISP_ALARM_OFFLINE 0x0008 /* output device is off line            */
#define ISP_ALARM_OQFULL  0x0010 /* Output queue is full                 */
#define ISP_ALARM_BQFULL  0x0020 /* Barometer queue is full              */
#define ISP_ALARM_RQFULL  0x0040 /* Raw input queue is full              */
#define ISP_ALARM_AUX     0x0080 /* Auxlillary data error                */
#define ISP_ALARM_DASDBG  0x0100 /* DAS is in debug mode                 */
#define ISP_ALARM_DASCLK  0x0200 /* DAS clock unlocked or in error       */
#define ISP_ALARM_AUXCLK  0x0400 /* AUX clock unlocked or in error       */
#define ISP_ALARM_NRTS    0x0800 /* NRTS subsystem disabled              */
#define ISP_ALARM_ADC     0x1000 /* ADC timeout suspected                */

struct isp_alarm {
    u_long code;    /* bit mask of above values            */
    time_t das;     /* time alarm was last set  */
    time_t oper;    /* time alarm was last set  */
    time_t ioerr;   /* time alarm was last set  */
    time_t offline; /* time alarm was last set  */
    time_t oqfull;  /* time alarm was last set  */
    time_t bqfull;  /* time alarm was last set  */
    time_t rqfull;  /* time alarm was last set  */
    time_t aux;     /* time alarm was last set  */
    time_t dasdbg;  /* time alarm was last set  */
    time_t dasclk;  /* time alarm was last set  */
    time_t auxclk;  /* time alarm was last set  */
    time_t nrts;    /* time alarm was last set  */
    time_t adc;     /* time alarm was last set  */
};

/* Auxiliary input stats */

typedef struct {
    int enabled;        /* non-zero if device enabled    */
    long datum;         /* current value                 */
    unsigned long nrec; /* number of records produced    */
    unsigned long rerr; /* number of device read errors  */
    unsigned long drop; /* number of dropped samples     */
    unsigned long miss; /* number of missed samples      */
} ISP_AUX_STAT;

/* ISP state of health */

typedef struct {
    time_t connected;
    time_t lastRead;
    struct {
        UINT32 p1;
        UINT32 p2;
        UINT32 p3;
        UINT32 reconn;
    } count;
} SAN_CONNSTAT;
        
struct isp_status {

    MUTEX lock;
    struct isp_alarm alarm;   /* system alarms                          */
    pid_t  pid;               /* ISPD process id                        */
    int digitizer;            /* ISP_DAS or ISP_SAN                     */
    time_t start;             /* ISPD start time                        */
    time_t clock;             /* most recent DAS external clock reading */
    int    clock_qual;        /* clock quality factor for above         */
    time_t tstamp;            /* time status record was recorded        */

    struct {
        time_t read;          /* time of most recent read from DAS      */
        time_t write;         /* time of most recent write to massio    */
        time_t pkt;           /* time of most recent good packet        */
        time_t commerr;       /* time of last uplink comm error         */
        time_t dasstat;       /* time of last DAS status report         */
        time_t change;        /* time last media exchange was started   */
        time_t stream[ISP_MAXSTREAM];/* time of last packet, per stream */
    } last;

    struct {
        time_t pkt;           /* time packet counters were cleared   */
        time_t comm;          /* time uplink statistics were cleared */
        time_t aux;           /* time aux    statistics were cleared */
    } cleared;

    int have_iddata;          /* If set, the following is valid      */
    IDA_IDENT_REC iddata;     /* DAS ident record                    */
    int have_config;          /* If set, the following is valid      */
    IDA_CONFIG_REC config;    /* DAS configuration record            */

    /* Heap */

    struct {
        struct { int nfree; int lowat; } barometer;
        struct { int nfree; int lowat; } dpm;
        struct { int nfree; int lowat; } commands;
        struct { int nfree; int lowat; } packets;
        struct { int nfree; int lowat; } obuf;
    } heap;

    /* Inter-thread message queues */

    struct {
        struct { int npend; int hiwat; } baro;
        struct { int npend; int hiwat; } dpm;
        struct { int npend; int hiwat; } das_read;
        struct { int npend; int hiwat; } das_write;
        struct { int npend; int hiwat; } das_process;
        struct { int npend; int hiwat; } massio;
        struct { int npend; int hiwat; } nrts;
        struct { int npend; int hiwat; } obuf;
    } queue;

    /* Record counters */

    struct {
        /* packet counters */
        long drec; /* number of data records received          */
        long lrec; /* number of log records received           */
        long irec; /* number of ident records received         */
        long krec; /* number of configuration records received */
        long crec; /* number of calibration records received   */
        long srec; /* number of status records received        */
        long erec; /* number of event records received         */
        long orec; /* number of unrecognized DAS record        */
        long arec; /* number of aux (non-DAS) records received */
        long urec; /* number of Unix log records received      */
        long nrec; /* total number of records received         */
        /* uplink counters */
        long badr; /* number of full but corrupt records recvd */
        long sync; /* number of sync errors                    */
        long shrt; /* number of short records received         */
        long naks; /* number of naks sent to DAS               */
        long dups; /* number of duplicate records received     */
        /* counted, but not currently reported */
        long njec; /* number of injected records received      */
    } count;

    /* DAS specific information */

    struct {
        long reboots;   /* number of DAS reboots (since ISPD start) */
        time_t start;   /* time of last DAS reboot                  */
        long naks;      /* number of xmit errors                    */
        long acks;      /* number of packets sent and ack'd         */
        long dropped;   /* number of dropped packets                */
        long timeouts;  /* number of xmit timeouts with ARS/ISP     */
        long triggers;  /* number of triggers                       */
    } das;

    /* Activity flags */

    struct {
        int  event;     /* event in progress flag       */
        int  calib;     /* calibration in progress flag */
        int  config;    /* configuration in progress    */
        int  operator;  /* set for operators            */
    } flag;

    /* Mass store status */

    struct {
        long nrec;    /* number of records buffered so far */
        int capacity; /* size of buffer in records         */
    } buffer;

    struct {
        char device[ISP_DEVNAMELEN];
        long bfact;         /* blocking factor            */
        int type;           /* device type code           */
        int state;          /* device status              */
        unsigned long nrec; /* number of records stored   */
        int capacity;       /* output capacity (if known) */
        long err;           /* I/O error counter          */
        int file;           /* If tape, crnt file number  */
        unsigned long lost; /* number of lost records     */
    } output;

    /* Internal buffering of incoming data */

    struct {
        int size;     /* total size of the queue */
        int minfree;  /* high water mark         */
    } msgq;

    /* Log facility statistics */

    struct {
        unsigned long nrec; /* number of records produced    */
        unsigned long drop; /* number of dropped messages    */
    } log;

    /* Barometer statistics */

    ISP_AUX_STAT baro;

    /* DPM statistics */

    ISP_AUX_STAT dpm;

    /* External clock statistics */

    struct {
        int enabled;        /* non-zero if device is enabled */
        time_t datum;       /* current value                 */
        char   code;        /* current time quality code     */
        short  qual;        /* numerical equivalent of code  */
        unsigned long err;  /* number of errors (all types)  */
    } extclock;

    /* SAN specific stuff */

    struct {
        SAN_CONNSTAT data;
        SAN_CONNSTAT cmnd;
        int cal;
        BOOL evt;
        BOOL oper;
        time_t boottime;
        struct {
            int  status;
            int  phase;
            int  pll;
        } gps;
        char   din;
        float  temp;
        int    nstream;
        struct {
            char    name[SAN_STREAM_NAMLEN+1];
            float   rms;
            float   ave;
        } stream[SAN_MAX_STREAMS];
    } san;
};

/* Used to support ISP connected auxiliary devices */

typedef struct {
    int enabled;
    char port[ISP_MAXDEVNAM]; /* serial port           */
    int  baud;  /* baud rate                           */
    int  flow;  /* flow control flag                   */
    int comp;   /* if set, compress the data           */
    int strm;   /* IDA8 Stream  id to use in packet headers */
    int chan;   /* IDA8 Channel  "  "  "   "    "      "    */
    int filt;   /* IDA8 Filter   "  "  "   "    "      "    */
    int sint;   /* desired sample interval, seconds    */
    int pr;                       /* barometer PR coefficient */
    char uf[ISP_MAXPAROPARAMLEN]; /* barometer UF coefficient */
    char name[IDA10_CNAMLEN+1];   /* IDA10 channel name */
    int  multiplier;              /* IDA10 multiplier */
    float thresh;                 /* DPM threshold */
} ISP_AUX_PARAM;

/* Used for remote power cycles via iboot */

typedef struct {
    char *server; /* name or dot decimal IP address of iboot (NULL to suppress) */
    int port;     /* port number to connect to */
    char *passwd; /* authentication password */
    int interval; /* interval in seconds to issue command, when condition exists */
    int maxcycle; /* Maximum number of power cycles permitted */
} ISP_IBOOT;

/* Used to support ISP connected external clocks */

struct isp_extclock {
    int enabled;
    char port[ISP_MAXDEVNAM]; /* serial port                    */
    int baud;      /* baud rate                                 */
    int flow;      /* flow control flag                         */
    int tolerance; /* maximum (host time - external time) error */
    int type;      /* clock type                                */
};

struct isp_reftime {
    time_t ext;     /* external time                              */
    time_t hz;      /* system time when external time was sampled */
    time_t qual;    /* external time quality code                 */
    MUTEX mutex;    /* protection                                 */
};

/* Run time parameters */

typedef struct isp_params {

    /* Stuff I wish I thought of 10 years ago */

    ISI_GLOB glob;
    IDA *ida;
    UINT32 flags;

    /* General information */

    int digitizer;            /* ISP_DAS or ISP_SAN */

    char sta[ISP_MAXSYSNAM];  /* NRTS station/system name   */

    int nrts;                 /* NRTS acquisition switch    */
    int inject;               /* injection service switch   */

    int pktlen;               /* digitizer packet length */

    /* DAS I/O parameters */

    char port[ISP_MAXDEVNAM]; /* uplink device name            */
    speed_t ibaud;       /* input uplink baud rate             */
    speed_t obaud;       /* output uplink baud rate            */
    int nodatato;        /* reboot DAS after this many seconds */
    int iddatato;        /* wait for IDDATA timeout interval   */

    /* Internal ISPD buffer lengths */

    int rawq;            /* Data packets (all sources) */
    int cmdq;            /* DAS comands                */
    int barq;            /* Barometer samples          */
    int dpmq;            /* DMP samples                */

    /* Output device specification */

    char odev[ISP_MAXDEVNAM]; /* output device name              */
    long bfact;          /* blocking factor (packets per write)  */
    long obuf;           /* massio buffer length, blocks         */
    long numobuf;        /* number of massio buffers to maintain */

    /* The following are not intended for general user modification */

    int ttyto;           /* uplink timeout (secs)              */
    int mtu;             /* DAS write MTU                      */
    unsigned long delay; /* DAS write delay (microseconds)     */
    int statint;         /* DAS status request interval (secs) */
    int savedasstats;    /* if set release DAS status packets  */

    int pktrev;          /* Data format revision               */

    /* SAN data and C&C endpoints */

    struct {
        char addr[ISP_MAXADDRLEN];
        struct {
            int cmnd;
            int data;
        } port;
        int timeout;
        int sohint;
    } san;

    /* Auxillary information */

    ISP_AUX_PARAM baro;      /* barograph control structure      */
    ISP_AUX_PARAM dpm;       /* DPM control structure            */
    struct isp_extclock clock; /* external clock control structure */

    /* For debugging convenience only */

    int overwrite;       /* if set, always overwrite media     */

    /* Optional correction for MK7 digitizers with RT593 digitizers */

    struct {
        BOOL present; /* if TRUE, then user has defined and RT593 correction */
        int correct;  /* the correction, in tics */
    } rt593;

    /* Optional support for iboot */

    ISP_IBOOT iboot;

} ISP_PARAMS;

typedef struct isp_server {
    int sd;         /* socket at which to listen for connections */
    int port;       /* port number                               */
    int maxclients; /* number of maximum simultaneous clients    */
    int to;         /* default I/O timeout interval              */
} ISP_SERVER;

/* Something big enough to hold largest possible socket message.  It is */
/* not expected to be used for anything other than computing ISP_BUFLEN.*/

struct isp_biggest_message {
    int length;
    union {
        struct isp_status status;
        struct isp_dascnf dascnf;
        struct isp_dascal dascal;
        char ida10[IDA10_MAXRECLEN];
        char ida[IDA_BUFSIZ];
        char name[MAXPATHLEN+1];
    } stuff;
    char overhead[64];
};

#define ISP_BUFLEN sizeof(struct isp_biggest_message)

/* Macros */

#ifndef SWAB
#ifdef M_I86 /*  Need to byte-swap on PC's, defined by MS C compiler */
#define SWAB
#endif
 
#ifdef vax   /*  Need to byte-swap on Vax's  */
#define SWAB
#endif
 
#ifdef i386  /*  Need to byte-swap on PC's  */
#define SWAB
#endif
#endif
 
#ifdef SWAB
#define SSWAB(a, b) util_sswap((short *) (a), (long) (b))
#define LSWAB(a, b) util_lswap((long *)  (a), (long) (b))
#else
#define SSWAB(a, b)
#define LSWAB(a, b)
#endif /* ifdef SWAB */

#define isp_getcnf(ptr) isp_readcnf(ISP_CNF_FILE,    ptr)
#define isp_defcnf(ptr) isp_readcnf(ISP_DEFCNF_FILE, ptr)
#define isp_bakcnf(ptr) isp_readcnf(ISP_BAKCNF_FILE, ptr)

/* libisp.a library function prototypes */

char *isp_setup(char **, char **);
char isp_alrm_code(struct isp_alarm *, int);
int  isp_bootmgr(int);
void isp_dascnf_text(struct isp_dascnf *, int, char **, char **, char **, char **);
void ispDecodeChnLocMapEntry(IDA_CHNLOCMAP *dest, UINT8 *src);
void isp_decode_dascal(struct isp_dascal *, char *);
int  isp_decode_dascnf(struct isp_dascnf *, char *);
void isp_decode_params(struct isp_params *, char *);
void isp_decode_status(struct isp_status *, char *);
void isp_prtdascnf(FILE *, struct isp_dascnf *);
void isp_prtstatus(FILE *, struct isp_status *);
int  isp_readcnf(char *fname, struct isp_dascnf *cnf);
int  isp_recv(int, char *, int, void **);
int  isp_send(int, int, char *, int, int);
int  ispSendChnLocMap(int sd, char *buffer, IDA *ida, int to);
int  isp_send_dascal(int, char *, struct isp_dascal *, int);
int  isp_send_dascnf(int, char *, struct isp_dascnf *, int);
int  isp_send_params(int, char *, struct isp_params *, int);
int  isp_send_state(int, char *, int, int);
int  isp_send_status(int, char *, struct isp_status *, int);
int  isp_send_timeout(int, char *, int);
int  isp_savcnf(char *fname, struct isp_dascnf *dascnf);
int  isp_setcnf(struct isp_dascnf *dascnf);

int ispPackStatus(char *dest, struct isp_status *src);
int ispUnpackStatus(char *src, struct isp_status *dest);

int ispPacketLength(UINT8 *packet);

int ispGetMediaType(ISP_PARAMS *params);
BOOL ispLoadRunParam(char *path, char *sta, ISP_PARAMS *params, ISP_SERVER *server);
void ispSetLogParameter(ISP_PARAMS *params, LOGIO *lp);

#endif /* isp_h_defined */

/* Revision History
 *
 * $Log: isp.h,v $
 * Revision 1.21  2008/10/09 20:38:38  dechavez
 * added ISP_IBOOT
 *
 * Revision 1.20  2007/01/25 20:25:11  dechavez
 * RT493 support
 *
 * Revision 1.19  2006/12/08 17:28:28  dechavez
 * rearranged includes
 *
 * Revision 1.18  2006/03/15 19:22:27  dechavez
 * cosmetic
 *
 * Revision 1.17  2006/02/09 00:02:07  dechavez
 * added IDA handle to master param structure
 *
 * Revision 1.16  2005/05/27 00:36:16  dechavez
 * using oldttyio
 *
 * Revision 1.15  2004/12/21 00:35:38  dechavez
 * define ISP_MIN_NRTS_FLUSH_INTERVAL
 *
 * Revision 1.14  2003/12/10 05:35:26  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.13  2002/09/10 17:54:32  dec
 * changed comments of ISP_AUX_PARAM to indicated IDA8 specific params
 *
 * Revision 1.12  2002/09/09 21:00:16  dec
 * recast barometer parameters in terms of new ISP_AUX_PARAM and added dpm
 *
 * Revision 1.11  2002/05/13 18:39:33  dec
 * use NRTS_CNAMLEN to dimension name in das config structure
 *
 * Revision 1.10  2002/03/15 22:33:26  dec
 * added ida10 (and hence variable length packet) support,
 * ispPacketLength(), ispGetMediaType(), ispLoadRunParam() prototypes
 *
 * Revision 1.9  2002/02/23 00:05:15  dec
 * added ISP_UPDATE_CDR_STATS to command list
 *
 * Revision 1.8  2002/02/21 22:32:22  dec
 * add pktlen (packet length) to parameter structure
 *
 * Revision 1.7  2001/10/24 23:00:48  dec
 * added ISP_OUTPUT_CDROM define and "iso" directory
 *
 * Revision 1.6  2001/05/20 15:56:41  dec
 * switch to platform.h MUTEX
 *
 * Revision 1.5  2001/04/19 22:43:25  dec
 * add ISP_DEBUG_BARO and ISP_DEBUG_CLOCK
 *
 * Revision 1.4  2000/11/02 20:20:36  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.3  2000/10/19 22:35:33  dec
 * 2.0.5
 *
 * Revision 1.2  2000/09/20 00:56:42  dec
 * 2.0.4 isp mods (SAN)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
