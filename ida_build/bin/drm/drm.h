#pragma ident "$Id: drm.h,v 1.6 2004/01/23 22:20:26 dechavez Exp $"
#ifndef drm_h_included
#define drm_h_included

#include <sys/param.h>
#include "xfer.h"

#define DRM_ADMIN     "drmgr@idahub.ucsd.edu"
#define DRM_CHECKFLAG "/tmp/drm.checkflag"

/*  Supported output data formats  */

#define DRM_SAC       1
#define DRM_SACASCII  2
#define DRM_CSS30     3
#define DRM_CM6       4
#define DRM_CM8       5
#define DRM_MINISEED  6
#define DRM_NATIVE    7

/*  Misc. constants  */

#define DRM_MAXHOP      6
#define DRM_MAXSTA     XFER_MAXSTA
#define DRM_MAXCHN     XFER_MAXCHN
#define DRM_MAXSET   1024
#define DRM_MAXQUEUE   8192
#define DRM_MAXNAMLEN  31
#define DRM_BUFLEN    (MAXPATHLEN+1)
#define DRM_MAXNUMCHAN 1024

#define DRM_MAXLINLEN  1024
#define DRM_DEFLINLEN    80
#define DRM_DEFSEEDEXP   12
#define DRM_MINSEEDEXP    8
#define DRM_MAXSEEDEXP   16
#define DRM_DEFCHNLST  "bh?"

#define DRM_HOME    "DRM_HOME"

#ifndef DRM_DEFHOME
#define DRM_DEFHOME "/usr/nrts/drm"
#endif

/*  Various function return codes and status flags  */

#define DRM_BAD            -1
#define DRM_OK              0
#define DRM_DATASET         1
#define DRM_RAW             2
#define DRM_DATAREADY       3
#define DRM_TRYLATER        4
#define DRM_NOTYET          5
#define DRM_DONTKNOW        6
#define DRM_NODATA          7
#define DRM_NOTCONFIGURED   8
#define DRM_NOCHAN          9
#define DRM_BADREQ         10
#define DRM_DELIVERED      11
#define DRM_FAIL           12
#define DRM_FTPFAIL        13
#define DRM_REMOTEFTP      14
#define DRM_HELP           15
#define DRM_SLIST          16
#define DRM_CALIB          17
#define DRM_EARLY          18
#define DRM_RCPFAIL        19
#define DRM_DIVERTED       20
#define DRM_DEFER          21
#define DRM_IGNORE         22
#define DRM_WATCHDOG       23
#define DRM_INTERRUPTED    24
#define DRM_DONE           25
#define DRM_NOPERM         26
#define DRM_SPAM           27

/*  Delivery agent flags  */

#define DRM_FTP      1
#define DRM_RCP      2
#define DRM_EMAIL    3
#define DRM_WILLCALL 4

/*  File locks inferred from mode  */

#define DRM_LOCKED   (mode_t) 0755
#define DRM_UNLOCKED (mode_t) 0644

/*  Request type identifiers  */

#define DRM_IDADRM_STRING  "idadrm"
#define DRM_SPYDER_STRING  "spyder"
#define DRM_AUTODRM_STRING "autodrm"
#define DRM_DCC_STRING     "IDA_DCC" /* default if not set in cnf file */

#define DRM_IDADRM  0
#define DRM_SPYDER  1
#define DRM_AUTODRM 2
#define DRM_NTYPE   3

/*  Macros and lookup tables  */

#ifdef DRM_TIMECODE

#ifndef leap_year
#define leap_year(i) ((i % 4 == 0 && i % 100 != 0) || i % 400 == 0)
#endif

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

#endif /* DRM_TIMECODE */

/*  Structure templates  */

struct drm_info {
    int nsta;
    struct {
        char name[XFER_SNAMLEN+1];
        int nchn;
        struct {
            char name[XFER_CNAMLEN+1];
            double reclen;
        } chn[DRM_MAXCHN];
    } sta[DRM_MAXSTA];
};

struct drm_sta {
    char *name;             /* station code                       */
    int  nhop;              /* number of systems for this station */
    char *node[DRM_MAXHOP]; /* ordered list of nodes to search    */
    int dialup;             /* if set, station is a a dialup      */
    int ignore;             /* if set, force no data returns      */
    int defer;              /* if set, move to manual queue       */
    int to;                 /* read timeout                       */
    int tto;                /* 'tiny-time-out' for connect()      */
    int maxconn;            /* maximum connect time               */
};

