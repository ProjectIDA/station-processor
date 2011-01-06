#pragma ident "$Id: qdp.h,v 1.33 2009/03/17 17:17:48 dechavez Exp $"
/*======================================================================
 *
 *  Quanterra QDP communication
 *
 *====================================================================*/
#ifndef qdp_h_included
#define qdp_h_included

#include "platform.h"
#include "udpio.h"
#include "util.h"
#include "msgq.h"
#include "logio.h"
#include "qdp/limits.h"
#include "qdp/codes.h"
#include "qdp/fsa.h"
#include "qdp/tokens.h"
#include "qdp/status.h"
#include "qdp/errno.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char *libqdpBuildIdent;

/* Constants */

#define QDP_VERSION 2

#define QDP_OK         0
#define QDP_TIMEOUT    1
#define QDP_IOERROR    2
#define QDP_CRCERR     3
#define QDP_OVERFLOW   4
#define QDP_INVALID    5
#define QDP_INCOMPLETE 6
#define QDP_ERROR      7 /* check errno */

#define QDP_CTRL  0
#define QDP_DATA  1

#define QDP_EPOCH_TO_1970_EPOCH 946684800
#define QDP_EPOCH_TO_1999_EPOCH  31536000

/* UDP port numbers relative to base port */

#define QDP_PORT_CONFIG  0 /* configuration */
#define QDP_PORT_SPECIAL 1 /* special functions */
#define QDP_PORT_1_CTRL  2 /* control on data port 1 */
#define QDP_PORT_1_DATA  3 /* data on data port 1 */
#define QDP_PORT_2_CTRL  4 /* control on data port 2 */
#define QDP_PORT_2_DATA  5 /* data on data port 2 */
#define QDP_PORT_3_CTRL  6 /* control on data port 3 */
#define QDP_PORT_3_DATA  7 /* data on data port 3 */
#define QDP_PORT_4_CTRL  8 /* control on data port 4 */
#define QDP_PORT_4_DATA  9 /* data on data port 4 */

/* Port numbers as specified by the app */

#define QDP_UNDEFINED_PORT -2
#define QDP_MIN_PORT       -1
#define QDP_SFN_PORT       -1 /* Special Functions */
#define QDP_CFG_PORT        0 /* Configuration */
#define QDP_LOGICAL_PORT_1  1 /* Logical Port 1 */
#define QDP_LOGICAL_PORT_2  2 /* Logical Port 2 */
#define QDP_LOGICAL_PORT_3  3 /* Logical Port 3 */
#define QDP_LOGICAL_PORT_4  4 /* Logical Port 4 */
#define QDP_MAX_PORT        4

/* Bits in the status request bitmap */

#define QDP_STATUS_GLOBAL          0x000000000001
#define QDP_STATUS_GPS             0x000000000002
#define QDP_STATUS_POWER_SUPPLY    0x000000000004
#define QDP_STATUS_BOOM_ETC        0x000000000008
#define QDP_STATUS_THREAD          0x000000000010
#define QDP_STATUS_PLL             0x000000000020
#define QDP_STATUS_SATELLITES      0x000000000040
#define QDP_STATUS_ARP             0x000000000080
#define QDP_STATUS_DATA_PORT_1     0x000000000100
#define QDP_STATUS_DATA_PORT_2     0x000000000200
#define QDP_STATUS_DATA_PORT_3     0x000000000400
#define QDP_STATUS_DATA_PORT_4     0x000000000800
#define QDP_STATUS_SERIAL_1        0x000000001000
#define QDP_STATUS_SERIAL_2        0x000000002000
#define QDP_STATUS_SERIAL_3        0x000000004000
#define QDP_STATUS_ETHERNET        0x000000008000
#define QDP_STATUS_BALER           0x000000010000
#define QDP_STATUS_DYNIP           0x000000020000
#define QDP_STATUS_AUXBOARD        0x000000040000
#define QDP_STATUS_SERIAL_SENSOR   0x000000080000

#define QDP_STATUS_DEFAULT_BITMAP  0xEF

/* Physical Ports */

#define QDP_PP_SER1 0
#define QDP_PP_SER2 1
#define QDP_PP_SER3 2
#define QDP_PP_ETH  3

/* Logging verbosity codes */

#define QDP_TERSE  0 /* only warnings and errors */
#define QDP_INFO   1 /* normal */
#define QDP_DEBUG  2 /* very noisy */

/* Clock Quality Flag Bits */

#define QDP_CQ_HAS_BEEN_LOCKED 1 /* Has been locked, else internal time */
#define QDP_CQ_2D 2 /* 2D Lock */
#define QDP_CQ_3D 4 /* 3D Lock */
#define QDP_CQ_1D 8 /* No fix, but time should be good */
#define QDP_CQ_FILT 0x10 /* Filtering in progress */
#define QDP_CQ_SPEC 0x20 /* Speculative 1-D startup */

#define QDP_CQ_IS_LOCKED (QDP_CQ_2D | QDP_CQ_3D | QDP_CQ_1D)

/* PLL State */

#define QDP_PLL_OFF 0x00 /* Not on */
#define QDP_PLL_HOLD 0x40
#define QDP_PLL_TRACK 0x80
#define QDP_PLL_MASK 0xC0
#define QDP_PLL_LOCK QDP_PLL_MASK

/* QDP common header */

typedef struct {
    UINT32 crc;   /* checksum over all the packet */
    UINT8 cmd;    /* command */
    UINT8 ver;    /* version */
    UINT16 dlen;  /* payload length, not including header */
    UINT16 seqno; /* sender's sequence number */
    UINT16 ack;   /* sender's acknowledge */
} QDP_CMNHDR;

/* Quanterra Data Packet */

typedef struct {
    QDP_CMNHDR hdr;         /* header */
    UINT8 *payload;         /* points to payload */
    UINT8 raw[QDP_MAX_MTU]; /* the entire raw packet, including header */
    int len;                /* number of bytes actually used in raw */
} QDP_PKT;

