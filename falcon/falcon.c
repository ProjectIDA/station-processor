/*
File:       falcon
Copyright:  (C) 2009 by Albuquerque Seismological Laboratory
Purpose:    Server for collecting data from falcon and sending it out as seed
            data.

Algorithm:  

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2009-04-10 FCS - Create skeleton
2009-10-28 FCS - Add lock file to allow only one invocation
2011-01-03 FCS - Improve recovery from falcon connection issues
2011-04-27 FCS - Tone down syslog verbosity every half hour
2011-11-14 JDE - New diskloop.config keywords for telemetry control
******************************************************************************/
#define FILENAME "falcon"
const char *VersionIdentString = "Release 2.0";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <time.h>

#include "alarm.h"
#include "buffer.h"
#include "csv.h"
#include "falcon.h"
#include "fmash.h"
#include "format_data.h"
#include "get.h"
#include "simclist.h"

#include "include/netreq.h"
#include "include/diskloop.h"
#include "include/q330arch.h"

// Global debug mode flag
int gDebug=0;
int gReport=1;

// Global shared values for routines in this file
static int iSeedRecordSize=0;         // Size of seed records usually 512
static char *seedRecordBuf=NULL;      // Points to current buffer
static int  numRecords=0;             // Number seed records allocated
static int  seqno=0;

void daemonize();

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
    fprintf(stderr,
"Usage:\n");
    fprintf(stderr,
"  falcon <configfile> [debug]\n"
"         Retrieves status information from falcon and sends it via opaque\n"
"         seed records.\n"
"    <configfile>    -- Usually /etc/q330/DLG1/diskloop.config\n"
"    debug           -- Turn on debug messages\n"
    );
    fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

//////////////////////////////////////////////////////////////////////////////
// FlushOpaque -- Send any outstanding seed record buffers from QueueOpaque
//                that are waiting to be filled
void FlushOpaque()
{
  int  i;
  char *retmsg;

  // Send any opaque seed records to q330arch daemon
  for (i=0; i < numRecords; i++)
  {
    if (gDebug)
      fprintf(stdout, "Sending seed record %d to q330arch\n", seqno);
    if ((retmsg=q330SeedSend(&seedRecordBuf[i*iSeedRecordSize])))
    {
      if (gDebug)
        fprintf(stderr, "%s[%d]: %s\n", __FILE__, __LINE__, retmsg);
      else
        syslog(LOG_ERR, "%s[%d]: %s\n", __FILE__, __LINE__, retmsg);

      // Don't swamp error logs if there was a send error
      sleep(5);
    } // error sending message
  } // loop sending out records

  // Free up the space allocated for the seed records
  free(seedRecordBuf);
  seedRecordBuf = NULL;
  numRecords=0;
} // FlushOpaque

//////////////////////////////////////////////////////////////////////////////
// QueueOpaque -- Add this opaque message to a seed record and send it when
//                the record buffer is full
void QueueOpaque(
  const void *data,        // Pointer to new data to add
  int data_length,         // Number of bytes in *data
  const char *station,     // The station name
  const char *network,     // Network ID
  const char *channel,     // Channel name
  const char *location,    // Location code
  const char *idstring)    // '~' Terminated ascii id string (keep this short)
{
  char *retmsg;
  static char *last_chan="";

  // If the channel name is not the same as the last one then flush any data
  if (strcmp(last_chan, channel) != 0)
  {
    last_chan = (char *)channel;
    FlushOpaque();
  }

  // See if we have a seed record ready to buffer this message
  if (seedRecordBuf != NULL)
  {
    // See if new message can be added to existing buffer
    if (AppendOpaqueSeed(seedRecordBuf, (void *)data, data_length, 
                         iSeedRecordSize))
    {
      if (gDebug)
        fprintf(stdout, "Added new opaque message to record %d\n", seqno);

      // Nothing else to do at this time
      return;
    } // Success adding new message to existing buffer

    // Append failed so send prior buffer
    FlushOpaque();
  } // An existing buffer record was found

  // If we get here, then we should have no buffer and a message
  if (seedRecordBuf == NULL)
  {
    seqno++;  // Increment sequence number
    if (gDebug)
      fprintf(stdout, "Creating new opaque message for record %d\n", seqno);
    if ((retmsg = MakeOpaqueSeed(data, data_length, seqno,
             station, network, channel, location, idstring,
             iSeedRecordSize, (void **)&seedRecordBuf, &numRecords)) != NULL)
    {
      if (gDebug)
        fprintf(stderr, "%s[%d]: %s\n", __FILE__, __LINE__, retmsg);
      else
        syslog(LOG_ERR, "%s[%d]: %s\n", __FILE__, __LINE__, retmsg);
      return;
    } // Error creating seed record

    // If data spanned more than one record then send it right away
    if (numRecords > 1)
      FlushOpaque();
  } // no existing seed record buffer

} // QueueOpaque()

