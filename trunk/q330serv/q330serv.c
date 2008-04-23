/*
File:       q330serv
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Interfaces with the Quantera Q330 controler via q330lib
License:   GNU General Public License

This program borrows heavily from the GNU Seneca project.  Therefore it
comes under the GNU General Public License if distributed.

q330serv is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

q330serv is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with q330serv; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Update History:
mmddyy who Changes
==============================================================================
012207 fcs Creation
050707 fcs Add argument for config directory for multiple q330 support
111907 fcs Convert to q330serv working with q330arch to merge multiple q330s
041408 fcs Add status information to mapstatus shared memory region
******************************************************************************/
#define WHOAMI "q330serv"
#define VERSION_DATE  "10 November 2007"

#include "globals.h"
#include "libtypes.h"
#include "libstrucs.h"
#include "libpoc.h"
#include "callbacks.h"
#include "libsupport.h"
#include "libmsgs.h"
#include "include/diskloop.h"
#include "include/idaapi.h"
#include "include/shmstatus.h"
#include "include/dcc_time_proto2.h"
#include <syslog.h>
/*
All these includes from former seneca program are not used here
#include "pascal.h"
#include "libclient.h"
#include "libstrucs.h"
#include "libpoc.h"
#ifndef OMIT_SEED
#include "libnetserv.h"
#endif
#include "libcvrt.h"
#include "q330cvrt.h"
#include "libpoc.h"
#include "libmd5.h"
#include "libsample.h"
#include "config.h"
#include "commands.h"
#include "callbacks.h"
#include "dump.h"
*/

void daemonize();

// Global variables
int main_pid=0;     // Saves process id of main thread
int debug_arg=0;    // Shares debug status
int iDlg=0;         // Which data logger this is in overall system
struct s_mapstatus *mapstatus=NULL;   // Communicates status to dispstatus program

static void sigterm_nodaemon(int sig)
{
  int  i;
  enum tliberr err ;
  enum tlibstate libstate;

  if (debug_arg)
    fprintf(stderr, "sigterm_nodaemon sig %d, pid=%d, main_pid=%d\n",
      sig, getpid(), main_pid);

  if (context == NULL)
    exit(EXIT_SUCCESS);

  // termination routine is only for main program
  if (main_pid != getpid())
    return;

  fprintf(stderr, "q330serv shutting down\n");
  lib_change_state (context, LIBSTATE_IDLE, LIBERR_CLOSED) ;
  // Give program up to 5 seconds to reach idle state
  for (i=1; i < 10 && lib_get_state(context, &err, &opstat) != LIBSTATE_IDLE; i++)
  {
    usleep(500000);
  }
  if ((libstate=lib_get_state(context, &err, &opstat)) != LIBSTATE_IDLE)
    fprintf(stderr, "lib330 failed to reach LIBSTATE_IDLE in sigterm_nodaemon\n");
  // Give program up to 2 seconds to reach terminated state
  lib_change_state (context, LIBSTATE_TERM, LIBERR_CLOSED) ;
  usleep(500000);
  for (i=1; i < 4 && lib_get_state(context, &err, &opstat) != LIBSTATE_TERM; i++)
    usleep(500000);

  usleep(500000);
  if (debug_arg &&
      (libstate=lib_get_state(context, &err, &opstat)) != LIBSTATE_TERM)
    fprintf(stderr, "lib330 failed to reach LIBSTATE_TERM in sigterm_nodaemon\n");
  fprintf(stderr, "Exiting q330serv\n");
  exit(EXIT_SUCCESS);
} // sigterm_nodaemon()

void ShowUsage()
{
  fprintf(stderr, "Usage: %s <configdir> [debug]\n", WHOAMI);
  fprintf(stderr, "  <configdir> holds diskloop.config and seneca.config\n");
  fprintf(stderr, "  debug for output to screen, no daemonizing\n");
}