#define QDP_MAX_DOTDECIMAL_LEN 16
#define QDP_MAX_PEER_LEN (QDP_MAX_DOTDECIMAL_LEN + 16)
#define QDP_IDENT_LEN 16

/* Command and response packets */

/* server IP, Port, Registration */

typedef struct {
    UINT32 ip;
    UINT16 port;
    UINT16 registration;
    char dotdecimal[QDP_MAX_DOTDECIMAL_LEN+1];
} QDP_DP_IPR;

/* server challenge */

typedef struct {
    UINT64 challenge;
    QDP_DP_IPR dp;
} QDP_TYPE_C1_SRVCH;

/* server challenge response */

typedef struct {
    UINT64 auth;
    UINT64 serialno;
    UINT64 challenge;
    QDP_DP_IPR dp;
    UINT64 random;
    UINT32 md5[4];
} QDP_TYPE_C1_SRVRSP;

/* memory request */

typedef struct {
    UINT32 offset;
    UINT16 nbyte;
    UINT16 type;
    UINT32 passwd[4];
} QDP_TYPE_C1_RQMEM;

/* memory response */

typedef struct {
    UINT32 offset;
    UINT16 nbyte;
    UINT16 type;
    UINT16 seg;
    UINT16 nseg;
    UINT8  contents[QDP_MAX_C1_MEM_PAYLOAD];
} QDP_TYPE_C1_MEM;

/* C1_FIX */

typedef struct {
    UINT8  raw;   /* raw 8-bit frequency code */
    UINT64 nsint; /* equivalent sample interval, nanoseconds */
    REAL64 dsint; /* equivalent sample interval, seconds */
} QDP_FREQ;

typedef struct {
    UINT32 last_reboot;
    UINT32 reboots;
    UINT32 backup_map;
    UINT32 default_map;
#define QDP_CAL_TYPE_QCAL330 33
    UINT16 cal_type;
    UINT16 cal_ver;
#define QDP_AUX_TYPE_AUXAD 32
    UINT16 aux_type;
    UINT16 aux_ver;
#define QDP_CLOCK_NONE 0
#define QDP_CLOCK_M12  1
    UINT16 clk_type;
#define QDP_C1_FIX_FLAG_ETHERNET 0x01
#define QDP_C1_FIX_FLAG_DHCP     0x02
    UINT16 flags;
    UINT16 sys_ver;
    UINT16 sp_ver;
    UINT16 pld_ver;
    UINT16 mem_block;
    UINT32 proper_tag;
    UINT64 sys_num;
    UINT64 amb_num;
    UINT64 sensor_num[QDP_NSENSOR];
    UINT32 qapchp1_num;
    UINT32 int_sz;
    UINT32 int_used;
    UINT32 ext_sz;
    UINT32 flash_sz;
    UINT32 ext_used;
    UINT32 qapchp2_num;
    UINT32 log_sz[QDP_NLP];
    QDP_FREQ freq[QDP_NFREQ];
    INT32  ch13_delay[QDP_NFREQ];
    INT32  ch46_delay[QDP_NFREQ];
} QDP_TYPE_C1_FIX;

/* C1_GLOB */

typedef struct {
    UINT16 clock_to;
    UINT16 initial_vco;
    UINT16 gps_backup;
    UINT16 samp_rates;
    UINT16 gain_map;
    UINT16 filter_map;
    UINT16 input_map;
    UINT16 web_port;
    UINT16 server_to;
    UINT16 drift_tol;
    UINT16 jump_filt;
    UINT16 jump_thresh;
    INT16  cal_offset;
    UINT16 sensor_map;
    UINT16 sampling_phase;
    UINT16 gps_cold;
    UINT32 user_tag;
    UINT16 scaling[QDP_NCHAN][QDP_NFREQ];
    UINT16 offset[QDP_NCHAN];
    UINT16 gain[QDP_NCHAN];
    UINT32 msg_map;
} QDP_TYPE_C1_GLOB;

/* C1_LOG */

typedef struct {
    UINT16 port;
    UINT16 flags;
    UINT16 perc;
    UINT16 mtu;
    UINT16 group_cnt;
    UINT16 rsnd_max;
    UINT16 grp_to;
    UINT16 rnsd_min;
    UINT16 window;
    UINT16 dataseq;
    UINT16 freqs[QDP_NCHAN];
    UINT16 ack_cnt;
    UINT16 ack_to;
    UINT32 olddata;
    UINT16 eth_throttle;
    UINT16 full_alert;
    UINT16 auto_filter;
    UINT16 man_filter;
} QDP_TYPE_C1_LOG;

/* C1_COMBO (aka C1_FLGS), ignoring the C1_SC portion */

typedef struct {
    QDP_TYPE_C1_FIX fix;
    QDP_TYPE_C1_GLOB glob;
    QDP_TYPE_C1_LOG log;
} QDP_TYPE_C1_COMBO;

/* C1_QCAL */

#define QDP_QCAL_SINE  0
#define QDP_QCAL_RED   1
#define QDP_QCAL_WHITE 2
#define QDP_QCAL_STEP  3
#define QDP_QCAL_RB    4

typedef struct {
    UINT32 starttime; /* start time, 0 for immediately */
    UINT16 waveform;  /* cal signal type, sign, auto flag */
    UINT16 amplitude; /* number of bits shifted, 0=-6db, 1=-12db, etc */
    UINT16 duration;  /* duration in seconds */
    UINT16 settle;    /* settling time in seconds */
    UINT16 trailer;   /* time after cal ends before control lines or relays are restored */
    UINT16 chans;     /* calibration channels (bitmap) */
    UINT16 sensor;    /* sensor control bitmap */
    UINT16 monitor;   /* monitor channel bitmap */
    UINT16 divisor;   /* frequency divider 1=1Hz, 2=.5Hz, 20=.05Hz, etc */
} QDP_TYPE_C1_QCAL;

