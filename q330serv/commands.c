/*   Seneca Command Handler
     Copyright 2006 Certified Software Corporation

    This file is part of Seneca

    Seneca is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Seneca is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Seneca; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Edit History:
   Ed Date       By  Changes
   -- ---------- --- ---------------------------------------------------
    0 2006-10-02 rdr Created
    1 2006-10-29 rdr Remove spurious "addr" functions when using address of
                     callback functions.
    2 2006-11-30 rdr Add handling of module versions command.
    3 2007-03-07 rdr Add call to WSACleanup.
    4 2007-08-04 rdr Add conditionals to remove network support.
    5 2007-08-07 rdr Add settings of continuity cache timer.
    6 2008-01-11 rdr Add handling for F and I commands in run state.
    7 2009-02-09 rdr Add support for EP status.
    8 2009-08-02 rdr Add setting max DSS memory.
*/
#ifndef globals_h
#include "globals.h"
#endif
#ifndef config_h
#include "config.h"
#endif
#ifndef callbacks_h
#include "callbacks.h"
#endif
#ifndef callbacks_h
#include "callbacks.h"
#endif
#ifndef dump_h
#include "dump.h"
#endif
#ifndef libpoc_h
#include "libpoc.h"
#endif
#ifndef libsupport_h
#include "libsupport.h"
#endif
#ifndef libmsgs_h
#include "libmsgs.h"
#endif

#ifndef OMIT_SEED
#ifndef libnetserv_h
#include "libnetserv.h"
#endif
#endif

char get_command_char (void)
begin
  string s ;

  repeat
    printf ("Command: ") ;
    getline(line) ;
    sscanf (line, "%s", s) ;
    if (strlen(s) >= 1)
      then
        return toupper(s[0]) ;
  until FALSE) ;
end

void run_commands (void)
begin
#ifndef OMIT_NETWORK
  tpoc_par poc_par ;
#endif
  string s ;
  word newverb ;
  enum tliberr err ;
  tpar_register *preg ;
  tpar_create *pcret ;
  word w ;

  preg = addr(configstruc.par_register) ;
  pcret = addr(configstruc.par_create) ;
  repeat
    switch (cfgphase) begin
      case CFG_IDLE :
        show_idle_menu () ;
        switch (get_command_char()) begin
          case 'C' :
            if (context == NIL)
              then
                cfgphase = CFG_CREATE ;
            break ;
          case 'D' :
            if (context)
              then
                dump_messages () ;
            break ;
          case 'E' :
            cfgphase = CFG_REG ;
            break ;
          case 'F' :
            cfgphase = CFG_FILE ;
            break ;
          case 'M' :
            show_modules () ;
            break ;
          case 'O' :
            if (context)
              then
                show_opstat () ;
            break ;
          case 'P' :
            if (context)
              then
                begin
                  if (preg->opt_dynamic_ip)
                    then
                      preg->q330id_address[0] = 0 ;
                  pcret->resp_err = lib_unregistered_ping (context, preg) ;
                  if (pcret->resp_err != LIBERR_NOERR)
                    then
                      printf ("Error Pinging Station: %s\n", lib_get_errstr(pcret->resp_err, addr(s))) ;
                end
            break ;
          case 'Q' :
#ifndef OMIT_NETWORK
            if (poc_context)
              then
                begin
                  printf ("POC Receiver Stopped\n") ;
                  lib_poc_stop (poc_context) ;
                end
#endif
            if (context)
              then
                lib_destroy_context (addr(context)) ;
#ifdef X86_WIN32
            WSACleanup () ;
#endif
            return ;
          case 'S' :
            if (context)
              then
                begin
                  if (preg->opt_dynamic_ip)
                    then
                      preg->q330id_address[0] = 0 ;
#ifdef TEST_MD5
                  preg->host_ctrlport = 1087 ;
#endif
                  err = lib_register (context, preg) ;
                  if (err == LIBERR_NOERR)
                    then
                      cfgphase = CFG_RUN ;
                    else
                      printf ("Error Starting Connection: %s\n", lib_get_errstr(err, addr(s))) ;
                end
            break ;
          case 'T' :
            if (context == NIL)
              then
                begin
                  pcret->call_state = sen_state_callback ;
                  pcret->call_messages = msgs_callback ;
                  pcret->call_baler = NIL ;
