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
            Reorganize into unwrapped order going from oldest to newest
         3. New size < number of records in .buf file
            If newest record is after new end point then wrap that section
             to the beginning of the file.
            Calculate new oldest record
            Start moving records starting at new oldest to current index +2
              until the end of the buffer is reached.
            Truncate file size to match new max record count
          
            Complication for 2 and 3 is there might not be enough disk space 
            to create a duplicate of the file so the existing file has to
            be reorganized in place.
Update History:
mmddyy who Changes
==============================================================================
071509 fcs Creation
073109 fcs Implement case 2 and 3 fixes
080609 fcs Add truncate option to more quickly and gracefuly downsize
020910 fcs New diskloop.config keywords for falcon
******************************************************************************/
#define FILENAME "dlresize"
const char *VersionIdentString = "Release 1.3";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "include/diskloop.h"

void ShowUsage()
{
    printf(
"Usage:\n");
    printf(
"  dlresize <configfile> [truncate]\n"
"          truncate  -- causes data after the current index point to be\n"
"                       erased, and wrap size set to diskloop.config value\n"
"                       NOTE: does nothing if index > diskloop.config value.\n"
);
    fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

int main (int argc, char **argv)
{
  char  station[8];
  char  network[4];
  char  loc[4];
  char  chan[4];
  char  homedir[MAXCONFIGLINELEN+1];
  char  stationdir[MAXCONFIGLINELEN*2+1];
  char  idx_filename[MAXCONFIGLINELEN*2+1];
  char  buf_filename[MAXCONFIGLINELEN*2+1];
  char  msg[MAXCONFIGLINELEN+1];
  char  seedrec[8192];
  char  seedrec_2[8192];
  char  *retmsg;
  char *moved = NULL;
  int   config_numrecords;
  int   idx_numrecords;
  int   buf_numrecords;
  int   iIndexRecord;
  int   iLoopRecordSize;
  int   iSeek;
  int   bTruncate = 0;

  // Needed to navigate .idx files
  struct dirent *nextdir;
  DIR           *dirptr;
  FILE          *fp_idx;
  FILE          *fp_buf;
  int           namelength;

  // Check for right number off arguments
  if (argc < 2 || argc > 3)
  {
    ShowUsage();
    exit(1);
  }

  // Check for right number off arguments
  if (argc == 3)
  {
    if (strcmp(argv[2], "truncate") != 0)
    {
      ShowUsage();
      exit(1);
    }
    bTruncate = 1;
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

  // Get seed record size
  if ((retmsg=LoopRecordSize(&iLoopRecordSize)) != NULL)
  {
    fprintf(stderr, "%s Line %d LoopRecordSize() call failed: %s\n", 
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
    if ((retmsg=NumChanRecords(station, chan, loc, &config_numrecords)) != NULL)
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
    if ((retmsg=ParseIndexInfo(fp_idx, &iIndexRecord, &idx_numrecords)) != NULL)
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
      fprintf(stdout, "Resizing file %s\n", idx_filename);

      // Find out how many records are stored in the buffer file
      strcpy(buf_filename, idx_filename);
      strcpy(&buf_filename[strlen(buf_filename)-3], "buf");
      if ((fp_buf=fopen(buf_filename, "r+")) == NULL)
      {
        // Buffer file does not exist 
        fprintf(stderr, "%s Line %d fopen(%s) call failed!\n", 
               __FILE__, __LINE__, buf_filename);
        exit(1);
      }
      fseek(fp_buf, 0, SEEK_END);
      iSeek = ftell(fp_buf);
      buf_numrecords = iSeek / iLoopRecordSize;

      // Handle case 1
      // 1. New size >= number of records currently stored and no wrap.
      //    Fix is simply to change the .idx max record size to the new value
      if ((buf_numrecords < idx_numrecords && 
           config_numrecords >= buf_numrecords)
         || iIndexRecord == config_numrecords-1)
      {
        fclose(fp_buf);

        // Just make idx_numrecords match new configuration and done
        fseek(fp_idx, 0, SEEK_SET);
        sprintf(msg, "%d %d", iIndexRecord, config_numrecords);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
      } // New size >= number of records currently stored and no wrap
      else if (bTruncate)
      {
        // Handle case where we will not be able to fully truncate
        if (iIndexRecord > config_numrecords)
        {
          printf( "Index %d > new configuration value of %d,\n"
                  " only partial truncation possible.\n",
             iIndexRecord, config_numrecords);

          config_numrecords = iIndexRecord+1;
        }

        fseek(fp_idx, 0, SEEK_SET);
        sprintf(msg, "%d %d", iIndexRecord, config_numrecords);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);

        // Truncate the file length
        fclose(fp_buf);
        if (truncate(buf_filename, ((iIndexRecord+1)*iLoopRecordSize)) != 0)
        {
          // Buffer file does not exist 
          fprintf(stderr, "%s Line %d truncate(%s) call failed!\n", 
                 __FILE__, __LINE__, buf_filename);
          exit(1);
        }
fprintf(stderr, "DEBUG truncate to index record %d, new size %d from %d\n",
         iIndexRecord, config_numrecords, idx_numrecords);
      }
/*  These cases disabled since they can consume a day or more on gigabyte files
      else if (iIndexRecord >= config_numrecords)
      {
fprintf(stderr, "DEBUG wrap, resize decrease %d %d, index %d >= new end\n", config_numrecords, idx_numrecords, iIndexRecord);
        // Wrap the records past the new end point around to the beginning
        int j;
        for (j=config_numrecords; j <= iIndexRecord; j++)
        {
if (j % 1000 == 0)
fprintf(stderr, "DEBUG Finished %d of %d\r", j, iIndexRecord);
          // Get data from old position
          iSeek = j * iLoopRecordSize;
          fseek(fp_buf, iSeek, SEEK_SET);
          if (fread(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
          {
            fprintf(stderr, "%s Line %d fread(%d) call failed: %s\n", 
                    __FILE__, __LINE__, j, retmsg);
            exit(1);
          }

          // seek to new position and save the record
          iSeek = (j % config_numrecords) * iLoopRecordSize;
          fseek(fp_buf, iSeek, SEEK_SET);
          if (fwrite(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
          {
            fprintf(stderr, "%s Line %d fwrite(%d,%s) call failed: %s\n", 
                __FILE__, __LINE__, j%config_numrecords, buf_filename, retmsg);
            exit(1);
          } // Failed to write record
        } // loop through all records after new end point

        iIndexRecord = iIndexRecord % config_numrecords;
        fseek(fp_idx, 0, SEEK_SET);
        sprintf(msg, "%d %d", iIndexRecord, config_numrecords);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);

        // Truncate the file length
        fclose(fp_buf);
        if (truncate(buf_filename, (config_numrecords*iLoopRecordSize)) != 0)
        {
          // Buffer file does not exist 
          fprintf(stderr, "%s Line %d truncate(%s) call failed!\n", 
                 __FILE__, __LINE__, buf_filename);
          exit(1);
        }
      } // making wrapped file smaller, index is after wrap point
      else if (iIndexRecord < config_numrecords-1 && 
               config_numrecords < idx_numrecords)
      {
fprintf(stderr, "DEBUG wrap, resize decrease %d %d, index %d < new end\n", config_numrecords, idx_numrecords, iIndexRecord);
        // Eliminate the old records after iIndexRecord that there
        // is no longer room for, shift rest down
        int j;
        for (j=iIndexRecord+1+idx_numrecords-config_numrecords;
             j < idx_numrecords; j++)
        {
if (j % 1000 == 0)
fprintf(stderr, "DEBUG Finished %d of %d\r", j, idx_numrecords);
          // Get data from old position
          iSeek = j * iLoopRecordSize;
          fseek(fp_buf, iSeek, SEEK_SET);
          if (fread(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
          {
            fprintf(stderr, "%s Line %d fread(%d) call failed: %s\n", 
                    __FILE__, __LINE__, j, retmsg);
            exit(1);
          }

          // seek to new position and save the record
          iSeek = (j - idx_numrecords + config_numrecords) * iLoopRecordSize;
          fseek(fp_buf, iSeek, SEEK_SET);
          if (fwrite(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
          {
            fprintf(stderr, "%s Line %d fwrite(%d,%s) call failed: %s\n", 
                __FILE__, __LINE__, (j - idx_numrecords + config_numrecords),
                buf_filename, retmsg);
            exit(1);
          } // Failed to write record
        } // loop through all records after new end point

        fseek(fp_idx, 0, SEEK_SET);
        sprintf(msg, "%d %d", iIndexRecord, config_numrecords);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);

        // Truncate the file length
        fclose(fp_buf);
        if (truncate(buf_filename, (config_numrecords*iLoopRecordSize)) != 0)
        {
          // Trouble with truncate
          fprintf(stderr, "%s Line %d truncate(%s) call failed!\n", 
                 __FILE__, __LINE__, buf_filename);
          exit(1);
        }
      } // making wrapped file smaller, index is before wrap point
      else if (config_numrecords > idx_numrecords)
      {
fprintf(stderr, "DEBUG wrap, resize increase %d %d\n", config_numrecords, idx_numrecords);
        int i,j,k;
        // Turned wrapped records into linear filling up buffer order
        if ((moved = (char *)malloc(idx_numrecords)) == NULL)
        {
          fprintf(stderr, "%s Line %d malloc(%d) call failed!\n", 
                 __FILE__, __LINE__, idx_numrecords);
          exit(1);
        }
        memset(moved, 0, idx_numrecords);
        for (i=0; i < idx_numrecords; i++)
        {
if (i % 1000 == 0)
fprintf(stderr, "DEBUG Finished %d of %d\r", i, idx_numrecords);
          // Don't move it if we already have
          if (moved[i])
            continue;
          j = i;
          k = ((j - iIndexRecord - 1) + idx_numrecords) % idx_numrecords;
          if (j == k) continue;

          // read this first record
          iSeek = j * iLoopRecordSize;
          fseek(fp_buf, iSeek, SEEK_SET);
          if (fread(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
          {
            fprintf(stderr, "%s Line %d fread(%d) call failed: %s\n", 
                    __FILE__, __LINE__, j, retmsg);
            exit(1);
          }

          // move records to new locations
          while (1)
          {
            k = ((j - iIndexRecord - 1) + idx_numrecords) % idx_numrecords;

            // If the destination has not already been moved, save it's record
            if (!moved[k])
            {
              iSeek = k * iLoopRecordSize;
              fseek(fp_buf, iSeek, SEEK_SET);
              if (fread(seedrec_2, iLoopRecordSize, 1, fp_buf) != 1)
              {
                fprintf(stderr, "%s Line %d fread(%d) call failed: %s\n", 
                        __FILE__, __LINE__, k, retmsg);
                exit(1);
              }
            }

            // Save the j record in it's new location
            iSeek = k * iLoopRecordSize;
            fseek(fp_buf, iSeek, SEEK_SET);
            if (fwrite(seedrec, iLoopRecordSize, 1, fp_buf) != 1)
            {
              fprintf(stderr, "%s Line %d fwrite(%d,%s) call failed: %s\n", 
                  __FILE__, __LINE__, k, buf_filename, retmsg);
              exit(1);
            } // Failed to write record
            moved[j] = 1;

            // If the destination has not already been moved, repeat process
            if (!moved[k])
            {
              moved[k] = 1;
              j = k;
              memcpy(seedrec, seedrec_2, iLoopRecordSize);
            }
            else
            {
              // All done moving records for this value of i so exit loop
              break;
            }
          } // while destination record needs to also be moved
        } // for each record in the original file
        free(moved);
        moved = NULL;
        fclose(fp_buf);

        // Update index file
        fseek(fp_idx, 0, SEEK_SET);
        sprintf(msg, "%d %d", idx_numrecords-1, config_numrecords);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
        fprintf(fp_idx, "%-30.30s\n", msg);
      } // making wrapped file larger
*/
      else
      {
        fprintf(stderr,
"This resize case is not implemented, config size=%d, idx size=%d\n",
                config_numrecords, idx_numrecords);
        fprintf(stderr,
" buf size=%d, index=%d, skipping...\n", buf_numrecords, iIndexRecord);
        fclose(fp_buf);
      } // Unhandled case
    } // File needs to be resized

    fclose(fp_idx);
  } // Loop through all station data directory entries  
  return 0 ;
} // main()
