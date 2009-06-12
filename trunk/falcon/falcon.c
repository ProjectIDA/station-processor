/*
File:       falcon
Copyright:  (C) 2009 by Albuquerque Seismological Laboratory
Purpose:    Server for collecting data from falcon and sending it out as seed
            data.

Algorithm:  

Update History:
mmddyy who Changes
==============================================================================
041009 fcs Create skeleton
******************************************************************************/
#define FILENAME "falcon"
const char *VersionIdentString = "Release 1.0";

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
int gDebug;

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
"  falcon <configfile> <falcon IP> <falcon port> [<debug>]\n"
"         Retrieves status information from falcon and sends it via opaque\n"
"         seed records.\n"
"    <configfile>    -- Usually /etc/q330/DLG1/diskloop.config\n"
"    <falcon IP>     -- IP address of falcon controller\n"
"    <falcon port>   -- Port on falcon to connect to\n"
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
    // Get the CSV data from the falcon
    csv_poll(csv_buffer_list, url_str, st_info, last_csv_time);

    // Get the alarm history from the falcon
    alarm_poll(alarm_lines, url_str, st_info, last_alarm_time);

    // Re-order alarms from earliest to latest
    list_sort(alarm_lines, 1);

    // Archive the latest data
    alarm_archive(alarm_lines, url_str, st_info);
    csv_archive(csv_buffer_list, url_str, st_info);

    // Ensure all opaque blockettes have been sent
    FlushOpaque();
}


void GetLastTimes( time_t* last_csv_time, time_t* last_alarm_time,
                   st_info_t* st_info)
{
  char *retmsg;

  uint8_t  seed_record[iSeedRecordSize];
  uint8_t* blockette = NULL;
  uint8_t* opaque_data = NULL;
  size_t offset = 0;
  char time_str[32];
  int max_to_check = 8;
  int index        = 0;

  int first_record = 0;
  int last_record  = 0;
  int record_count = 0;
  int loop_size    = 0;

  STDTIME2 time_begin;
  STDTIME2 time_end;

  *last_csv_time = 0;
  *last_alarm_time = 0;

  memset(&time_begin, 0, sizeof(STDTIME2));
  memset(&time_end, 0, sizeof(STDTIME2));

  time_begin.year = 1972;
  time_begin.day  = 1;
  time_end.year   = 3000;
  time_end.day    = 1;

  if ((retmsg=GetRecordRange(st_info->station, st_info->channel, st_info->location,
                            time_begin, time_end, &first_record, &last_record,
                            &record_count, &loop_size)) == NULL)
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

  while (max_to_check--) {

    memset(seed_record, 0, iSeedRecordSize);
    if ((retmsg=ReadIndex(st_info->station, st_info->channel, st_info->location,
                         index, (char*)seed_record)) == NULL)
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
      if (gDebug) printf("No data in SEED record\n"); // TODO XXX Remove 
      goto next;
    }
    if (gDebug) printf("Found SEED record with data\n"); // TODO XXX Remove 
    blockette = seed_record + offset;
    // Make sure the first blockette is a blockette 1000
    if (ntohs(*(uint16_t*)(blockette)) != 1000) {
      if (gDebug) printf("First blockette is not a blockette 1000\n"); // TODO XXX Remove 
      goto next;
    }
    if (gDebug) printf("Found a blockette 1000\n"); // TODO XXX Remove 
    offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
    // Make sure there is a blockette following this one
    if (!offset) {
      if (gDebug) printf("No data after blockette 1000\n"); // TODO XXX Remove 
      goto next;
    }
    if (gDebug) printf("There is more data :)\n"); // TODO XXX Remove 

    do {
      blockette = seed_record + offset;
      offset = (size_t)ntohs(*(uint16_t*)(blockette + 2));
      // Make sure this is a blockette 2000
      if (ntohs(*(uint16_t*)(blockette)) != 2000) {
        if (gDebug) printf("  Founds an unexpected blockette!!\n"); // TODO XXX Remove 
        continue;
      }
      if (gDebug) printf("  Found a blockette 2000\n"); // TODO XXX Remove 
      // Make sure this opaque blocketter contains either a complete
      // record or the first piece of a fragmented record
      if (((*(uint8_t*)(blockette + 13) & 0x0c) >> 2) > 1 ) {
        if (gDebug) printf("  Ran into a continuation blockette\n"); // TODO XXX Remove 
        continue;
      }
      if (gDebug) printf("  This is not a continuation blockette.\n"); // TODO XXX Remove 

      opaque_data = blockette + (size_t)ntohs(*(uint16_t*)(blockette + 6));
      if (ntohs(*(uint16_t*)(opaque_data)) & 0x8000) {
        *last_alarm_time = (time_t)ntohl(*(uint32_t*)(opaque_data + 6));
        if (gDebug) printf("  Got Alarm time data.\n"); // TODO XXX Remove 
      }
      else {
        *last_csv_time = (time_t)ntohl(*(uint32_t*)(opaque_data + 6));
        if (gDebug) printf("  Got FMash (CSV) time data.\n"); // TODO XXX Remove 
      }

      // If we have what we need, bail
      if (*last_alarm_time && *last_csv_time)
        break;
    } while (offset);

next:
    // If we have what we need, bail
    if (*last_alarm_time && *last_csv_time)
      break;
  }
  if (gDebug)
  {
    strftime(time_str, 32, "%Y/%m/%d %H:%M:%S", localtime(last_csv_time));
    printf("Last CSV time is [%li] %s\n", *last_csv_time, time_str);
    strftime(time_str, 32, "%Y/%m/%d %H:%M:%S", localtime(last_alarm_time));
    printf("Last Alarm time is [%li] %s\n", *last_alarm_time, time_str);
  }
}