struct drm_cnf {
    int  version;       /* unused for now (and forever?)                  */
    int  maxproc;       /* max number of active requests                  */
    char *hostname;     /* my fully domained hostname                     */
    char *dcc;          /* data center code (GSE 2.0 mesg source string)  */
    char *admin;        /* email address of IDADRM administrator          */
    char *ftphome;      /* anonymous ftp home                             */
    char *anonftp;      /* anonymous ftp directory relative to ftphome    */
    long holdanon;      /* max days to hold undelivered data in anon ftp  */
    long patience;      /* seconds before bouncing unsatisfied requests   */
    char *spyder_host;  /* SPYDER host name                               */
    char *spyder_user;  /* SPYDER user name                               */
    char *spyder_dir;   /* SPYDER data directory                          */
    int  rcp_maxtry;    /* number of times to retry failed rcp's          */
    long maxhold;       /* number of seconds to wait for pending data     */
    long maxpast;       /* oldest acceptable request                      */
    long waitnotify;    /* notify if to be held longer than this (secs)   */
    long maxemail;      /* max size of a file for email delivery          */
    long maxlen1;       /* max size of an individual request (full time)  */
    long maxlen2;       /* max size of an individual request (dial-up)    */
    long maxconn1;      /* max connect time in seconds (full time)        */
    long maxconn2;      /* max connect time in seconds (dial-up)          */
    int  nsta;          /* number of configured stations                  */
    struct drm_sta sta[DRM_MAXSTA];  /* the station info                  */
    int  NumChan;
    char *MasterChanList[DRM_MAXNUMCHAN]; /* all the possible channel names */
};

struct drm_finfo {
    int  type;   /* request type                       */
    int  yr;     /* year ( minus 1900) request arrived */
    int  day;    /* day request arrived                */
    long seqno;  /* sequence number (for year/day)     */
    int  set;    /* data set number (0 if raw request) */
};

/* Directory names */

struct drm_dir {
    char *home;
    char *etc;
    char *flags;
    char *msgs;
    char *tmp;
    char *incoming;
    char *hold;
    char *queue;
    char *manual;
    char *deferred;
    char *archive;
    char *problems;
    char *work;
    char *ftp;
    char *pickup;
    char *anonftp;
    char *anonpickup;
};

/* Path names */

struct drm_path {
    char *pid;
    char *incoming;
    char *hold;
    char *queue;
    char *manual;
    char *archive;
    char *problems;
    char *locklock;
    char *cnf;
    char *log;
    char *timestamp;
    char *info;
    char *raw;
    char *ftpscript;
    char *ftplog;
    char *message;
    char *remap;
    char *abusers;
    char *auth;
};

/* Warning flags and other status flags */

struct drm_flag {
    char *patience;
    char *queued;
    char *ignored;
    char *deferred;
    char *interrupted;
    char *failed;
    char *done;
};

/* Canned messages */

struct drm_msg {

    char *tmp;
    char *delivered;
    char *fail;
    char *nodata;
    char *dataready;
    char *ftpfail;
    char *rcpfail;
    char *nochan;
    char *remoteftp;
    char *forceftp;
    char *wait;
    char *early;
    char *queued;
    char *noperm;

    char *badreq;

    char *help;
    char *calib;
    char *slist;

    char *header;

};

struct drm_names {
    char *request;
    struct drm_dir  dir;
    struct drm_path path;
    struct drm_msg  canned;
    struct drm_msg  msg;
    struct drm_flag flag;
};

struct drm_remote {
    int agent;     /* DRM_FTP, DRM_RCP, DRM_EMAIL               */
    char *address; /* remote host name or email address address */
    char *user;    /* user name on remote host (ftp, rcp only)  */
    char *passwd;  /* password on remote host (ftp only)        */
    char *dir;     /* remote directory (ftp, rcp only)          */
};

struct drm_event {
    float evla;  /* event latitude            */
    float evlo;  /* event longitude           */
    float evdp;  /* event depth               */
    float mag;   /* event magnitude           */
    float gcarc; /* event to station distance */
    float az;    /* event to station azimuth  */
};

