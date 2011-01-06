#pragma ident "$Id: qdplus.h,v 1.13 2009/02/23 21:48:19 dechavez Exp $"
/*======================================================================
 *
 *  QDP packet extensions
 *
 *====================================================================*/
#ifndef qdplus_h_included
#define qdplus_h_included

#include "platform.h"
#include "qdp.h"
#include "isi.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  Wrapped QDP packet
 *
 *  Offset Type    Length      Description
 *    0    UINT64     8        Digitizer serial number
 *    8    UINT32     4        ISI disk loop sequence number signature
 *   12    UINT64     8        ISI disk loop sequence number counter
 *   20    UINT8   12 to 576   Raw QDP packet
 */

typedef struct {
    UINT64 serialno;        /* digitizer serial number */
    ISI_SEQNO seqno;        /* packet sequence number */
    QDP_PKT qdp;            /* the packet, decoded */
} QDPLUS_PKT;

#define QDPLUS_HDRLEN 20
#define QDPLUS_PKTLEN (QDPLUS_HDRLEN + QDP_MAX_MTU)

#define QDPLUS_SERIALNO_STRING_LEN 16

/* Identifier tags for DT_USER packets */

#define QDPLUS_DT_USER_AUX 1
/*
    Offset Type    Length       Description
      0    UINT8         1      type identifier (QDPLUS_DT_USER_AUX)
      1    UINT8         1      reserved
      2    UINT8         1      datum type (following ISI_TYPE_x codes)
      3    CHAR          3      channel name
      6    CHAR          2      location code
      8    UINT16        2      sample interval, msec
     10    UINT32        4      beg system time of first sample
     14    UINT32        4      beg system time of external pps
     18    UINT32        4      beg GPS time of external pps
     22    CHAR          1      beg time quality code
     23    UINT8         1      reserved
     24    UINT32        4      end system time of first sample
     28    UINT32        4      end system time of external pps
     32    UINT32        4      end GPS time of external pps
     36    CHAR          1      end time quality code
     37    UINT8         1      reserved
     38    UINT16        2      number of samples
     40    var         496      up to 496 bytes of data
*/
#define QDPLUS_DT_USER_AUX_MAX_PAYLOAD 496

/* An decoded DT_USER payload when type is QDPLUS_DT_USER_AUX */

typedef struct {
    char chn[ISI_CHNLEN+1]; /* channel name */
    char loc[ISI_LOCLEN+1]; /* location code */
    UINT16 sint;            /* sample interval, msec */
    UINT16 nsamp;           /* number of samples */
    int type;               /* datum descriptor */
    struct {
        UINT32 sys;
        UINT32 pps;
        UINT32 ext;
        char   code;
    } tofs, tols;           /* time of first/last samples */
    UINT8 *data;            /* points to up to 512 bytes of data */
} QDPLUS_AUXPKT;

/* QDP metadata for a single instrument */

typedef struct {
    char path[MAXPATHLEN+1]; /* path name of metadata file */
    ISI_SEQNO_RANGE range;   /* range of sequence numbers for which data valid */
    QDP_META data;           /* the metadata */
} QDPLUS_META_DATUM;

typedef struct {
    UINT64 serialno;  /* digitizer serial number */
    LNKLST *list;     /* linked list of QDPLUS_META_DATUM structures */
} QDPLUS_META_DATA;

/* Used for recovering state */

typedef struct {
    ISI_SEQNO seqno;
    char chn[QDP_CNAME_LEN+1];
    char loc[QDP_LNAME_LEN+1];
    INT32 offset;
} QDPLUS_STATE;

#define QDPLUS_FLAG_CLEAN 0xabcd
#define QDPLUS_FLAG_DIRTY 0xcdef

#define QDPLUS_STATE_OK    0
#define QDPLUS_STATE_BUG   1
#define QDPLUS_STATE_NOMEM 2
#define QDPLUS_STATE_IOERR 3
#define QDPLUS_STATE_EMPTY 4
#define QDPLUS_STATE_DIRTY 5

