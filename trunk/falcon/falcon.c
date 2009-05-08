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
#define RELEASE "1.0"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/shm.h>

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
    fprintf(stderr, "Version %s  %s\n", RELEASE, __DATE__);
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
int main (int argc, char **argv)
{
  char  *retmsg;

  char  station[8];
  char  loc[4];
  char  chan[4];
  char  network[4];

  char  host[16];
  char  port[6];

  const char *name = "ANMO";

  const char *user = NULL;
  const char *pass = NULL;

  st_info_t st_info;

  int i = 0;
  unsigned int interval = 0;
  unsigned int unslept  = 0;

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
  // XXX: Comment the following out before releasing
  //* We are currently working on the ANMO Falcon
  user = name;
  pass = name;
  // */

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

  // Polling loop, which checks the Falcon for new data
  // every 'interval' seconds after the completion of
  // the previous check.
  while (1)
  {
    if (gDebug && (i++ == DEBUG_ITERATIONS))
      break;

    /* poll the Falcon for data */
    alarm_poll(alarm_lines, url_str, &st_info);
    csv_poll_channels(csv_buffers, url_str, &st_info);

    unslept = sleep(interval);
    fprintf(stdout, "slept %u seconds %s\n", (interval - unslept),
            unslept ? "(sleep interrupted)" : "" );
  }

clean:
  csv_buffers = csv_context_destroy(csv_buffers);
  alarm_lines = alarm_context_destroy(alarm_lines);
  url_str = buffer_destroy(url_str);

  return 0;
} // main()