/* C1_PHY */

typedef struct {
    UINT64 serialno;       /* serial number */
    UINT16 baseport;       /* starting UDP port number */
    struct {
        UINT32 ip;         /* IP address */
        UINT16 baud;       /* baud rate code */
        UINT16 flags;      /* flags bitmask */
        UINT16 throttle;   /* 1024000 / bytes per second (or zero if disabled) */
    } serial[3];
    struct {
        UINT32 ip;         /* IP address */
        UINT8  mac[6];     /* hardware address */
        UINT16 flags;      /* flags bitmask */
    } ethernet;
    UINT16 reserved[2];    /* presently unused */
} QDP_TYPE_C1_PHY;

/* C1_DCP */

typedef struct {
    INT32 grounded[QDP_NCHAN];  /* counts with grounded input */
    INT32 reference[QDP_NCHAN]; /* counts with reference input */
} QDP_TYPE_C1_DCP;

/* C1_GID */

typedef char QDP_TYPE_C1_GID[QDP_GIDLEN+1];

/* C1_SPP */

typedef struct {
    UINT16 max_main_current;
    UINT16 min_off_time;
    UINT16 min_ps_voltage;
    UINT16 max_antenna_current;
     INT16 min_temp;
     INT16 max_temp;
    UINT16 temp_hysteresis;
    UINT16 volt_hysteresis;
    UINT16 default_vco;
} QDP_TYPE_C1_SPP;

/* C1_MAN */

typedef struct {
    UINT32 password[4];
    UINT16 qapchp1_type;
    UINT16 qapchp1_ver;
    UINT16 qapchp2_type;
    UINT16 qapchp2_ver;
    UINT32 qapchp1_num;
    UINT32 qapchp2_num;
    INT32 reference[QDP_NCHAN];
    UINT32 born_on;
    UINT32 packet_memory;
    UINT16 clk_type;
    UINT16 model;
    INT16 default_calib_offset;
    UINT16 flags;
    UINT32 proper_tag;
    UINT32 max_power_on;
} QDP_TYPE_C1_MAN;

/* C2_AMASS */

typedef struct {
    UINT16 tolerance[3];
    UINT16 maxtry;
    UINT16 duration;
    struct {
        UINT16 squelch;
        UINT16 normal;
    } interval;
    UINT16 bitmap;
} QDP_TYPE_C2_AMASS_SENSOR;

typedef struct {
    QDP_TYPE_C2_AMASS_SENSOR sensor[QDP_NSENSOR];
} QDP_TYPE_C2_AMASS;

typedef struct {
    UINT16 mode;          /* timing mode */
    UINT16 flags;         /* power cycling mode */
    UINT16 off_time;      /* GPS off time */
    UINT16 resync;        /* GPS resync hour */
    UINT16 max_on;        /* GPS maximum on time */
    UINT16 lock_usec;     /* PLL lock usec */
    UINT16 interval;      /* PLL update interval (sec) */
    UINT16 initial_pll;   /* PLL flags */
    REAL32 pfrac;         /* pfrac */
    REAL32 vco_slope;     /* VCO slope */
    REAL32 vco_intercept; /* VCO intersept */
    REAL32 max_ikm_rms;   /* Max. initial KM rms */
    REAL32 ikm_weight;    /* initial KM weight */
    REAL32 km_weight;     /* KM weight */
    REAL32 best_weight;   /* best VCO weight */
    REAL32 km_delta;      /* KM delta */
} QDP_TYPE_C2_GPS;

/* DT_DACK */

typedef struct {
    UINT16 throttle;
    UINT32 ack[4];
} QDP_TYPE_DT_DACK;

/* User supplied function for dealing with received packets */

typedef void (*QDP_USER_FUNC)(void *arg, QDP_PKT *pkt);

/* Structure for receiving C1_MEMs */

typedef struct {
    UINT16 type;
    UINT8 data[QDP_TOKEN_BUFLEN];
    INT16 pending[QDP_MAX_MEM_SEG];
    INT16 nbyte;
    INT16 remain;
    BOOL full;
} QDP_MEMBLK;

/* Raw meta-data needed for processing DT_DATA packets */

typedef struct {
    struct {
        QDP_MEMBLK token;             /* DP token memory */
        UINT8 combo[QDP_MAX_PAYLOAD]; /* C1_COMBO payload */
    } raw;
    QDP_TYPE_C1_COMBO combo; /* decoded raw.combo */
    QDP_DP_TOKEN token;      /* decoded raw.token */
    UINT8 state;  /* completeness state */
} QDP_META;

#define QDP_META_TAG_EOF   0
#define QDP_META_TAG_TOKEN 1
#define QDP_META_TAG_COMBO 2
#define QDP_META_STATE_HAVE_MN232 0x01
#define QDP_META_STATE_HAVE_COMBO 0x02
#define QDP_META_STATE_HAVE_TOKEN 0x04
#define QDP_META_STATE_COMPLETE (QDP_META_STATE_HAVE_MN232 | QDP_META_STATE_HAVE_COMBO | QDP_META_STATE_HAVE_TOKEN)

/* User supplied function for dealing with meta-data */

typedef void (*QDP_META_FUNC)(void *arg, QDP_META *meta);

/* Parameters for initializing a QDP connection */

typedef struct {
    int base;  /* base */
    int value; /* assigned value */
} QDP_CLIENT;

