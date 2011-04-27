/*   Seneca Callback related routines
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
    1 2006-10-26 rdr Change mseed__.BHZ type file names to DORK_1__.BHZ format.
    2 2006-12-01 rdr Change method of clearing global variables since they 
                     not be in the order specified on all platforms.
    3 2010-09-14 fcs Add station name remap, add 1 sec callback to get
                     vacuum readings from channel ??/VY?/
    4 2010-12-02 fcs Exit program when state goes into LIBSTATE_TERM
                     otherwise we can get segmentation faults
*/
#include <syslog.h>

#ifndef globals_h
#include "globals.h"
#endif
#ifndef callbacks_h
#include "callbacks.h"
#endif
#ifndef config_h
#include "config.h"
#endif
#ifndef libseed_h
#include "libseed.h"
#endif
#ifndef libsupport_h
#include "libsupport.h"
#endif
#ifndef libcvrt_h
#include "libcvrt.h"
#endif
#ifndef libmsgs_h
#include "libmsgs.h"
#endif

#ifndef OMIT_SEED
#ifndef libnetserv_h
#include "libnetserv.h"
#endif
#ifndef libseed_h
#include "libseed.h"
#endif
#endif

#include "include/diskloop.h"
#include "include/q330arch.h"
#include "include/shmstatus.h"

extern int debug_arg;
extern int iDlg;
extern struct s_mapstatus *mapstatus;   // Communicates status to dispstatus program

#ifndef OMIT_SEED
static tctrlstat ctrlstat ; /* just for display purposes */
static tdetstat detstat ;
static tlcqstat lcqstat ;
#endif

#ifndef X86_WIN32
#include <sys/stat.h>       
#endif

static void replace_prompt (const char *s)
begin

  if (screen_busy)
    then
      return ;
  printf ("\b\b\b\b\b\b\b\b\b%s\n", s) ;
  if (!debug_arg)
    syslog(LOG_INFO, s);
end

void poc_handler (enum tpocstate pocstate, tpoc_recvd *poc_recv)
begin
  tpocmsg pocmsg ;
  tpar_create *pcret ;
  string s, s1 ;
  integer i ;

  pcret = addr(configstruc.par_create) ;
  if (pocstate == PS_CONNRESET)
    then
      begin
        replace_prompt ("POC Receiver Connection Reset, Terminated") ;
        lib_poc_stop (poc_context) ;
        poc_context = NIL ;
      end
  else if ((poc_recv->data_port == pcret->q330id_dataport) land (cfgphase == CFG_RUN) land
         (poc_recv->serial_number[0] == pcret->q330id_serial[0]) land
         (poc_recv->serial_number[1] == pcret->q330id_serial[1]))
    then
      begin
        pocmsg.new_ip_address = poc_recv->ip_address ;
        pocmsg.new_base_port = poc_recv->base_port ;
        i = poc_recv->base_port ;
        sprintf(s, "%s:%u", showdot(poc_recv->ip_address, (string15 *)addr(s1)),
                i) ;
        strcpy((char *)addr(pocmsg.log_info), s) ;
        lib_poc_received (context, addr(pocmsg)) ;
      end
end

static void ready_to_run (void)
begin
  string s ;
  longint l1, l2 ;

  lib_get_dpcfg (context, addr(dpcfg)) ;
  strcpy((char *)addr(station_name), (const char *)addr(dpcfg.station_name)) ;
  sprintf(s, "Station Name: %s", addr(station_name)) ;
  replace_prompt ((const char *)addr(s)) ;
  lib_change_state (context, LIBSTATE_RUN, LIBERR_NOERR) ;
#ifndef OMIT_SEED
  if ((configstruc.run_netserver) land (dpcfg.net_port))
    then
      begin
        netserve.sync_time = 30 ;
        netserve.whitecount = 0 ;
        netserve.ns_port = dpcfg.net_port ;
        netserve.server_number = 1 ;
        NetBufferSize(&l1);
        l2 = LIB_REC_SIZE ;
