#pragma ident "$Id: lcq.h,v 1.11 2007/12/20 22:43:56 dechavez Exp $"
/*======================================================================
 *
 *  DT_DATA processing (logical channel queues)
 *
 *====================================================================*/
#ifndef qdp_lcq_h_included
#define qdp_lcq_h_included

#include "platform.h"
#include "isi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DT_DATA */

#define QDP_DCM_STATUS      0x80   /* Mask to identify status blockettes */
#define QDP_DCM_ST          0xe0   /* Mask for status blockettes */
#define QDP_DCM             0xf8   /* Mask for all others */
#define QDP_DC_ST38         0x00   /* Status - 3 x 8 bit parameters */
#define QDP_DC_ST816        0x20   /* Status - 8 and 16 */
#define QDP_DC_ST32         0x40   /* Status - 8, 16, and 32 */
#define QDP_DC_ST232        0x60   /* Status - 8, 16, and 2 X 32 */
#define QDP_DC_MN38         0x80   /* Main data sample - 3 x 8 */
#define QDP_DC_MN816        0x88   /* Main - 8 and 16 */
#define QDP_DC_MN32         0x90   /* Main - 8, 16, and 32 */
#define QDP_DC_MN232        0x98   /* Main - 8, 16, and 2 X 32 */
#define QDP_DC_AG38         0xa0   /* Analog sample - 3 x 8 */
#define QDP_DC_AG816        0xa8   /* Analog sample - 8 and 16 */
#define QDP_DC_AG32         0xb0   /* Analog sample - 8, 16, and 32 */
#define QDP_DC_AG232        0xb8   /* Analog sample - 8, 16, and 2 x 32 */
#define QDP_DC_CNP38        0xc0   /* CNP - 3 x 8 */
#define QDP_DC_CNP816       0xc8   /* CNP - 8 and 16 */
#define QDP_DC_CNP316       0xd0   /* CNP - 3 x 16 */
#define QDP_DC_CNP232       0xd8   /* CNP - 8, 16, and 2 x 32 */
#define QDP_DC_D32          0xe0   /* Digitizer - 8, 16, and 32, used for 1Hz */
#define QDP_DC_COMP         0xe8   /* Digitizer - compression map and multiple samples */
#define QDP_DC_MULT         0xf0   /* Digitizer - continuation of above */
#define QDP_DC_SPEC         0xf8   /* Special purpose packets */
#define QDP_DC_ROOT         0xe0   /* inidicates "digitizer" data */
/* DC_MULT flags */
#define QDP_DMLS            0x8000L /* Last segment */
#define QDP_DMSZ            0x3ff   /* blockette size mask */
#define QDP_DMFBIT          0x7     /* frequency bit number mask */
/* Manually Blocked Blockettes */
#define QDP_MBB_GPS         1       /* Gps Power cycling */
#define QDP_MBB_DIG         2       /* Digitizer Phase Changes */
#define QDP_MBB_CHRG        4       /* Power Supply charging phase */
#define QDP_MBB_CAL         8       /* Calibration */
#define QDP_MBB_CFG         0x10    /* Configuration changes */
/* Automatic Blocked Blockettes */
#define QDP_ABB_MN38        1       /* DC_MN38 blockettes */
#define QDP_ABB_MN816       2       /* DC_MN816 blockettes */
#define QDP_ABB_BOOM        4       /* Boom positions */
#define QDP_ABB_STEMP       8       /* Seismo Temperatures */
#define QDP_ABB_AVOLT       0x10    /* Analog Voltage */
#define QDP_ABB_PWR         0x20    /* Power supply parameters */
#define QDP_ABB_CBLK        0x40    /* CNP block data */
/* Digitizer Phase Change Constants, first parameter */
#define QDP_DPC_STARTUP     0
#define QDP_DPC_WAIT        1
#define QDP_DPC_INT         2
#define QDP_DPC_EXT         3
/* Digitizer Phase Change Constants, second parameter */
#define QDP_DPR_NORM        0
#define QDP_DPR_DRIFT       1
#define QDP_DPR_GOTCLK      2
#define QDP_DPR_CMD         3
/* Recording Window status, first parameter */
#define QDP_RWR_START       0
#define QDP_RWR_STOP        1
/* Special Purpose Blockettes */
#define QDP_SP_CALSTART     0   /* calibration start */
#define QDP_SP_CALABORT     1   /* calibration abort */
#define QDP_SP_CNPBLK       2   /* CNP Block data */
#define QDP_SP_CFGBLK       3   /* Configuration Blockette */