typedef struct {
    char host[MAXPATHLEN+1]; /* digitizer name or dot decimal address */
    struct {
        int base;        /* server side base IP port */
        int link;        /* link port: logical data port (1-4), cnf (0), or special (-1) */
        QDP_CLIENT ctrl; /* client side control port */
        QDP_CLIENT data; /* client side data port */
    } port;
    struct {
        UINT32 ctrl;     /* msec read timeout on control port */
        UINT32 data;     /* msec read timeout on data port */
    } timeout;
    struct {
        UINT32 open;     /* msec DT_OPEN on idle data link interval */
        UINT32 retry;    /* msec retry registration interval */
    } interval;
    struct {
        UINT32 interval; /* msec status request interval */
        BOOL forward;    /* if TRUE, copy replies to application */
        UINT32 bitmap;   /* C1_RQSTAT bitmap parameter */
    } hbeat;
    UINT64 authcode;     /* authentication code */
    UINT64 serialno;     /* digitizer serial number */
    int nbuf;            /* number of packets to buffer */
    int debug;           /* library log message verbosity */
    MUTEX mutex;         /* for protection */
    struct {
        BOOL pkts;       /* if TRUE, corrupt packets at random */
        BOOL link;       /* if TRUE, deregister the link at random */
        UINT32 interval; /* min link duration granularity */
    } trash;
    struct {
        void *arg;          /* points to user supplied parameters for func */
        QDP_USER_FUNC func; /* user supplied packet saving function */
    } user;
    struct {
        void *arg;          /* points to user supplied parameters for func */
        QDP_META_FUNC func; /* user supplied meta-data saving function */
    } meta;
} QDP_PAR;

/* Default parameters for both C&C and data links */

#define QDP_DEFAULT_BASEPORT          5330
#define QDP_DEFAULT_DEBUG        QDP_TERSE
#define QDP_DEFAULT_CTRL_TIMEOUT      5000 /*   5 seconds */
#define QDP_DEFAULT_DATA_TIMEOUT      1000 /*   1 second  */
#define QDP_DEFAULT_RETRY_INTERVAL   10000 /*  10 seconds */
#define QDP_DEFAULT_OPEN_INTERVAL   100000 /* 100 second  */
#define QDP_DEFAULT_HBEAT_FORWARD     TRUE
#define QDP_DEFAULT_HBEAT_BITMAP      QDP_STATUS_DEFAULT_BITMAP
#define QDP_DEFAULT_TRASH_PKTS        FALSE
#define QDP_DEFAULT_TRASH_LINK        FALSE
#define QDP_DEFAULT_TRASH_INTERVAL    60000 /* 60 seconds */
#define QDP_DEFAULT_USER_ARG           NULL
#define QDP_DEFAULT_USER_FUNC          NULL
#define QDP_DEFAULT_META_ARG           NULL
#define QDP_DEFAULT_META_FUNC          NULL

#define QDP_DEFAULT_CTRL_BASEPORT      6330
#define QDP_DEFAULT_DATA_BASEPORT      7330

/* Default parameters for C&C links */

#define QDP_DEFAULT_CMD_NBUF        10
#define QDP_DEFAULT_CMD_HBEAT_INTERVAL 0 /* no automatic heartbeat */

/* Default parameters for data links */

#define QDP_DEFAULT_DATA_NBUF      (2 * QDP_MAX_WINDOW_NBUF)
#define QDP_DEFAULT_DATA_HBEAT_INTERVAL 60000 /* 60 seconds */

typedef struct {
    MSGQ_BUF pkt;   /* circular buffer of packets */
    MSGQ_BUF event; /* event queue */
    MSGQ_BUF cmd;   /* outbound command queue */
} QDP_BUF;

typedef struct {
    int code;      /* event identifier */
    MSGQ_MSG *msg; /* if not NULL, a packet from the pkt queue */
    int next;      /* transition state */
} QDP_EVENT;

typedef struct {
    MSGQ_MSG *msg; /* pending command, if not NULL */
    SEMAPHORE sem; /* to signal command processing complete */
    BOOL ok;       /* FALSE if C1_CERR was received, else TRUE */
} QDP_CMD;

/* Digitizer coordinates */

typedef struct {
    char ident[QDP_IDENT_LEN + 1]; /* serial number */
    UINT32 addr; /* ip address in host byte order*/
    struct {
        int ctrl; /* ip port for control */
        int data; /* ip port for data */
    } port;
} QDP_PEER;

/* Statistics */

typedef struct {
    UINT32 overrun; /* number of packets that failed to make it to application layer */
    UINT32 crc;     /* number of packets dropped for CRC errors */
    UINT32 data;    /* number of data packets received */
    UINT32 dupdata; /* number of duplicate data packets received */
    UINT32 fill;    /* number of fill packets received */
    UINT32 drop;    /* number of spurious packets dropped */
    UINT32 retry;   /* number of command retries */
} QDP_COUNTER;

typedef struct {
    MUTEX mutex;    /* for protection */
    UINT64 tstamp;  /* time when last registered */
    UINT32 nreg;    /* number of times we've registered with the digitizer */
    UINT64 uptime;  /* used when processing a snapshot of this structure */
    struct {
        QDP_COUNTER session;  /* counts for this registration instance */
        QDP_COUNTER total;    /* counts for the life of this application */
    } counter;
} QDP_STATS;

/* Reorder buffer */

typedef struct {
    UINT32 ack[4];
    UINT16 ack_seqno;
    UINT16 last_packet;
    MSGQ_MSG *msg[QDP_MAX_WINDOW_NBUF];
    UTIL_TIMER timer;
    UINT16 count;
    UINT64 ack_to;
} QDP_DTBUF;

/* Handle for reliable QDP I/O */