/* CbuilderX returns some weird huge value for "NETBUFSIZE / LIB_REC_SIZE", so get around it */
        netserve.record_count = l1 div l2 ;
        netserve.nsbuf = malloc (l2 * netserve.record_count) ;
fprintf(stdout, "netserver buffer size set to %d bytes\n",
l2 * netserve.record_count);
        netserve.stnctx = context ;
        netcontext = lib_ns_start (addr(netserve)) ;
      end
#endif
end

void send_tunneled_request (void)
begin
  enum tliberr err ;

  err = lib_send_tunneled (context, 0x2D, 0xAE, addr(tunbuf), 0) ;
  if (err != LIBERR_NOERR)
    then
      replace_prompt ("Could not send tunneled command") ;
end

void tunnel_response_available (void)
begin
typedef struct {
  word max_main ; /* maximum main current */
  word min_off ; /* minimum off time in seconds */
  word min_ps ; /* minimum power supply voltage */
  word max_ant ; /* maximum antenna current */
  int16 min_temp ; /* minimum temperature */
  int16 max_temp ; /* maximum temperature */
  word temp_hyst ; /* temperature hysterisis */
  word volt_hyst ; /* voltage hysterisis */
  word def_vco ; /* default VCO voltage */
  word spare2 ;
} toplimits ;
  enum tliberr err ;
  byte resp ;
  integer size ;
  pbyte p ;
  float r ;
  toplimits oplimits ;

  err = lib_get_tunneled (context, addr(resp), addr(tunbuf), addr(size)) ;
  if (err != LIBERR_NOERR)
    then
      replace_prompt ("Error getting tunneled response") ;
    else
      begin
        p = (pbyte)addr(tunbuf) ;
        oplimits.max_main = loadword (addr(p)) ;
        oplimits.min_off = loadword (addr(p)) ;
        oplimits.min_ps = loadword (addr(p)) ;
        oplimits.max_ant = loadword (addr(p)) ;
        oplimits.min_temp = loadint16 (addr(p)) ;
        oplimits.max_temp = loadint16 (addr(p)) ;
        oplimits.temp_hyst = loadword (addr(p)) ;
        oplimits.volt_hyst = loadword (addr(p)) ;
        oplimits.def_vco = loadword (addr(p)) ;
        oplimits.spare2 = loadword (addr(p)) ;
        printf ("\n") ;
        printf ("Maximum Main Current: %d\n", oplimits.max_main) ;
        printf ("Minimum Off Time: %d\n", oplimits.min_off) ;
        r = oplimits.min_ps * 0.15 ;
        printf ("Minimum Input Voltage: %4.2f\n", r) ;
        printf ("Maximum Antenna Current: %d\n", oplimits.max_ant) ;
        printf ("Minimum Temperature: %d\n", oplimits.min_temp) ;
        printf ("Maximum Temperature: %d\n", oplimits.max_temp) ;
        printf ("Temperature Hysteresis: %d\n", oplimits.temp_hyst) ;
        printf ("Input Voltage Hysteresis: %d\n", oplimits.volt_hyst) ;
        printf ("Default VCO: %d\n", oplimits.def_vco) ;
        printf ("\n") ;
        show_run_menu () ;
        printf ("Command :") ;
      end
end

static longword sen_make_bitmap (longword bit)
begin

  return (longword)1 shl bit ;
end

static char *showh6 (tsix *mac, string *result)
begin
  string31 s ;
  string7 s1 ;
  integer i, last ;

  s[0] = 0 ;
#ifdef ENDIAN_LITTLE
  last = 0 ;
  for (i = 2 ; i >= 0 ; i--)
#else
  last = 2 ;
  for (i = 0 ; i <= 2 ; i++)
#endif
    begin
      sprintf(s1, "%X", (*mac)[i] shr 8) ;
      strcat(s, zpad(s1, 2)) ;
      strcat(s, ":") ;
      sprintf(s1, "%X", (*mac)[i] and 255) ;
      strcat(s, zpad(s1, 2)) ;
      if (i != last)
        then
          strcat(s, ":") ;
    end
  strcpy((char *)result, s) ;
  return (char *)result ;
