/*
File:       dlutil
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Allows user to do basic examination of diskloop files

Update History:
mmddyy who Changes
==============================================================================
020807 fcs Creation
******************************************************************************/
#define FILENAME "dlutil"
#define VERSION_DATE  "8 February 2007"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"

int main (int argc, char **argv)
{
  char  station[8];
  char  loc[4];
  char  chan[4];
  char  *retmsg;
  int   iFirst;
  int   iLast;
  int   iCount;
  int   iMaxRecord;

  STDTIME2  tStartTime;
  STDTIME2  tEndTime;

  signal (SIGPIPE, SIG_IGN) ; /* don't panic if LISS client disconnects */

  // Check for right number off arguments
  if (argc != 4 && argc != 6)
  {
    printf(
"Usage:\n");
    printf(
"  dlutil <configfile> <station> <loc/channel> [<start> <end>]\n");
    printf("    Start and end dates are yyyy,ddd,hh:mm:ss.ssss\n");
    printf("    Without timespan it lists all contiguous time blocks\n");
    printf("    With timespan it lists range of records within timespan\n");
    exit(1);
  }

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
    exit(1);
  }

  // Handle command line debug request to look up records
  if (argc == 6)
  {
    // pull station, location, channel out of first two arguments
    strncpy(station, argv[2], 8);
    station[5] = 0;
    if (argv[3][0] != '/')
    {
      loc[0] = argv[3][0];
      loc[1] = argv[3][1];
      loc[2] = 0;
      strncpy(chan, &argv[3][3], 4);
      chan[3] = 0;
    }
    else
    {
      // location code is blank
      strcpy(loc, "  ");
      strncpy(chan, &argv[3][1], 4);
      chan[3] = 0;
    }

    // Parse start time out of 3rd argument
    tStartTime = ST_ParseTime2((unsigned char *)argv[4]);

    // Parse end time out of 4th argument
    tEndTime = ST_ParseTime2((unsigned char *)argv[5]);

    // Get index of first and last data record in the requested time span
    retmsg = GetRecordRange(station, chan, loc, tStartTime, tEndTime,
                  &iFirst, &iLast, &iCount, &iMaxRecord);
    if (retmsg != NULL)
    {
      fprintf(stderr, "dlutil: %s\n", retmsg);
      exit(1);
    }

    printf("Returned %d records for %s %s/%s from %d to %d, max %d\n",
          iCount, station, loc, chan, iFirst, iLast, iMaxRecord);
    exit(0);
  } // command line arguments request data lookup

  // Handle command line debug request to Dump all spans for a channel
  if (argc == 4)
  {
    // pull station, location, channel out of first two arguments
    strncpy(station, argv[2], 8);
    station[5] = 0;
    if (argv[3][0] != '/')
    {
      loc[0] = argv[3][0];
      loc[1] = argv[3][1];
      loc[2] = 0;
      strncpy(chan, &argv[3][3], 4);
      chan[3] = 0;
    }
    else
    {
      // location code is blank
      strcpy(loc, "  ");
      strncpy(chan, &argv[3][1], 4);
      chan[3] = 0;
    }

    // Parse start time out of 3rd argument
    retmsg = DumpSpans(station, chan, loc);

    if (retmsg != NULL)
    {
      fprintf(stderr, "dlutil: %s\n", retmsg);
      exit(1);
    }

    exit(0);
  } // command line arguments request channel data dump

  return 0 ;
} // main()