typedef struct {
    UDPIO ctrl;              /* UDP control port */
    UDPIO data;              /* UDP data port */
    QDP_PAR par;             /* connection parameters */
    QDP_PEER peer;           /* digitizer coordinates */
    QDP_BUF Q;               /* packet and event buffer message queues */
    MUTEX mutex;             /* protects state */
    int state;               /* current state */
    int regerr;              /* error code in case of failed registration */
    UINT16 seqno;            /* outbound sequence number */
    LOGIO *lp;               /* for logging */
    UINT64 start;            /* system start time stamp */
    QDP_PKT pkt;             /* holds outgoing registration commands */
    QDP_CMD cmd;             /* holds pending user command */
    QDP_STATS stats;         /* statistics */
    QDP_DTBUF recv;          /* reorder buffer */
    BOOL dataLink;           /* TRUE for data connections */
    QDP_TYPE_C1_COMBO combo; /* decoded configuration information */
    UTIL_TIMER open;         /* DT_OPEN interval timer */
    SEMAPHORE sem;           /* to synchronize startup of FSA threads */
    QDP_STATE_MACHINE *fsa;  /* automaton for this type of link */
    QDP_TYPE_C1_MEM c1_mem;  /* C1_MEM packets decoded here */
    int nthread;             /* number of active FSA threads */
    QDP_META meta;           /* meta-data */
    int errcode;             /* library error codes */
} QDP;

/* macros for simple Q330 commands */

#define qdpReboot(qp, wait)                 qdpCtrl(qp, QDP_CTRL_REBOOT, wait)
#define qdpGPSOn(qp, wait)                  qdpCtrl(qp, QDP_CTRL_GPS_ON, wait)
#define qdpGPSOff(qp, wait)                 qdpCtrl(qp, QDP_CTRL_GPS_OFF, wait)
#define qdpGPSColdStart(qp, wait)           qdpCtrl(qp, QDP_CTRL_GPS_COLDSTART, wait)
#define qdpResync(qp, wait)                 qdpCtrl(qp, QDP_CTRL_GPS_RESYNC, wait)
#define qdpSaveCurrentProgramming(qp, wait) qdpCtrl(qp, QDP_CTRL_SAVE, wait)

/* Checkout packets */

typedef struct {
    char ident[QDP_IDENTLEN+1];
    QDP_TYPE_C1_FIX fix;
    QDP_TYPE_C1_GLOB glob;
    QDP_TYPE_C1_DCP dcp;
    UINT32 sc[QDP_NSC];
    QDP_TYPE_C1_GID gid[QDP_NGID];
    QDP_TYPE_C2_GPS gps;
    QDP_TYPE_C2_AMASS amass;
    QDP_TYPE_C1_STAT status;
    QDP_TYPE_C1_PHY phy;
    QDP_TYPE_C1_SPP spp;
    QDP_TYPE_C1_MAN man;
} QDP_TYPE_CHECKOUT;

/* Logical channel queues are off in its own file */

#include "qdp/lcq.h"

/* function prototypes */

/* clock.c */
UINT8 qdpClockQuality(QDP_TOKEN_CLOCK *dp_clock, QDP_MN232_DATA *mn232);

/* cmds.c */
BOOL qdpPostCmd(QDP *qp, QDP_PKT *pkt, BOOL wait);
BOOL qdpDeregister(QDP *qp, BOOL wait);
BOOL qdpRqflgs(QDP *qp, BOOL wait);
BOOL qdpCtrl(QDP *qp, UINT16 flags, BOOL wait);
BOOL qdpNoParCmd(QDP *qp, int command, BOOL wait);
BOOL qdpRqstat(QDP *qp, UINT32 bitmap, BOOL wait);

/* connect.c */
QDP *qdpConnectWithPar(QDP_PAR *par, LOGIO *lp);
QDP *qdpConnect(char *host, UINT64 serialno, UINT64 authcode, int port, int verbosity, LOGIO *lp);

/* crc.c */
UINT32 qdpCRC(UINT8 *buf, int len);
BOOL qdpVerifyCRC(QDP_PKT *pkt);

/* data.c */
void qdpLoadMN232(QDP_BLOCKETTE_232 *blk, QDP_MN232_DATA *dest);
BOOL qdpDecode_DT_DATA(UINT8 *start, UINT16 dlen, QDP_TYPE_DT_DATA *dest);

/* debug.c */
void qdpDebug(QDP *qp, char *format, ...);
void qdpInfo(QDP *qp, char *format, ...);
void qdpWarn(QDP *qp, char *format, ...);
void qdpError(QDP *qp, char *format, ...);
void qdpLcqDebug(QDP_LCQ *lcq, char *format, ...);
void qdpLcqInfo(QDP_LCQ *lcq, char *format, ...);
void qdpLcqWarn(QDP_LCQ *lcq, char *format, ...);
void qdpLcqError(QDP_LCQ *lcq, char *format, ...);

/* decode.c */
void qdpDecode_C1_SRVCH(UINT8 *start, QDP_TYPE_C1_SRVCH *dest);
void qdpDecode_C1_SRVRSP(UINT8 *start, QDP_TYPE_C1_SRVRSP *dest);
void qdpDecode_C1_RQMEM(UINT8 *start, QDP_TYPE_C1_RQMEM *dest);
void qdpDecode_C1_MEM(UINT8 *start, QDP_TYPE_C1_MEM *dest);
void qdpDecode_C1_FIX(UINT8 *start, QDP_TYPE_C1_FIX *dest);
void qdpDecode_C1_GID(UINT8 *start, QDP_TYPE_C1_GID *dest);
void qdpDecode_C1_SPP(UINT8 *start, QDP_TYPE_C1_SPP *dest);
void qdpDecode_C1_MAN(UINT8 *start, QDP_TYPE_C1_MAN *dest);
void qdpDecode_C1_GLOB(UINT8 *start, QDP_TYPE_C1_GLOB *dest);
void qdpDecode_C1_LOG(UINT8 *start, QDP_TYPE_C1_LOG *dest);
void qdpDecode_C1_COMBO(UINT8 *start, QDP_TYPE_C1_COMBO *dest);
void qdpDecode_C1_PHY(UINT8 *start, QDP_TYPE_C1_PHY *dest);
void qdpDecode_C1_SC(UINT8 *start, UINT32 *dest);
void qdpDecode_C1_DCP(UINT8 *start, QDP_TYPE_C1_DCP *dest);
void qdpDecode_C2_AMASS(UINT8 *start, QDP_TYPE_C2_AMASS *dest);
void qdpDecode_C2_GPS(UINT8 *start, QDP_TYPE_C2_GPS *dest);
int qdpDecodeCMNHDR(UINT8 *start, QDP_CMNHDR *hdr);

