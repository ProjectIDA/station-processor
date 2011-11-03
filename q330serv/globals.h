/*   Seneca global definitions
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
    0 2006-09-09 rdr Created
    1 2006-11-30 rdr Add definitions for module versions.
    2 2007-01-18 rdr Remove duplicate define for VER_GLOBALS.
*/
#ifndef globals_h
#define globals_h
#define VER_GLOBALS 3

#ifndef libtypes_h
#include "libtypes.h"
#endif
#ifndef libclient_h
#include "libclient.h"
#endif
#ifndef libseed_h
#include "libseed.h"
#endif
#ifndef OMIT_SEED
#ifndef libnetserv_h
#include "libnetserv.h"
#endif
#endif

#define SEN_VER "1.10"
#define MSG_QUEUE_SIZE 256 /* must be a power of 2 */
#define MSG_QUEUE_MASK (MSG_QUEUE_SIZE - 1)

#ifndef OMIT_SEED
#define NETBUFSIZE 100352
#define ARC_RECORD_SIZE 4096
#define ARC_FRAMES_PER_RECORD 64
#define ARC_RECORD_EXP 12
#define CONFIG_VERSION 3
#else
#define CONFIG_VERSION 103
#endif

typedef char string25[26] ;
typedef char string127[128] ;

#ifndef OMIT_SEED
typedef compressed_frame tcompressed_record[ARC_FRAMES_PER_RECORD] ;
typedef tcompressed_record *pcompressed_record ;
#endif

typedef tmsg_call tsenmsgqueue[MSG_QUEUE_SIZE] ;
typedef struct {
  word ping_type ;
  word ping_opt ;
  longint ping_ms ;
} tpingresponse ;
typedef struct {
  integer cfg_ver ; /* configuration version number */
  tpar_create par_create ; /* parameters to create context */
  tpar_register par_register ; /* parameters to register */
  word write_1sec ; /* non-zero to write to seneca.sec */
#ifndef OMIT_SEED
  word write_mseed ; /* non-zero to write to seneca.mseed */
  word write_archive ; /* non-zero to write to mseedll.sss such as mseed__.BHZ */
  word run_netserver ; /* non-zero to open netserver if configured in tokens */
#endif
  word poc_port ; /* non-zero to run POC receiver */
} tconfigstruc ;
enum tcfgphase {CFG_IDLE, CFG_CREATE, CFG_REG, CFG_FILE, CFG_RUN} ;

/* Global variables definitions */
extern tcontext context ; /* context for this station */
extern enum tlibstate current_state ;
extern longint highest_msgnum ;
extern longint highest_avail_msgnum ;
extern integer msgcnt ;
extern enum tcfgphase cfgphase ;
extern word q330phy ; /* Q330 physical port */
extern word myphy ; /* later converted to par_create parameters */
extern string9 station_name ; /* network-station */
extern tconfigstruc configstruc ; /* all the parameters needed */
extern tclock qclock ; /* initialized to defaults */
extern longword have_cfg ; /* stuff we already have that can't change */
extern longword status_received ; /* have this status, might be old */
extern topstat opstat ; /* operation status */
extern tstat_arp stat_arp ; /* arp status */
extern tstat_ep stat_ep ; /* EP status */
extern tdpcfg dpcfg ; /* DP configuration from tokens */
extern tpingreq pingreq ; /* for sending ping */
extern tfile_handle onesec_file ;
extern tsenmsgqueue msgqueue ;
extern string line ; /* input line from user */
extern tfile_owner fowner ;
extern tfixed fixed ;

#ifndef OMIT_SEED
extern tns_par netserve ;
extern tfile_handle mseed_file ;
#endif

extern word first_clear ; /* first word to clear after de-registratin */
extern enum tlibstate previous_state ;
extern boolean state_change ;
extern boolean cfg_change ; /* new configuration available */
extern boolean is_stalled ;
extern boolean screen_busy ;
extern tpingresponse pingresponse ;
extern byte tunbuf[MAXDATA] ;

#ifndef OMIT_SEED
extern pointer netcontext ;
#endif

extern word last_clear ; /* last word to clear after de-registration */

#ifdef X86_WIN32
extern WSADATA wdata ;
#endif

extern word poc_udp_port ;
extern pointer poc_context ;

typedef tmodule tsen_modules[6] ; /* Null name to indicate end of list */

extern pmodules get_modules (void) ;

#endif
