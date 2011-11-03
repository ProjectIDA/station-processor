/*   Seneca configuration routines
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
    0 2006-10-01 rdr Created
    1 2006-10-29 rdr Always update configuration with current Seneca version.
    2 2006-11-30 rdr Add Module versions menu items.
    3 2007-07-19 rdr Fix show_rate for sub-hertz.
    4 2007-08-07 rdr Add settings of continuity cache timer.
    5 2008-01-11 rdr Add F and I commands in run mode.
    6 2009-07-30 rdr uppercase routine removed.
    7 2009-08-02 rdr Add DSS memory menu item.
*/
#ifndef globals_h
#include "globals.h"
#endif
#ifndef config_h
#include "config.h"
#endif
#ifndef libclient_h
#include "libclient.h"
#endif
#ifndef libmsgs_h
#include "libmsgs.h"
#endif
#ifndef libsupport_h
#include "libsupport.h"
#endif

void getline (string *s)
begin
#ifndef X86_WIN32
  integer i ;
#endif

#ifdef X86_WIN32
  gets(s) ;
#else
  fgets(s, 250, stdin) ;
  for (i = 0 ; i < strlen(s) ; i++)
    if ((*s)[i] < 0x20)
      then
        begin
          (*s)[i] = 0 ; /* terminate at first control character */
          break ;
        end
#endif
end

#ifdef X86_WIN32
static longint gettz (void)
begin
  TIME_ZONE_INFORMATION tzi ;

  GetTimeZoneInformation(addr(tzi)) ;
  return tzi.Bias * 60 ;
end
#else
static longint gettz (void)
begin
  struct timeval tv ;
  struct timezone tz ;

  if (gettimeofday(addr(tv), addr(tz)))
    then
      return 0 ; /* error, just use zero */
    else
      return -(tz.tz_minuteswest * 60) ;
end
#endif

static boolean yes (boolean default_yes)
begin
  string7 s ;
  boolean result ;

  sscanf(line, "%1s", s) ;
  lib330_upper (s) ;
  result = default_yes ;
  if (s[0])
    then
      if (s[0] == 'Y')
        then
          result = TRUE ;
      else if (s[0] == 'N')
        then
          result = FALSE ;
  return result ;
end

static char *show_verbosity (word verb, string *result)
begin
  string s ;

  s[0] = 0 ;
  if (verb and VERB_SDUMP)
    then
      strcpy(s, " Status-Dump") ;
  if (verb and VERB_RETRY)
    then
      strcat(s, " Command-Retries") ;
  if (verb and VERB_REGMSG)
    then
      strcat(s, " Registration-Msgs") ;
  if (verb and VERB_LOGEXTRA)
    then
      strcat(s, " Verbose") ;
  if (verb and VERB_AUXMSG)
    then
      strcat(s, " Server-Msgs") ;
  if (verb and VERB_PACKET)
    then
      strcat(s, " Packet-Debug") ;
  strcpy(result, s) ;
  return result ;
end

static char *show_rate (integer rate, string *result)
begin
  string s ;

  if (rate > 0)
    then
      sprintf(s, "%d", rate) ;
    else
      sprintf(s, "%5.3f", fabs(1.0 / rate)) ;
  strcpy(result, s) ;
  return result ;
end

static char *show_yesno (word val, string *result)
begin

  if (val)
    then
      strcpy(result, "Yes") ;
    else
      strcpy(result, "No") ;
  return result ;
end

void load_configuration (void)
begin
  tfile_handle cf ;
  boolean good ;
  tpar_register *preg ;
  tpar_create *pcret ;
#ifndef OMIT_SERIAL
  boolean isd ;
