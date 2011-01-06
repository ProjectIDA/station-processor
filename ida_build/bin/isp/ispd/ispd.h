#pragma ident "$Id: ispd.h,v 1.29 2008/08/21 21:28:19 dechavez Exp $"
/*------------------------------------------------------------------
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *-------------------------------------------------------------------
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *-------------------------------------------------------------------
 */

#ifndef ispd_h_defined
#define ispd_h_defined

#include <sys/types.h>
#include <sys/param.h>
#include <time.h>

#include "platform.h"

#include "isp.h"
#include "ida.h"
#include "ida10.h"
#include "util.h"
#include "oldttyio.h"
#include "oldparo.h"
#include "sclk.h"
#include "oldmsgq.h"
#include "xfer.h"
#include "sanio.h"

static char ispd_copyright[] =
"Copyright (c) 1997-2008 Regents of the University of California.";

/* Module ident's for exit codes */

#define ISPD_MOD_BARO      100
#define ISPD_MOD_CLOCK     200
#define ISPD_MOD_CONFIG    300
#define ISPD_MOD_CONSOLE   400
#define ISPD_MOD_DASIO     500
#define ISPD_MOD_DASOPS    600
#define ISPD_MOD_DISKIO    700
#define ISPD_MOD_INIT      800
#define ISPD_MOD_INJECT    900
#define ISPD_MOD_MEDIAOPS 1000
#define ISPD_MOD_SIGNALS  1200
#define ISPD_MOD_TAPEIO   1300
#define ISPD_MOD_MSGQ     1400
#define ISPD_MOD_MASSIO   1500
#define ISPD_MOD_NRTS     1600
#define ISPD_MOD_PROCESS  1700
#define ISPD_MOD_SANIO    1800
#define ISPD_MOD_SANOPS   1900
#define ISPD_MOD_SOH      2000
#define ISPD_MOD_FLUSHCF  2100
#define ISPD_MOD_RESTART  2200
#define ISPD_MOD_CDIO     2300
#define ISPD_MOD_AUX      2400
#define ISPD_MOD_DPM      2500
#define ISPD_MOD_IDA8     2600
#define ISPD_MOD_IDA10    2700
#define ISPD_MOD_TEE      2800
#define ISPD_MOD_ISI      2900
#define ISPD_MOD_IBOOT    3000

/* Supported clock types */

#define ISPD_EXTCLK_NONE         0 /* No clock         */
#define ISPD_EXTCLK_IDA_MAGELLAN 1 /* IDA Magellan GPS */

/* limits */

#define ISPD_MAX_CMDLEN    64 /* max length of a DAS command message   */
#define ISPD_MAXCLIENTS    10 /* max number of simultaneous isp clients*/

/* defaults */

#define ISPD_DEF_USER         "nrts"
#define DEFAULT_BACKGROUND_LOG "syslogd:user"
#define DEFAULT_FOREGROUND_LOG "-"
#define ISPD_DEF_DEBUG        FALSE

/* Buffer flush codes */

#define ISPD_FLUSH_FULL  0 /* flushing a full buffer                */
#define ISPD_FLUSH_FORCE 1 /* forced flush, with w/o media exchange */
#define ISPD_FLUSH_MEDIA 2 /* forced flush, with a media exchange   */

/* The heap */

struct ispd_heap {
    OLD_MSGQ packets;    /* data packets        */
    OLD_MSGQ commands;   /* DAS commands        */
    OLD_MSGQ barometer;  /* barometer "samples" */
    OLD_MSGQ dpm;        /* DPM       "samples" */
    OLD_MSGQ obuf;       /* output buffer       */
};

struct ispd_queue {
    OLD_MSGQ baro;
    OLD_MSGQ dpm;
    OLD_MSGQ das_read;
    OLD_MSGQ das_write;
    OLD_MSGQ das_process;
    OLD_MSGQ massio;
    OLD_MSGQ nrts;
    OLD_MSGQ obuf;
};
 
/* Used to support ISP sampled aux data */

#define ISPD_BAROMETER 1
#define ISPD_DPM       2

typedef struct {
    long   value;
    time_t sys_time;
    time_t hz_time;
    time_t ext_time;
    char   qual;
} ISPD_AUX_DATUM;