//////////////////////////////////////////////////////////////////////////////
// PollFalcon -- Poll the Falcon for the latest data, and write it to
//                the diskloop.
void PollFalcon( csv_context_t* csv_buffer_list, alarm_context_t* alarm_lines,
                 buffer_t* url_str, st_info_t* st_info,
                 time_t last_csv_time, time_t last_alarm_time)
{
  alarm_line_t* alarm;
  
  // Keep last_alarm_time up to date
  if (!list_empty(alarm_lines))
  {
// This sort appears to be redundant Frank, done after alarm_filter_lines
//    list_sort(alarm_lines, 1);
    alarm = (alarm_line_t*)list_get_at(alarm_lines, list_size(alarm_lines));
    if (alarm) {
      last_alarm_time = alarm->timestamp;
    }
  }

  // Get the CSV data from the falcon
  csv_poll(csv_buffer_list, url_str, st_info, last_csv_time);

  // Get the alarm history from the falcon
  alarm_poll(alarm_lines, url_str, st_info, last_alarm_time);

  // Archive the latest data
  alarm_archive(alarm_lines, url_str, st_info);
  if (gDebug) fprintf(stderr,"Alarms Archived.\n");
  csv_archive(csv_buffer_list, url_str, st_info);
  if (gDebug) fprintf(stderr,"CSV Data Archived.\n");

  // Ensure all opaque blockettes have been sent
  FlushOpaque();
} // PollFalcon()


