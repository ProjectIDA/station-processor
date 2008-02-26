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
******************************************************************************/
#define WHOAMI "q330serv"
#define VERSION_DATE  "10 November 2007"

#include "globals.h"
#include "libtypes.h"
#include "libstrucs.h"
#include "libpoc.h"
#include "callbacks.h"
#include "libsupport.h"
#include "include/diskloop.h"
#include "include/idaapi.h"
#include <syslog.h>
/*
All these includes from former seneca program are not used here
#include "pascal.h"
#include "libclient.h"
#include "libstrucs.h"
#include "libmsgs.h"
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

static void sigterm_nodaemon()
{
  int  i;
  enum tliberr err ;

  fprintf(stderr, "q330serv shutting down\n");
  lib_change_state (context, LIBSTATE_IDLE, LIBERR_CLOSED) ;
  // Give program up to 5 seconds to reach idle state
  for (i=1; i < 10 && lib_get_state(context, &err, &opstat) != LIBSTATE_IDLE; i++)
    usleep(500000);
  // Give program up to 5 seconds to reach terminated state
  lib_change_state (context, LIBSTATE_TERM, LIBERR_CLOSED) ;
  usleep(500000);
  for (i=1; i < 10 && lib_get_state(context, &err, &opstat) != LIBSTATE_TERM; i++)
    usleep(500000);

  usleep(500000);
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

  signal (SIGPIPE, SIG_IGN) ; /* don't panic if LISS client disconnects */

  // Set up to run program as a daemon
  if (argc == 2)
  {
    daemonize();
  }
  else
  {
    signal(SIGINT,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGKILL,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGHUP,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGTERM,sigterm_nodaemon); /* Die on SIGTERM */
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
    if (argc > 2)
      fprintf (stderr, "Error creating station thread: %s\n", 
            lib_get_errstr(pcret->resp_err, addr(s))) ;
    else
      syslog (LOG_ERR, "Error creating station thread: %s\n", 
            lib_get_errstr(pcret->resp_err, addr(s))) ;
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
    if (argc > 2)
      fprintf (stderr, "Error Starting Connection: %s\n",
              lib_get_errstr(err, addr(s)));
    else
      syslog (LOG_ERR, "Error Starting Connection: %s\n",
              lib_get_errstr(err, addr(s)));
    }
  }

  printf("Infinite loop, ^C to exit\n");
  while (1) sleep(1);
  return 0 ;
} // main()