typedef struct {
    char *tag;
    int missed;
    int dropped;
    BOOL first;
    ISPD_AUX_DATUM prev;
    ISP_AUX_STAT *stat;
    ISP_AUX_PARAM *par;
    struct {
        OLD_MSGQ *empty;
        OLD_MSGQ *full;
    } q;
} ISPD_AUX_HANDLE;

/* Union to hold all supported record types */

union runion {
    IDA_DHDR       d;
    IDA_CALIB_REC  c;
    IDA_IDENT_REC  i;
    IDA_CONFIG_REC k;
    IDA_LOG_REC    l;
    IDA_EVENT_REC  e;
    IDA_DAS_STATS  s;
    IDA10_TS      ts;
    IDA10_LM      lm;
};

/* Macros */

#define calib_off()          (calib_status() == ISP_CALIB_OFF)
#define calib_pending()      (calib_status() == ISP_CALIB_PENDING)
#define calib_in_progress()  (calib_status() == ISP_CALIB_ON)

#define config_unknown()     (config_status() == ISP_CONFIG_UNKNOWN)
#define config_in_progress() (config_status() == ISP_CONFIG_ON)
#define config_verified()    (config_status() == ISP_CONFIG_VERIFIED)

/* Function prototypes */

/* aux.c */
void initAuxHandle(ISPD_AUX_HANDLE *ap, int which);
void SaveLocalDatum(ISPD_AUX_HANDLE *ap, ISPD_AUX_DATUM *sample);

/* baro.c */
void InitBaro(void);
void baroDebugToggle(void);

/* cdio.c */
int cdio_init(void);
int eject_cdrom(int unused);
void FlushIsoImage(void);
void UpdateCdStats(BOOL verbose);

/* clock.c */
void InitClock(void);
void clockDebugToggle(void);

/* config.c */
void SaveDasConfig(IDA_CONFIG_REC *config);
void ConfigureDas(int wait);

/* console.c */
void server_init(void);
int media_alert(int local);
void set_cnflag(void);
void serve(int sd);
BOOL SendBootMgrMsg(int op);

/* dasio.c */
void InitDasIO(void);

/* daslog.c */
void SaveDasLog(IDA_LOG_REC *lrec);

/* dasops.c */
void das_command(char *cmd, int loglevel);
void das_reboot(void);
void das_ack(int loglevel);
void das_nak(int loglevel);
void das_iddata(int loglevel);
void das_statreq(int loglevel);
void das_flushlog(int loglevel);
void das_calibrate(int loglevel, struct isp_dascal *cal);
void das_abortcal(int loglevel);
void das_loginfo(int loglevel);
void das_logdebug(int loglevel);
void das_setdfcf(int loglevel);
void das_setdftrig(int loglevel);
void VerifyDasConfig(int wait);

/* decode.c */
int decodeSanPacket(UINT8 *packet, int *type, union runion *record, long *tstamp, int *tqual);
int decode(char *packet, int *type, union runion *record, long *tstamp, int *tqual);

/* die.c */
void SaveDLHandle(void *dlptr);
void complete_shutdown(void);
void ispd_die(int status);

/* diskio.c */
int eject_disk(int sync);
int disk_write(char *buf, int bfact, size_t buflen, int reason);
int diskio_init(void);

/* dpm.c */
void dpmDebugToggle(void);
void InitDPM(void);

/* flags.c */
void set_initializing(void);
void set_sreqsent(void);
void set_have_dascnf(void);
void set_config(int flag);
void set_dasreboot(void);
void set_event(int new_state);
void set_calib(int new_state);
void set_shutdown(int state);
void enable_output(int loglevel);
void clear_initializing(void);
void clear_sreqsent(void);
void clear_have_dascnf(void);
void clear_dasrebootflag(void);
void disable_output(int loglevel);
int initializing(void);
int sreqsent(void);
int have_dascnf(void);
int event_in_progress(void);
int calib_status(void);
int reboot_pending(void);
int config_status(void);
int output_enabled(void);
int shutting_down(void);
void set_alarm(int alarm);
void clear_alarm(int alarm);

/* flushcf.c */
void BuildSanCF(SANIO_CONFIG *cnf, time_t tstamp, UINT16 boxId);
void FlushSanCF(void);

/* ida8.c */
THREAD_FUNC AuxDataToIDA8Thread(void *argptr);

/* ida10.c */
THREAD_FUNC AuxDataToIDA10Thread(void *argptr);

/* init.c */
void init(int argc, char **argv);

