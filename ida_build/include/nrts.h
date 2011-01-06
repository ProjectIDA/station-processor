#pragma ident "$Id: nrts.h,v 1.24 2009/02/03 22:24:27 dechavez Exp $"
/*======================================================================
 *
 *  Include file of defines and type declarations for routines in
 *  version 2 of the NRTS.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef nrts_h_included
#define nrts_h_included

#include "platform.h"
#include "ida.h"
#include "ida10.h"
#include "cssio.h"
#include "dbio.h"
#include "util.h"  /* for the record locking macros given later */
#include "nrts/types.h"
#include "isi/dl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* system codes */

#define NRTS_IDA   1
#define NRTS_ASL   2
#define NRTS_IDA10 3

/* native packet types */

#define NRTS_IDA_REV8  1
#define NRTS_IDA_REV10 2

/* various flags */

#define NRTS_SYSTEM 0x01

#define NRTS_NOP    0
#define NRTS_CREATE 1
#define NRTS_EXTEND 2
#define NRTS_REDUCE 3
#define NRTS_TESTWD 4
#define NRTS_DECODE 5

#define NRTS_FDCMP  0x01

#define NRTS_READ   0x01
#define NRTS_WRITE  0x02

#define NRTS_IDLE    0x01
#define NRTS_BUSY1   0x02
#define NRTS_BUSY2   0x04
#define NRTS_BUSY3   0x08
#define NRTS_CORRUPT 0x10
#define NRTS_STABLE  0x20

#define NRTS_SYSINFO 0x01
#define NRTS_CONFIG  0x02
#define NRTS_INDICES 0x04
#define NRTS_TIMES   0x08
#define NRTS_PRTALL  (NRTS_SYSINFO | NRTS_CONFIG | NRTS_INDICES | NRTS_TIMES)

#define NRTS_DATREQ  0x01
#define NRTS_WDREQ   0x02
#define NRTS_CNFREQ  0x04

#define NRTS_YNGEST ((double) -1) /* beg of disk loop */
#define NRTS_OLDEST ((double) -2) /* end of disk loop */

/* used in nrts_inidecode() */

#define NRTS_TODISK    0x0001 /* data are going to disk (ie, not tapping) */
#define NRTS_FROMNRTS  0x0002 /* data come from a remote nrts             */
#define NRTS_FROMTTY   0x0004 /* data come from a tty                     */
#define NRTS_FROMMEDIA 0x0008 /* data come from disk or tape              */
#define NRTS_FROMISP   0x0010 /* data come from ISP shared memory ring    */
#define NRTS_STRICT    0x0020 /* perform strict header content checks     */
#define NRTS_ADDSEQNO  0x0040 /* add sequence number to packets           */
#define NRTS_FORCETTST 0x0080 /* to force playback ttag tests             */
#define NRTS_FROMUDP   0x0100 /* data coming as UDP broadcasts            */

/*  various constants  */

#define NRTS_MAXLOG 9
#define NRTS_DEFLOG 1

#define NRTS_OK     0
#define NRTS_ERROR  1
#define NRTS_EOF    2

#define NRTS_HOME     "NRTS_HOME"
#define NRTS_DEFHOME  "/usr/nrts"
#define NRTS_DEFDBDIR NRTS_DEFHOME
#define NRTS_SYSTEMS  "etc/Systems"

#define NRTS_DEFTTYTO  1
#define NRTS_DEFTCPTO 60

#define NRTS_CM6 1
#define NRTS_CM8 2

#define NRTS_AUTOINC '+'

#define NRTS_UNDEFINED_TIMESTAMP -1

/* Structure templates (most have moved to nrts/types.h) */

struct nrts_time {
    long sec;       /* seconds since Jan 1, 1970      */
    int  msec;      /* factional part in milliseconds */
};

struct nrts_timetag {
    double time;    /* seconds since Jan 1, 1970                      */
    char code;      /* device   dependent external clock quality code */
    short qual;     /* device independent external clock quality code */
};