/* encode.c */
void qdpEncode_C1_SRVRSP(QDP_PKT *pkt, QDP_TYPE_C1_SRVRSP *src);
void qdpEncode_C1_RQSRV(QDP_PKT *pkt, UINT64 serialno);
void qdpEncode_C1_DSRV(QDP_PKT *pkt, UINT64 serialno);
void qdpEncode_C1_RQFLGS(QDP_PKT *pkt, UINT16 dataport);
void qdpEncode_C1_RQSTAT(QDP_PKT *pkt, UINT32 bitmap);
void qdpEncode_C1_CTRL(QDP_PKT *pkt, UINT16 flags);
void qdpEncode_C1_RQMEM(QDP_PKT *pkt, QDP_TYPE_C1_RQMEM *src);
void qdpEncode_C1_QCAL(QDP_PKT *pkt, QDP_TYPE_C1_QCAL *src);
void qdpEncode_NoParCmd(QDP_PKT *pkt, int command);
void qdpEncode_C1_PHY(QDP_PKT *pkt, QDP_TYPE_C1_PHY *src);
void qdpEncode_C1_PULSE(QDP_PKT *pkt, UINT16 bitmap, UINT16 duration);
void qdpEncode_C1_SC(QDP_PKT *pkt, UINT32 *sc);
void qdpEncode_C1_GLOB(QDP_PKT *pkt, QDP_TYPE_C1_GLOB *glob);
void qdpEncode_C1_SPP(QDP_PKT *pkt, QDP_TYPE_C1_SPP *spp);
void qdpEncode_C2_GPS(QDP_PKT *pkt, QDP_TYPE_C2_GPS *gps);

/* fsa.c */
int qdpState(QDP *qp);
BOOL qdpStartFSA(QDP *qp);

/* hlp.c */
void qdpFlushHLP(QDP_LCQ *lcq, QDP_LC *lc);
BOOL qdpBuildHLP(QDP_LCQ *lcq);
BOOL qdpInitHLPRules(QDP_HLP_RULES *dest, UINT32 maxbyte, UINT8 format, QDP_HLP_FUNC func, void *args, UINT32 flags);
void *qdpDestroyHLP(QDP_HLP *hlp);
BOOL qdpInitHLP(QDP_HLP_RULES *rules, QDP_LC *lc, QDP_META *meta);

/* ida10.c */
BOOL qdpHlpToIDA10(UINT8 *start, QDP_HLP *hlp, UINT32 seqno);

/* init.c */
void qdpInitMemBlock(QDP *qp);
BOOL qdpInit(QDP *qp, QDP_PAR *par, LOGIO *lp);

/* io.c */
THREAD_FUNC TrashLinkThread(void *argptr);
void qdpHostToNet(QDP_PKT *pkt);
void qdpNetToHost(QDP_PKT *pkt);
MSGQ_MSG *qdpRecvPkt(QDP *qp, UDPIO *up, int *status);
void qdpSendPkt(QDP *qp, int which, QDP_PKT *pkt);

/* md5.c */
void qdpMD5(QDP_TYPE_C1_SRVRSP *c1_srvrsp, UINT64 auth);

/* mem.c */
void qdpInitMemBlk(QDP_MEMBLK *blk);
BOOL qdpSaveMem(QDP_MEMBLK *blk, QDP_TYPE_C1_MEM *mem);
void qdpGetMemBlk(QDP *qp, QDP_MEMBLK *mem);

/* meta.c */
int qdpWriteMeta(FILE *fp, QDP_META *meta);
int qdpReadMeta(FILE *fp, QDP_META *meta);
int qdpReadMetaFile(char *path, QDP_META *meta);
INT32 qdpLookupFilterDelay(QDP_META *meta, UINT8 *src);
UINT64 qdpRootTimeTag(QDP_MN232_DATA *mn232);
UINT64 qdpDelayCorrectedTime(QDP_MN232_DATA *mn232, INT32 delay);

/* msgq.c */
MSGQ_MSG *qdpGetEmptyPktMsg(char *fid, QDP *qp, int wait);
void qdpReturnPktMsgToHeap(char *fid, QDP *qp, MSGQ_MSG *msg);
void qdpForwardFullPktMsg(char *fid, QDP *qp, MSGQ_MSG *msg);
MSGQ_MSG *qdpGetFullPktMsg(char *fid, QDP *qp, int wait);
BOOL qdpNextPkt(QDP *qp, QDP_PKT *dest);

/* par.c */
void qdpInitPar(QDP_PAR *par, int port);
BOOL qdpParseArgString(QDP_PAR *par, char *argstr);
BOOL qdpSetHost(QDP_PAR *par, char *host);
BOOL qdpSetBaseport(QDP_PAR *par, int value);
BOOL qdpSetCtrlBaseport(QDP_PAR *par, int value);
BOOL qdpSetMyCtrlPort(QDP_PAR *par, int value);
BOOL qdpSetDataBaseport(QDP_PAR *par, int value);
BOOL qdpSetMyDataPort(QDP_PAR *par, int value);
BOOL qdpSetAuthcode(QDP_PAR *par, UINT64 value);
BOOL qdpSetSerialno(QDP_PAR *par, UINT64 value);
BOOL qdpSetCtrlTimeout(QDP_PAR *par, UINT32 value);
BOOL qdpSetDataTimeout(QDP_PAR *par, UINT32 value);
BOOL qdpSetRetryInterval(QDP_PAR *par, UINT32 value);
BOOL qdpSetOpenInterval(QDP_PAR *par, UINT32 value);
BOOL qdpSetHeartbeat(QDP_PAR *par, UINT32 interval, UINT32 bitmap, BOOL forward);
BOOL qdpSetDebug(QDP_PAR *par, int value);
BOOL qdpSetUser(QDP_PAR *par, void *arg, QDP_USER_FUNC func);
BOOL qdpSetMeta(QDP_PAR *par, void *arg, QDP_META_FUNC func);
BOOL qdpSetTrash(QDP_PAR *par, BOOL pkts, BOOL link);
void qdpLogPar(QDP *qp);
void qdpPrintPar(FILE *fp, QDP *qp);

