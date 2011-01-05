/*   Seneca Operating Status Dump Routine
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
    0 2006-10-03 rdr Created
    1 2006-10-26 rdr Add display of last data time and current ip and port.
    2 2006-11-30 rdr Add show_modules.
    3 2006-12-04 rdr Fix dump when minutes value has invalid value.
    4 2008-02-25 rdr Fix display of negative data latency.
*/
#ifndef dump_h
#include "dump.h"
#endif
#ifndef config_h
#include "config.h"
#endif
#ifndef libmsgs_h
#include "libmsgs.h"
#endif
#ifndef libsupport_h
#include "libsupport.h"
#endif

static char *scale_value (integer v, string *result)
begin
  string s ;
  float r ;

  if (v != INVALID_ENTRY)
    then
      if (v > 9999999)
        then
          begin
            r = v / 1000000.0 ;
            sprintf(s, "%5.2fM", r) ;
          end
      else if (v > 99999)
        then
          begin
            r = v / 1000.0 ;
            sprintf(s, "%4.1fK", r) ;
          end
        else
          sprintf(s, "%d", v) ;
    else
      s[0] = 0 ;
  strcpy(result, s) ;
  return result ;
end

static char *condtwo (integer v, boolean pad, string *result, string *suffix)
begin

  sprintf(result, "%d", v) ;
  if (pad)
    then
      zpad(result, 2) ;
  strcat(result, suffix) ;
  return result ;
end

static char *format_time (integer sec, string *result)
begin
  integer days, hours, minutes ;
  string s, s1 ;

  if (sec >= 0)
    then
      (*result)[0] = 0 ;
    else
      begin
        strcpy(result, "-") ;
        sec = abs(sec) ;
      end
  days = sec div 86400 ;
  sec = sec - days * 86400 ;
  hours = sec div 3600 ;
  sec = sec - hours * 3600 ;
  minutes = sec div 60 ;
  sec = sec - minutes * 60 ;
  s[0] = 0 ;
  if (days > 0)
    then
      sprintf(s, "%dd", days) ;
  if ((s[0]) lor (hours > 0))
    then
      begin
        condtwo (hours, (s[0]), s1, "h") ;
        strcat(s, s1) ;
      end
  if ((s[0]) lor (minutes > 0))
    then
      begin
        condtwo (minutes, (s[0]), s1, "m") ;
        strcat(s, s1) ;
      end
  condtwo (sec, (s[0]), s1, "s") ;
  strcat(s, s1) ;
  strcat(result, s) ;
  return result ;
end