end

void arp_status (void)
begin
  enum tliberr err ;
  integer i ;
  tarp1 *parp ;
  string31 s1, s2 ;

  err = lib_get_status (context, SRB_ARP, addr(stat_arp)) ;
  if (err == LIBERR_NOERR)
    then
      begin
        printf ("\n") ;
        for (i = 0 ; i <= stat_arp.arphdr.arp_count - 1 ; i++)
          begin
            parp = addr(stat_arp.arps[i]) ;
            printf ("IP=%s MAC=%s Timeout=%d\n",
                    showdot(parp->ip, (string15 *)addr(s1)),
                    showh6(addr(parp->mac), (string *)s2), parp->timeout) ;
          end
        printf ("\n") ;
      end
    else
      begin
        replace_prompt ("ARP Status not available, adding to request list") ;
        lib_request_status (context, sen_make_bitmap(SRB_ARP), 10) ;
      end
end

void clear_globals (void)
begin

  previous_state = LIBSTATE_IDLE ;
  state_change = FALSE ;
  cfg_change = FALSE ; 
  is_stalled = FALSE ;
  screen_busy = FALSE ;
  memset (addr(pingresponse), 0, sizeof(tpingresponse)) ;
  memset (addr(tunbuf), 0, MAXDATA) ;
end

void sen_state_callback (pointer p)
{
  tstate_call *ps ;
  string s, s1 ;
  enum tliberr err ;

  ps = p ;
  switch (ps->state_type) begin
    case ST_STATE :
      current_state = (enum tlibstate)ps->info ;
      strcpy(s, "New State: ") ;
      strcat(s, lib_get_statestr(current_state, (string63 *)s1)) ;
      replace_prompt (s) ;
      if (current_state != previous_state)
        then
          switch (current_state) begin
            case LIBSTATE_RUN :
              break ;
            case LIBSTATE_RUNWAIT :
              ready_to_run () ;
              break ;
            case LIBSTATE_IDLE :
            case LIBSTATE_WAIT :
#ifndef OMIT_SEED
              if (netcontext)
                then
                  begin
                    lib_ns_stop (netcontext) ;
                    free (netserve.nsbuf) ;
                    netcontext = NIL ;
                  end
#endif
              clear_globals () ;
              lib_get_state (context, addr(err), addr(opstat)) ;
              if (err == LIBERR_INVREG)
              {
                  if (debug_arg)
                  {
                    fprintf (stderr, "Q330 registration failure!\n");
                  }
                  else
                  {
                    syslog (LOG_ERR, "Q330 registration failure!\n");
                  }
                  lib_change_state (context, LIBSTATE_TERM, LIBERR_CLOSED) ; /* terminate thread */
              }
              break ;
            case LIBSTATE_TERM :
              if (context)
                then
                  lib_destroy_context (addr(context)) ;
              if (onesec_file != INVALID_FILE_HANDLE)
                then
                  lib_file_close (NIL, onesec_file) ;
              onesec_file = INVALID_FILE_HANDLE ;
#ifndef OMIT_SEED
              if (mseed_file != INVALID_FILE_HANDLE)
                then
                  lib_file_close (NIL, mseed_file) ;
              mseed_file = INVALID_FILE_HANDLE ;
#endif
              cfgphase = CFG_IDLE ;
              exit(1);
              break ;
          end ;
      previous_state = current_state ;
      break ;
    case ST_STALL :
      is_stalled = (ps->info != 0) ;
      if (is_stalled)
        then
          replace_prompt ("Connection stalled") ;
        else
          replace_prompt ("Connection is no longer stalled") ;
      break ;
    case ST_PING :
      pingresponse.ping_type = ps->subtype ;
      pingresponse.ping_opt = 0 ; /* unknown */
      pingresponse.ping_ms = ps->info ;
      if (pingresponse.ping_ms < 0)
        then
          replace_prompt ("No Response from Ping") ;
        else
          begin
            sprintf(s, "Ping Response in %5.3f Seconds", pingresponse.ping_ms * 0.001) ;
            replace_prompt (s) ;
          end
      break ;
    case ST_TUNNEL :
      tunnel_response_available () ;
      break ;
  end
} // sen_state_callback()