#ifndef OMIT_SEED
                  pcret->mini_firchain = NIL ;
                  fowner.call_fileacc = sen_file_callback ;
                  fowner.station_ptr = NIL ; /* Not required for seneca, only one station */
                  pcret->file_owner = addr(fowner) ;
                  pcret->call_minidata = mini_callback ;
                  if (configstruc.write_archive)
                    then
                      pcret->call_aminidata = amini_callback ;
                    else
                      pcret->call_aminidata = NIL ;
                  if (configstruc.write_mseed)
                    then
                      mseed_file = lib_file_open (NIL, "seneca.mseed", LFO_CREATE or LFO_WRITE) ;
                    else
                      mseed_file = INVALID_FILE_HANDLE ; /* not open */
#endif
                  if (configstruc.write_1sec)
                    then
                      begin
                        pcret->call_secdata = onesec_callback ;
                        onesec_file = lib_file_open (NIL, "seneca.sec", LFO_CREATE or LFO_WRITE) ;
                      end
                    else
                      begin
                        pcret->call_secdata = NIL ;
                        onesec_file = INVALID_FILE_HANDLE ; /* not open */
                      end
                  lib_create_context (addr(context), pcret) ;
                  if (pcret->resp_err == LIBERR_NOERR)
                    then
                      begin
                        printf ("Station Thread Created\n") ;
                        cfgphase = CFG_IDLE ;
#ifndef OMIT_NETWORK
                        if (configstruc.poc_port)
                          then
                            begin
                              poc_par.poc_port = configstruc.poc_port ;
                              poc_par.poc_callback = poc_handler ;
                              poc_context = lib_poc_start (addr(poc_par)) ;
                              printf ("POC Receiver Started\n") ;
                            end
#endif
                      end
                    else
                      begin
                        context = NIL ;
                        printf ("Error creating station thread: %s\n", lib_get_errstr(pcret->resp_err, addr(s))) ;
                      end
                end
              else
                lib_change_state (context, LIBSTATE_TERM, LIBERR_CLOSED) ;
            break ;
        end
        break ;
      case CFG_CREATE :
        show_create_pars () ;
        switch (get_command_char()) begin
          case 'A' :
            get_t64 ("Serial number: ", addr(pcret->q330id_serial)) ; /* serial number */
            break ;
          case 'B' :
            w = get_word ("Data port (1-4): ") ; /* Data port, use LP_TEL1 .. LP_TEL4 */
            w = w - 1 ;
            pcret->q330id_dataport = w ;
            break ;
          case 'C' :
            get_string("Station name (3-5 characters): ", 5, addr(pcret->q330id_station)) ; /* initial station name */
            lib330_upper(addr(pcret->q330id_station)) ;
            break ;
          case 'D' :
            pcret->host_timezone = get_integer ("Timezone offset in seconds: ") ; /* seconds offset of host time. Initial value if auto-adjust enabled */
            break ;
          case 'E' :
            get_string("Host software name: ", 95, addr(pcret->host_software)) ; /* host software type and version */
            break ;
          case 'F' :
            get_string("Continuity file name (Null to disable): ", 250, addr(pcret->opt_contfile)) ; /* continuity file path and name, null for no continuity */
            break ;
          case 'G' :
            pcret->opt_verbose = get_verbosity () ; /* VERB_xxxx bitmap */
            break ;
#ifndef OMIT_SEED
          case 'H' :
            pcret->amini_512highest = get_frequency ("Highest frequency for incremental update: ") ; /* rates up to this value are updated every 512 bytes */
            break ;
          case 'I' :
            pcret->mini_embed = get_yesno("Embed blockettes (Y/n): ", TRUE) ; /* 1 == embed calibration and event blockettes into data */
            break ;
          case 'J' :
            pcret->mini_separate = get_yesno("Separate blockettes (y/N): ", FALSE) ; /* 1 == generate separate calibration and event records */
            break ;
