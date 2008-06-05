/*
File:       log330
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Server for log messages that go into common LOG seed file

Algorithm:  
Starts a message server in a backround thread.
The message server thread accepts connection reqeusts and spawns threads
Each client connection thread reads messages, and copies them to shared mem
loop in main program checks for new messages in shared mem
  When a new message arrives combine it with queued message if possible.
    If combine would be too large, send queued message, make new message
       the new queued message
  Once queued message reaches BUFFER_TIME_SEC in age:
    Send it out, empty queue
end loop

Update History:
mmddyy who Changes
==============================================================================
032607 fcs Creation
053008 fcs Remove daemon run options, q330arch serves this funcition now
******************************************************************************/
#define FILENAME "log330"
#define VERSION_DATE  "10 April 2007"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/log330.h"

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
    printf(
"Usage:\n");
    printf(
"  log330 <configfile> dump <loc> [<start> <end>]\n");
    printf("    Dump alone requests times and text of all log messages\n");
    printf("    Dump with time requests text within time frame\n");
    printf("    <loc> is the location code to use\n");
    printf("    The station, network, channel will be pulled from config\n");
    printf("    Start and end dates are yyyy,ddd,hh:mm:ss.ssss\n");
    printf(
"  log330 <configfile> msg 'message'\n");
    printf("    Sends the message string to the seed log server\n");
} // ShowUsage()

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  char  station[8];
  char  loc[4];
  char  chan[4];
  char  network[4];
  char  *retmsg;
  int   iFirst;
  int   iLast;
  int   iCount;
  int   iMaxRecord;
  int   iSeedRecordSize;
  int   i;
  int   year, doy, hour, min, sec;

  STDTIME2  tStartTime;
  STDTIME2  tEndTime;

  int   iRecord;
  int   iSeek;
  char  loopDir[MAXCONFIGLINELEN+1];
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  FILE  *fp_buf;
  char  str_record[8192];
  char  msg[8192];

  // Check for right number off arguments
  if (argc != 4 && argc != 6)
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

  // Handle msg request
  if (argc == 4)
  {
    if (strcmp(argv[2], "msg") == 0)
    {
      // Send arg 3 to the log server
      LogSNCL(station, network, chan, loc);
      sprintf(msg, "%s\r\n", argv[3]);
      if ((retmsg=log330Msg(msg, station, network, chan, loc)) != NULL)
      {
        // error trying to log the message
        fprintf(stderr, "log330: %s\n", retmsg);
        exit(1);
      } 
      exit(0);
    } // msg keyword parsed
  } // test for msg command

  // Handle command line dump request to look up records
  if (argc == 6 || argc == 4)
  {
    // Make sure dump keyword is present
    if (strcmp(argv[2], "dump") != 0)
    {
      fprintf(stderr, "Invalid keyword '%s'\n", argv[2]);
      exit(1);
    } // invalid keyword

    // Parse start time argument
    if (argc == 6)
      tStartTime = ST_ParseTime2((unsigned char *)argv[4]);
    else
      tStartTime = ST_ParseTime2((unsigned char *)"1950,1");

    // Parse end time argument
    if (argc == 6)
      tEndTime = ST_ParseTime2((unsigned char *)argv[5]);
    else
      tEndTime = ST_ParseTime2((unsigned char *)"3000,1");

    // Get index of first and last data record in the requested time span
    LogSNCL(station, network, chan, loc);
    strncpy(loc, argv[3], 2);
    retmsg = GetRecordRange(station, chan, loc, tStartTime, tEndTime,
                  &iFirst, &iLast, &iCount, &iMaxRecord);
    if (retmsg != NULL)
    {
      fprintf(stderr, "log330: %s\n", retmsg);
      exit(1);
    }

    printf("Returned %d records for %s %s/%s from %d to %d, max %d\n",
          iCount, station, loc, chan, iFirst, iLast, iMaxRecord);

    // Get name of buffer file
    // If blank location code, leave off leading location code in filename
    LoopDirectory(loopDir);
    if (loc[0] == ' ' || loc[0] == 0)
    {
      sprintf(buf_filename, "%s/%s/%s.buf",
          loopDir, station, chan);
    }
    else
    {
      sprintf(buf_filename, "%s/%s/%s_%s.buf",
          loopDir, station, loc, chan);
    }

    // Open the buffer file
    if ((fp_buf=fopen(buf_filename, "r")) == NULL)
    {
      // Buffer file does not exist so no records to return
      printf("Log file %s does not exist\n", buf_filename);
      exit(1);
    }

    for (i=0; i < iCount; i++)
    {
      iRecord = (i + iFirst) % iMaxRecord;
      iSeek = iRecord * iSeedRecordSize;

      // Seek to the record position
      fseek(fp_buf, iSeek, SEEK_SET);
      if (iSeek != ftell(fp_buf))
      {
        // If seek failed, we hit end of file, set iHigh
        fprintf(stderr, "log330: Unable to seek to %d in %s\n",
                iSeek, buf_filename);
        fclose(fp_buf);
        exit(1);
      } // Failed to seek to required file buffer position

      // Read in the data
      if (fread(str_record, iSeedRecordSize, 1, fp_buf) != 1)
      {
        fprintf(stderr, "log330: Unable to read record %d in %s",
                iRecord, buf_filename);
        fclose(fp_buf);
        exit(1);
      } // Failed to read record

      // Output timetag and message text
      SeedRecordMsg(msg, str_record, station, network, chan, loc,
                    &year, &doy, &hour, &min, &sec);
      
      printf("%d,%03d,%02d:%02d:%02d\n", year, doy, hour, min, sec);
      printf("%s\n", msg);

    } // loop through all records returned

    exit(0);
  } // command line arguments request data lookup
  return 0;
} // main()