void msgs_callback (pointer p)
{
  tmsg_call *pm ;

  pm = p ;
  memcpy (addr(msgqueue[(pm->msgcount - 1) and MSG_QUEUE_MASK]), pm, sizeof(tmsg_call)) ;
  highest_avail_msgnum = pm->msgcount ;
}

enum tfilekind {FK_UNKNOWN, FK_CONT, FK_IDXDAT} ;

enum tfilekind translate_file (char *s)
begin
  char *drive ;
  char *fname ;

  fname = strchr(s, '/') ;
  if (fname)
    then
      begin
        *fname++ = 0 ; /* terminate drive part, file name starts right after / */
        drive = s ; /* drive is first part */
      end
    else
      return FK_UNKNOWN ; /* no drive, leave filename alone */
  if (strcmp(drive, "cont") == 0)
    then
      begin
        /* In this example, the continuity file is in the current directory */
        strcpy(s, fname) ; /* just strip off cont/ */
        return FK_CONT ;
      end
    else
      begin
        /* For balelib you would need to pickup the drive number from idxn or
           datan drive and build a new string with the appropriate path names
           and copy to s */
        return FK_IDXDAT ;
      end
end

/* Extremely minimal implementation. Doesn't check return type from translate_file,
   but as a result cont.bin works as well as cont/cont.bin */
void sen_file_callback (pointer p)
begin
  tfileacc_call *pfa ;
  enum tfilekind kind ;
  tfile_handle cf ;
#ifndef X86_WIN32
  off_t result, long_offset ;
  struct stat sb ;
#endif

  pfa = p ;
  switch (pfa->fileacc_type) begin
    case FAT_OPEN :
      kind = translate_file (pfa->fname) ;
#ifdef X86_WIN32
      cf = CreateFile (pfa->fname, pfa->opt1, 0, NIL, pfa->opt2, FILE_ATTRIBUTE_NORMAL, 0) ;
#else
      cf = open (pfa->fname, pfa->opt2, pfa->opt1) ;
#endif
      pfa->response = (integer) cf ;
      break ;
    case FAT_CLOSE :
#ifdef X86_WIN32
      CloseHandle ((tfile_handle) pfa->opt1) ;
#else
      close ((tfile_handle) pfa->opt1) ;
#endif
      break ;
    case FAT_DEL :
      kind = translate_file (pfa->fname) ;
#ifdef X86_WIN32
      DeleteFile (pfa->fname) ;
#else
      remove (pfa->fname) ;
#endif
      break ;
    case FAT_SEEK :
#ifdef X86_WIN32
      pfa->response = (SetFilePointer ((tfile_handle) pfa->opt1, pfa->opt2, NIL, FILE_BEGIN) == 0xFFFFFFFF) ;
#else
      long_offset = pfa->opt2 ;
      result = lseek(pfa->opt1, long_offset, SEEK_SET) ;
#ifdef __APPLE__
      pfa->response = (result < 0) ;
#else
      pfa->response = (result != long_offset) ;
#endif
#endif
      break ;
    case FAT_READ :
#ifdef X86_WIN32
      ReadFile ((tfile_handle) pfa->opt1, pfa->fname, pfa->opt2, addr(pfa->response), NIL) ;
#else
      pfa->response = read (pfa->opt1, pfa->fname, pfa->opt2) ;
#endif
      break ;
    case FAT_WRITE :
#ifdef X86_WIN32
      WriteFile ((tfile_handle) pfa->opt1, pfa->fname, pfa->opt2, addr(pfa->response), NIL) ;