#endif
          case 'K' :
            pcret->opt_zoneadjust = get_yesno("Calculate host timezone automatically (Y/n): ", TRUE) ; /* calculate host's timezone automatically */
            break ;
          case 'L' : pcret->opt_secfilter = get_onesec_filter () ; /* OSF_xxx bits */
            break ;
#ifndef OMIT_SEED
          case 'M' :
            pcret->opt_minifilter = get_seed_filter ("512 byte MiniSeed data filter: ") ; /* OMF_xxx bits */
            break ;
          case 'N' :
            pcret->opt_aminifilter = get_seed_filter ("Archival MiniSeed data filter: ") ; /* OMF_xxx bits */
            break ;
#endif
          case 'S' :
            save_configuration () ;
            break ;
          case 'X' :
            cfgphase = CFG_IDLE ;
            break ;
        end
        break ;
      case CFG_REG :
        show_register_pars () ;
        switch (get_command_char()) begin
          case 'A' :
            get_t64("Authentication code: ", addr(preg->q330id_auth)) ; /* authentication code */
            break ;
          case 'B' :
            get_string("Q330 IP address: ", 250, addr(preg->q330id_address)) ; /* domain name or IP address in dotted decimal */
            break ;
          case 'C' :
            preg->q330id_baseport = get_word ("Q330 base port: ") ; /* base UDP port number */
            break ;
          case 'D' :
            preg->host_mode = get_hostmode () ;
            break ;
          case 'E' :
            get_string("Host interface: ", 250, addr(preg->host_interface)) ; /* ethernet or serial port path name */
            break ;
          case 'F' :
            preg->host_mincmdretry = get_timeout("Host minimum command timeout: ") ; /* minimum command retry timeout */
            break ;
          case 'G' :
            preg->host_maxcmdretry = get_timeout("Host maximum command timeout: ") ; /* maximum command retry timeout */
            break ;
          case 'H' :
            preg->host_ctrlport = get_word("Host control port (0 for automatic): ") ; /* set non-zero to use specified UDP port at host end */
            break ;
          case 'I' :
            preg->host_dataport = get_word("Host data port (0 for automatic): ") ; /* set non-zero to use specified UDP port at host end */
            break ;
#ifndef OMIT_SERIAL
          case '1' :
            preg->serial_flow = get_yesno("Host serial port hardware flow control (Y/n): ", TRUE) ; /* 1 == hardware flow control */
            break ;
          case '2' :
            preg->serial_baud = get_integer("Host serial port baud: ") ; /* in bps */
            break ;
          case '3' :
            preg->serial_hostip = get_ip_address("Host serial IP address: ") ; /* IP address to identify host */
            break ;
#endif
          case 'J' :
            preg->opt_dynamic_ip = get_yesno("Q330 has dynamic IP address (y/N): ", FALSE) ; /* 1 == dynamic IP address */
            break ;
          case 'K' :
            preg->opt_hibertime = get_word("Hibernate time in minutes: ") ; /* hibernate time in minutes if non-zero */
            break ;
          case 'L' :
            preg->opt_conntime = get_word("Maximum connection time in minutes: ") ; /* maximum connection time in minutes if non-zero */
            break ;
          case 'M' :
            preg->opt_connwait = get_word("Connection wait time in minutes: ") ; /* wait this many minutes after connection time or buflevel shutdown */
            break ;
          case 'N' :
            preg->opt_regattempts = get_word("Maximum registration attempts before hibernation: ") ; /* maximum registration attempts before hibernate if non-zero */
            break ;
          case 'O' :
            preg->opt_ipexpire = get_word("Dynamic IP address expiration in minutes: ") ; /* dyanmic IP address expires after this many minutes since last POC */
            break ;
          case 'P' :
            preg->opt_buflevel = get_word("Buffer level to stop connection: ") ; /* terminate connection when buffer level reaches this value if non-zero */
            break ;
          case 'Q' :
            preg->opt_q330_cont = get_word("Minutes before writing Q330 continuity to file: ") ;
            break ;
          case 'R' :
            preg->opt_dss_memory = get_word("Maximum DSS memory in KB: ") ;
            break ;
          case 'S' :
            save_configuration () ;
            break ;
          case 'X' :
            cfgphase = CFG_IDLE ;
            break ;
        end
        break ;
      case CFG_FILE :
        show_file_menu () ;
        switch (get_command_char()) begin
          case 'A' :
            configstruc.write_1sec = get_yesno("Enable writing one second data to seneca.sec (y/N): ", FALSE) ; /* non-zero to write to seneca.sec */
            break ;