struct drm_datreq {
    char *email;              /* email address of requestor     */
    char *ident;              /* requestor provided ident       */
    struct drm_finfo finfo;   /* request identifiers            */
    int  type;                /* request type                   */
    struct drm_remote remote; /* remote data destination        */
    int format;               /* Output data format code        */
    double beg;               /* window start time              */
    double end;               /* window end   time              */
    char *sname;              /* station code                   */
    char *chnlst;             /* channel list derived from chn  */
    char *sc;                 /* xfer style sc string           */
    int nchn;                 /* number of tokens in chn, next  */
    char *chn[DRM_MAXCHN];    /* input channel tokens           */
    struct drm_event event;   /* optional event information     */
    char *fin;                /* for SPYDER only, fin.sta.chn   */
    int nset;                 /* number of data sets in request */
    int linelen;              /* line length (for CM6/8 output) */
    int allchan;              /* set if all channels requested  */
    long maxconn;             /* max connect time (if positive) */
    long nrec;                /* number of records retrieved    */
};

struct drm_rawreq {
    char *email;            /* email address of requestor     */
    char *ident;            /* requestor provided ident       */
    int  type;              /* request type                   */
    struct drm_finfo finfo; /* request identifiers            */
    struct {
        int help;
        int slist;
        int calib;
    } aux;                             /* aux data flags                 */
    int nset;                          /* number of data sets in request */
    struct drm_datreq set[DRM_MAXSET]; /* data request window(s)         */
};

struct stachan {
    char sta[XFER_SNAMLEN+1];
    char chn[XFER_CNAMLEN+1];
};
 
struct stachnmap {
   struct stachan internal;
   struct stachan external;
   float lat;
   float lon;
   float elev;
   float depth;
};
 
struct remap {
    int nentry;
    struct stachnmap *info;
};

/*  Library function prototypes  */

#ifdef __STDC__

void drm_abort(
    int
);

void drm_ack(
    char *,
    int
);

void drm_ack_init(
    int
);

int drm_avail(
    struct drm_datreq *,
    char **,
    char *,
    struct drm_cnf *,
    double *,
    struct drm_sta *
);

int drm_chanlst(
    struct drm_datreq *,
    char ***,
    char *
);

struct drm_cnf *drm_cnf(
    void
);

void drm_die(
    int,
    char *,
    char *
);

void drm_exit(int status);

struct drm_finfo *drm_fparse(
    char *,
    char *
);

int drm_init(
    char *,
    char *,
    char *
);

char *drm_mkname(
    struct drm_finfo *
);

char *drm_msgfile(
    char *,
    char *
);

struct drm_names *drm_names(
    void
);

int drm_prtsys(
    struct xfer_cnfnrts *
);

struct drm_cnf *drm_rdcnf(
    char *,
    char *
);

void drm_rddatreq(
    FILE *,
    struct drm_datreq *
);

int drm_rdfile(
    char *,
    void **,
    struct drm_cnf *,
    char *
);

int drm_rdautodrm(
    FILE *,
    struct drm_finfo *,
    struct drm_rawreq *,
    struct drm_cnf *,
    char *,
    int *
);

int drm_rdidadrm(
    FILE *,
    struct drm_finfo *,
    struct drm_rawreq *,
    struct drm_cnf *,
    char *
);

int drm_rdspyder(
    FILE *,
    struct drm_finfo *,
    struct drm_rawreq *,
    struct drm_cnf *,
    char *
);

void drm_unlock(
    char *
);

void drm_wrdatreq(
    FILE *,
    struct drm_datreq *
);

int exitcode(void);
int gse_msg_init(char *fname, struct drm_finfo *finfo, char *ref_id);
int prep_line( char *buffer, char **token, int maxtoken, char comment, char *delimiter);
void set_return_address(char *address);
int abuser(char *address);
int authorized(char *address);
void set_merged_flag(void);
void set_delivered_flag(void);
int drm_prtinfo(struct xfer_cnfnrts *info);

#else

#endif /* ifdef __STDC__ */

#endif

/* Revision History
 *
 * $Log: drm.h,v $
 * Revision 1.6  2004/01/23 22:20:26  dechavez
 * added DRM_NATIVE
 *
 * Revision 1.5  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.4  2003/04/24 00:10:37  dechavez
 * Anti-spam support introduced
 *
 * Revision 1.3  2002/04/26 01:05:27  nobody
 * increased DRM_MAXQUEUE to 8192
 *
 * Revision 1.2  2002/02/11 17:37:16  dec
 * added MasterChanList element to drm_cnf structure
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