#endif

  good = FALSE ;
  cf = lib_file_open (NIL, "seneca.config", LFO_OPEN or LFO_READ) ;
  if (cf != INVALID_FILE_HANDLE)
    then
      begin
        printf ("Load configuration from Seneca.config? (Y/n) : ") ;
        getline(line) ;
        if (yes (TRUE))
          then
            begin
              lib_file_read (NIL, cf, addr(configstruc), sizeof(tconfigstruc)) ;
              if (configstruc.cfg_ver == CONFIG_VERSION)
                then
                  good = TRUE ;
                else
                  printf ("Configuration file version is not current, values set to defaults\n") ;
            end
        lib_file_close (NIL, cf) ;
      end
  if (lnot good)
    then
      begin /* initialize to reasonable values */
        memset (addr(configstruc), 0, sizeof(tconfigstruc)) ;
        configstruc.cfg_ver = CONFIG_VERSION ;
        preg = addr(configstruc.par_register) ;
        pcret = addr(configstruc.par_create) ;
        strcpy(pcret->q330id_station, "SENCA") ;
        pcret->host_timezone = gettz () ;
        strcpy(pcret->opt_contfile, "cont/cont.bin") ;
        pcret->opt_verbose = VERB_REGMSG ;
  #ifndef OMIT_SEED
        pcret->amini_exponent = 12 ;
        pcret->amini_512highest = 20 ;
        pcret->mini_embed = 1 ;
        pcret->mini_separate = 0 ;
        pcret->opt_minifilter = OMF_NETSERV ;
        pcret->opt_aminifilter = OMF_ALL ;
  #endif
        strcpy(preg->q330id_address, "10.11.12.13") ;
        preg->q330id_baseport = 5330 ;
        preg->host_mode = HOST_ETH ;
        strcpy(preg->host_interface, "") ;
        preg->host_mincmdretry = 10 ;
        preg->host_maxcmdretry = 100 ;
  #ifndef OMIT_SERIAL
        preg->serial_flow = 1 ;
        preg->serial_baud = 19200 ;
        preg->serial_hostip = getip ("10.1.2.3", addr(isd)) ;
  #endif
        pcret->opt_zoneadjust = 1 ;
        pcret->opt_secfilter = OSF_DATASERV ;
      end
  sprintf(configstruc.par_create.host_software, "Seneca Version %s", SEN_VER) ;
end

void save_configuration (void)
begin
  tfile_handle cf ;

  cf = lib_file_open (NIL, "seneca.config", LFO_CREATE or LFO_WRITE) ;
  if (cf == INVALID_FILE_HANDLE)
    then
      begin
        printf ("Could could not create Seneca.config\n") ;
        return ;
      end
  lib_file_write (NIL, cf, addr(configstruc), sizeof(tconfigstruc)) ;
  lib_file_close (NIL, cf) ;
end

boolean parse_t64 (string *s, t64 *value)
begin
  integer good, len ;
  string15 sh, sl ;

  (*value)[0] = 0 ;
  (*value)[1] = 0 ;
  len = strlen(s) ;
  if (len > 8)
    then
      begin
        strncpy(sh, s, len - 8) ; /* first n - 8 characters */
        sh[len - 8] = 0 ; /* null terminator */
        memcpy(addr(sl), addr((*s)[len - 8]), 9) ; /* last 8 characters and terminator */
#ifdef ENDIAN_LITTLE
        good = sscanf(sh, "%x", addr((*value)[1])) ;
        good = good + sscanf(sl, "%x", addr((*value)[0])) ;
#else
        good = sscanf(sh, "%x", addr((*value)[0])) ;
        good = good + sscanf(sl, "%x", addr((*value)[1])) ;
#endif
        if (good != 2)
          then
            return FALSE ;
      end
  else if (s[0])
    then
      begin
#ifdef ENDIAN_LITTLE
        good = sscanf(s, "%x", addr((*value)[0])) ;
#else
        good = sscanf(s, "%x", addr((*value)[1])) ;
#endif
        if (good != 1)
          then
            return FALSE ;
      end
  return TRUE ;
end

void get_t64 (string *prompt, t64 *result)
begin
  t64 sn ;

  repeat
    printf ("%s", prompt) ;
    getline(line) ;
    if (parse_t64 (addr(line), addr(sn)))
      then
        begin
          memcpy(result, addr(sn), sizeof(t64)) ;
          return ;
        end
      else
        printf ("INVALID SERIAL NUMBER\n") ;
  until FALSE) ;
end

word get_word (string *prompt)
begin
  integer i ;
  integer good ;

  repeat
    printf ("%s", prompt) ;
    getline(line) ;
    good = sscanf (line, "%u", addr(i)) ;
    if (good == 1)
      then
        return i ;
      else
        printf ("INVALID WORD VALUE\n") ;
  until FALSE) ;
end

integer get_integer (string *prompt)
begin
  integer i ;
  integer good ;

  repeat
    printf ("%s", prompt) ;
    getline(line) ;
    good = sscanf (line, "%d", addr(i)) ;
    if (good == 1)
      then
        return i ;
      else
        printf ("INVALID INTEGER VALUE\n") ;
  until FALSE) ;