#else
      pfa->response = write(pfa->opt1, pfa->fname, pfa->opt2) ;
#endif
      break ;
    case FAT_SIZE :
#ifdef X86_WIN32
      pfa->response = GetFileSize ((tfile_handle) pfa->opt1, NIL) ;
#else
      fstat (pfa->opt1, addr(sb)) ;
      pfa->response = sb.st_size ;
#endif
      break ;
    /* Remainder of cases only required for balelib support */
  end
end

void dump_messages (void)
begin
  integer last, midx ;
  string s, s1 ;
  string25 s2, s3 ;
  tmsg_call *pmsg ;

  msgcnt = 0 ;
  if (highest_avail_msgnum == 0)
    then
      begin
        printf ("NO MESSAGES QUEUED\n") ;
        return ;
      end
  last = (highest_avail_msgnum - 1) and MSG_QUEUE_MASK ;
  if (highest_avail_msgnum > MSG_QUEUE_SIZE)
    then
      midx = (last + 1) and MSG_QUEUE_MASK ; /* wrapped */
    else
      midx = 0 ; /* not wrapped */
  repeat
    pmsg = addr(msgqueue[midx]) ;
    lib_get_msg (pmsg->code, (string95 *)s) ;
    strcat(s, pmsg->suffix) ;
    sprintf(s1, "%s:{%d}", jul_string (pmsg->timestamp, s2), pmsg->code) ;
    if (pmsg->datatime)
      then
        begin
          sprintf(s3, "[%s] ", jul_string (pmsg->datatime, s2)) ;
          strcat(s1, s3) ;
        end
      else
        strcat(s1, " ") ;
    printf ("%s%s\n", s1, s) ;
    inc(msgcnt) ;
    if (midx == last)
      then
        break ;
    midx = (midx + 1) and MSG_QUEUE_MASK ;
  until (msgcnt >= MSG_QUEUE_SIZE)) ;
  highest_msgnum = highest_avail_msgnum ;
end

void onesec_callback (pointer p)
{
  tonesec_call *ps ;
  int iWriteIndex;

  ps = p ;
  if (onesec_file != INVALID_FILE_HANDLE)
  {
    lib_file_write (NIL, onesec_file, ps, ps->total_size) ;
  }

  // Code to get 1 second */VY* vacuum
  if (ps->channel[0] == 'V' && ps->channel[1] == 'Y')
  {
    // Let status program see the latest vacuum readings
    if (mapstatus != NULL)
    {
      // Store the latest vacuum reading
      if (ps->channel[2] == 'Z')
      {
        // Don't worry about locking since a singe long value write
        for (iWriteIndex=0; iWriteIndex < 3; iWriteIndex++)
        {
          mapstatus->dlg[iWriteIndex][iDlg].vacuum[0] = ps->samples[0];
        }
      }
      else if (ps->channel[2] == '1' || ps->channel[2] == 'N')
      {
        // Don't worry about locking since a singe long value write
        for (iWriteIndex=0; iWriteIndex < 3; iWriteIndex++)
        {
          mapstatus->dlg[iWriteIndex][iDlg].vacuum[1] = ps->samples[0];
        }
      }
      else if (ps->channel[2] == '2' || ps->channel[2] == 'E')
      {
        // Don't worry about locking since a singe long value write
        for (iWriteIndex=0; iWriteIndex < 3; iWriteIndex++)
        {
          mapstatus->dlg[iWriteIndex][iDlg].vacuum[2] = ps->samples[0];
        }
      }
    } // if memory map present
//  if (debug_arg)
//  {
//    fprintf(stderr, "DEBUG 1 sec data %s/%s size %d val = %ld\n", 
//            ps->location, ps->channel, ps->total_size, ps->samples[0]);
//  }
  } // VY* vacuum pressure to store

  // Code to get 1 second */LDO * pressure
  if (ps->channel[0] == 'L' && ps->channel[1] == 'D' && ps->channel[2] == 'O')
  {
    // Let status program see the latest pressure readings
    if (mapstatus != NULL)
    {
      // Store the latest pressure reading
      for (iWriteIndex=0; iWriteIndex < 3; iWriteIndex++)
      {
        mapstatus->dlg[iWriteIndex][iDlg].pressure = ps->samples[0];
      }
    } // if memory map present
  } // LDO data to store
} // onesec_callback()