/* pkts.c */
int qdpErrorCode(QDP_PKT *pkt);
void qdpInitPkt(QDP_PKT *pkt, UINT8 cmd, UINT16 seqno, UINT16 ack);
void qdpCopyPkt(QDP_PKT *dest, QDP_PKT *src);
int qdpReadPkt(FILE *fp, QDP_PKT *pkt);
int qdpWritePkt(FILE *fp, QDP_PKT *pkt);
BOOL qdp_C1_RQGLOB(QDP *qdp, QDP_TYPE_C1_GLOB *glob);
BOOL qdp_C1_RQSC(QDP *qdp, UINT32 *sc);
BOOL qdp_C1_RQPHY(QDP *qdp, QDP_TYPE_C1_PHY *phy);
BOOL qdp_C1_RQFIX(QDP *qdp, QDP_TYPE_C1_FIX *fix);
BOOL qdp_C1_RQGID(QDP *qdp, QDP_TYPE_C1_GID *gid);
BOOL qdp_C1_RQDCP(QDP *qdp, QDP_TYPE_C1_DCP *dcp);
BOOL qdp_C1_RQSPP(QDP *qdp, QDP_TYPE_C1_SPP *spp);
BOOL qdp_C1_RQMAN(QDP *qdp, QDP_TYPE_C1_MAN *man);
BOOL qdp_C2_RQAMASS(QDP *qdp, QDP_TYPE_C2_AMASS *amass);
BOOL qdp_C2_RQGPS(QDP *qdp, QDP_TYPE_C2_GPS *gps);
BOOL qdp_C1_RQSTAT(QDP *qp, UINT32 bitmap, QDP_TYPE_C1_STAT *stat);
BOOL qdpRequestCheckout(QDP *qdp, QDP_TYPE_CHECKOUT *co, char *ident);

/* print.c */
void qdpPrintBT_SPEC(FILE *fp, QDP_BLOCKETTE_SPEC *blk);
void qdpPrintBT_38(FILE *fp, QDP_BLOCKETTE_38 *blk);
void qdpPrintBT_816(FILE *fp, QDP_BLOCKETTE_816 *blk);
void qdpPrintBT_316(FILE *fp, QDP_BLOCKETTE_316 *blk);
void qdpPrintBT_32(FILE *fp, QDP_BLOCKETTE_32 *blk);
void qdpPrintBT_232(FILE *fp, QDP_BLOCKETTE_232 *blk);
void qdpPrintBT_COMP(FILE *fp, QDP_BLOCKETTE_COMP *blk);
void qdpPrintBT_MULT(FILE *fp, QDP_BLOCKETTE_MULT *blk);
void qdpPrintBlockette(FILE *fp, QDP_DT_BLOCKETTE *blk, UINT32 seqno);
void qdpPrintTokenVersion(FILE *fp, QDP_TOKEN_VERSION *src);
void qdpPrintTokenSite(FILE *fp, QDP_TOKEN_SITE *src);
void qdpPrintTokenServer(FILE *fp, QDP_TOKEN_SERVER *src);
void qdpPrintTokenServices(FILE *fp, QDP_TOKEN_SERVICES *src);
void qdpPrintTokenDss(FILE *fp, QDP_TOKEN_DSS *src);
void qdpPrintTokenClock(FILE *fp, QDP_TOKEN_CLOCK *src);
void qdpPrintTokenLogid(FILE *fp, QDP_TOKEN_LOGID *src);
void qdpPrintTokenCnfid(FILE *fp, QDP_TOKEN_CNFID *src);
void qdpPrintTokenLcq(FILE *fp, QDP_TOKEN_LCQ *src);
void qdpPrintTokenLcqList(FILE *fp, LNKLST *src);
void qdpPrintTokenIir(FILE *fp, QDP_TOKEN_IIR *iir);
void qdpPrintTokenIirList(FILE *fp, LNKLST *src);
void qdpPrintTokenFir(FILE *fp, QDP_TOKEN_FIR *fir);
void qdpPrintTokenFirList(FILE *fp, LNKLST *src);
void qdpPrintTokenCds(FILE *fp, QDP_TOKEN_CDS *cds);
void qdpPrintTokenCdsList(FILE *fp, LNKLST *src);
void qdpPrintTokenMhd(FILE *fp, QDP_TOKEN_MHD *mhd);
void qdpPrintTokenMhdList(FILE *fp, LNKLST *src);
void qdpPrintTokenTds(FILE *fp, QDP_TOKEN_TDS *tds);
void qdpPrintTokenTdsList(FILE *fp, LNKLST *src);
void qdpPrintTokens(FILE *fp, QDP_DP_TOKEN *src);
void qdpPrint_C1_FIX(FILE *fp, QDP_TYPE_C1_FIX *fix);
void qdpPrint_C1_GID(FILE *fp, QDP_TYPE_C1_GID *gid);
void qdpPrint_C1_GLOB(FILE *fp, QDP_TYPE_C1_GLOB *glob);
void qdpPrint_C1_LOG(FILE *fp, QDP_TYPE_C1_LOG *log);
void qdpPrint_C1_COMBO(FILE *fp, QDP_TYPE_C1_COMBO *combo);
void qdpPrint_C1_PHY(FILE *fp, QDP_TYPE_C1_PHY *phy);
void qdpPrint_C1_SC(FILE *fp, UINT32 *sc);
void qdpPrint_C1_DCP(FILE *fp, QDP_TYPE_C1_DCP *dcp);
void qdpPrint_C1_DCPMAN(FILE *fp, QDP_TYPE_C1_DCP *dcp, QDP_TYPE_C1_MAN *man);
void qdpPrint_C1_SPP(FILE *fp, QDP_TYPE_C1_SPP *spp);
void qdpPrint_C1_MAN(FILE *fp, QDP_TYPE_C1_MAN *man);
void qdpPrint_C2_AMASS_SENSOR(FILE *fp, QDP_TYPE_C2_AMASS_SENSOR *sensor);
void qdpPrint_C2_AMASS(FILE *fp, QDP_TYPE_C2_AMASS *amass);
void qdpPrint_C2_GPS(FILE *fp, QDP_TYPE_C2_GPS *gps);
void qdpPrintCheckoutPackets(FILE *fp, QDP_TYPE_CHECKOUT *co);
void qdpPrint_CMNHDR(FILE *fp, QDP_CMNHDR *hdr);
void qdpPrintPkt(FILE *fp, QDP_PKT *pkt, UINT16 print);
void qdpPrintLCQ(FILE *fp, QDP_LCQ *lcq, UINT16 print);