end

longword get_ip_address (string *prompt)
begin
  string s ;
  boolean isd ;
  longword ip ;

  repeat
    printf ("%s", prompt) ;
    getline(line) ;
    sscanf (line, "%s", s) ;
    ip = getip (addr(s), addr(isd)) ;
    if (ip != 0xFFFFFFFF)
      then
        return ip ;
      else
        printf ("INVALID IP ADDRESS\n") ;
  until FALSE) ;
end

char *get_string (string *prompt, integer maxlth, string *result)
begin
  string s ;

  repeat
    printf ("%s", prompt) ;
    getline(s) ;
    if ((integer)strlen(s) <= maxlth)
      then
        begin
          strcpy(result, s) ;
          return result ;
        end
      else
        printf ("MAXIMUM LENGTH IS %d CHARACTERS\n", maxlth) ;
  until FALSE) ;
end

static char *parse (string *in_line, integer arg, string *result)
begin
#define SPACE ' '
  integer idx, len ;
  string work ;
  char c[2] ;

  idx = 0 ;
  len = strlen(in_line) ;
  while (arg > 0)
    begin
      work[0] = 0 ;
      while ((idx < len) land ((*in_line)[idx] == SPACE))
        inc(idx) ;
      while ((idx < len) land ((*in_line)[idx] != SPACE))
        begin
          c[0] = (*in_line)[idx] ;
          c[1] = 0 ;
          strcat(work, c) ;
          inc(idx) ;
        end
      dec(arg) ;
    end
  strcpy(result, work) ;
  return result ;
end

word get_verbosity (void)
begin
  string s, s1 ;
  integer i ;
  word w ;
  boolean badval ;

  repeat
    printf ("Separate values to include with spaces, valid values are:\n") ;
    printf (" SD, CR, RM, VB, SM, and PD which represent Status-Dump, Command-Retries,\n") ;
    printf ("Registration-Msgs, Verbose, Server-Msgs, and Packet-Debug\n") ;
    printf ("Values: ") ;
    getline(s) ;
    badval = FALSE ;
    w = 0 ;
    for (i = 1 ; i <= 6 ; i++)
      begin
        parse (addr(s), i, addr(s1)) ;
        lib330_upper(s1) ;
        if (strcmp(s1, "SD") == 0)
          then
            w = w or VERB_SDUMP ;
        else if (strcmp(s1, "CR") == 0)
          then
            w = w or VERB_RETRY ;
        else if (strcmp(s1, "RM") == 0)
          then
            w = w or VERB_REGMSG ;
        else if (strcmp(s1, "VB") == 0)
          then
            w = w or VERB_LOGEXTRA ;
        else if (strcmp(s1, "SM") == 0)
          then
            w = w or VERB_AUXMSG ;
        else if (strcmp(s1, "PD") == 0)
          then
            w = w or VERB_PACKET ;
        else if (strcmp(s1, "") == 0)
          then
            break ;
          else
            begin
              printf ("Unknown Value '%s'\n", s1) ;
              badval = TRUE ;
              break ;
            end
      end
    if (lnot badval)
      then
        return w ;
  until FALSE) ;
end

boolean get_yesno (string *prompt, boolean default_yes)
begin

  printf (prompt) ;
  getline(line) ;
  return yes(default_yes) ;
end

integer get_frequency (string *prompt)
begin
  float r ;
  integer good ;

  repeat
    printf (prompt) ;
    getline(line) ;
    good = sscanf(line, "%f", addr(r)) ;
    if (good == 1)
      then
        begin
          if (r >= 0.99)
            then
              return lib_round(r) ;
            else
              return -lib_round(1.0 / r) ;
        end
      else
        printf ("INVALID FREQUENCY\n") ;
  until FALSE) ;
end

static char *show_hostmode (enum thost_mode mode, string *result)
begin

  if (mode == HOST_SER)
    then
      strcpy(result, "Serial") ;
    else
      strcpy(result, "Ethernet") ;
  return result ;
end

enum thost_mode get_hostmode (void)
begin
  string s ;

  repeat
    printf ("Host Mode (ETH or SER): ") ;
    getline(line) ;
    sscanf (line, "%s", s) ;
    lib330_upper(s) ;
    if (strcmp(s, "ETH") == 0)
      then
        return HOST_ETH ;
    else if (strcmp(s, "SER") == 0)
      then
        return HOST_SER ;
      else
        printf ("INVALID HOST MODE\n") ;
  until FALSE) ;