#ifndef OMIT_SEED
          case 'B' :
            configstruc.write_mseed = get_yesno("Enable writing 512 byte MiniSeed to seneca.mseed (y/N): ", FALSE) ; /* non-zero to write to seneca.mseed */
            break ;
          case 'C' :
            configstruc.write_archive = get_yesno("Enable writing 4096 byte MiniSeed to individual files (y/N): ", FALSE) ; /* non-zero to write to mseedll.sss such as mseed__.BHZ */
            break ;
          case 'D' :
            configstruc.run_netserver = get_yesno("Enable NetServer if configured in tokens (y/N): ", FALSE) ; /* non-zero to open netserver if configured in tokens */
            break ;
#endif
          case 'E' :
            configstruc.poc_port = get_word("POC Receiver port (zero to disable, standard is 2254): ") ; /* non-zero to run POC receiver */
            break ;
          case 'S' :
            save_configuration () ;
            break ;
          case 'X' :
            cfgphase = CFG_IDLE ;
            break ;
        end
        break ;
      case CFG_RUN :
        show_run_menu () ;
        switch (get_command_char()) begin
          case 'A' :
            arp_status () ;
            break ;
          case 'C' :
            lib_abort_command (context) ;
            break ;
          case 'D' :
            dump_messages () ;
            break ;
          case 'E' :
            if (fixed.flags and FF_EP)
              then
                ep_status () ;
            break ;
          case 'G' :
            send_tunneled_request () ;
            break ;
#ifndef OMIT_SEED
          case 'F' :
            if (current_state == LIBSTATE_RUN)
              then
                lib_flush_data (context) ;
            break ;
          case 'H' :
            if (current_state == LIBSTATE_RUN)
              then
                show_detectors () ;
            break ;
          case 'I' :
            if (current_state == LIBSTATE_RUN)
              then
                lib_set_freeze_timer (context, 30) ;
            break ;
          case 'L' :
            show_lcqs () ;
            break ;
#endif
          case 'M' :
            show_modules () ;
            break ;
          case 'O' :
            screen_busy = TRUE ;
            show_opstat () ;
            screen_busy = FALSE ;
            break ;
          case 'P' :
            pingreq.pingtype = 0 ; /* normal ping */
            pingreq.pingopt = 0 ;
            pingreq.pingreqmap = 0 ; /* not used */
            lib_ping_request (context, addr(pingreq)) ; /* registered ping */
            break ;
          case 'Q' :
            if (context == NIL)
              then
                begin /* invalid registration moved me to idle and then I went to terminate */
#ifndef OMIT_NETWORK
                  if (poc_context)
                    then
                      begin
                        printf ("POC Receiver Stopped\n") ;
                        lib_poc_stop (poc_context) ;
                      end
#endif
#ifdef X86_WIN32
                  WSACleanup () ;
#endif
                  return ;
                end
            break ;
          case 'S' :
            lib_change_state (context, LIBSTATE_IDLE, LIBERR_CLOSED) ;
            cfgphase = CFG_IDLE ;
            break ;
          case 'U' :
            screen_busy = TRUE ;
            printf ("Message: ") ;
            getline(line) ;
            lib_send_usermessage (context, addr(line)) ;
            screen_busy = FALSE ;
            break ;
          case 'V' :
            screen_busy = TRUE ;
            newverb = get_verbosity () ;
            configstruc.par_create.opt_verbose = lib_change_verbosity (context, newverb) ;
            screen_busy = FALSE ;
            break ;
        end
        break ;
    end
  until FALSE) ;
end