/* process.c */
BOOL qdpProcessPacket(QDP_LCQ *lcq, QDP_PKT *pkt);
BOOL qdpInitLCQ(QDP_LCQ *lcq, QDP_LCQ_PAR *par);
void qdpDestroyLCQ(QDP_LCQ *lcq);

/* stats.c */
void qdpInitStats(QDP *qp);
void qdpSetStatsTstamp(QDP *qp);
void qdpResetSessionStats(QDP *qp);
void qdpIncrStatsNreg(QDP *qp);
void qdpIncrStatsOverrun(QDP *qp);
void qdpIncrStatsCrc(QDP *qp);
void qdpIncrStatsData(QDP *qp);
void qdpIncrStatsDupData(QDP *qp);
void qdpIncrStatsFill(QDP *qp);
void qdpIncrStatsRetry(QDP *qp);
void qdpGetStats(QDP *qp, QDP_STATS *dest);
UINT64 qdpSessionUptime(QDP *qp);
void qdpLogStats(QDP *qp);
void qdpPrintStats(FILE *fp, QDP *qp);

/* status.c */
BOOL qdpClear_C1_STAT(QDP_TYPE_C1_STAT *dest);
BOOL qdpInit_C1_STAT(QDP_TYPE_C1_STAT *dest);
void qdpPrint_C1_STAT(FILE *fp, QDP_TYPE_C1_STAT *stat);
BOOL qdpDecode_C1_STAT(UINT8 *start, QDP_TYPE_C1_STAT *dest);

/* steim.c */
int qdpDecompressCOMP(QDP_BLOCKETTE_COMP *blk, INT32 *out, UINT32 maxsamp, UINT32 *nsamp);

/* string.c */
char *qdpCmdString(UINT8 code);
char *qdpErrString(UINT16 code);
char *qdpCtrlString(UINT8 code);
char *qdpPortString(int code);
char *qdpDebugString(UINT8 code);
char *qdpMemTypeString(UINT8 code);
char *qdpBlocketteIdentString(UINT8 code);
char *qdpPLLStateString(UINT16 code);
char *qdpPhysicalPortString(UINT16 code);
char *qdpSensorControlString(UINT32 code);
char *qdpAuxtypeString(UINT16 code);
char *qdpClocktypeString(UINT16 code);
char *qdpCalibratorString(UINT16 code);

/* tokens.c */
BOOL qdpUnpackTokens(QDP_MEMBLK *blk, QDP_DP_TOKEN *token);
void qdpInitTokens(QDP_DP_TOKEN *token);
void qdpClearTokens(QDP_DP_TOKEN *token);
void qdpDestroyTokens(QDP_DP_TOKEN *token);

/* version.c */
char *qdpVersionString(void);
VERSION *qdpVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* qdp_h_included */

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
 * $Log: qdp.h,v $
 * Revision 1.33  2009/03/17 17:17:48  dechavez
 * added extern char *libqdpBuildIdent;
 *
 * Revision 1.32  2009/02/23 21:48:18  dechavez
 * added new prototypes
 *
 * Revision 1.31  2009/02/04 05:42:59  dechavez
 * added QDP_TYPE_C1_DCP, QDP_TYPE_C1_SPP, QDP_TYPE_C1_MAN, QDP_TYPE_C2_GPS,
 * QDP_TYPE_CHECKOUT and associated function prototypes, added errcode field
 * to QDP handle
 *
 * Revision 1.30  2009/01/23 23:54:14  dechavez
 * added QDP_TYPE_C2_AMASS typedef, updated prototypes
 *
 * Revision 1.29  2009/01/06 20:44:25  dechavez
 * updated prototypes
 *
 * Revision 1.28  2008/10/09 20:38:09  dechavez
 * Added constants for the C1_STAT bitmap, added QDP_TYPE_C1_PHY typedef,
 * updated prototypes
 *
 * Revision 1.27  2008/03/11 20:49:46  dechavez
 * removed QDP_QCAL_MAX_DIVISOR (found now in q330.h)
 *
 * Revision 1.26  2007/12/20 22:46:04  dechavez
 * moved various limits to new qdp/limits.h, added support for C1_STAT, defined physical ports
 *
 * Revision 1.25  2007/10/31 17:20:06  dechavez
 * added C1_QCAL support
 *
 * Revision 1.24  2007/05/17 22:27:41  dechavez
 * initial production release
 *
 */