/* The 8 possible DT_DATA blockete formats */

#define QDP_BT_UNKNOWN 0 /* unknown blockette format */
#define QDP_BT_38      1 /* 3 8-bit values */
#define QDP_BT_816     2 /* 1 8-bit and 1 16-bit value */
#define QDP_BT_316     3 /* 1 8-bit and 3 16-bit values */
#define QDP_BT_32      4 /* 1 8-bit, 1 16-bit, and 1 32-bit values */
#define QDP_BT_232     5 /* 1 8-bit, 1 16-bit, and 2 32-bit values */
#define QDP_BT_COMP    6 /* compressed data */
#define QDP_BT_MULT    7 /* multi-packet compressed data */
#define QDP_BT_SPEC    8 /* special data */

typedef struct {
    UINT8 *data;  /* pointer to compressed data */
    UINT32 *seq;  /* compressed data as 32-bit sequences */
    UINT32 nseq;  /* number of elements in the seq array */
} QDP_COMP_DATA;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    UINT32 sec;   /* seconds offset */
    UINT32 usec;  /* usec offset */
    UINT8  qual;  /* clock quality byte */
    UINT16 loss;  /* minutes since GPS lock was lost */
} QDP_MN232_DATA;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    INT8 bit8[3];
} QDP_BLOCKETTE_38;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    INT8 bit8;
    INT16 bit16;
} QDP_BLOCKETTE_816;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    INT8 bit8;
    INT16 bit16[3];
} QDP_BLOCKETTE_316;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    INT8 bit8;
    INT16 bit16;
    INT32 bit32;
} QDP_BLOCKETTE_32;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    INT8 bit8;
    INT16 bit16;
    INT32 bit32[2];
} QDP_BLOCKETTE_232;

typedef struct {
    UINT32 seqno;       /* data record sequence number, imported from DT_DATA header */
    UINT8 bit8;         /* frequency byte */
    UINT16 bit16;       /* blockette size, with room for something else later */
    INT32 prev;         /* uncompressed previous sample */
    UINT16 doff;        /* offset to start of compressed data */
    UINT8 *map;         /* variable length Steim 2 compression map */
    int maplen;         /* number of bytes in map (not all neccesarily valid) */
    int filt;           /* frequency bit number "fbitno", bottom 3 bits of bit 8 */
    int chan;           /* input channel number (resid of blockette "channel") */
    int size;           /* total length of blockette, bottom 10 bits of bit16 */
    QDP_COMP_DATA comp; /* compressed data */
} QDP_BLOCKETTE_COMP;

typedef struct {
    UINT32 seqno;       /* data record sequence number, imported from DT_DATA header */
    UINT8 bit8;         /* segment/frequency byte */
    UINT16 bit16;       /* blockette size and last segment flag */
    int filt;           /* frequency bit number "fbitno", bottom 3 bits of bit 8 */
    int chan;           /* input channel number (resid of blockette "channel") */
    UINT8 segno;        /* segment number, top 5 bits of bit 8 */
    UINT8 size;         /* bottom 10 bits of bit16 */
    BOOL  lastseg;      /* top bit of bit16 */
    QDP_COMP_DATA comp; /* compressed data */
    /* The following are filled in only when segment number is 0 */
    INT32 prev;         /* uncompressed previous sample */
    UINT16 doff;        /* offset to start of compressed data */
    UINT8 *map;         /* variable length Steim 2 compression map */
    int maplen;         /* number of bytes in map (not all neccesarily valid) */
} QDP_BLOCKETTE_MULT;

typedef struct {
    UINT32 seqno; /* data record sequence number, imported from DT_DATA header */
    UINT8 *VariableLengthData; /* "special" blockettes */
} QDP_BLOCKETTE_SPEC;

typedef struct {
    UINT32 seqno;   /* data record sequence number, imported from DT_DATA header */
    UINT8 channel;  /* raw 8-bit "channel" field preceeding each blockette */
    UINT8 root;     /* "root" channel for LCQ lookups */
    UINT8 ident;    /* blockette identifier, extracted from channel */
    UINT8 resid;    /* the remaining 5 or 3 bits from channel, after ident is removed */
    BOOL is_status; /* true if ident is for a status blockette */
    int format;     /* blockette format */
    union {
        QDP_BLOCKETTE_38 bt_38;
        QDP_BLOCKETTE_816 bt_816;
        QDP_BLOCKETTE_316 bt_316;
        QDP_BLOCKETTE_32 bt_32;
        QDP_BLOCKETTE_232 bt_232;
        QDP_BLOCKETTE_COMP bt_comp;
        QDP_BLOCKETTE_MULT bt_mult;
        QDP_BLOCKETTE_SPEC bt_spec;
    } data;
} QDP_DT_BLOCKETTE;