struct nrts_packet {
    char sname[NRTS_SNAMLEN+1];  /* station name                         */
    char cname[NRTS_CNAMLEN+1];  /* channel name                         */
    char instype[NRTS_INAMLEN+1];/* GSE2.0 instype                       */
    struct nrts_timetag beg;     /* time of first sample in packet       */
    struct nrts_timetag end;     /* time of last  sample in packet       */
    float tear;                  /* time tear (if any) in seconds        */
    float sint;                  /* nominal sample interval in seconds   */
    float calib;                 /* CSS 3.0 calib                        */
    float calper;                /* CSS 3.0 calper                       */
    float vang;                  /* vertical orientation of sensor       */
    float hang;                  /* horizontal orientation of sensor     */
    long nsamp;                  /* number of samples                    */
    int wrdsiz;                  /* sample word size in bytes            */
    unsigned long order;         /* data byte order                      */
    int type;                    /* system type code                     */
    int hlen;                    /* header length in bytes               */
    int dlen;                    /* data length in bytes (nsamp * wsize) */
    char *header;                /* original header                      */
    char *data;                  /* the data                             */
    struct nrts_sys *sys;        /* system configuration (if non-null)   */
};

#define NRTS_LTL_ENDIAN_ORDER LTL_ENDIAN_BYTE_ORDER
#define NRTS_BIG_ENDIAN_ORDER BIG_ENDIAN_BYTE_ORDER

/* System type specific information, to be mapped onto system info field */

#define IDA_MAXMAPLEN 32

struct nrts_ida {
    int rev;
    char map[IDA_MAXMAPLEN+1];
    int shift;
    short mk4_year;
};

#define ASL_MAXMAPLEN 32

struct nrts_asl {
    int blksiz;
    int reformat;
    char map[ASL_MAXMAPLEN+1];
};

/* macros */

#define nrts_wlockw(map) util_wlockw(map->fd, 0, SEEK_SET, 0)
#define nrts_rlockw(map) util_rlockw(map->fd, 0, SEEK_SET, 0)
#define nrts_wlock(map)  util_wlock(map->fd, 0, SEEK_SET, 0)
#define nrts_rlock(map)  util_rlock(map->fd, 0, SEEK_SET, 0)
#define nrts_unlock(map) util_unlock(map->fd, 0, SEEK_SET, 0)

/* Function prototypes */

/* asldecode.c */
int nrts_asldecodeinit(char *home, char *syscode, struct nrts_sys *sys_ptr, int new_flags, IDA *ida, struct nrts_asl *info, void *unused1, int unused2);
struct nrts_header *nrts_asldecode(char *buffer, int len, int *action);

/* bwdb.c */
void nrtsUpdateBwdDb(DBIO *db, WFDISC *wfdisc, int action);

/* chksys.c */
int nrts_chksys(struct nrts_sys *sys);
int nrts_fixsys(struct nrts_sys *sys);

/* decode.c */
void *nrts_inidecode(char *home, char *syscode, struct nrts_sys *sys, int flags, IDA *ida, void *opt, int fdhdr);

/* die.c */
void nrts_die(int status);

/* files.c */
struct nrts_files *nrts_files(char **home, char *syscode);
BOOL nrtsBuildFileNames(char *home, char *syscode, NRTS_DLNAMES *out);
VOID nrtsFreeFileNames(NRTS_DLNAMES *fname);

/* fixwd.c */
int nrts_fixwd(struct nrts_files *file, struct nrts_mmap *mmap, char *home, char *syscode, IDA *ida);

/* getmap.c */
int nrts_getmap(char *home, char *staname, struct nrts_mmap *map);

/* ida10decode.c */
int nrts_ida10decodeinit(char *home, char *syscode, struct nrts_sys *sys_ptr, int new_flags, IDA *unused1, struct nrts_ida *unused2, void *opt, int fdhdr);
struct nrts_header *nrts_ida10decode(char *buffer, int len, int *action);

/* idadecode.c */
int nrts_idadecodeinit(char *home, char *syscode, struct nrts_sys *sys_ptr, int new_flags, IDA *ida, struct nrts_ida *info, void *opt, int unused);
void nrtsToggleCheckTstampLoggingFlag( void );
struct nrts_header *nrts_idadecode(char *buffer, int len, int *action);

/* indx.c */
int nrts_chnndx(struct nrts_sta *sta, char *name);
int nrts_standx(struct nrts_sys *sys, char *name);

/* mmap.c */
int nrts_mmap(char *fname, char *perm, int code, struct nrts_mmap *maptr);
void nrts_unmap(struct nrts_mmap *maptr);
int nrts_mmap(char *fname, char *perm, int code, struct nrts_mmap *maptr);
void nrts_unmap(struct nrts_mmap *maptr);