end

static char *show_timeout (word val, string *result)
begin

  sprintf(result, "%3.1f", val * 0.1) ;
  return result ;
end

word get_timeout (string *prompt)
begin
  float r ;
  integer good ;

  repeat
    printf (prompt) ;
    getline(line) ;
    good = sscanf(line, "%f", addr(r)) ;
    if (good == 1)
      then
        return (r * 10 + 0.5) ;
      else
        printf ("INVALID TIMEOUT\n") ;
  until FALSE) ;
end

static char *show_onesec_filter (word val, string *result)
begin
  string s ;

  if (val and OSF_ALL)
    then
      strcpy(s, "All") ;
    else
      s[0] = 0 ;
  if (val and OSF_DATASERV)
    then
      strcat(s, " DataServ") ;
  if (val and OSF_1HZ)
    then
      strcat(s, " 1Hz") ;
  strcpy(result, s) ;
  return result ;
end

word get_onesec_filter (void)
begin
  string s ;
  string s1 ;
  integer i ;
  word w ;
  boolean badval ;


  repeat
    printf ("Separate values to include with spaces, valid values are:\n") ;
    printf ("ALL, DATA, and 1HZ : ") ;
    getline(s) ;
    badval = FALSE ;
    w = 0 ;
    for (i = 1 ; i <= 3 ; i++)
      begin
        parse(addr(s), i, addr(s1)) ;
        lib330_upper(s1) ;
        if (strcmp(s1, "ALL") == 0)
          then
            w = w or OSF_ALL ;
        else if (strcmp(s1, "DATA") == 0)
          then
            w = w or OSF_DATASERV ;
        else if (strcmp(s1, "1HZ") == 0)
          then
            w = w or OSF_1HZ ;
        else if (strcmp(s1, "") == 0)
          then
            break ;
          else
            begin
              printf ("Unknown Value '%s'\n", s1) ;
              badval = TRUE ;
              break ;
            end
      end
    if (lnot badval)
      then
        return w ;
  until FALSE) ;
end

#ifndef OMIT_SEED
char *show_seed_filter (word val, string *result)
begin
  string s ;

  if (val and OMF_ALL)
    then
      strcpy(s, "All") ;
    else
      s[0] = 0 ;
  if (val and OMF_NETSERV)
    then
      strcat(s, " Netserv") ;
  if (val and OMF_CFG)
    then
      strcat(s, " Config") ;
  if (val and OMF_TIM)
    then
      strcat(s, " Timing") ;
  if (val and OMF_MSG)
    then
      strcat(s, " Messages") ;
  strcpy(result, s) ;
  return result ;
end

word get_seed_filter (string *prompt)
begin
  string s, s1 ;
  integer i ;
  word w ;
  boolean badval ;

  repeat
    printf ("Separate values to include with spaces, valid values are:\n") ;
    printf ("ALL, NETSRV, CFG, TIM, and MSG\n") ;
    printf (prompt) ;
    getline(s) ;
    badval = FALSE ;
    w = 0 ;
    for (i = 1 ; i <= 5 ; i++)
      begin
        parse (addr(s), i, addr(s1)) ;
        lib330_upper(s1) ;
        if (strcmp(s1, "ALL") == 0)
          then
            w = w or OMF_ALL ;
        else if (strcmp(s1, "NETSRV") == 0)
          then
            w = w or OMF_NETSERV ;
        else if (strcmp(s1, "CFG") == 0)
          then
            w = w or OMF_CFG ;
        else if (strcmp(s1, "TIM") == 0)
          then
            w = w or OMF_TIM ;
        else if (strcmp(s1, "MSG") == 0)
          then
            w = w or OMF_MSG ;
        else if (strcmp(s1, "") == 0)
          then
            break ;
          else
            begin
              printf ("Unknown Value '%s'\n", s1) ;
              badval = TRUE ;
              break ;
            end
      end
    if (lnot badval)
      then
        return w ;
  until FALSE) ;
end
#endif

