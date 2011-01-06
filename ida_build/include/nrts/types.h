#pragma ident "$Id: types.h,v 1.3 2007/01/05 00:26:43 dechavez Exp $"
/*======================================================================
 *
 *  NRTS types common to both old NRTS and ISI
 *
 *====================================================================*/
#ifndef nrts_types_h_included
#define nrts_types_h_included

#include "platform.h"
#include "cssio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NRTS_MAXSTA       1 /* max number of stations per system     */
#define NRTS_MAXCHN     128 /* max number of channels per station    */
#define NRTS_MAXINFO   1024 /* max bytes of system specific info     */
#define NRTS_DATNAME  "dat" /* name of data file in disk buffer      */
#define NRTS_LOGNAME  "log" /* channel name assigned to log data     */
#define NRTS_SNAMLEN 7
#define NRTS_CNAMLEN 7
#define NRTS_INAMLEN 7

typedef struct nrts_files {
    char cnf[MAXPATHLEN+1];  /* configuration         */
    char log[MAXPATHLEN+1];  /* log file              */
    char hdr[MAXPATHLEN+1];  /* dl hdr file           */
    char dat[MAXPATHLEN+1];  /* dl dat file           */
    char bwd[MAXPATHLEN+1];  /* binary wfdiscs        */
    char sys[MAXPATHLEN+1];  /* space for mmap'd info */
} NRTS_FILES;

typedef struct nrts_mmap {
    void *ptr;     /* address at which mapping is placed */
#ifdef _WIN32
    HANDLE fd; /* Handle to the file from which to create a mapping object */
    HANDLE hMapHandle ; /* Handle to the file-mapping object */
#else
    int fd;        /* file descriptor of mmap'd file     */
#endif
} NRTS_MAP;

typedef struct nrts_buffer {
    long nrec;    /* number of records to buffer     */
    long hide;    /* number of "hidden" records      */
    long oldest;  /* index of oldest   rec in buffer */
    long yngest;  /* index of youngest rec in buffer */
    long lend;    /* index of logical end of buffer  */
    off_t off;    /* file offset to start of buffer  */
    int   len;    /* record length in bytes          */
    long  siz;    /* total length of buffer in bytes */
    struct {
        long oldest;
        long yngest;
        long lend;
    } backup;
} NRTS_BUFFER;

typedef struct nrts_chn {
    char name[NRTS_CNAMLEN+1]; /* channel name (alphanumeric)               */
    NRTS_BUFFER hdr;           /* header buffer                             */
    NRTS_BUFFER dat;           /* data buffer                               */
    int status;                /* status flag                               */
    double beg;                /* time of oldest sample in the disk loop    */
    double end;                /* time of last   sample in the disk loop    */
    float sint;                /* nominal sample interval, to be determined */
    long nseg;                 /* number of data segments (wfdisc records)  */
    long nread;                /* number of records received since start up */
    unsigned long count;       /* total number of records ever received     */
    time_t tread;              /* time last packet was received             */
} NRTS_CHN;

typedef struct nrts_sta {
    char name[NRTS_SNAMLEN+1];    /* station name        */
    int nchn;                   /* number of channels  */
    NRTS_CHN chn[NRTS_MAXCHN];  /* channel information */
} NRTS_STA;

typedef struct nrts_sys {
    pid_t pid;                  /* dl writer's process id */
    int type;                   /* system type code */
    char info[NRTS_MAXINFO];    /* type specific information */
    int reclen;                 /* raw data packet length */
    NRTS_BUFFER UNUSED;         /* unused field, kept only to preserve sizeof(NRTS_SYS) */
    int status;                 /* system acquistion status */
    int bwd;                    /* bwd status flag */
    unsigned long order;        /* host byte order */
    time_t start;               /* time data acquistion began */
    long nread;                 /* number of records read */
    unsigned long count;        /* total no. of records ever received */
    int nsta;                   /* number of stations */
    NRTS_STA sta[NRTS_MAXSTA];  /* station information */
} NRTS_SYS;

typedef struct {
    NRTS_SYS *sys;
    NRTS_STA *sta;
    NRTS_CHN *chn;
    char ident[NRTS_SNAMLEN+1+NRTS_CNAMLEN+1];
} NRTS_STREAM;

typedef struct nrts_header {
    long standx;          /* station index                    */
    long chnndx;          /* channel index                    */
    int wrdsiz;           /* sample size                      */
    int order;            /* sample byte order                */
    int hlen;             /* raw header length in bytes       */
    int dlen;             /* data length in bytes             */
    int action;           /* action flag                      */
    struct {
        char code;
        short qual;
    } beg;                /* begin time clock quality         */
    struct {
        char code;
        short qual;
    } end;                /* end time clock quality           */
    float tear;           /* time tear (if any) in seconds    */
    float hang;           /* horizontal orientation of sensor */
    float vang;           /* vertical orientation of sensor   */
    struct wfdisc wfdisc; /* wfdisc                           */
} NRTS_HEADER;

typedef struct {
    char *hdr;
    char *dat;
    char *bwd;
    char *sys;
} NRTS_DLNAMES;

#ifdef __cplusplus
}
#endif

#endif /* nrts_types_h_included */

/* Revision History
 *
 * $Log: types.h,v $
 * Revision 1.3  2007/01/05 00:26:43  dechavez
 * increased NRTS_MAXCHN to 128 (multiple Q330's per station really drives up the count!)
 *
 * Revision 1.2  2006/11/10 05:42:51  dechavez
 * Increased NRTS_MAXCHN to 64
 *
 * Revision 1.1  2005/07/25 22:47:34  dechavez
 * initial release
 *
 */