//////////////////////////////////////////////////////////////////////////////
void GetLastTimeCSV( time_t* last_csv_time, st_info_t* st_info)
{
  char *retmsg;

  uint8_t  seed_record[iSeedRecordSize];
  uint8_t* blockette = NULL;
  uint8_t* opaque_data = NULL;
  size_t offset = 0;
  char time_str[32];
  int max_to_check = 8;
  int index        = 0;
  uint32_t netuint32;

  int first_record = 0;
  int last_record  = 0;
  int record_count = 0;
  int loop_size    = 0;

  STDTIME2 time_begin;
  STDTIME2 time_end;

  *last_csv_time = 0;

  memset(&time_begin, 0, sizeof(STDTIME2));
  memset(&time_end, 0, sizeof(STDTIME2));

  time_begin.year = 1972;
  time_begin.day  = 1;
  time_end.year   = 3000;
  time_end.day    = 1;

  if ((retmsg=GetRecordRange(st_info->station, st_info->csv_chan, 
                            st_info->location,
                            time_begin, time_end, &first_record, &last_record,
                            &record_count, &loop_size)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
    exit(1);
  }

  if (max_to_check > record_count)
    max_to_check = record_count;
  index = last_record;

  if (gDebug)
  {
    printf("Record Range Info:\n");
    printf("  first record : %d\n", first_record);
    printf("  last record  : %d\n", last_record);
    printf("  record count : %d\n", record_count);
    printf("  loop size    : %d\n", loop_size);
    printf("  max to check : %d\n", max_to_check);
  }

  while (max_to_check-- > 0) {

    memset(seed_record, 0, iSeedRecordSize);
    if ((retmsg=ReadIndex(st_info->station,st_info->csv_chan,st_info->location,
                         index, (char*)seed_record)) != NULL)
    {
      fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
      exit(1);
    }
    index--;

    if (gDebug)
    {
      printf("Last SEED record (raw):\n");
//    format_data((uint8_t*)seed_record, iSeedRecordSize, 0, 0);
    }

    offset = (size_t)ntohs(*(uint16_t*)(seed_record + 46));
    // There must be at least one blockette within this record
    if (!offset) {
      goto next;
    }
    blockette = seed_record + offset;
    // Make sure the first blockette is a blockette 1000
    if (ntohs(*(uint16_t*)(blockette)) != 1000) {
      goto next;
    }
    offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
    // Make sure there is a blockette following this one
    if (!offset) {
      goto next;
    }

    do {
      blockette = seed_record + offset;
      offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
      // Make sure this is a blockette 2000
      if (ntohs(*(uint16_t*)(blockette)) != 2000) {
        if (gDebug) printf("  Founds an unexpected blockette!!\n"); // TODO XXX Remove 
        continue;
      }
      // Make sure this opaque blocketter contains either a complete
      // record or the first piece of a fragmented record
      if (((*(uint8_t*)(blockette + 13) & 0x0c) >> 2) > 1 ) {
        if (gDebug) printf("  Ran into a continuation blockette\n"); // TODO XXX Remove 
        continue;
      }

      opaque_data = blockette + (size_t)ntohs(*(uint16_t*)(blockette + 6));
      if ((ntohs(*(uint16_t*)(opaque_data)) & 0x7fff) > FALCON_VERSION) {
        continue;
      }


      // Verify csv opaque data
      if (!(ntohs(*(uint16_t*)(opaque_data)) & 0x8000))
      {
        if (!(*last_csv_time)) {
          memcpy(&netuint32, opaque_data+6, 4);
          *last_csv_time = (time_t)ntohl(netuint32);
          if (gDebug) printf("  Got FMash (CSV) time data.\n"); // TODO XXX Remove 
        } else {
          if (gDebug) printf("  Already have FMash (CSV) time data.\n"); // TODO XXX Remove 
        }
      }

      // If we have what we need, bail
      if (*last_csv_time)
        break;
    } while (offset);

next:
    // If we have what we need, bail
    if (*last_csv_time)
      break;
  }
  if (gDebug)
  {
    strftime(time_str, 32, "%Y/%m/%d %H:%M:%S", gmtime(last_csv_time));
    printf("Last CSV time is [%li] %s\n", *last_csv_time, time_str);
  }
} // GetLastTimeCSV

//////////////////////////////////////////////////////////////////////////////
void GetLastTimeAlarm( time_t* last_alarm_time, st_info_t* st_info)
{
  char *retmsg;

  uint8_t  seed_record[iSeedRecordSize];
  uint8_t* blockette = NULL;
  uint8_t* opaque_data = NULL;
  size_t offset = 0;
  char time_str[32];
  int max_to_check = 8;
  int index        = 0;
  uint32_t netuint32;

  int first_record = 0;
  int last_record  = 0;
  int record_count = 0;
  int loop_size    = 0;

  STDTIME2 time_begin;
  STDTIME2 time_end;

  *last_alarm_time = 0;

  memset(&time_begin, 0, sizeof(STDTIME2));
  memset(&time_end, 0, sizeof(STDTIME2));

  time_begin.year = 1972;
  time_begin.day  = 1;
  time_end.year   = 3000;
  time_end.day    = 1;

  if ((retmsg=GetRecordRange(st_info->station, st_info->alarm_chan, st_info->location,
                            time_begin, time_end, &first_record, &last_record,
                            &record_count, &loop_size)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
    exit(1);
  }

  if (max_to_check > record_count)
    max_to_check = record_count;
  index = last_record;

  if (gDebug)
  {
    printf("Record Range Info:\n");
    printf("  first record : %d\n", first_record);
    printf("  last record  : %d\n", last_record);
    printf("  record count : %d\n", record_count);
    printf("  loop size    : %d\n", loop_size);
    printf("  max to check : %d\n", max_to_check);
  }

  while (max_to_check-- > 0) {

    memset(seed_record, 0, iSeedRecordSize);
    if ((retmsg=ReadIndex(st_info->station,st_info->alarm_chan,
                     st_info->location, index, (char*)seed_record)) != NULL)
    {
      fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
      exit(1);
    }
    index--;

    if (gDebug)
    {
      printf("Last SEED record (raw):\n");
      format_data((uint8_t*)seed_record, iSeedRecordSize, 0, 0);
    }

    offset = (size_t)ntohs(*(uint16_t*)(seed_record + 46));
    // There must be at least one blockette within this record
    if (!offset) {
      goto next;
    }
    blockette = seed_record + offset;
    // Make sure the first blockette is a blockette 1000
    if (ntohs(*(uint16_t*)(blockette)) != 1000) {
      goto next;
    }
    offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
    // Make sure there is a blockette following this one
    if (!offset) {
      goto next;
    }

    do {
      blockette = seed_record + offset;
      offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
      // Make sure this is a blockette 2000
      if (ntohs(*(uint16_t*)(blockette)) != 2000) {
        continue;
      }
      // Make sure this opaque blocketter contains either a complete
      // record or the first piece of a fragmented record
      if (((*(uint8_t*)(blockette + 13) & 0x0c) >> 2) > 1 ) {
        if (gDebug) printf("  Ran into a continuation blockette\n"); // TODO XXX Remove 
        continue;
      }

      opaque_data = blockette + (size_t)ntohs(*(uint16_t*)(blockette + 6));
      if ((ntohs(*(uint16_t*)(opaque_data)) & 0x7fff) > FALCON_VERSION) {
        continue;
      }


      if (ntohs(*(uint16_t*)(opaque_data)) & 0x8000) {
        if (!(*last_alarm_time)) {
          memcpy(&netuint32, opaque_data+6, 4);
          *last_alarm_time = (time_t)ntohl(netuint32);
        }
      }

      // If we have what we need, bail
      if (*last_alarm_time)
        break;
    } while (offset);

next:
    // If we have what we need, bail
    if (*last_alarm_time)
      break;
  }
  if (gDebug)
  {
    strftime(time_str, 32, "%Y/%m/%d %H:%M:%S", gmtime(last_alarm_time));
    printf("Last Alarm time is [%li] %s\n", *last_alarm_time, time_str);
  }
} // GetLastTimeAlarm


//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  char  *retmsg;

  char  station[8];
  char  chan[4];
  char  loc[4];
  char  network[4];

  char  host[16];
  char  portstr[8];
  int   port;

  char username[MAXCONFIGLINELEN+2];
  char password[MAXCONFIGLINELEN+2];

  st_info_t st_info;

  unsigned int interval = 0;
  unsigned int unslept  = 0;

  time_t last_csv_time = 0;
  time_t last_alarm_time = 0;

  //char  msg[8192];

  // Url string for accessing falcon
  buffer_t* url_str = NULL;

  // Context structures
  csv_context_t* csv_buffers = NULL;
  alarm_context_t* alarm_lines = NULL;

  // Initialize the url buffer
  url_str = buffer_init();
  if (!url_str)
    goto clean;

  // Initialize csv buffer list
  csv_buffers = csv_context_init();
  if (!csv_buffers)
    goto clean;

  // Initialize alarm list
  alarm_lines = alarm_context_init();
  if (!alarm_lines)
    goto clean;

  // Check for debug mode verses daemon mode
  if (argc == 3 && strcmp("debug", argv[2]) == 0)
  {
    gDebug = 1;
  }
  else if (argc == 2)
  {
    gDebug = 0;
    daemonize();
  }
  else
  {
    ShowUsage();
    exit(1);
  }

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
    exit(1);
  }
  LoopRecordSize(&iSeedRecordSize);
  LogSNCL(station, network, chan, loc);

  // Set the channel name for our opaque blocks
  strcpy(loc, FALCON_LOC);

  FalconIP(host);
  FalconPort(&port);
  sprintf(portstr, "%d", port);

  FalconUsername(username);
  FalconPassword(password);
  if (!strlen(username))
    strcpy(username, station);
  if (!strlen(password))
    strcpy(password, station);

  // If FalconIP returns "0.0.0.0" exit and complain that no IP was set
  // in the configurtion file.  We do this so that a standard release
  // can have the falcon program self exit if a falcon is not part of
  // the hardware configuration.
  if (strcmp(host, "0.0.0.0") == 0)
  {
    if (gDebug)
      fprintf(stderr,
          "falcon:  No 'Falcon IP: <address>' specified in %s, exiting!\n",
          argv[1]);
    else
      syslog(LOG_NOTICE,
          "falcon:  No 'Falcon IP: <address>' specified in %s, exiting!\n",
          argv[1]);
    exit(0);
  }

  // Populate the station information struct
  st_info.station  = station;
  st_info.network  = network;
  st_info.alarm_chan  = FALCON_ALARM_CHAN;
  st_info.csv_chan  = FALCON_CSV_CHAN;
  st_info.location = loc;

  // Contstruct the base URL
  buffer_write(url_str, (uint8_t*)"http://", 7);
  buffer_write(url_str, (uint8_t*)username, strlen(username));
  buffer_write(url_str, (uint8_t*)":", 1);
  buffer_write(url_str, (uint8_t*)password, strlen(password));
  buffer_write(url_str, (uint8_t*)"@", 1);
  buffer_write(url_str, (uint8_t*)host, strlen(host));
  buffer_write(url_str, (uint8_t*)":", 1);
  buffer_write(url_str, (uint8_t*)portstr, strlen(portstr));

  // How long we wait between polling events
  interval = SLEEP_INTERVAL;
  if (gDebug) {
    interval = DEBUG_INTERVAL;
  }

  GetLastTimeCSV(&last_csv_time, &st_info);
  GetLastTimeAlarm(&last_alarm_time, &st_info);

  // Polling loop, which checks the Falcon for new data
  // every 'interval' seconds after the completion of
  // the previous check.
  while (1)
  {
    /* poll the Falcon for data, and record if available */
    PollFalcon(csv_buffers, alarm_lines, url_str, &st_info,
               last_csv_time, last_alarm_time);

    unslept = sleep(interval);
    if (gDebug) {
      fprintf(stdout, "slept %u seconds%s\n", (interval - unslept),
              unslept ? " (sleep interrupted)" : "" );
    }

    if (last_csv_time)
      last_csv_time = 0;
//    if (last_alarm_time && !list_empty(alarm_lines))
//      last_alarm_time = 0;
  }

clean:

  csv_buffers = csv_context_destroy(csv_buffers);
  alarm_lines = alarm_context_destroy(alarm_lines);
  url_str = buffer_destroy(url_str);

  return 0;
} // main()

