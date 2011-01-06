#pragma ident "$Id: edes.h,v 1.16 2006/02/10 02:05:34 dechavez Exp $"
/*
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 */
#ifndef edes_h_included
#define edes_h_included

#include <sys/types.h>
#include "nrts.h"
#include "xfer.h"

extern char *VersionIdentString;

#define MAXSYS 64

struct edes_span {
    double beg;  /* time of first sample in record */
    double end;  /* time of last  sample in record */
};

#define EDES_IGNORE  0 /* ignore this channel             */
#define EDES_READY   1 /* data are ready for transfer     */
#define EDES_DONE    2 /* all requested data transferred  */
#define EDES_WAITING 3 /* waiting for data to be acquired */

/* Stuff the sys structure does not contain, but we would like to use */

struct edes_inf_chn {
    double beg;  /* requested begin time                        */
    double end;  /* requested end time                          */
    long begndx; /* dl index of first record to transfer        */
    long endndx; /* dl index of last  record to transfer        */
    long nxtndx; /* dl index of next  record to transfer        */
    long count;  /* dl counter at time indices were determined  */
    int  status; /* status flag                                 */
};

struct edes_inf_sta {
    struct edes_inf_chn chn[NRTS_MAXCHN]; /* channel information */
};

struct edes_inf {
    IDA *ida;   /* IDA handle for this dl */
    int count;  /* number of requested channels from this dl */
    int hfd;    /* dl hdr file descriptor */
    int dfd;    /* dl dat file descriptor */
    struct edes_inf_sta sta[NRTS_MAXSTA]; /* station information */
};

struct edes_cnf_chn {
    int    index;                   /* index into xfer_cnf array */
    int    wrdsiz;                  /* sample word size          */
    char   instype[NRTS_INAMLEN+1]; /* GSE2.0 instype            */
    u_long order;                   /* sample byte order         */
    float  sint;                    /* sample interval           */
    float  calib;                   /* GSE2.0 calib              */
    float  calper;                  /* GSE2.0 calper             */
    float  vang;                    /* GSE2.0 vang               */
    float  hang;                    /* GSE2.0 hang               */
    float  paklen;                  /* packet length in seconds  */
};

struct edes_cnf_sta {
    int   index;   /* index into xfer_cnf array */
    float lat;     /* station latitude          */
    float lon;     /* station longitude         */
    float elev;    /* station elevation         */
    float depth;   /* station depth             */
    struct edes_cnf_chn chn[NRTS_MAXCHN];
};

struct edes_cnf {
    struct edes_cnf_sta sta[NRTS_MAXSTA];
};

struct edes_params {
    int isd;                          /* input socket descriptor      */
    int osd;                          /* output socket descriptor     */
    int nsys;                         /* number of supported systems  */
    time_t tstamp;                    /* time request was received    */
    int keepup;                       /* keep up flag                 */
    struct xfer_packet *(*decode[MAXSYS])();/* raw decoder functions  */
    struct edes_cnf  cnf[MAXSYS];     /* configuration information    */
    struct edes_inf  inf[MAXSYS];     /* bookkeeping information      */
    struct nrts_mmap map[MAXSYS];     /* mmap'd dl status information */
    struct nrts_sys  sys[MAXSYS];     /* snapshots of the mmap'd info */
};

/* Protocol/type independent request */

struct edes_chnreq {
    char name[XFER_CNAMLEN+1];
    double beg;
    double end;
};

struct edes_stareq {
    char name[XFER_SNAMLEN+1];
    int nchn;
    struct edes_chnreq chn[XFER_MAXCHN];
};

struct edes_req {
    int type;                 /* request code                      */
    int timeout;              /* agreed upon timeout interval      */
    int ident;                /* client supplied identifier        */
    int cnf_form;             /* desired format of config packet   */
    /* following only if type is XFER_WAVREQ */
    int wav_form;             /* desired format of waveform packet */
    int keepup;               /* keep up time (aka cont/seg flag)  */
    int comp;                 /* desired compression flag          */
    int nsta;                 /* number of stations to follow      */
    struct edes_stareq sta[XFER_MAXSTA];   /* per station requests */
};