void show_opstat (void)
begin
  enum tliberr err ;
  enum tacctype acctype ;
  enum taccdur accdur ;
  string s, s1 ;
  double toff ;
  float r ;
  integer pwc, i, v ;
  string15 shour, sday ;

  lib_get_state (context, addr(err), addr(opstat)) ;
  printf ("\n") ;
  printf ("Station Name: %s\n", opstat.station_name) ;
  printf ("Data Port: %d\n", opstat.station_port + 1) ;
  printf ("Tag ID: %d\n", opstat.station_tag) ;
  printf ("Serial Number: %s\n", showsn(opstat.station_serial, addr(s1))) ;
  toff = now () + opstat.timezone_offset - opstat.station_reboot ; /* difference in seconds */
  r = toff / 86400.0 ; /* number of days */
  printf ("Time of Last Reboot: %s/%4.2fd\n", jul_string (opstat.station_reboot, addr(s)), r) ;
  if (opstat.last_data_time)
    then
      printf("Last Data Received: %s\n", jul_string (opstat.last_data_time, addr(s))) ;
    else
      printf ("No Data Recieved\n") ;
  printf ("Current Address: %s:%d\n", showdot(opstat.current_ip, addr(s1)), (integer)opstat.current_port) ;
  if (opstat.minutes_of_stats < 2)
    then
      shour[0] = 0 ;
  else if (opstat.minutes_of_stats >= 60)
    then
      strcpy(shour, " Hour=") ;
    else
      sprintf(shour, " %dmin=", opstat.minutes_of_stats) ;
  if (opstat.hours_of_stats < 2)
    then
      sday[0] = 0 ;
  else if (opstat.hours_of_stats >= 24)
    then
      strcpy(sday, " Day=") ;
    else
      sprintf(sday, " %dhr=", opstat.hours_of_stats) ;
  for (acctype = AC_FIRST ; acctype <= AC_LAST ; acctype++)
    begin
      lib_acc_types (acctype, addr(s)) ;
      for (accdur = AD_MINUTE ; accdur <= AD_DAY ; accdur++)
        begin
          switch (accdur) begin
            case AD_MINUTE :
              if (opstat.accstats[acctype][accdur] != INVALID_ENTRY)
                then
                  strcat(s, " minute=") ;
              break ;
            case AD_HOUR :
              strcat(s, shour) ;
              break ;
            case AD_DAY :
              strcat(s, sday) ;
              break ;
          end
          if ((accdur == AD_HOUR) land ((opstat.minutes_of_stats < 2) lor (shour[0] == 0)) lor
             (accdur == AD_DAY) land ((opstat.hours_of_stats < 2) lor (sday[0] == 0)) lor
             (opstat.accstats[acctype][accdur] == INVALID_ENTRY))
            then
              ; /* no change */
            else
              switch (acctype) begin
                case AC_DUTY :
                case AC_COMEFF : /* value is times 10 in percent */
                  r = opstat.accstats[acctype][accdur] * 0.1 ;
                  sprintf(s1, "%3.1f%", r) ;
                  strcat(s, s1) ;
                  break ;
                case AC_THROUGH : /* value is times 100 */
                  r = opstat.accstats[acctype][accdur] * 0.01 ;
                  sprintf(s1, "%4.2f", r) ;
                  strcat(s, s1) ;
                  break ;
                default :
                  strcat(s, scale_value(opstat.accstats[acctype][accdur], addr(s1))) ;
                  break ;
              end
        end
      if (s[0])
        then
          printf ("%s\n", s) ;
    end
  if (opstat.runtime >= 0)
    then
      printf("Time Connected: %s\n", format_time(abs(opstat.runtime), addr(s1))) ;
    else
      printf("Time Disonnected: %s\n", format_time(abs(opstat.runtime), addr(s1))) ;
  if (current_state == LIBSTATE_RUN)
    then
      begin
        if (opstat.data_latency != INVALID_LATENCY)
          then
            printf ("Data Latency: %s\n", format_time(opstat.data_latency, addr(s1))) ;
        printf ("Total Data Gaps: %s\n", scale_value(opstat.totalgaps, addr(s1))) ;
        s[0] = 0 ;
        for (i = 0 ; i <= CHANNELS - 1 ; i++)
          if (opstat.calibration_errors and (1 shl i))
            then
              begin
                sprintf(s1, "%d", i + 1) ;
                strcat(s, s1) ;
              end
        if (s[0])
          then
            printf ("Digitzer Cal. Error on %s\n", s) ;
      end
   if (opstat.status_latency != INVALID_LATENCY)
    then
      printf ("Status Latency: %s\n", format_time(opstat.status_latency, addr(s1))) ;
  if ((current_state == LIBSTATE_RUN) land (opstat.pwr_volt > 5))
    then
      begin
        r = opstat.pwr_volt ;
        printf ("Input Supply Voltage: %4.2f\n", r) ;
        pwc = lib_round(opstat.pwr_cur * 1000) ; /* ma */
        printf ("Supply Current: %dma\n", pwc) ;
        printf ("Temperature: %d\n", opstat.sys_temp) ;
      end
  if (opstat.gps_age != -1)
    then
      begin
        printf ("GPS State: %s\n", lib_gps_state (opstat.gps_stat, addr(s1))) ;
        printf ("GPS Fix: %s\n", lib_gps_fix (opstat.gps_fix, addr(s1))) ;
        printf ("GPS Age: %s\n", format_time(opstat.gps_age, addr(s1))) ;
        printf ("PLL State: %s\n", lib_pll_state(opstat.pll_stat, addr(s1))) ;
        r = opstat.gps_lat ;
        printf ("Latitude: %8.6f\n", r) ;
        r = opstat.gps_long ;
        printf ("Longitude: %8.6f\n", r) ;
        r = opstat.gps_elev ;
        printf ("Elevation: %3.1f\n", r) ;
      end
  if (current_state == LIBSTATE_RUN)
    then
      begin
        strcpy(s, "Mass Positions:") ;
        for (i = 0 ; i <= 5 ; i++)
          begin
            sprintf(s1, " %d", opstat.mass_pos[i]) ;
            strcat(s, s1) ;
          end
        printf ("%s\n", s) ;
        v = lib_round(opstat.pkt_full) ;
        printf ("Packet Buffer: %d%%\n", v) ;
        printf ("Clock Quality: %d%%\n", opstat.clock_qual) ;
        v = opstat.clock_drift ;
        printf ("Clock Drift: %dusec\n", v) ;
      end
end

void show_modules (void)
begin
  integer i ;
  pmodules pm ;
  tmodule *pone ;

  pm = get_modules () ;
  printf ("Seneca Modules and Versions:\n") ;
  for (i = 0 ; i <= (MAX_MODULES - 1) ; i++)
    begin
      pone = addr((*pm)[i]) ;
      if ((*pm)[i].name[0])
        then
          printf ("%13s %d\n", addr(pone->name), pone->ver) ;
        else
          break ;
    end
  pm = lib_get_modules () ;
  printf ("Lib330 Modules and Versions:\n") ;
  for (i = 0 ; i <= (MAX_MODULES - 1) ; i++)
    begin
      pone = addr((*pm)[i]) ;
      if ((*pm)[i].name[0])
        then
          printf ("%13s %d\n", addr(pone->name), pone->ver) ;
        else
          break ;
    end
end