/* Handle for processing a packet stream */

typedef struct {
    UINT64 serialno;  /* digitizer serial number */
    QDP_LCQ lcq;      /* logical channel queue for this digitizer */
    LNKLST *state;    /* linked list of QDPLUS_STATE structures */
} QDPLUS_DIGITIZER;

typedef struct {
    struct {
        char *meta;      /* metadata directory */
        char *state;     /* hlp state file */
    } path;
    char *site;      /* site name */
    QDP_LCQ_PAR lcq; /* handle parameters */
} QDPLUS_PAR;

#define QDPLUS_DEFAULT_PAR {NULL, NULL, NULL, QDP_DEFAULT_LCQ_PAR}

typedef struct {
    struct {
        LNKLST *digitizer; /* linked list of QDPLUS_DIGITIZER structures */
        LNKLST *meta;      /* linked list of QDPLUS_META_DATA structures */
    } list;
    QDPLUS_PAR par;   /* user parameters */
    int state;        /* status of state file */
} QDPLUS;

/* decode.c */
int qdplusUnpackWrappedQDP(UINT8 *start, QDPLUS_PKT *dest);
void qdplusDecodeUserAux(UINT8 *start, QDPLUS_AUXPKT *dest);

/* digitizer.c */
void qdplusDestroyDigitizer(QDPLUS_DIGITIZER *digitizer);
void qdplusDestroyDigitizerList(LNKLST *list);
QDPLUS_DIGITIZER *qdplusGetDigitizer(QDPLUS *handle, UINT64 serialno);
QDPLUS_DIGITIZER *qdplusLoadPacket(QDPLUS *handle, QDPLUS_PKT *pkt);

/* handle.c */
void qdplusDestroyHandle(QDPLUS *handle);
QDPLUS *qdplusCreateHandle(QDPLUS_PAR *par);
void qdplusSetLcqStateFlag(QDPLUS *handle, int value);

/* io.c */
BOOL qdplusRead(FILE *fp, QDPLUS_PKT *pkt);
BOOL qdplusWrite(FILE *fp, QDPLUS_PKT *pkt);

/* meta.c */
void qdplusDestroyMetaData(QDPLUS_META_DATA *meta);
void qdplusDestroyMetaDataList(LNKLST *list);
LNKLST *qdplusReadMetaData(char *base);
void qdplusPrintMetaDatum(FILE *fp, QDPLUS_META_DATUM *datum);
void qdplusPrintMetaData(FILE *fp, QDPLUS_META_DATA *meta);
void qdplusPrintMetaDataList(FILE *fp, LNKLST *list, BOOL verbose);
BOOL qdplusInitializeLCQMetaData(QDP_LCQ *lcq, LNKLST *head, UINT64 serialno, ISI_SEQNO *seqno);

/* print.c */
void qdplusPrintPkt(FILE *fp, QDPLUS_PKT *pkt, UINT16 print);

/* process.c */
QDP_LCQ *qdplusProcessPacket(QDPLUS *handle, QDPLUS_PKT *pkt);

/* state.c */
void qdplusPrintState(FILE *fp, QDPLUS *handle);
BOOL qdplusSaveState(QDPLUS *handle);
int  qdplusReadStateFile(QDPLUS *handle);
BOOL qdplusStateSeqnoLimits(QDPLUS *handle, ISI_SEQNO *beg, ISI_SEQNO *end);

/* version.c */
char *qdplusVersionString(void);
VERSION *qdplusVersion(void);

#ifdef __cplusplus
}
#endif

#endif /* qdplus_h_included */

/* Revision History
 *
 * $Log: qdplus.h,v $
 * Revision 1.13  2009/02/23 21:48:19  dechavez
 * added new prototypes
 *
 * Revision 1.12  2008/12/15 23:23:47  dechavez
 * added QDPLUS_STATE values
 *
 * Revision 1.11  2007/05/17 22:25:39  dechavez
 * initial production release
 *
 */