/* oldbwd.c */
void nrts_inibwdlist(int ofd, struct nrts_sys *sysptr);
int nrts_inibwd(struct nrts_files *file, struct nrts_sys *sysptr, char *syscode, char *home);
int nrts_inibwd(struct nrts_files *file, struct nrts_sys *sysptr, char *syscode, char *home);
int _unlock_bwd(int status);
int nrts_wrtbwd( void );
int nrts_bwd(struct nrts_header *hd, int action);
void nrts_closebwd( void );

/* prt.c */
void nrts_prtsta(FILE *fp, struct nrts_sta *sta, char *log, int what);
void nrts_prtsys(FILE *fp, struct nrts_sys *sys, char *log, int what);
char *nrts_latency(time_t secs);
char *nrts_status(int status);

/* rcnf.c */
int nrts_rcnf(FILE *ffp, char *fname, struct nrts_sys *sys);

/* stawd.c */
int nrts_stawd(char *home, char *sta, struct wfdisc **output, int flush, int maxdur, char *dbspec);

/* string.c */
char *nrtsPktString(NRTS_PKT *pkt, char *buf);

/* syscode.c */
char *nrts_syscode(char *home, char *staname);

/* sysident.c */
int nrts_sysident(char *string);

/* systems.c */
int nrts_systems(char *home, char **sysname, int maxsys);

/* syswd.c */
int nrts_syswd(char *home, char *syscode, struct wfdisc **output, int flush, int maxdur, char *dbspec);

/* time.c */
struct nrts_time *nrts_time(double dtime);
double nrts_dtime(struct nrts_time *ntime);

/* version.c */
char *nrtsVersionString( void );
VERSION *nrtsVersion( void );

/* wrtdl.c */
int nrts_iniwrtdl(char *home, char *syscode, struct nrts_sys *sysptr, int flags, IDA *ida, void *opt);
int nrts_wrtdl(char *buf, struct nrts_header **hdptr);
int nrts_term(int status);
void nrts_closedl( void );

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: nrts.h,v $
 * Revision 1.24  2009/02/03 22:24:27  dechavez
 * added nrtsPktString prototype
 *
 * Revision 1.23  2007/06/01 19:02:19  dechavez
 * added nrtsUpdateBwdDb() prototype
 *
 * Revision 1.22  2006/02/09 19:29:53  dechavez
 * updated prototypes
 *
 * Revision 1.21  2005/10/10 23:46:29  dechavez
 * made byte order macros cross-consistent
 *
 * Revision 1.20  2005/07/26 00:12:10  dechavez
 * 3.0.0
 *
 * Revision 1.19  2005/07/25 23:38:14  dechavez
 * split with nrts/types.h
 *
 * Revision 1.18  2005/06/24 21:44:53  dechavez
 * version 2.6.1
 *
 * Revision 1.17  2005/05/06 22:17:42  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.16  2005/02/09 16:58:24  dechavez
 * added byte order field to NRTS_PKT
 *
 * Revision 1.15  2005/02/07 19:10:15  dechavez
 * updated prototypes
 *
 * Revision 1.14  2004/08/24 18:57:27  dechavez
 * redfine NRTS_DEFDBDIR
 *
 * Revision 1.13  2004/06/25 18:34:57  dechavez
 * C++ compatibility
 *
 * Revision 1.12  2004/06/24 16:59:12  dechavez
 * removed unnecessary system includes, added win32 nrts_mmap support (aap)
 *
 * Revision 1.11  2004/06/11 17:12:16  dechavez
 * added home dir to NRTS handle, updated prototypes
 *
 * Revision 1.10  2004/04/23 00:36:52  dechavez
 * updated prototypes
 *
 * Revision 1.9  2003/11/03 23:09:16  dechavez
 * added NRTS_UNDEFINED_TIMESTAMP
 *
 * Revision 1.8  2003/10/16 17:42:41  dechavez
 * many changes related to 2.1.0
 *
 * Revision 1.7  2003/05/23 19:55:11  dechavez
 * added NRTS_FROMUDP
 *
 * Revision 1.6  2002/11/20 01:03:26  dechavez
 * added dbdir to nrts_syswd and nrts_stawd prototypes
 *
 * Revision 1.5  2002/11/19 18:36:33  dechavez
 * added maxdur to nrts_syswd and nrts_stawd prototypes
 *
 * Revision 1.4  2002/04/29 17:27:34  dec
 * updated prototypes
 *
 * Revision 1.3  2001/05/20 15:58:10  dec
 * added platform.h
 *
 * Revision 1.2  2000/02/18 00:25:24  dec
 * Added NRTS_FORCETTST flag
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