typedef struct {
    UINT32 seqno;
    LNKLST blist; /* linked list of one or more QDP_TYPE_DT_DATA_BLOCKETTEs */
#define QDP_DT_DATA_SIG_LEN 15
    char signature[QDP_DT_DATA_SIG_LEN+1]; /* typing monkeys validity flag */
    int used; /* number of bytes decoded... better be same as packet payload! */
} QDP_TYPE_DT_DATA;

/* Used for accumulating samples into block multiplexed (high level) packets */

#define QDP_HLP_FORMAT_INVALID  0 /* uncompressed INT32 */
#define QDP_HLP_FORMAT_NOCOMP32 1 /* uncompressed INT32 */
#define QDP_HLP_FORMAT_STEIM1   2 /* Steim 1 */
#define QDP_HLP_FORMAT_STEIM2   3 /* Steim 2 */

#define QDP_HLP_IDENT_LEN (16+1+QDP_CNAME_LEN+1+QDP_LNAME_LEN)

typedef struct {
    UINT8 format;         /* QDP_HLP_FORMAT_x data format */
    UINT8 *data;          /* the accumulated data */
    UINT16 nbyte;         /* the number of used bytes in data array */
    UINT16 nsamp;         /* the equivalent number of samples */
    QDP_MN232_DATA mn232; /* channel 98 start of packet data for first sample in data */
    UINT8 qual;           /* clock quality, percent */
    INT32 delay;          /* filter delay for this stream (if any) */
    INT32 offset;         /* QDP packet index of first hlp datum */
    REAL64 tofs;          /* fully corrected time of first sample, 1970 epoch */
    UINT64 tols;          /* nanosecond time of last sample, QDP epoch */
    UINT64 nsint;         /* sample interval, nanoseconds */
    char chn[QDP_CNAME_LEN+1]; /* channel name */
    char loc[QDP_LNAME_LEN+1]; /* location code */
    QDP_META *meta;       /* metadata block */
    char ident[QDP_HLP_IDENT_LEN+1]; /* for logging convenience */
    ISI_SEQNO seqno;      /* disk loop sequence number of QDP used to start this HLP */
} QDP_HLP;

typedef void (*QDP_HLP_FUNC)(void *arg, QDP_HLP *hlp);

typedef struct {
    BOOL reqtoken;     /* if TRUE, ignore channels with no LCQ token entries */
    UINT32 maxbyte;    /* the maximum number of bytes than can fit in data array */
    UINT8 format;      /* desired QDP_HLP_FORMAT_X data format */
    QDP_HLP_FUNC func; /* user supplied function for processing full blocks */
    void *args;        /* pointer to optional user supplied argument for func */
    BOOL valid;        /* TRUE when all the above fields are defined */
#define QDP_HLP_RULE_FLAG_LCASE  0x00000001 /* force chn/loc to lower case */
#define QDP_HLP_RULE_FLAG_UCASE  0x00000002 /* force chn/loc to upper case */
#define QDP_HLP_RULE_FLAG_STRICT 0x00000004 /* require LCQ token entries for all streams */
    UINT32 flags;      /* option flags */
} QDP_HLP_RULES;

#define QDP_DEFAULT_HLP_RULE_FLAG 0
#define QDP_DEFAULT_HLP_RULES {TRUE, 0, 0, NULL, NULL, FALSE, QDP_DEFAULT_HLP_RULE_FLAG}

/* Logical channel queue for processing DT_DATA blockettes */

#define QDP_LC_STATE_EMPTY      0
#define QDP_LC_STATE_FULL       1
#define QDP_LC_STATE_ACTIVE     2

typedef struct {
    UINT8 src[QDP_LCQSRC_LEN]; /* source parameters for LCQ token lookup */
    char chn[QDP_CNAME_LEN+1]; /* channel name (from token, if avail, else src[0]) */
    char loc[QDP_LNAME_LEN+1]; /* location code (from token, if avail, else src[1]) */
    QDP_TOKEN_LCQ *token;      /* points to static token data for this stream */
    INT32 delay;               /* filter delay for this stream (if any) */
    UINT64 nsint;              /* sample interval, nanoseconds */
    INT32 *data;               /* allocated to hold up to 1 second of data */
    UINT32 maxsamp;            /* number of samples allocated in data array */
    UINT32 nsamp;              /* number of samples present */
    int state;                 /* QDP_LC_STATE_x flag */
    QDP_HLP *hlp;              /* if non-NULL, used for creating high level packets */
    // PUT SOMETHING HERE FOR DT_MULT BUFFERING (also in process.c) !!!!
} QDP_LC;