void show_idle_menu (void)
begin

  printf ("\n") ;
  if (context == NIL)
    then
      printf ("C) Edit creation parameters\n") ;
    else
      printf ("D) Dump Messages\n") ;
  printf ("E) Edit registration parameters\n") ;
  printf ("F) Edit File & Server parameters\n") ;
  printf ("M) Module Versions\n") ;
  if (context)
    then
      begin
        printf ("O) Operational Status\n") ;
        printf ("P) Ping Q330\n") ;
        printf ("S) Start connection to station\n") ;
        printf ("T) Terminate station Thread\n") ;
      end
    else
      begin
        printf ("Q) Quit Seneca\n") ;
        printf ("T) Create station Thread\n") ;
      end
end

void show_create_pars (void)
begin
  tpar_create *pcret ;
  string s ;

  pcret = addr(configstruc.par_create) ;
  printf ("\n") ;
  printf ("A) Serial number: %s\n", showsn(addr(pcret->q330id_serial), addr(s))) ; /* serial number */
  printf ("B) Data port: %u\n", pcret->q330id_dataport + 1) ; /* Data port, use LP_TEL1 .. LP_TEL4 */
  printf ("C) Station name: %s\n", pcret->q330id_station) ; /* initial station name */
  printf ("D) Host timezone offset in seconds: %d\n", pcret->host_timezone) ; /* seconds offset of host time. Initial value if auto-adjust enabled */
  printf ("E) Host software: %s\n", pcret->host_software) ; /* host software type and version */
  printf ("F) Continuity file: %s\n", pcret->opt_contfile) ; /* continuity file path and name, null for no continuity */
  printf ("G) Verbosity: %s\n", show_verbosity (pcret->opt_verbose, addr(s))) ; /* VERB_xxxx bitmap */
#ifndef OMIT_SEED
  printf ("   Archival miniseed exponent: %u\n", pcret->amini_exponent) ; /* 2**exp size of archival miniseed, range of 9 to 14 */
  printf ("H) Archival miniseed incremental update max. frequency: %s\n", show_rate(pcret->amini_512highest, addr(s))) ; /* rates up to this value are updated every 512 bytes */
  printf ("I) Embed cal/event blockettes into data: %s\n", show_yesno(pcret->mini_embed, addr(s))) ; /* 1 == embed calibration and event blockettes into data */
  printf ("J) Separate cal/event records: %s\n", show_yesno(pcret->mini_separate, addr(s))) ; /* 1 == generate separate calibration and event records */
#endif
  printf ("K) Calculate host timezone automatically: %s\n", show_yesno(pcret->opt_zoneadjust, addr(s))) ; /* calculate host"s timezone automatically */
  show_onesec_filter(pcret->opt_secfilter, addr(s)) ;
  printf ("L) One second data filter: %s\n", s) ; /* OSF_xxx bits */
#ifndef OMIT_SEED
  show_seed_filter(pcret->opt_minifilter, addr(s)) ;
  printf ("M) 512 byte MiniSeed data filter: %s\n", s) ; /* OMF_xxx bits */
  show_seed_filter(pcret->opt_aminifilter, addr(s)) ;
  printf ("N) Archival MiniSeed data filter: %s\n", s) ; /* OMF_xxx bits */
#endif
  printf ("S) Save configuration\n") ;
  printf ("X) Return to idle menu\n") ;
end

void show_register_pars (void)
begin
  tpar_register *preg ;
  string s ;

  preg = addr(configstruc.par_register) ;
  printf ("\n") ;
  printf ("A) Authentication code: %s\n", showsn(preg->q330id_auth, addr(s))) ; /* authentication code */
  printf ("B) Q330 IP address: %s\n", preg->q330id_address) ; /* domain name or IP address in dotted decimal */
  printf ("C) Q330 base port: %u\n", preg->q330id_baseport) ; /* base UDP port number */
  show_hostmode(preg->host_mode, addr(s)) ;
  printf ("D) Host mode: %s\n", s) ;
  printf ("E) Host interface: %s\n", preg->host_interface) ; /* ethernet or serial port path name */
  show_timeout(preg->host_mincmdretry, addr(s)) ;
  printf ("F) Host minimum command timeout: %s\n", s) ; /* minimum command retry timeout */
  show_timeout(preg->host_maxcmdretry, addr(s)) ;
  printf ("G) Host maximum command timeout: %s\n", s) ; /* maximum command retry timeout */
  printf ("H) Host control port: %u\n", preg->host_ctrlport) ; /* set non-zero to use specified UDP port at host end */
  printf ("I) Host data port: %u\n", preg->host_dataport) ; /* set non-zero to use specified UDP port at host end */
