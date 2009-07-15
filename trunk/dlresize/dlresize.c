/*
File:       dlresize
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Resizes disk loop archive sizes to match the numbers in the
            diskloop.config files
            NOTE: Stop all aquisition software before running this

Resize cases:
         1. New size >= number of records currently stored and no wrap.
            Fix is simply to change the .idx max record size to the new value
         2. New size > .idx max size and a wrap has occured
            Currently not implemented
         3. New size < number of records in .buf file
            Calculate new oldest record
            Start moving records starting at oldest too current index +2
              until the end of the buffer is reached.
            Truncate file size to match new max record count
          
Update History:
mmddyy who Changes
==============================================================================
071509 fcs Creation
******************************************************************************/
#define FILENAME "dlresize"
const char *VersionIdentString = "Release 1.0";

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "include/diskloop.h"

int main (int argc, char **argv)
{
  char  station[8];
  char  network[4];
  char  loc[4];
  char  chan[4];
  char  homedir[MAXCONFIGLINELEN+1];
  char  stationdir[MAXCONFIGLINELEN*2+1];
  char  idx_filename[MAXCONFIGLINELEN*2+1];
  char  *retmsg;
  int   config_numrecords;
  int   idx_numrecords;
  int   iIndexRecord;

  // Needed to navigate .idx files
  struct dirent *nextdir;
  DIR           *dirptr;
  FILE          *fp_idx;
  int           namelength;

  // Check for right number off arguments
  if (argc != 2)
  {
    printf(
"Usage:\n");
    printf(
"  dlresize <configfile>\n");
    fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
    exit(1);
  }

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", FILENAME, retmsg);
    exit(1);
  }

  // Get home data directory
  if ((retmsg=LoopDirectory(homedir)) != NULL)
  {
    fprintf(stderr, "%s Line %d LoopDirectory() call failed: %s\n",
            __FILE__, __LINE__, retmsg);
    exit(1);
  }

  // Get station name
  if ((retmsg=LogSNCL(station, network, chan, loc)) != NULL)
  {
    fprintf(stderr, "%s Line %d LogSNCL() call failed: %s\n", 
            __FILE__, __LINE__, retmsg);
    exit(1);
  }


  // Get station directory
  strcpy(stationdir, homedir);
  stationdir[strlen(homedir)] = '/';
  strcpy(&stationdir[strlen(homedir)+1], station);
  stationdir[strlen(stationdir)+1] = 0;
  stationdir[strlen(stationdir)] = '/';

  // Open station data directory
  dirptr = opendir(stationdir);
  if (dirptr == NULL)
  {
    retmsg = strerror(errno);
    fprintf(stderr, "%s line %d: Unable to open data directory: '%s'\n",
          __FILE__, __LINE__, stationdir);
    fprintf(stderr, "opendir failed, errno=%d, '%s'\n", errno, retmsg);
    exit(1);
  }

  // Loop through directory files
  while ((nextdir=readdir(dirptr)) != NULL)
  {
    // Only check files that end in .idx
    namelength = strlen(nextdir->d_name);
    if (namelength < 5) continue;
    if (strcmp(&nextdir->d_name[namelength-4], ".idx") != 0)
      continue;

    // Get location and channel
    if (nextdir->d_name[2] == '_')
    {
      // location code is given
      strncpy(loc, nextdir->d_name, 2);
      loc[2] = 0;
      strncpy(chan, &nextdir->d_name[3], 3);
      chan[3] = 0;
    }
    else
    {
      // blank location
      strcpy(loc, "  ");
      strncpy(chan, nextdir->d_name, 3);
      chan[3] = 0;
    }

    // Get how many records diskloop.config says this loc/channel should have
    if ((retmsg=NumChanRecords(chan, loc, &config_numrecords)) != NULL)
    {
      fprintf(stderr, "%s Line %d NumChanRecords() call failed: %s\n", 
              __FILE__, __LINE__, retmsg);
      exit(1);
    }

    // Open the index file
    strcpy(idx_filename, stationdir);
    strcat(idx_filename, nextdir->d_name);
    if ((fp_idx=fopen(idx_filename, "r+")) == NULL)           
    {
      // Index file did not open 
      fprintf(stderr, "%s Line %d fopen(%s) call failed!\n", 
              __FILE__, __LINE__, idx_filename);
      exit(1);
    } // unable to open index file               

    // Find out how many records the index file says it has
    if (ParseIndexInfo(fp_idx, &iIndexRecord, &idx_numrecords) != NULL)
    {
      fprintf(stderr, "%s Line %d ParseIndexInfo() call failed: %s\n", 
              __FILE__, __LINE__, retmsg);
      exit(1);
    }
    
    // See if numbers match
    if (idx_numrecords != config_numrecords)
    {
      fprintf(stdout, "Configuration size %d does not match idx file size %d\n",
                      config_numrecords, idx_numrecords); 
      fprintf(stdout, "resizing file %s\n", idx_filename);
fprintf(stderr, "TODO resize\n");
    }

  } // Loop through all station data directory entries  
  return 0 ;
} // main()