#ifndef OMIT_SEED
char *yesno (boolean b, string7 *result)
{

  if (b)
    then
      strcpy((char *)result, "Yes") ;
    else
      strcpy((char *)result, "No") ;
  return (char *)result ;
}

void show_detectors (void)
{
  enum tliberr err ;
  integer i ;
  tonedetstat *pd ;
  tonectrlstat *pc ;
  string7 s1, s2, s3, s4 ;

  printf ("\n") ;
  err = lib_get_detstat (context, addr(detstat)) ;
  if ((err != LIBERR_NOERR) lor (detstat.count < 1))
    then
      begin
        printf ("Status not available\n") ;
        return ;
      end
  printf ("%20s%4s%9s%6s%8s\n", "Detector Name", "On", "Declared", "First", "Enabled") ;
  for (i = 0 ; i <= detstat.count - 1 ; i++)
    begin
      pd = addr(detstat.entries[i]) ;
      printf ("%20s%4s%9s%6s%8s\n", addr(pd->name), yesno(pd->ison, addr(s1)), yesno(pd->declared, addr(s2)),
              yesno(pd->first, addr(s3)), yesno(pd->enabled, addr(s4))) ;
    end
  err = lib_get_ctrlstat (context, addr(ctrlstat)) ;
  printf ("%22s%4s\n", "Control Detector Name", "On") ;
  for (i = 0 ; i <= ctrlstat.count - 1 ; i++)
    begin
      pc = addr(ctrlstat.entries[i]) ;
      printf ("%22s%4s\n", addr(pc->name), yesno(pc->ison, addr(s1))) ;
    end
  printf ("\n") ;
}

void show_lcqs (void)
{
  enum tliberr err ;
  integer i ;
  string15 s ;
  tonelcqstat *pl ;

  printf ("\n") ;
  err = lib_get_lcqstat (context, addr(lcqstat)) ;
  if ((err != LIBERR_NOERR) lor (lcqstat.count < 1))
    then
      begin
        printf ("Status not available\n") ;
        return ;
      end
  printf ("%7s%7s%8s%6s%8s%8s%7s%9s%7s%7s\n", "LCQ", "Seq#", "Ses Cnt", "Since", "Det Cnt", "Cal Cnt",
          "ASeq#", "ASes Cnt", "AOver", "ASince") ;
  for (i = 0 ; i <= lcqstat.count - 1 ; i++)
    begin
      pl = addr(lcqstat.entries[i]) ;
      if (pl->location[0])
        then
          begin
            strcpy(s, pl->location) ;
            strcat(s, "-") ;
          end
        else
          s[0] = 0 ;
      strcat(s, pl->channel) ;
      printf ("%7s%7d%8d%6d%8d%8d%7d%9d%7d%7d\n", s, pl->rec_seq, pl->rec_cnt, pl->rec_age, pl->det_count,
              pl->cal_count, pl->arec_seq, pl->arec_cnt, pl->arec_over, pl->arec_age) ;
    end
  printf ("\n") ;
}