#ifndef OMIT_SERIAL
  if (preg->host_mode == HOST_SER)
    then
      begin
        printf ("1) Host serial port hardware flow control: %s\n", show_yesno(preg->serial_flow, addr(s))) ; /* 1 == hardware flow control */
        printf ("2) Host serial port baud: %u\n", preg->serial_baud) ; /* in bps */
        printf ("3) Host serial IP address: %s\n", showdot(preg->serial_hostip, addr(s))) ; /* IP address to identify host */
      end
#endif
  printf ("J) Q330 has dynamic IP address: %s\n", show_yesno(preg->opt_dynamic_ip, addr(s))) ; /* 1 == dynamic IP address */
  printf ("K) Hibernate time in minutes: %u\n", preg->opt_hibertime) ; /* hibernate time in minutes if non-zero */
  printf ("L) Maximum connection time in minutes: %u\n", preg->opt_conntime) ; /* maximum connection time in minutes if non-zero */
  printf ("M) Connection wait time in minutes: %u\n", preg->opt_connwait) ; /* wait this many minutes after connection time or buflevel shutdown */
  printf ("N) Maximum registration attempts before hibernation: %u\n", preg->opt_regattempts) ; /* maximum registration attempts before hibernate if non-zero */
  printf ("O) Dynamic IP address expiration in minutes: %u\n", preg->opt_ipexpire) ; /* dyanmic IP address expires after this many minutes since last POC */
  printf ("P) Buffer level to stop connection: %u%%\n", preg->opt_buflevel) ; /* terminate connection when buffer level reaches this value if non-zero */
  printf ("Q) Minutes before writing Q330 continuity to file: %u\n", preg->opt_q330_cont) ;
  printf ("R) Maximum DSS memory in KB: %u\n", preg->opt_dss_memory) ;
  printf ("S) Save Configuration\n") ;
  printf ("X) Return to idle menu\n") ;
end

void show_file_menu (void)
begin
  string s ;

  printf ("\n") ;
  printf ("A) Enable writing one second data to seneca.sec: %s\n", show_yesno(configstruc.write_1sec, addr(s))) ; /* non-zero to write to seneca.sec */
#ifndef OMIT_SEED
  printf ("B) Enable writing 512 byte MiniSeed to seneca.mseed: %s\n", show_yesno(configstruc.write_mseed, addr(s))) ; /* non-zero to write to seneca.mseed */
  printf ("C) Enable writing 4096 byte MiniSeed to individual files: %s\n", show_yesno(configstruc.write_archive, addr(s))) ; /* non-zero to write to mseedll.sss such as mseed__.BHZ */
  printf ("D) Enable NetServer if configured in tokens: %s\n", show_yesno(configstruc.run_netserver, addr(s))) ; /* non-zero to open netserver if configured in tokens */
#endif
  printf ("E) POC Receiver port (zero to disable, standard is 2254): %u\n", configstruc.poc_port) ; /* non-zero to run POC receiver */
  printf ("S) Save Configuration\n") ;
  printf ("X) Return to idle menu\n") ;
end

void show_run_menu (void)
begin
  string s ;

  printf ("\n") ;
  printf ("A) ARP Status\n") ;
  printf ("D) Dump Messages\n") ;
  if (fixed.flags and FF_EP)
    then
      printf ("E) EnvProc Status\n") ;
#ifndef OMIT_SEED
  if (current_state == LIBSTATE_RUN)
    then
      printf ("F) Flush to Disk\n") ;
#endif
  printf ("G) Get Operating Limits using Tunnelling\n") ;
#ifndef OMIT_SEED
  if (current_state == LIBSTATE_RUN)
    then
      begin
        printf ("H) Show Detectors\n") ;
        printf ("I) Ignore Data from Q330 for 30 seconds\n") ;
        printf ("L) LCQ Status\n") ;
      end
#endif
  printf ("M) Module Versions\n") ;
  printf ("O) Operational Status\n") ;
  printf ("P) Ping Q330\n") ;
  printf ("S) Stop connection\n") ;
  printf ("U) Send User Message\n") ;
  printf ("V) Change Verbosity from %s\n", show_verbosity (configstruc.par_create.opt_verbose, addr(s))) ;
end