#define QDP_LCQ_IGNORE       0
#define QDP_LCQ_SAVE_MEMBLK  1
#define QDP_LCQ_LOAD_TOKEN   2
#define QDP_LCQ_LOAD_COMBO   3
#define QDP_LCQ_LOAD_DT_DATA 4
#define QDP_LCQ_IS_DT_USER   5
#define QDP_LCQ_LOAD_C1_STAT 6

typedef struct {
    UINT16 bitmap; /* 2-bit per channel status bitmap */
    UINT8 abort;   /* non-zero to indicate calibration abort has occured */
} QDP_LCQ_CALIB;

typedef struct {
    QDP_LCQ_CALIB calib; /* AG816 calibration status bitmap */
    UINT16 pktbufpct;    /* percentage full of packet buffer */
    UINT32 seqno;        /* sequence number of most recent DT_DATA that modified this structure */
    BOOL empty;          /* TRUE until the first time any field has been updated */
} QDP_LCQ_STATUS;

typedef struct {
    LOGIO *lp;           /* for library event logging */
    int debug;           /* log verbosity */
    QDP_HLP_RULES rules; /* HLP processing rules */
} QDP_LCQ_PAR;

#define QDP_DEFAULT_LCQ_PAR {NULL, QDP_TERSE, QDP_DEFAULT_HLP_RULES}

#define QDP_LCQ_STATE_INITIALIZE 1
#define QDP_LCQ_STATE_PRODUCTION 2

typedef struct {
    UINT8 flag;      /* HLP state (QDP_LCQ_STATE_x) */
    ISI_SEQNO seqno; /* ISI disk loop sequence number of current packet */
} QDP_LCQ_STATE;

typedef struct {
    QDP_PKT *pkt;             /* points to current packet */
    UINT32 seqno;             /* DT_DATA sequence number for these data */
    QDP_MN232_DATA mn232;     /* channel 98 start of packet data */
    QDP_META meta;            /* metadata */
    UINT8 qual;               /* clock quality, percent */
    QDP_LCQ_STATE state;      /* HLP state */
    int action;               /* action taken by qdpProcessPacket() */
    int event;                /* qdpProcessPacket events (QDP_LCQ_EVENT_x bits below) */
    QDP_TYPE_DT_DATA dt_data; /* decoded DT_DATA packet */
    LNKLST *lc;               /* linked list of QDP_LC logical data channels */
    QDP_LCQ_STATUS status;    /* status stuff */
    BOOL fresh;               /* TRUE until we've been used once */
    QDP_LCQ_PAR par;          /* user supplied parameters */
    QDP_TYPE_C1_STAT c1_stat; /* decoded C1_STAT packet */
} QDP_LCQ;

#define QDP_LCQ_EVENT_OK           0x0000
#define QDP_LCQ_EVENT_NO_META      0x0001 /* insufficient meta data to process */
#define QDP_LCQ_EVENT_UNSUPPORTED  0x0002 /* unsupported DT_DATA blockette */
#define QDP_LCQ_EVENT_UNDOCUMENTED 0x0004 /* undocumented DT_DATA blockette */
#define QDP_LCQ_EVENT_NO_TOKENS    0x0008 /* no token data for src[0], src[1] combination */
#define QDP_LCQ_EVENT_DECOMP_ERR   0x0010 /* decompression error */
#define QDP_LCQ_EVENT_SINT_CHANGE  0x0020 /* sample interval changed */
#define QDP_LCQ_EVENT_FATAL        0x1000 /* something foobar, better quit */

#ifdef __cplusplus
}
#endif

#endif /* qdp_lcq_h_included */

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
 * $Log: lcq.h,v $
 * Revision 1.11  2007/12/20 22:43:56  dechavez
 * added c1_stat to LCQ
 *
 * Revision 1.10  2007/12/14 21:13:47  dechavez
 * changed blockette datum types from unsigned to signed
 *
 * Revision 1.9  2007/09/14 19:21:13  dechavez
 * made length of QDP_HLP ident field a simple constant (QDP_HLP_IDENT_LEN) for ease of reuse
 *
 * Revision 1.8  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
