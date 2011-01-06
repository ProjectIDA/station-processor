#pragma ident "$Id: wrtdl.h,v 1.24 2006/02/10 02:07:35 dechavez Exp $"
#ifndef wrtdl_h_included
#define wrtdl_h_included

#include <signal.h>
#include "nrts.h"
#include "util.h"
#include "xfer.h"
#include "seed.h"
#include "isi.h"

extern char *VersionIdentString;

struct param {
    size_t len;
    char *prev;
    char *crnt;
    int timeout;
    int tto;
    int retry;
    struct nrts_sys *sys;
    struct xfer_req req;
    struct xfer01_wavreq *wavreq;
    char *keeparg;
    char *tapfile;
    int port;
    double beg;
    double end;
    int blksiz;
    int tolerance;
    int fwd;
    char *IsiStatPath;
    IDA *ida;
    char *dbgpath;
    int  (*read_func)();
};

/*  Function prototypes  */

/* exitcode.c */
void LockDiskloop(void);
void UnlockDiskloop(void);
void SetSys(struct nrts_sys *sysptr);
void die(int status);

/* flush.c */
VOID StartFlushThread(struct param *param);

/* getrec.c */
BOOL LogIsiIacpStats(time_t tstamp);
int init_getrec(char *homeptr, char *syscodeptr, char *iname, struct param *paramptr, int ctsrts, int leap_fix, int *flags);

/* init.c */
int init(int argc, char **argv, struct param *param);

/* isi.c */
VOID LogIsiMessage(char *message);
REAL64 ConvertToIsiBegTime(REAL64 beg);
REAL64 ConvertToIsiEndTime(REAL64 end, BOOL keepup);
ISI_DATA_REQUEST *BuildIsiDataRequest(NRTS_SYS *sys, REAL64 beg, REAL64 end, ISI_PARAM *param);

/* log.c */
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, BOOL debug);

/* main.c */
void set_flushflag(void);
void flush_wfdiscs(void);
int main(int argc, char **argv);

/* request.c */
int init_request(struct nrts_sys *sys, char *keeparg);
int request(struct nrts_sys *sys, double beg, double end, struct xfer01_wavreq *req);

/* signals.c */
VOID StartSignalHandler(VOID);

/* tee.c */
void CloseTeeFile(VOID);
void InitTeeDir(char *home, char *syscode);
void TeePacket(char *buffer, int len);

/* ttyio.c */
int tty_init(char *name, long baud, int ttyto, struct nrts_sys *system, int ctsrts, int leapfix, char *tapfile, IDA *idaptr);
int tty_read(int ttyfd, char *obuf, int want, int timeout);
void tty_close(void);
int tty_reset(int fd);

#endif /* wrtdl_h_included */

/* Revision History
 *
 * $Log: wrtdl.h,v $
 * Revision 1.24  2006/02/10 02:07:35  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.23  2005/07/26 00:55:29  dechavez
 * include isi.h since it is no longer part of nrts.h
 *
 * Revision 1.22  2005/05/06 01:09:29  dechavez
 * added optional logging of ISI:IACP recv stats
 *
 * Revision 1.21  2005/01/28 02:09:45  dechavez
 * Added dbgpath debug support for ISI feeds
 *
 * Revision 1.20  2004/09/28 23:25:32  dechavez
 * added LockDiskloop() UnlockDiskloop() prototypes
 *
 * Revision 1.19  2004/02/05 22:33:49  dechavez
 * tee packets at fixed length
 *
 * Revision 1.18  2004/02/05 21:08:57  dechavez
 * added support for tee files
 *
 * Revision 1.17  2003/12/22 22:13:20  dechavez
 * updated prototypes, added fwd to param structure
 *
 * Revision 1.16  2003/11/25 20:39:17  dechavez
 * cleaned up prototypes
 *
 * Revision 1.15  2003/05/23 19:50:17  dechavez
 * added support for UDP input
 *
 * Revision 1.14  2002/04/25 20:20:55  dec
 * changed VERSION to extern variable (in new ReleaseNotes.c)
 *
 * Revision 1.13  2001/10/05 16:59:09  dec
 * 2.11.2
 *
 * Revision 1.12  2001/09/12 22:39:08  dec
 * 2.11.1
 *
 * Revision 1.11  2001/09/09 01:23:03  dec
 * 2.12.0
 *
 * Revision 1.10  2000/10/12 17:24:09  dec
 * 2.10.8
 *
 * Revision 1.9  2000/10/07 22:04:23  dec
 * 2.10.7
 *
 * Revision 1.8  2000/09/21 22:07:31  nobody
 * 2.10.6
 *
 * Revision 1.7  2000/09/19 23:23:15  nobody
 * release 2.10.5
 *
 * Revision 1.6  2000/06/23 05:43:34  dec
 * 2.10.4
 *
 * Revision 1.5  2000/03/09 17:36:10  dec
 * check for clobbered sys->pid and reset if needed
 *
 * Revision 1.4  2000/02/28 18:16:18  dec
 * 2.10.2, leapfix hack changed for 2000.
 *
 * Revision 1.3  2000/02/18 06:39:25  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 00:15:38  dec
 * added VERSION string #define
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