void mini_callback (pointer p)
{
  pminiseed_call pm ;

  pm = p ;
  if (pm->miniseed_action == MSA_512)
    then
      begin
        if (((pm->filter_bits and OMF_NETSERV) lor
             (pm->packet_class == PKC_MESSAGE) lor
             (pm->packet_class == PKC_TIMING))
            land (netcontext))
          then
            lib_ns_send (netcontext, pm->data_address) ;

        // Save 512 records localy, and to IDA disk loop
        {
          char  *retstr;
          char  station[8];
          char  chan[4];
          char  loc[4];
          STDTIME2  recStartTime;
          STDTIME2  recEndTime;
          int       iSeqNum;
          int       iSamples;
          int       iWriteIndex;

          // Implement MapStation: keyword in diskloop.config
          RemapStationName((char *)pm->data_address);

          // parse record for time and station name
          retstr = ParseSeedHeader(pm->data_address,
                       station, chan, loc, 
                       &recStartTime, &recEndTime, &iSeqNum, &iSamples);

          // Let status program know we received a record
          if (mapstatus != NULL)
          {
            // Calculate which mapstatus->ixWriteData[iDlg] value to use
            if (mapstatus->ixWriteData[iDlg] != 0 && mapstatus->ixReadData[iDlg] != 0)
              iWriteIndex = 0;
            else if (mapstatus->ixWriteData[iDlg] != 1 && mapstatus->ixReadData[iDlg] != 1)
              iWriteIndex = 1;
            else
              iWriteIndex = 2;

            // Make arrival time be the record end time
            mapstatus->dlg[iWriteIndex][iDlg].timeLastData = recEndTime;
            mapstatus->ixWriteData[iDlg] = iWriteIndex;
          } // If status shared memory segment has been setup

          // Send seed record to q330arch for archival and data transmission
          // q330arch will merge data from all data loggers into a single stream
          while ((retstr = q330SeedSend((void *)pm->data_address)) != NULL)
          {
            if (debug_arg)
              fprintf(stderr, "mini_callback q330SeedSend: %s\n", retstr);
            else
              syslog(LOG_ERR, "mini_callback q330SeedSend: %s\n", retstr);
	    sleep(1);
          }
        } // End addition to save 512 records to files
/* Original Seneca save to file
        if (mseed_file != INVALID_FILE_HANDLE)
          then
            lib_file_write (NIL, mseed_file, pm->data_address, LIB_REC_SIZE) ;
*/
      end
} // mini_callback()

void amini_callback (pointer p)
{
  pminiseed_call pm ;
  string fname ;
  string s, s1 ;
  tfile_handle afile ;
  longint size ;
  pchar minus ;
  char  *retstr;

  if (configstruc.write_archive == 0)
    then
      return ;
  pm = p ;
  strcpy(s1, pm->station_name) ;
  minus = strpbrk(s1, "-") ;
  if (minus)
    then
      begin
        inc(minus) ; /* skip minus */
        strcpy(s, minus) ;
      end
    else
      strcpy(s, s1) ; /* no network */
  strcpy(s1, (const char *)addr(pm->location)) ;
  while (strlen(s1) < 2)
    strcat(s1, "_") ;
  sprintf((char *)addr(fname), "%s_%d%s.%s", s, configstruc.par_create.q330id_dataport + 1, s1, addr(pm->channel)) ;
  switch (pm->miniseed_action)
  {
    case MSA_FIRST :
    case MSA_INC :
    case MSA_ARC :
    case MSA_FINAL : /* write or update data */
/* This code was used for the 4096 record archive format which has gone away
      if ((retstr = WriteChan(pm->station_name, pm->channel, pm->location,
                              pm->data_address)) != NULL)
      {
        fprintf(stderr, "amini_callback WriteChan: %s\n", retstr);
      }
      if ((retstr = idaWriteChan(pm->station_name, pm->channel, pm->location,
                              pm->data_address)) != NULL)
      {
        fprintf(stderr, "amini_callback idaWriteChan: %s\n", retstr);
      }
*/
      break ;
    case MSA_GETARC : /* return the last record */
/*
      if ((retstr = ReadLast(pm->station_name, pm->channel, pm->location,
                              pm->data_address)) != NULL)
      {
        fprintf(stderr, "amini_callback: %s\n", retstr);
        break;
      }

      // Only return MSA_RETARC if data is present
      if (*((char *)pm->data_address) != 0)
        pm->miniseed_action = MSA_RETARC ; // returning last written
*/
      break ;
  } // switch  (pm->miniseed_action)
} // amini_callback ()
#endif
