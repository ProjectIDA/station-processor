#pragma ident "$Id: isp_console.h,v 1.12 2006/02/10 02:23:42 dechavez Exp $"
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifndef isp_console_h_defined
#define isp_console_h_defined

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <curses.h>
#include "util.h"
#include "isp.h"

#define SHELL  "/usr/bin/rksh"
#define EDITOR "/usr/bin/vi"
#define ISP_BURN_CD_COMMAND "/usr/nrts/scripts/isiBurnCD ViaISP"
#define MAXSTREAM 64

#define DASLOG 0
#define ISPLOG 1

/* Possible states */

#define OPERATOR   0x01
#define LOCALHOST  0x02
#define CONNECTED  0x04
#define PRIVILEGED 0x08

/* Beep control */

#define BEEP_REQUEST 0
#define BEEP_ENABLE  1
#define BEEP_DISABLE 2
#define BEEP_SHUTUP  3

/* Window constants */

#define MINCOLS  80
#define MAXCOLS 132

#define CMD_NLINES   1
#define DAS_SOH_NLINES  19
#define DAS_LOG_MINLINES 3
#define DAS_MINLINES (DAS_SOH_NLINES + DAS_LOG_MINLINES + CMD_NLINES)

#define SAN_SOH_NLINES  17
#define SAN_LOG_MINLINES 5
#define SAN_MINLINES (SAN_SOH_NLINES + SAN_LOG_MINLINES + CMD_NLINES)

#define MINLINES (DAS_MINLINES < SAN_MINLINES ? DAS_MINLINES : SAN_MINLINES)

/* Function prototypes */

/* beep.c */
void SoundBeep(int flag);

/* calib.c */
int DasCalibrate(struct isp_dascal *cal);

/* cmddas.c */
void DasProcess(char *input, WINDOW *sohwin, WINDOW *cmdwin);

/* cmdsan.c */
void SanProcess(char *input, WINDOW *sohwin, WINDOW *cmdwin);

/* cnfdas.c */
int DasConfigure(struct isp_dascnf *output);

/* dispdas.c */
void *DasDisplay(void *dummy);
void InitDasSoh(void);

/* dispgen.c */
void FirstDisplay(void);
void SetMaxIdle(time_t value);
void RequestShutdown(int status);
void Prompt(void);
int ClearStats(char **token, int ntoken);
void NoteConnecting(void);
void InitSoh(void);
void UpdateLogWin(void);
void InitNrts(void);
void UpdateNrts(void);
void InitDisplay(void);
int OpenDisplay(char *server, int port, char *logfile_name);
int Confirm(void);
int Reconfirm(void);
void WaitToClear(void);
int TakeInput(WINDOW *win);
void ShowVersion(void);
void DisplayAlarms(struct isp_alarm *alarm, time_t tstamp);
void SetWindowsFor(int digitizer);

/* dispsan.c */
void *SanDisplay(void *dummy);
void InitSanSoh(void);
void IncrementCrntNdx(void);

/* main.c */
void EndWin(void);

/* reboot.c */
int RebootDigitizer(char **token, int ntoken, WINDOW *sohwin, int certain);

/* save.c */
int Digitizer(void);
void InitChanMap(void);
void SetPid(pid_t value);
void SetWantNrts(int value);
int WantNrts(void);
void SetDasStat(int value);
int WantDasStat(void);
void SetQueueStats(int value);
int WantQueue(void);
void SetIntervals(int value);
int WantIntervals(void);
void SetPrivilege(int value);
int Privileged(void);
void SaveState(void *data);
void SetNeedOper(int value);
int NeedOperator(void);
void SaveStreamMap(void *data);
void SaveChanMap(void *data);
IDA *GetIdaHandle(void);
int GetState(void);
int Operator(void);
void SaveStatus(void *data);
int ChangedState(void);
int GetStatus(struct isp_status *output, int clear);
void ClearStatus(void);
void SaveDasCnf(void *data);
int GetDasCnf(struct isp_dascnf *output, int clear);
int WantDasCnf(void);
void NeedDasCnf(int flag);
void SaveParams(void *data);
int GetParams(struct isp_params *output, int clear);
void ClearParams(void);
int WantParams(void);
void NeedParams(int flag);
int GetAlarms(struct isp_alarm *output, time_t *tstamp, int clear);
int WantAlarms(void);
void NeedAlarms(int flag);
int HavePerm(int test);
pid_t Pid(void);

/* signals.c */
BOOL SuccessfulBurn(void);
void ClearBurnFlag(void);
void InitSignals(void);

/* socket.c */
int Connected(void);
int TakeCharge(void);
int RemoteCommand(int command);
void Quit(int status);
void Reconnect(int where);
int SendDasCal(struct isp_dascal *cal);
int SendDasCnf(struct isp_dascnf *cnf);
int SetTimeout(int to);
void ServerConnect(char *server_name, int port_number, int to, BOOL ReadToo);

/* status.c */
void SetStatusInterval(int newinterval);
void *StatusRequest(void *dummy);
int OutputMediaType(void);
void SetOutputMediaType(int type);

/* viewlog.c */
void ViewLog(int type, char *default_file);

#endif

/* Revision History
 *
 * $Log: isp_console.h,v $
 * Revision 1.12  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.11  2005/10/11 18:30:20  dechavez
 * support for improved burn script
 *
 * Revision 1.10  2005/08/26 20:18:52  dechavez
 * redefined ISP_BURN_CD_COMMAND to use new isi scripts
 *
 * Revision 1.9  2003/12/10 06:25:41  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.8  2002/05/21 17:00:42  dec
 * RequestShutdown() prototype
 *
 * Revision 1.7  2002/02/22 23:47:27  dec
 * replace BURNER with ISP_BURN_CD_COMMAND
 *
 * Revision 1.6  2001/10/26 23:40:08  dec
 * added new signals.c prototypes
 *
 * Revision 1.5  2001/10/24 23:20:54  dec
 * added CDROM support (only partial support for DAS systems)
 *
 * Revision 1.4  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/02 20:30:43  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
