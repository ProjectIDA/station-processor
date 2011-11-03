/*   Seneca global variable allocations
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
    1 2006-11-30 rdr Add module table.
    2 2009-02-10 rdr Add fixed and stat_ep.
*/
#ifndef globals_h
#include "globals.h"
#endif
#ifndef callbacks_h
#include "callbacks.h"
#endif
#ifndef commands_h
#include "commands.h"
#endif
#ifndef config_h
#include "config.h"
#endif
#ifndef dump_h
#include "dump.h"
#endif

tcontext context ; /* context for this station */
enum tlibstate current_state ;
longint highest_msgnum ;
longint highest_avail_msgnum ;
integer msgcnt ;
enum tcfgphase cfgphase ;
word q330phy ; /* Q330 physical port */
word myphy ; /* later converted to par_create parameters */
string9 station_name ; /* network-station */
tconfigstruc configstruc ; /* all the parameters needed */
tclock qclock ; /* initialized to defaults */
longword have_cfg ; /* stuff we already have that can't change */
longword status_received ; /* have this status, might be old */
topstat opstat ; /* operation status */
tstat_arp stat_arp ; /* arp status */
tstat_ep stat_ep ; /* EP status */
tdpcfg dpcfg ; /* DP configuration from tokens */
tpingreq pingreq ; /* for sending ping */
tfile_handle onesec_file ;
tsenmsgqueue msgqueue ;
string line ; /* input line from user */
tfile_owner fowner ;
tfixed fixed ;

#ifndef OMIT_SEED
tns_par netserve ;
tfile_handle mseed_file ;
#endif

enum tlibstate previous_state ;
boolean state_change ;
boolean cfg_change ; /* new configuration available */
boolean is_stalled ;
boolean screen_busy ;
tpingresponse pingresponse ;
byte tunbuf[MAXDATA] ;

#ifndef OMIT_SEED
pointer netcontext ;
#endif

#ifdef X86_WIN32
WSADATA wdata ;
#endif

word poc_udp_port ;
pointer poc_context ;

const tsen_modules sen_modules =
   {{/*name*/"Globals", /*ver*/VER_GLOBALS},   {/*name*/"Callbacks", /*ver*/VER_CALLBACKS},
    {/*name*/"Commands", /*ver*/VER_COMMANDS}, {/*name*/"Config", /*ver*/VER_CONFIG},
    {/*name*/"Dump", /*ver*/VER_DUMP},         {/*name*/"", /*ver*/0}} ;

pmodules get_modules (void)
begin

  return addr(sen_modules) ;
end