//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  char  *retmsg;

  char  station[8];
  char  loc[4];
  char  chan[4];
  char  network[4];

  char  host[16];
  char  port[6];

  const char *user = NULL;
  const char *pass = NULL;

  st_info_t st_info;

  int i = 0;
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
  if (argc == 5 && strcmp("debug", argv[4]) == 0)
  {
    gDebug = 1;
  }
  else if (argc == 4)
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
  strcpy(chan, FALCON_CHAN);
  strcpy(loc, FALCON_LOC);

  strcpy(host, argv[2]);
  strcpy(port, argv[3]);

  user = station;
  pass = station;

  // Populate the station information struct
  st_info.station  = station;
  st_info.network  = network;
  st_info.channel  = chan;
  st_info.location = loc;

  // Contstruct the base URL
  buffer_write(url_str, (uint8_t*)"http://", 7);
  buffer_write(url_str, (uint8_t*)user, strlen(user));
  buffer_write(url_str, (uint8_t*)":", 1);
  buffer_write(url_str, (uint8_t*)pass, strlen(pass));
  buffer_write(url_str, (uint8_t*)"@", 1);
  buffer_write(url_str, (uint8_t*)host, strlen(host));
  buffer_write(url_str, (uint8_t*)":", 1);
  buffer_write(url_str, (uint8_t*)port, strlen(port));

  // How long we wait between polling events
  interval = SLEEP_INTERVAL;
  if (gDebug) {
    interval = DEBUG_INTERVAL;
  }

  GetLastTimes(&last_csv_time, &last_alarm_time, &st_info);

  // Polling loop, which checks the Falcon for new data
  // every 'interval' seconds after the completion of
  // the previous check.
  while (1)
  {
    if (gDebug && (i++ == DEBUG_ITERATIONS))
      break;

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
    if (last_alarm_time)
      last_alarm_time = 0;
  }

clean:
  csv_buffers = csv_context_destroy(csv_buffers);
  alarm_lines = alarm_context_destroy(alarm_lines);
  url_str = buffer_destroy(url_str);

  return 0;
} // main()