int main (int argc, char **argv)
{
  tpoc_par poc_par ;
  string s ;
  word newverb ;
  enum tliberr err ;
  tpar_register *preg ;
  tpar_create *pcret ;
  word w ;
  char *retmsg;
  char configfile[MAXCONFIGLINELEN+2];

  STDTIME2    nowTimetag;
  STDTIME2    gpsTimetag;
  tstat_global    stat_global={0};
  tstat_gps       stat_gps={0}, old_stat_gps={0}, day_stat_gps={0};
  tstat_boom      stat_boom={0};
  tstat_pwr       stat_pwr={0};
  tstat_pll       stat_pll={0};
  int             iWriteIndex;
  int             iWriteIndexServ;
  int             iBoom;


  if (argc < 2 || argc > 3)
  {
    ShowUsage();
    exit(1);
  }

  if (argc == 3)
  {
    if (strcmp(argv[2], "debug") != 0)
    {
      ShowUsage();
      exit(1);
    }
  }

  // load diskloop.config
  if (strlen(argv[1]) + strlen("/diskloop.config") > MAXCONFIGLINELEN)
  {
    fprintf(stderr, "%s/diskloop.config > %d chars\n",
       argv[1], MAXCONFIGLINELEN);
    exit(1);
  }
  sprintf(configfile, "%s/diskloop.config", argv[1]);
  if ((retmsg=ParseDiskLoopConfig(configfile)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
    exit(1);
  }

  // Load configuration file seneca.config
  if (strlen(argv[1]) + strlen("/seneca.config") > MAXCONFIGLINELEN)
  {
    fprintf(stderr, "%s/seneca.config > %d chars\n",
       argv[1], MAXCONFIGLINELEN);
    exit(1);
  }
  sprintf(configfile, "%s/seneca.config", argv[1]);
  load_configuration (configfile) ;

  // Determine which data logger we are
  iDlg = GetDlgIndex(argv[1]);

  signal (SIGPIPE, SIG_IGN) ; // don't panic if LISS client disconnects

  // Set up to run program as a daemon
  main_pid = getpid();
  if (argc == 2)
  {
    daemonize();
  }
  else
  {
    signal(SIGINT,sigterm_nodaemon);
    signal(SIGQUIT,sigterm_nodaemon);
    signal(SIGKILL,sigterm_nodaemon);
    signal(SIGHUP,sigterm_nodaemon);
    signal(SIGTERM,sigterm_nodaemon);
    debug_arg = 1;
  }

  // Open the dispstatus shared memory region
  retmsg = MapStatusMem((void **)&mapstatus);
  if (retmsg != NULL)
  {
    if (debug_arg)
      fprintf (stderr, "Error loading dispstatus shared memory: %s\n", 
            retmsg);
    else
      syslog (LOG_ERR, "Error loading dispstatus shared memory: %s\n",
            retmsg);
    exit(1);
  }

  // Create the station thread
  preg = addr(configstruc.par_register) ;
  pcret = addr(configstruc.par_create) ;
  pcret->call_state = sen_state_callback;
  pcret->call_messages = msgs_callback;
  pcret->mini_firchain = NIL ;
  pcret->call_minidata = mini_callback ;
  if (configstruc.write_archive)
  {
    pcret->call_aminidata = amini_callback ;
  }
  else
  {
    pcret->call_aminidata = NIL ;
  }
  if (configstruc.write_mseed)
  {
    mseed_file = lib_file_open ("seneca.mseed", LFO_CREATE or LFO_WRITE) ;
  }
  else
  {
    mseed_file = INVALID_FILE_HANDLE ; /* not open */
  }
  if (configstruc.write_1sec)
  {
    pcret->call_secdata = onesec_callback ;
    onesec_file = lib_file_open ("seneca.sec", LFO_CREATE or LFO_WRITE) ;
  }
  else
  {
    pcret->call_secdata = NIL ;
    onesec_file = INVALID_FILE_HANDLE ; /* not open */
  }
  lib_create_context (addr(context), pcret) ;
  if (pcret->resp_err == LIBERR_NOERR)
  {
    cfgphase = CFG_IDLE ;
    if (configstruc.poc_port)
    {
      poc_par.poc_port = configstruc.poc_port ;
      poc_par.poc_callback = poc_handler ;
      poc_context = lib_poc_start (addr(poc_par)) ;
    }
  }
  else
  {
    context = NIL ;
    if (debug_arg)
      fprintf (stderr, "Error creating station thread: %s\n", 
            lib_get_errstr(pcret->resp_err, (string63 *)s)) ;
    else
      syslog (LOG_ERR, "Error creating station thread: %s\n", 
            lib_get_errstr(pcret->resp_err, (string63 *)s)) ;
  }

  if (context)
  {
    if (preg->opt_dynamic_ip)
      preg->q330id_address[0] = 0 ;
    err = lib_register (context, preg) ;
    if (err == LIBERR_NOERR)
      cfgphase = CFG_RUN ;
    else
    {
    if (debug_arg)
      fprintf (stderr, "Error Starting Connection: %s\n",
              lib_get_errstr(err, (string63 *)s));
    else
      syslog (LOG_ERR, "Error Starting Connection: %s\n",
              lib_get_errstr(err, (string63 *)s));
    }
  }

  printf("Infinite loop, ^C to exit\n");

  // Tell q330lib how often to get status (10 seconds was default)
  lib_request_status(context,
      make_bitmap(SRB_GLB) | make_bitmap(SRB_GST) | make_bitmap(SRB_PWR) | 
      make_bitmap(SRB_BOOM) | make_bitmap(SRB_PLL), 10);

  // Loop forever getting status information
  while (1)
  {
    pq330 q330;
    q330 = context;

    // How often we will update status
    sleep(5);
    nowTimetag = ST_GetCurrentTime2();

    // Calculate which mapstatus->ixWriteServ[iDlg] value to use for watchdog
    if (mapstatus->ixWriteServ[iDlg] != 0 && mapstatus->ixReadServ[iDlg] != 0)
      iWriteIndexServ = 0;
    else if (mapstatus->ixWriteServ[iDlg] != 1 && mapstatus->ixReadServ[iDlg] != 1)
      iWriteIndexServ = 1;
    else
      iWriteIndexServ = 2;

    // Update the watchdog timer for this q330serv instance
    mapstatus->servWatchdog[iWriteIndexServ][iDlg] = nowTimetag;
    mapstatus->ixWriteServ[iDlg] = iWriteIndexServ;

    // Store the current library state
    mapstatus->libstate[iDlg]=lib_get_state(context, &err, &opstat);

fprintf(stderr, "DEBUG counter=%d, interval=%d have_status=%x stat_request=%x\n",
q330->share.interval_counter, q330->share.status_interval, 
q330->share.have_status, q330->stat_request);

    // Make sure the status values we want are available
    if ((q330->share.have_status &
      (make_bitmap(SRB_GLB) | make_bitmap(SRB_GST) | make_bitmap(SRB_PWR) | 
       make_bitmap(SRB_BOOM) | make_bitmap(SRB_PLL)) ) !=
      (make_bitmap(SRB_GLB) | make_bitmap(SRB_GST) | make_bitmap(SRB_PWR) | 
       make_bitmap(SRB_BOOM) | make_bitmap(SRB_PLL)) )
    {
      continue;
    } // not all status bits are set

    // Get latest status
    err = lib_get_status(context, SRB_GLB, &stat_global);
    if (err != LIBERR_NOERR)
    {
      fprintf(stderr, "Global status not available\n");
      continue;
    } // error getting status

    err = lib_get_status(context, SRB_GST, &stat_gps);
    if (err != LIBERR_NOERR)
    {
      fprintf(stderr, "GPS status not available\n");
      continue;
    } // error getting status

    err = lib_get_status(context, SRB_PWR, &stat_pwr);
    if (err != LIBERR_NOERR)
    {
      fprintf(stderr, "Power status not available\n");
      continue;
    } // error getting status

    err = lib_get_status(context, SRB_BOOM, &stat_boom);
    if (err != LIBERR_NOERR)
    {
      fprintf(stderr, "Boom status not available\n");
      continue;
    } // error getting status

    err = lib_get_status(context, SRB_PLL, &stat_pll);
    if (err != LIBERR_NOERR)
    {
      fprintf(stderr, "PLL status not available\n");
      continue;
    } // error getting status

    // Make sure status information is not stale
    if (strcmp(old_stat_gps.time, stat_gps.time) == 0)
      continue;  // no changes, old information
    old_stat_gps = stat_gps;
  
    // Calculate which mapstatus->ixWriteStatus[iDlg] value to use
    if (mapstatus->ixWriteStatus[iDlg] != 0 && mapstatus->ixReadStatus[iDlg] != 0)
      iWriteIndex = 0;
    else if (mapstatus->ixWriteStatus[iDlg] != 1 && mapstatus->ixReadStatus[iDlg] != 1)
      iWriteIndex = 1;
    else
      iWriteIndex = 2;

    // Save data into unused buffer
    mapstatus->dlg[iWriteIndex][iDlg].property_tag =
        q330->share.fixed.property_tag;
    mapstatus->dlg[iWriteIndex][iDlg].sys_temp =
        stat_boom.sys_temp;
    mapstatus->dlg[iWriteIndex][iDlg].analog_voltage =
        stat_boom.amb_pos * 0.01;
    mapstatus->dlg[iWriteIndex][iDlg].input_voltage =
        stat_boom.supply * 0.15;
    mapstatus->dlg[iWriteIndex][iDlg].main_cur =
        stat_boom.main_cur;
    for (iBoom=0; iBoom < 6; iBoom++)
      mapstatus->dlg[iWriteIndex][iDlg].boom_pos[iBoom] =
          stat_boom.booms[iBoom];
    mapstatus->dlg[iWriteIndex][iDlg].stat_gps = stat_gps;
    mapstatus->dlg[iWriteIndex][iDlg].pll_state =
        stat_pll.state;
    mapstatus->dlg[iWriteIndex][iDlg].pll_time_error =
        stat_pll.time_error;
    mapstatus->dlg[iWriteIndex][iDlg].clock_quality =
        translate_clock(&q330->qclock, 
                        stat_global.clock_qual, stat_global.clock_loss);

fprintf(stderr, "DEBUG serial number %08X%08X\n",
 q330->share.fixed.sys_num[1], q330->share.fixed.sys_num[0]);
fprintf(stderr, "DEBUG KMI number %d\n", q330->share.fixed.property_tag);
fprintf(stderr, "DEBUG %s %s  sats %d/%d\n", stat_gps.date, stat_gps.time,
 stat_gps.sat_used, stat_gps.sat_view);
fprintf(stderr, "DEBUG GPS Fix Type: %s\n", stat_gps.fix) ;
fprintf(stderr, "DEBUG Clock Quality: %d%%\n",
 mapstatus->dlg[iWriteIndex][iDlg].clock_quality);

    // Update new write index so status display knows it is ready
    mapstatus->ixWriteStatus[iDlg] = iWriteIndex;

    // Log custom midnight rollover log messages, also when program first started
    if (strcmp(day_stat_gps.date, stat_gps.date) != 0)
    {
      string95 s;

      sprintf(s, "KMI Property Tag Number: %d", q330->share.fixed.property_tag);
      libmsgadd(q330, LIBMSG_FIXED, addr(s)) ;
      sprintf(s, "System Software Version: %d.%d",
              q330->share.fixed.sys_ver shr 8, q330->share.fixed.sys_ver and 255) ;
      libmsgadd(q330, LIBMSG_FIXED, addr(s)) ;
      sprintf(s, "Slave Processor Version: %d.%d", 
              q330->share.fixed.sp_ver shr 8, q330->share.fixed.sp_ver and 255) ;
      libmsgadd(q330, LIBMSG_FIXED, addr(s)) ;


      sprintf(s, "Fix Type: %s", stat_gps.fix) ;
      libmsgadd(q330, LIBMSG_GPS, addr(s)) ;
      sprintf(s, "Height: %s", stat_gps.height) ;
      libmsgadd(q330, LIBMSG_GPS, addr(s)) ;
      sprintf(s, "Latitude: %s", stat_gps.lat) ;
      libmsgadd(q330, LIBMSG_GPS, addr(s)) ;
      sprintf(s, "Longitude: %s", stat_gps.longt) ;
      libmsgadd(q330, LIBMSG_GPS, addr(s)) ;

      // For the first data logger we also want to keep the unix clock
      // synchronized to GPS within a 5 minute boundary
      if (stat_pll.state == PLL_LOCK)
      {
        // Parse out clock time
        int hour, minute, second;
        int year, month, day;
        STDTIME2 julTime;
        DELTA_T2 diffTime;
        long     diffMS;

        if (3 == sscanf(stat_gps.date, "%d/%d/%d", &day, &month, &year))
        {
          if (3 == sscanf(stat_gps.time, "%d:%d:%d", &hour, &minute, &second))
          {
            julTime = ST_CnvJulToSTD2(ST_Julian2(year, month, day));
            gpsTimetag = ST_AddToTime2(julTime, 0, hour, minute, second, 0);
            diffTime = ST_DiffTimes2(nowTimetag, gpsTimetag);
            diffMS = ST_DeltaToMS2(diffTime);
            // Check for delta > 5 minutes in tenth of millisecond units
            if ((diffMS<0?-diffMS:diffMS) > (5*60*10000))
            {
              char cmdstr[80];
              sprintf(cmdstr, "date %02d%02d%02d%02d%04d.%02d",
                      month, day, hour, minute, year, second);
              system(cmdstr);
              if (debug_arg)
                fprintf(stderr, "Unix clock changed:\n%s\n", cmdstr);
              // log the time change so analyst knows why times jumped
              sprintf(s,
"Changed Unix clock from %04d,%03d,%02d:%02d:%02d to %04d,%03d,%02d:%02d:%02d",
                      nowTimetag.year, nowTimetag.day,
                      nowTimetag.hour, nowTimetag.minute, nowTimetag.second,
                      gpsTimetag.year, gpsTimetag.day,
                      gpsTimetag.hour, gpsTimetag.minute, gpsTimetag.second);
              libmsgadd(q330, LIBMSG_GPS, addr(s)) ;
            } // time needs to be adjusted
          } // time parsed okay
        } // date parsed okay
      } // phased lock loop is locked onto GPS time

      // remember the last day that log messages where written
      day_stat_gps = stat_gps;
    }  // Midnight rollover or fresh program start
  } // loop forever

  return 0 ;
} // main()
