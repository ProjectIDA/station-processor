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
#define VERSION_DATE  "10 April 2009"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/shm.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"
#include "include/q330arch.h"

#define FALCON_IDSTRING  "FALC~"
#define FALCON_CHAN      "OFA"
#define FALCON_LOC       "90"

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
    printf(
"Usage:\n");
    printf(
"  falcon <configfile> <falcon IP> <falcon port> [<debug>]\n"
"         Retrieves status information from falcon and sends it via opaque\n"
"         seed records.\n"
"    <configfile>    -- Usually /etc/q330/DLG1/diskloop.config\n"
"    <falcon IP>     -- IP address of falcon controller\n"
"    <falcon port>   -- Port on falcon to connect to\n"
    );
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
      fprintf(stderr, "Sending seed record %d to q330arch\n", seqno);
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
        fprintf(stderr, "Added new opaque message to record %d\n", seqno);

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
      fprintf(stderr, "Creating new opaque message for record %d\n", seqno);
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

  char  msg[8192];

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

  //
  // Test code demonstrating log message usage
  //
  sprintf(msg, "Test falcon log message for %s %s %s/%s IP %s port %s\n",
          station, network, loc, chan,
          argv[2], argv[3]);
  if (gDebug)
    fprintf(stderr, "DEBUG %s, line %d, date %s: %s\n", 
            __FILE__, __LINE__, __DATE__, msg);
  if ((retmsg=q330LogMsg(msg, station, network, "LOG", loc)) != NULL)
  {
    // error trying to log the message
    if (gDebug)
      fprintf(stderr, "falcon: %s\n", retmsg);
    else
      syslog(LOG_ERR, "falcon: %s\n", retmsg);
    exit(1);
  }

// Temp test QueueOpaque and FlushOpaque code
{
char *msg;

msg="Test message 01 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 02 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 03 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 04 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 05 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 06 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 07 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 08 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 09 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 10 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 11 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 12 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 13 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 14 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);

msg="Test message 15 to fill up buffer record";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);
msg=
"An extremely long message that will take at least 3 512 byte seed records"
"to store.  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXZ";
QueueOpaque(msg, strlen(msg), 
station, network, chan, loc, FALCON_IDSTRING);
FlushOpaque();
} // debug code

  return 0;
} // main()