/* Prototypes */

/* check_wavreq.c */
int check_wavreq(struct edes_req *request, struct edes_params *edes);

/* cnfgen1.c */
int load_cnfgen1(struct xfer_cnfgen1 *cnf, struct edes_params *edes);

/* cnfnrts.c */
int load_cnfnrts(struct xfer_cnfnrts *cnf, struct edes_params *edes);

/* dumppar.c */
void dump_par(int level, struct edes_params *par);

/* getrec.c */
struct xfer_packet *getrec(struct edes_params *edes, int i, int j, int k, long index, char *buffer, int flag, int swap, int *ws);

/* handlers.c */
int handlers(int sd);

/* ida10_decode.c */
void ida10_decode_init(BOOL DumpBadDataFlag);
struct xfer_packet *ida10_decode(struct edes_params *edes, int i, int j, int k, int init_flag, char *buffer, int swap, int *ws);

/* ida_decode.c */
void ida_decode_init(BOOL DumpBadDataFlag);
struct xfer_packet *ida_decode(struct edes_params *edes, int i, int j, int k, int init_flag, char *buffer, int swap, int *ws);

/* init.c */
int init(int argc, char **argv, char **home, struct edes_params *edes, char *buffer);

/* load_config.c */
int load_config(struct edes_req *request,  struct xfer_cnf *cnf, struct edes_params *edes);

/* reformat.c */
int reformat(struct xfer_wav *wav, int format, int comp, struct edes_params *edes, int i, int j, int k, struct xfer_packet *packet);

/* remap_req.c */
int remap_req(struct xfer_req *xfer, struct edes_req *er);

/* search.c */
long search(struct edes_params *edes, int i, int j, int k, double target, long ldef, long rdef, char *buffer);

/* set_indices.c */
void set_indices(struct edes_params *edes, int i, int j, int k, char *buffer);

/* span.c */
struct edes_span *get_span(struct edes_params *edes, int i, int j, int k, int index, char *buffer);

/* transfer.c */
void transfer(struct xfer_cnf *cnf, int format, int comp, struct edes_params *edes, int i, int j, int k, char *buffer, char *peer);

/* wavreq.c */
void logWhoWhat(void);
char *SetWhoWhat(struct xfer_req *req);
void set_wait_flag(int flag);
void catch_alarm(int sig);
int send_data(void);
void wavreq(struct xfer_cnf *cnf_ptr, struct edes_req *request_ptr, struct edes_params *edes_ptr, char *buffer_ptr, char *peer_ptr);

#endif

/* Revision History
 *
 * $Log: edes.h,v $
 * Revision 1.16  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.15  2003/12/10 06:22:38  dechavez
 * updated prototypes
 *
 * Revision 1.14  2002/04/25 21:59:49  dec
 * SetWhoWhat added
 *
 * Revision 1.13  2002/04/25 21:13:22  dec
 * use extern version ident string instead of VERSION
 *
 * Revision 1.12  2002/04/02 01:23:55  nobody
 * 2.12.1
 *
 * Revision 1.11  2002/03/11 17:31:12  dec
 * 2.12.0, added DumpBadData parameter to decode init routines
 *
 * Revision 1.10  2002/02/23 00:03:50  dec
 * 2.11.5
 *
 * Revision 1.9  2001/10/24 23:34:28  dec
 * 2.11.4
 *
 * Revision 1.8  2001/09/09 01:21:05  dec
 * 2.11.3
 *
 * Revision 1.7  2001/02/09 00:49:22  nobody
 * 2.11.2 - fixed search bug when target lies on exact end of disk loop
 *
 * Revision 1.6  2000/06/07 21:31:17  dec
 * release 2.11.1 (relink only)
 *
 * Revision 1.5  2000/03/16 06:54:48  nobody
 * Corrected errors in checking requests which caused it to reject
 * beg=value end=XFER_YNGEST
 *
 * Revision 1.4  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.3  2000/02/18 06:39:21  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 01:11:06  dec
 * Added ReleaseNotes, christened 2.10.1
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