/* inject.c */
void shutdown_inject(void);
void isp_inject(void);

/* iboot.c */
void StartIbootWatchdog(void);

/* isi.c */
char *LoadIsiGlob(char *path);
THREAD_FUNC IsiThread(void *argptr);
void InitIsi(LOGIO *lp);

/* log.c */
VOID LogMsgLevel(int level);
VOID LogMsg(int level, char *format, ...);
LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug);

/* massio.c */
void save_iddata(char *buf);
void save_config(char *buf);
void flush_buffer(int force);
void flush_idents(void);
void InitMassio(void);

/* mediaops.c */
void SetOutputMediaType(void);
int OutputMediaType(void);
void eject_media(void);
void InitMediaOps(void);

/* msgq.c */
void ShowMessageQueues(void);
void InitMsgqs(void);

/* patch.c */
void patchMK7(char *packet);

/* process.c */
void InitProcess(void);

/* restart.c */
void SaveCommandLine(int numargs, char **arglist, char **envlist);
void ForcedRestart(void);

/* request.c */
BOOL GenerateRequest(struct xfer_req *req);

/* sanio.c */
BOOL GetSanCnf(SANIO_CONFIG *dest);
char *SanStreamName(int index);
void InitSanIO(void);

/* sanlog.c */
BOOL SaveSanLog(IDA10_LM *lm);

/* sanops.c */
void SanCmnd(int cmnd);
void VerifySanConfig(int wait);

/* signals.c */
void signals_init(void);

/* soh.c */
void InitSoh(void);

/* status.c */
void clear_packetcounters(int loglevel);
void clear_commstats(int loglevel);
void clear_auxstats(int loglevel);
BOOL SanCmndConn(void);
void SetSanCalState(BOOL state);
void SetSanEvtState(BOOL state);
void UpdateSanSoh(SAN_HANDLE *san, SANIO_SOH *soh);
void status_init(void);

/* tapeio.c */
void set_eject_flag(int value);
int eject_tape(int sync);
void check_eject_flag(void);
int tapeio_init(void);

#endif

/* Revision History
 *
 * $Log: ispd.h,v $
 * Revision 1.29  2008/08/21 21:28:19  dechavez
 * added iboot support
 *
 * Revision 1.28  2007/01/25 20:28:24  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.27  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.26  2006/06/02 21:32:19  dechavez
 * oldmsgq.h
 *
 * Revision 1.25  2006/03/30 22:44:35  dechavez
 * switch to oldparo
 *
 * Revision 1.24  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.23  2005/10/11 18:24:39  dechavez
 * updated prototypes
 *
 * Revision 1.22  2005/09/10 21:59:08  dechavez
 * replace ISPD_DEF_LOG with DEFAULT_BACKGROUND_LOG and DEFAULT_FOREGROUND_LOG
 *
 * Revision 1.21  2005/08/26 20:16:30  dechavez
 * updated prototypes
 *
 * Revision 1.20  2005/07/26 18:38:11  dechavez
 * prefix logio log messages with site tag
 *
 * Revision 1.19  2005/07/26 00:58:14  dechavez
 * initial ISI dl support (2.4.6d)
 *
 * Revision 1.18  2005/07/06 15:43:27  dechavez
 * added ISI support
 *
 * Revision 1.17  2005/05/27 00:23:31  dechavez
 * using oldttyio
 *
 * Revision 1.16  2005/04/04 20:21:57  dechavez
 * ISPD_MOD_TEE support
 *
 * Revision 1.15  2003/12/10 06:25:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.14  2002/09/09 21:59:09  dec
 * general aux data interface introduced
 *
 * Revision 1.13  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.12  2002/02/27 00:27:04  dec
 * added DEF_USER
 *
 * Revision 1.11  2002/02/22 23:52:30  dec
 * defined default packet length
 *
 * Revision 1.10  2001/10/24 23:17:56  dec
 * added CDROM support
 *
 * Revision 1.9  2001/10/08 18:16:48  dec
 * added restart module ident and prototypes, OutputMediaType() prototype
 *
 * Revision 1.8  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.7  2001/02/23 19:09:30  dec
 * added check_eject_flag() prototype
 *
 * Revision 1.6  2001/02/23 17:42:08  dec
 * update copyright date range
 *
 * Revision 1.5  2000/11/08 01:57:23  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.4  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.3  2000/10/19 22:26:02  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
