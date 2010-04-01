/*
File:       diskloop.c
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Routines for accessing the diskloop software
Routines:
  Configuration file routines:
    ParseDiskLoopConfig
            Reads the configuration file.  Stores the values in static local
            variables so that callback routines can make calls to retrieve
            them.
    LoopRecordSize
            Returns how many bytes in each disk loop record
    NumChanRecords
            Returns the number of records for the given channel
    NumLogRecords
            Returns the number of records for the LOG file
    NetBufferSize
            Returns the number of bytes to buffer for netserver
    LoopDirectory
            Returns the directory string where archive files are stored
    LogServerPort
            Returns the port where driver sends updates to
    LogSNCL
            Returns Station, Network, Channel Location to use for log messages
            NOTE that some programs will want to use a different channel
    SetLogSNCL
            Can be used to change the defaults returned by LogSNCL
    WriteChan
            Writes a record of SEED data to the diskloop
    GetRecordRange
            Given a start and stop time, returns first and last index of
            records that fall within that range
    DumpSpans
            Debug type print of all the spans in the circular buffer
    NoIDA
            Add records to archive, but don't put them on IDA disk loop

Update History:
mmddyy who Changes
==============================================================================
012507 fcs Creation
050207 fcs Eliminate seed record time overlap caused by restarts
060107 fcs Add sequence number and samples return values to ParseSeedHeader
060107 fcs Use sequence number and samples for overwrite last record
102209 fcs Support for new Falcon configuration keywords
040110 fcs Add NoIDA keyword supporting channels for archive only
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>    // Needed for ntohl,ntohs 
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"

/*
First are the tools for getting data from the configuration file
These routines return NULL if everything is okay, or a pointer to
an error message string.

Since these routines can be called by callback functions, everything
is localy stored in static variables.
*/

// Need to know when/if the file has been successfully parsed
// 0 = not parsed
// 1 = parsed with no errors
// -1 = parsed with errors
static int parse_state=0;

// Will get filled in by Buffer: entries in configuration file
static struct s_bufconfig *pChanSizeList=NULL;

// Will get filled in by NoIDA: entries in configuration file
static struct s_bufconfig *pNoIDAList=NULL;

// Need a list of remapped station names
static struct s_mapstation *pMapStationList=NULL;

// Should either be 512 or 4096 to match SEED record size
static int                iLoopRecordSize=512;

// What port to send update messages to
static int                iLogServerPort=8888;

// How many bytes the netserver buffer should have
static int                iNetBufferSize=401408;

static int                iLogBufferDepth=1000;

// Where the archive data files are stored
static char loopDir[MAXCONFIGLINELEN+2];

static char looperrstr[MAXCONFIGLINELEN+2];

// Default Station, Network, Channel, Location to use for log messages
static char log_station[8] = "ZZZZ";
static char log_network[4] = "XX";
static char log_channel[4] = "LOG";
static char log_location[4] = "90";

// Username and Password option for Falcon
// If character pointers are null then use station name
static char *falcon_username=NULL;
static char *falcon_password=NULL;

// Falcon IP and port number
static char falcon_IP[MAXCONFIGLINELEN+2] = "0.0.0.0";
static int  falcon_port    = 5080;

//////////////////////////////////////////////////////////////////////////////
// Reads the configuration file.  Stores the values in static local
// variables so that callback routines can make calls to retrieve
// them.
char *ParseDiskLoopConfig(
  const char *filename    // Name of the configuration file
  )                       // returns NULL or an error string pointer
{
  FILE  *fp;
  char  linestr[MAXCONFIGLINELEN+2];
  char  argstr[MAXCONFIGLINELEN+2];
  char  argstr2[MAXCONFIGLINELEN+2];
  char  chan[MAXCONFIGLINELEN+2];
  char  loc[MAXCONFIGLINELEN+2];
  int   bOK=1;
  int   bParsed=1;
  int   iLineNum;
  int   iArgs;
  int   count;
  int   i;
  int   iMaxLoopSize;
  struct s_bufconfig *newbuf;
  struct s_mapstation *newmap;

  // Open the configuration file
  if ((fp = fopen(filename, "r")) == NULL)
  {
    sprintf(looperrstr, "ParseDiskLoopConfig: Failed to open %s", filename);
    return looperrstr;
  } // error opening file
  
  // Loop through all lines in the file
  iLineNum=0;
  while (fgets(linestr, MAXCONFIGLINELEN+1, fp) != NULL)
  {
    bParsed = 0;
    iLineNum++;

    // Check for comment
    if (linestr[0] == '#')
    {
      bParsed = 1;
      continue;
    }

    // Blank line
    if (linestr[0] == '\n' || linestr[0] == 0)
    {
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Directory: %s", argstr)) == 1)
    {
      strcpy(loopDir, argstr);
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Record Size: %d", &count)) == 1)
    {
      iLoopRecordSize = count;
      bParsed = 1;
      continue;
    }

    // Buffer: <loc>/<chan> size
    loc[0] = 0;
    chan[0] = 0;
    if ((iArgs=sscanf(linestr, "Buffer: %s %d", argstr, &count)) == 2)
    {
      // test for blank location code
      int i=0;
      if (argstr[0] == '/')
      {
        strcpy(loc, "  ");
        for (i=0; i < 3; i++)
         chan[i] = argstr[i+1];
      }
      else if (argstr[2] == '/')
      {
        loc[0] = argstr[0];
        loc[1] = argstr[1];
        loc[2] = 0;

        for (i=0; i < 3; i++)
          chan[i] = argstr[i+3];
        chan[i] = 0;
      } // else location code is not blank
      else argstr[0] = 0;

      if (argstr[0] != 0)
      {
        if (count < 4)
        {
          sprintf(looperrstr, "Count %d < minimum of 4, Line %d in %s",
            count, iLineNum, filename);
          fprintf(stderr, "%s\n%s\n", looperrstr, linestr);
        }
        else
        {
          // allocate space for new entry
          newbuf = (struct s_bufconfig *) malloc(sizeof (struct s_bufconfig));
          if (newbuf == NULL)
          {
            fprintf(stderr, "%s(%d): malloc in ParseDiskLoopConfig failed.\n",
                    __FILE__, __LINE__);
            exit(1);
          }

          // Fill  in record
          strncpy(newbuf->loc, loc, 4);
          newbuf->loc[3] = 0;
          strncpy(newbuf->chan, chan, 4);
          newbuf->chan[3] = 0;
          newbuf->records = count;

          // Insert new record at head of the list
          newbuf->next = pChanSizeList;
          pChanSizeList = newbuf;

          bParsed = 1;
          continue;
        } // else everything appears to be in order
      } // location/channel syntax appears to be okay
    } // Buffer: keyword parsed

    // NoIDA: <loc>/<chan>
    loc[0] = 0;
    chan[0] = 0;
    if ((iArgs=sscanf(linestr, "NoIDA: %s", argstr)) == 1)
    {
      // test for blank location code
      int i=0;
      if (argstr[0] == '/')
      {
        strcpy(loc, "  ");
        for (i=0; i < 3; i++)
         chan[i] = argstr[i+1];
      }
      else if (argstr[2] == '/')
      {
        loc[0] = argstr[0];
        loc[1] = argstr[1];
        loc[2] = 0;

        for (i=0; i < 3; i++)
          chan[i] = argstr[i+3];
        chan[i] = 0;
      } // else location code is not blank
      else argstr[0] = 0;

      if (argstr[0] != 0)
      {
        // allocate space for new entry
        newbuf = (struct s_bufconfig *) malloc(sizeof (struct s_bufconfig));
        if (newbuf == NULL)
        {
          fprintf(stderr, "%s(%d): malloc in ParseDiskLoopConfig failed.\n",
                   __FILE__, __LINE__);
          exit(1);
        }

        // Fill  in record
        strncpy(newbuf->loc, loc, 4);
        newbuf->loc[3] = 0;
        strncpy(newbuf->chan, chan, 4);
        newbuf->chan[3] = 0;
        newbuf->records = 0;

        // Insert new record at head of the list
        newbuf->next = pNoIDAList;
        pNoIDAList = newbuf;

        bParsed = 1;
        continue;
      } // location/channel syntax appears to be okay
    } // NoIDA: keyword parsed

    if ((iArgs=sscanf(linestr, "MapStation: %s %s\n", argstr, argstr2)) == 2)
    {
      // Verify station name lengths
      count = strlen(argstr);
      if (count < 1 || count > 5)
      {
        sprintf(looperrstr,
          "Invalid station name 1 length (%d not 1..5), Line %d in %s",
          count, iLineNum, filename);
        fprintf(stderr, "%s\n%s\n", looperrstr, linestr);
        bOK = 0;
        continue;
      }

      count = strlen(argstr2);
      if (count < 1 || count > 5)
      {
        sprintf(looperrstr,
          "Invalid station name 2 length (%d not 1..5), Line %d in %s",
          count, iLineNum, filename);
        fprintf(stderr, "%s\n%s\n", looperrstr, linestr);
        bOK = 0;
        continue;
      }
      
      // Allocate memory for new mapping
      newmap = (struct s_mapstation *) malloc(sizeof (struct s_mapstation));
      if (newmap == NULL)
      {
        fprintf(stderr, "malloc in ParseDiskLoopConfig failed.\n");
        exit(1);
      }

      for (i=0; i < 5 && argstr[i] != 0; i++)
      {
        newmap->station_q330[i] = toupper((int)argstr[i]);
      }
      newmap->station_q330[i] = 0;

      for (i=0; i < 5 && argstr2[i] != 0; i++)
      {
        newmap->station[i] = toupper((int)argstr2[i]);
      }
      newmap->station[i] = 0;

      newmap->next = pMapStationList;
      pMapStationList = newmap;

      bParsed = 1;
      continue;
    } // MapStation: parsed

    if ((iArgs=sscanf(linestr, "LogStation: %s\n", argstr)) == 1)
    {
      strncpy(log_station, argstr, 5);
      for (i=0; i<5 && isalnum((int)argstr[i]); i++)
        ; // find end of station
      log_station[i] = 0;
      bParsed = 1;
      continue;
    } // LogStation: parsed

    if ((iArgs=sscanf(linestr, "LogNetwork: %s\n", argstr)) == 1)
    {
      strncpy(log_network, argstr, 2);
      log_network[2] = 0;
      bParsed = 1;
      continue;
    } // LogNetwork: parsed

    if ((iArgs=sscanf(linestr, "LogChannel: %s\n", argstr)) == 1)
    {
      strncpy(log_channel, argstr, 3);
      log_channel[3] = 0;
      bParsed = 1;
      continue;
    } // LogChannel: parsed

    if ((iArgs=sscanf(linestr, "LogLocation: %s\n", argstr)) == 1)
    {
      strncpy(log_location, argstr, 2);
      log_location[3] = 0;
      bParsed = 1;
      continue;
    } // LogLocation: parsed

    if ((iArgs=sscanf(linestr, "Logbuffer: %d", &count)) == 1)
    {
      iLogBufferDepth = count;
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Netbuffer: %d", &count)) == 1)
    {
      iNetBufferSize = count;
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Logserver Port: %d", &count)) == 1)
    {
      iLogServerPort = count;
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Falcon Username: %s", argstr)) == 1)
    {
      falcon_username = malloc(sizeof(argstr)+1);
      strcpy(falcon_username, argstr);
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Falcon Password: %s", argstr)) == 1)
    {
      falcon_password = malloc(sizeof(argstr)+1);
      strcpy(falcon_password, argstr);
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Falcon IP: %s", argstr)) == 1)
    {
      strcpy(falcon_IP, argstr);
      bParsed = 1;
      continue;
    }

    if ((iArgs=sscanf(linestr, "Falcon Port: %d", &count)) == 1)
    {
      falcon_port = count;
      bParsed = 1;
      continue;
    }

    // If line not recognized then return an error
    if (!bParsed)
    {
      // Only last parse error message will be returned, all will be printed
      sprintf(looperrstr, "Failed to parse line %d in %s",
        iLineNum, filename);
      fprintf(stderr, "%s\n%s\n", looperrstr, linestr);
      bOK = 0;
    } // if parsing errors
  } // while more lines of data to parse

  fclose (fp);

  // Make sure that none of the diskloop depths will exceed 2 GByte
  iMaxLoopSize = (0x7fffffff / iLoopRecordSize) - 2;
  for (newbuf = pChanSizeList; newbuf != NULL; newbuf = newbuf->next)
  {
    if (newbuf->records > iMaxLoopSize)
    {
      sprintf(looperrstr,
          "Buffer: %s/%s %d -- Size exceeds 2 GByte limit, max %d!",
             newbuf->loc, newbuf->chan, newbuf->records, iMaxLoopSize);
      fprintf(stderr, "%s\n", looperrstr);
      bOK = 0;
    } // buffer was too large
  } // loop trough all buffer size definitions

  // Set parse state dependent upon any errors detected
  if (bOK)
    parse_state = 1;
  else
    parse_state = -1;

  if (!bOK) return looperrstr;
  return NULL;
} // ParseDiskLoopConfig()

//////////////////////////////////////////////////////////////////////////////
// Returns how many bytes in each disk loop record
char *LoopRecordSize(
  int   *size             // return bytes in each disk loop record
  )                       // returns NULL or an error string pointer
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "LoopRecordSize: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  *size = iLoopRecordSize;
  return NULL;
} // LoopRecordSize()

//////////////////////////////////////////////////////////////////////////////
// Tell whether the given location/channel is on the NoIDA channel list
int CheckNoIDA(
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  )
{
  int bFound = 0;
  int bMatch = 0;
  int i;
  struct s_bufconfig *ptr;
  struct s_bufconfig entry;

  if (parse_state == 0)
  {
    return 0;
  }
  
  // Loop through list, return last entry to match
  for (ptr=pNoIDAList; ptr != NULL; ptr = ptr->next)
  {
    // Check location code for match
    bMatch = 1;
    for (i=0; i < 2 && bMatch; i++)
    {
      if ((toupper(loc[i]) != toupper(ptr->loc[i]))
          && (ptr->loc[i] != '?'))
        bMatch = 0; 
    }

    // Check channel name for match
    for (i=0; i < 3 && bMatch; i++)
    {
      if ((toupper(chan[i]) != toupper(ptr->chan[i]))
          && (ptr->chan[i] != '?'))
        bMatch = 0; 
    }

    if (bMatch)
    {
      bFound = 1;
      entry = *ptr;
    }
  } // check all entries

  return bFound;
} // CheckNoIDA()

//////////////////////////////////////////////////////////////////////////////
// Returns the number of records for the given channel
char *NumChanRecords(
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int         *records
  )                       // returns NULL or an error string pointer
{
  int bFound = 0;
  int bMatch = 0;
  int i;
  struct s_bufconfig *ptr;
  struct s_bufconfig entry;

  if (parse_state == 0)
  {
    sprintf(looperrstr, "NumChanRecords: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }
  
  // Loop through list, return last entry to match
  for (ptr=pChanSizeList; ptr != NULL; ptr = ptr->next)
  {
    // Check location code for match
    bMatch = 1;
    for (i=0; i < 2 && bMatch; i++)
    {
      if ((toupper(loc[i]) != toupper(ptr->loc[i]))
          && (ptr->loc[i] != '?'))
        bMatch = 0; 
    }

    // Check channel name for match
    for (i=0; i < 3 && bMatch; i++)
    {
      if ((toupper(chan[i]) != toupper(ptr->chan[i]))
          && (ptr->chan[i] != '?'))
        bMatch = 0; 
    }

    if (bMatch)
    {
      bFound = 1;
      entry = *ptr;
    }
  } // check all entries

  if (bFound)
    *records = entry.records;

  if (!bFound)
  {
    sprintf(looperrstr, "NumChanRecords: No Buffer: entries matching %s/%s",
      loc, chan);
    return looperrstr;
  }

  return NULL;
} // NumChanRecords()

//////////////////////////////////////////////////////////////////////////////
// Returns the number of LOG records
char *NumLogRecords(
  int *records            // stores how many records disk buffer stores
  )                       // returns NULL or an error string pointer
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "NumLogRecords: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  *records = iLogBufferDepth;
  return NULL;
} // NumLogRecords()

//////////////////////////////////////////////////////////////////////////////
// Returns the directory string where archive files are stored
char *LoopDirectory(
  char *dir               // Should have MAXCONFIGLINELEN+1 chars allocated
  )                       // returns NULL or an error string pointer
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "LoopDirectory: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  strcpy(dir, loopDir);
  return NULL;
} // LoopDirectory()

//////////////////////////////////////////////////////////////////////////////
// Returns the number of bytes to use for the netserver buffer
char *NetBufferSize(
  int *size               // return value for buffer size in bytes
  )
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "NetBufferSize: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  *size = iNetBufferSize;
  return NULL;
} // NetBufferSize()

//////////////////////////////////////////////////////////////////////////////
// Return the Station, Network, Channel, Location to use for log messages
char *LogSNCL(char *station, char *network, char *channel, char *location)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "LogSNCL: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }
  strcpy(station, log_station);
  strcpy(network, log_network);
  strcpy(channel, log_channel);
  strcpy(location, log_location);
  return NULL;
} // LogSNCL()


//////////////////////////////////////////////////////////////////////////////
// Used to change the defaults returned by LogSNCL
char *SetLogSNCL(const char *station, const char *network,
                 const char *channel, const char *location)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "SetLogSNCL: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }
  strncpy(log_station, station, 5);
    log_station[4] = 0;
  strncpy(log_network, network, 2);
    log_network[2] = 0;
  strncpy(log_channel, channel, 3);
    log_channel[3] = 0;
  strncpy(log_location, location, 2);
    log_location[2] = 0;
  return NULL;
} // SetLogSNCL()

//////////////////////////////////////////////////////////////////////////////
// Returns the TCP port where log messages are sent to
char *LogServerPort(
  int *port               // returns port number
  )                       // returns NULL or an error string pointer
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "LoopServerPort: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  *port = iLogServerPort;
  return NULL;
} // LoopServerPort()

//////////////////////////////////////////////////////////////////////////////
// Returns username to log into falcon, NULL means use station name
char *FalconUsername(char *username)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "FalconUsername: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  if (falcon_username != NULL)
    strcpy(username, falcon_username);
  else
    username[0] = 0;

  return NULL;
} // FalconUsername()

//////////////////////////////////////////////////////////////////////////////
// Returns password to log into falcon, NULL means use station name
char *FalconPassword(char *password)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "FalconPassword: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  if (falcon_password != NULL)
    strcpy(password, falcon_password);
  else
    password[0] = 0;

  return NULL;
} // FalconPassword()

//////////////////////////////////////////////////////////////////////////////
// Returns Falcon IP address
char *FalconIP(char *ip)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "FalconIP: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  strcpy(ip, falcon_IP);

  return NULL;
} // FalconIP()

//////////////////////////////////////////////////////////////////////////////
// Returns Falcon port number
char *FalconPort(int *port)
{
  if (parse_state == 0)
  {
    sprintf(looperrstr, "FalconPort: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  *port = falcon_port;
  return NULL;
} // FalconPort()

//////////////////////////////////////////////////////////////////////////////
// Station name from 330 contains Network ID, this code strips off network
// Also maps q330 station name to final station name if needed
// Returns pointer to string after network ID
char *StripNetworkID(
  const char  *station    // Station name possibly includeing network id
  )                       // returns pointer skipping past network id
{
  int i;
  char *retstation;
  struct s_mapstation *ptr;

  for (i=0; station[i] != 0 && station[i] != '-'; i++)
    ; // just find end of string, of network-station separator

  if (station[i] == '-')
    retstation = (char *)&station[i+1];
  else
    retstation = (char *)station;

  // Now remap q330 station name if match found
  for (ptr=pMapStationList; ptr != NULL; ptr=ptr->next)
  {
    if (strcmp(retstation, ptr->station_q330) == 0)
    {
      retstation = ptr->station;
      break;
    }
  } // check all station name remap entries

  return retstation;
} // StripNetworkID()

//////////////////////////////////////////////////////////////////////////////
// Called when we need to start a new buffer set
char *NewBuffer(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  )                       // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  char  msg[2*MAXCONFIGLINELEN+2];
  FILE  *fp_buf;
  FILE  *fp_idx;
  int   iRecord;
  int   iMaxRecord;

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Create/truncate buffer file
  if ((fp_buf=fopen(buf_filename, "w")) == NULL)
  {
    // Buffer file does not exist so start a new set
    sprintf(looperrstr, "NewBuffer: failed to create %s",
             idx_filename);
    return looperrstr;
  }
  fclose(fp_buf);

  // Create/truncate index file
  if ((fp_idx=fopen(idx_filename, "w")) == NULL)
  {
    sprintf(looperrstr, "NewBuffer: failed to find %s",
             idx_filename);
    return looperrstr;
  } // unable to create index file

  // Get how many records this channel should have
  if (NumChanRecords(chan, loc, &iMaxRecord) != NULL)
  {
    fclose(fp_idx);
    return looperrstr;
  }

  // Update the index file
  iRecord = -1;
  sprintf(msg, "%d %d", iRecord, iMaxRecord);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fclose(fp_idx);

  return NULL;
} // NewBuffer()

//////////////////////////////////////////////////////////////////////////////
// The index info is repeated 3 times.  So even if we read it during the
// middle of a write, either the first two, or second two lines should agree.
char *ParseIndexInfo(
  FILE *fp_idx,       // opened file pointer to index file
  int  *iFlipRecord,  // Returns current position within circular buffer
  int  *iMaxRecord    // Maximum size of circular buffer
  )                   // NULL okay, error string otherwise
{
  int flip1,flip2,flip3;
  int max1,max2,max3;
  int iArg;
  int iCount;
  char indexbuf[93];

  if ((iCount=fread(indexbuf, 1, 93, fp_idx)) != 93)
  {
    sprintf(looperrstr, "ParseIndexInfo:  Index data file size != 93 chars");
    return looperrstr;
  }

  if ((iArg = sscanf(&indexbuf[0], "%d %d", &flip1, &max1)) != 2)
  {
    sprintf(looperrstr, "ParseIndexInfo: format error in index file line 1");
    return looperrstr;
  }
  *iFlipRecord = flip1;
  *iMaxRecord = max1;
  if ((iArg = sscanf(&indexbuf[31], "%d %d", &flip2, &max2)) != 2)
  {
    sprintf(looperrstr, "ParseIndexInfo: format error in index file line 3");
    return looperrstr;
  }

  // If line 1 and 2 are in agreement we are done
  if (flip1 == flip2 || max1 == max2)
    return NULL;

  // Otherwise line 3 will contain uncontaminated data
  // Because write point had to be between line 1 and 2
  if ((iArg = sscanf(&indexbuf[62], "%d %d", &flip3, &max3)) != 2)
  {
    sprintf(looperrstr, "ParseIndexInfo: format error in index file line 3");
    return looperrstr;
  }

  *iFlipRecord = flip3;
  *iMaxRecord = max3;
  return NULL;
} // ParseIndexInfo()

//////////////////////////////////////////////////////////////////////////////
// Write a record of SEED data
// Hides all of the logic needed to run a circular buffer from main program
char *WriteChan(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf    // Seed record pointer
  )                       // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  char  msg[2*MAXCONFIGLINELEN+2];
  FILE  *fp_buf;
  FILE  *fp_idx;
  int   iRecord;
  int   iMaxRecord;
  int   iSeek;

  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "WriteChan: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0 || loc[0] == '_')
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so start a new set
    if (NewBuffer(station, chan, loc) != NULL)
      return looperrstr;
  }
  if (fp_buf != NULL) fclose(fp_buf);

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so start fresh
    if (NewBuffer(station, chan, loc) != NULL)
      return looperrstr;
    if ((fp_idx=fopen(idx_filename, "r")) == NULL)
    {
      sprintf(looperrstr, "WriteChan: failed to find %s",
               idx_filename);
      return looperrstr;
    }
  } // unable to open index file

  // Load index info
  if (ParseIndexInfo(fp_idx, &iRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "WriteChan: Index file format error in %s",
             idx_filename);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  if (iRecord < -1 || iRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "WriteChan: Invalid index -1 <= %d < %d in %s",
            iRecord, iMaxRecord, idx_filename);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value
  fclose(fp_idx);

  // Get the header for the last record written
  if (iRecord >= 0)
  {
    char      tempbuf[4096];
    char      rec_station[6];
    char      rec_chan[4];
    char      rec_loc[4];
    STDTIME2  rec_tStart;
    STDTIME2  rec_tEnd;
    STDTIME2  cur_tStart;
    STDTIME2  cur_tEnd;
    DELTA_T2  tDelta;
    long      lMs;
    int       rec_seqnum;
    int       rec_samples;
    int       cur_seqnum;
    int       cur_samples;
    int       iRateFactor;
    seed_header *pheader;


    // A previous record exists
    if (ReadLast(station, chan, loc, tempbuf) == NULL)
    {
      // We successfully read last record so get timespan
      ParseSeedHeader(tempbuf, rec_station, rec_chan, rec_loc,
                      &rec_tStart, &rec_tEnd, &rec_seqnum, &rec_samples);
      // Get timespan for current record
      ParseSeedHeader(databuf, rec_station, rec_chan, rec_loc,
                      &cur_tStart, &cur_tEnd, &cur_seqnum, &cur_samples);

      // Test to see if this record Starts before last record ended
      // Only test records with data in them
      tDelta = ST_DiffTimes2(cur_tStart, rec_tEnd);
      lMs = ST_DeltaToMS2(tDelta);
      pheader = (seed_header *)databuf;
      iRateFactor = (short)ntohs(pheader->sample_rate_factor);
      if (cur_samples > 0 && rec_samples > 0 && iRateFactor != 0 &&
          lMs < -1)
      {
        printf("DEBUG detected overlap (%ld) %s %s/%s new record %d vs %d.\n",
            lMs, station, loc, chan, cur_seqnum, rec_seqnum);
        printf("%s < ", ST_PrintDate2(cur_tStart, TRUE));
        printf("%s\n", ST_PrintDate2(rec_tEnd, TRUE));
      }
      if (cur_samples > 0 && rec_samples > 0 && iRateFactor != 0 &&
          lMs > 1)
      {
        printf("DEBUG detected gap (%ld) %s %s/%s new record %d vs %d.\n",
            lMs, station, loc, chan, cur_seqnum, rec_seqnum);
        printf("%s > ", ST_PrintDate2(cur_tStart, TRUE));
        printf("%s\n", ST_PrintDate2(rec_tEnd, TRUE));
      }

/* Remove former 4096 check for overwrite record
      // Check for matching record number and a greater number of samples
      if (rec_seqnum == cur_seqnum && rec_samples < cur_samples)
      {
        // overwrite older partial data
        // Cause an overwrite by decrementing pointer
        iRecord--;
        fprintf(stdout, "Replacing duplicate data for %s %s/%s\n",
                rec_station, rec_loc, rec_chan);
      } // sequence numbers match, and additional data
*/
    } // Able to read previuos record
  } // previous record should exist

  // Increment index to point to next free record
  iRecord = (iRecord + 1) % iMaxRecord;

  // Open the buffer file for write access
  if ((fp_buf=fopen(buf_filename, "r+")) == NULL)
  {
    // Buffer file does not exist so start a new set
    sprintf(looperrstr, "WriteChan: Failed to open %s for updating",
            buf_filename);
    return looperrstr;
  }

  // Go to desired position in the circular buffer file
  iSeek = iRecord * iLoopRecordSize;
  fseek(fp_buf, iSeek, SEEK_SET);
  if (iSeek != ftell(fp_buf))
  {
    sprintf(looperrstr, "WriteChan: Unable to seek to record %d in %s",
            iRecord, buf_filename);
    fclose(fp_buf);
    return looperrstr;
  } // Failed to seek to required file buffer position

  // Write the new record
  if (fwrite(databuf, iLoopRecordSize, 1, fp_buf) != 1)
  {
    sprintf(looperrstr, "WriteChan: Unable to write record %d in %s",
            iRecord, buf_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    return looperrstr;
  } // Failed to write record
  fclose(fp_buf);

  // Write the new index values
  if ((fp_idx=fopen(idx_filename, "r+")) == NULL)
  {
    // Index file does not exist so start fresh
    sprintf(looperrstr, "WriteChan: failed to open %s for updating",
             idx_filename);
    return looperrstr;
  } // unable to open index file

  // Update the index file
  // index is writen three times so reader can detect midstream update
  sprintf(msg, "%d %d", iRecord, iMaxRecord);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fprintf(fp_idx, "%-30.30s\n", msg);
  fclose(fp_idx);

//fprintf(stdout, "DEBUG WriteChan %s/%s to %d/%d\n",
//loc, chan, iRecord, iMaxRecord);
  return NULL;
} // WriteChan()

//////////////////////////////////////////////////////////////////////////////
// Reads the seed record at the specified index location
// Nominal index is 0 .. iLoopSize-1, but code will wrap index to legal value
// Return NULL if no errors
// If no data exists , the first byte of databuf will be null
// Returns error string if something bad happened.
char *ReadIndex(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int         index,      // Index of record to read, will wrap index if needed
  char        *databuf    // Seed record pointer
  )                       // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  FILE  *fp_buf;
  FILE  *fp_idx;
  int   iRecord;
  int   iMaxRecord;
  int   iSeek;

  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "ReadLast: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no records to return
    databuf[0] = 0;
    return NULL;
  }

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so no last record to return
    fclose(fp_buf);
    databuf[0] = 0;
    return NULL;
  }

  // Load index info
  if (ParseIndexInfo(fp_idx, &iRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "ReadLast: Data format error in %s",
             idx_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  if (iRecord < 0 || iRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "ReadLast: Invalid index 0 <= %d < %d in %s",
            iRecord, iMaxRecord, idx_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  // Normalize user requested index
  if (index < 0)
    index += (1+index/iMaxRecord)*iMaxRecord;
  if (index >= iMaxRecord)
    index = index % iMaxRecord;

  // Get data at the specified index
  iSeek = index * iLoopRecordSize;
  fseek(fp_buf, iSeek, SEEK_SET);
  if (iSeek != ftell(fp_buf))
  {
    sprintf(looperrstr, "ReadLast: Unable to seek to record %d in %s",
            index, buf_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // Failed to seek to required file buffer position

  if (fread(databuf, iLoopRecordSize, 1, fp_buf) != 1)
  {
    sprintf(looperrstr, "ReadLast: Unable to read record %d in %s",
            iRecord, buf_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // Failed to read record

//fprintf(stdout,"DEBUG ReadLast(%s,%s,%s) return %d/%d\n",
//station, chan, loc, iRecord, iMaxRecord);
  fclose(fp_buf);
  fclose(fp_idx);
  return NULL;
} // ReadIndex()

//////////////////////////////////////////////////////////////////////////////
// Read the last record of data written to this channel
// Required to implement the MSA_GETARC callback from q330lib
char *ReadLast(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf    // Seed record pointer
  )                       // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  FILE  *fp_buf;
  FILE  *fp_idx;
  int   iRecord;
  int   iMaxRecord;
  int   iSeek;

  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "ReadLast: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }


  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no last record to return
    databuf[0] = 0;
    return NULL;
  }

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so no last record to return
    fclose(fp_buf);
    databuf[0] = 0;
    return NULL;
  }

  // Load index info
  if (ParseIndexInfo(fp_idx, &iRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "ReadLast: Data format error in %s",
             idx_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  if (iRecord < 0 || iRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "ReadLast: Invalid index 0 <= %d < %d in %s",
            iRecord, iMaxRecord, idx_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  // Get data at the specified index
  iSeek = iRecord * iLoopRecordSize;
  fseek(fp_buf, iSeek, SEEK_SET);
  if (iSeek != ftell(fp_buf))
  {
    sprintf(looperrstr, "ReadLast: Unable to seek to record %d in %s",
            iRecord, buf_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // Failed to seek to required file buffer position

  if (fread(databuf, iLoopRecordSize, 1, fp_buf) != 1)
  {
    sprintf(looperrstr, "ReadLast: Unable to read record %d in %s",
            iRecord, buf_filename);
    databuf[0] = 0;
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // Failed to read record

//fprintf(stdout,"DEBUG ReadLast(%s,%s,%s) return %d/%d\n",
//station, chan, loc, iRecord, iMaxRecord);
  fclose(fp_buf);
  fclose(fp_idx);
  return NULL;
} // ReadLast()

//////////////////////////////////////////////////////////////////////////////
// Parses str_header to get station, channel, location, time start/end
char *ParseSeedHeader(
const char    *str_header,  // Header buffer
char          *station,     // return station name
char          *chan,        // return Channel ID
char          *loc,         // return Location ID
STDTIME2      *ptRecStart,  // Returns start time for record
STDTIME2      *ptRecEnd,    // Returns end time for record
int           *piSeqNum,    // Returns the sequence number for the record
int           *piSamples    // Returns the number of samples
  )                         // returns NULL or an error string pointer
{
  int i;
  int iRateFactor;
  int iRateMult;
  int iSamples;
  int iSpanDay;
  int iSpanHour;
  int iSpanMin;
  int iSpanSec;
  int iSpanTMSec;
  double dSampleRate;
  seed_header *pheader;
  char str[10];

  pheader = (seed_header *)str_header;

  // Parse the sequence number
  *piSeqNum = 0;
  strncpy(str, pheader->sequence, 6);
  str[6] = 0;
  sscanf(str, "%d", piSeqNum);
  
  // Parse station name
  for (i=0; i < 5 
       && isalnum((int)pheader->station_id_call_letters[i]); i++)
    station[i] = pheader->station_id_call_letters[i];
  station[i] = 0;

  // Parse location name
  for (i=0; i < 2; i++)
    loc[i] = pheader->location_id[i];
  loc[i] = 0;

  // Parse channel name
  for (i=0; i < 3; i++)
    chan[i] = pheader->channel_id[i];
  chan[i] = 0;

  // Parse Record Start time
  ptRecStart->year = ntohs(pheader->yr);
  ptRecStart->day = ntohs(pheader->jday);
  ptRecStart->hour = (int)pheader->hr;
  ptRecStart->minute = (int)pheader->minute;
  ptRecStart->second = (int)pheader->seconds;
  ptRecStart->tenth_msec = ntohs(pheader->tenth_millisec);

  // Parse samples
  iSamples = (unsigned short)ntohs(pheader->samples_in_record);
  iRateFactor = (short)ntohs(pheader->sample_rate_factor);
  iRateMult = (short)ntohs(pheader->sample_rate_multiplier);

  *piSamples = iSamples;

  // Get sample rate, See SEED Reference Manual Chp 8
  // Fixed Section of Data Header for formulas
  dSampleRate = 0;
  if (iRateFactor > 0 && iRateMult > 0)
    dSampleRate = (double)(iRateFactor * iRateMult);

  if (iRateFactor > 0 && iRateMult < 0)
    dSampleRate = -((double)iRateFactor / (double)iRateMult);
  
  if (iRateFactor < 0 && iRateMult > 0)
    dSampleRate = -((double)iRateMult / (double)iRateFactor);

  if (iRateFactor < 0 && iRateMult < 0)
    dSampleRate = 1.0 / (double)(iRateFactor * iRateMult);

  // Get Span time in tenths of milliseconds
  if (iRateFactor != 0 && iRateMult != 0)
    iSpanTMSec = (int)((iSamples / dSampleRate) * 10000.0);
  else
    iSpanTMSec = 0;

//fprintf(stdout, "Samp=%d Factor=%d Mult=%d dSampleRate=%.5f, TMSec=%d\n",
//iSamples, iRateFactor, iRateMult, dSampleRate, iSpanTMSec);


  // Add to start time to get end time
  iSpanSec = iSpanTMSec/10000;
  iSpanTMSec = iSpanTMSec % 10000;
  iSpanMin = iSpanSec / 60;
  iSpanSec = iSpanSec % 60;
  iSpanHour = iSpanMin / 60;
  iSpanMin = iSpanMin % 60;
  iSpanDay = iSpanHour / 24;
  iSpanHour = iSpanHour % 24;
//fprintf(stdout, "%s + ", ST_PrintDate2(*ptRecStart, 1));
//fprintf(stdout, "%03d,%02d:%02d:%02d.%04d",
//iSpanDay, iSpanHour, iSpanMin, iSpanSec, iSpanTMSec);
  *ptRecEnd = ST_AddToTime2(*ptRecStart, 
          iSpanDay, iSpanHour, iSpanMin, iSpanSec, iSpanTMSec);
//fprintf(stdout, " = %s\n", ST_PrintDate2(*ptRecEnd, 1));
  return NULL;
} // ParseSeedHeader()

//////////////////////////////////////////////////////////////////////////////
// Read the last record of data written to this channel
char *GetRecordRange(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  STDTIME2    tBeginTime, // Start time
  STDTIME2    tEndTime,   // End time
  int         *iFirst,    // Returns the first record index within the time
  int         *iLast,     // Returns the last record index within the time
  int         *iCount,    // Returns number of records within the time
  int         *iLoopSize  // Returns size of circular buffer in records
  )                       // returns NULL or an error string pointer
                          // iFirst == -1 if no records were found
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  char  str_header[FRAME_SIZE];
  char    recStation[8];
  char    recLoc[4];
  char    recChan[4];
  FILE  *fp_buf;
  FILE  *fp_idx;
  STDTIME2    tRecStart;
  STDTIME2    tRecEnd;
  int   iFlipRecord;
  int   iMaxRecord;
  int   iRecord;
  int   iSeek;
  int   iHigh;
  int   iLow;
  int   iMid;
  int   iPre;
  int   iPost;
  int   iCmp;
  int   iSeqNum;
  int   iSamples;

  *iFirst = *iLast = *iLoopSize = -1;
  *iCount = 0;

  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "GetRecordRange: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no last record to return
    return NULL;
  }

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so no last record to return
    fclose(fp_buf);
    return NULL;
  }

  // Load index info
  if (ParseIndexInfo(fp_idx, &iFlipRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "GetRecordRange: Data format error in %s",
             idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  // Skip records just ahead of write point as a safety buffer
  iLow = LOOPDEADRECORDS+1;
  iHigh = iMaxRecord;

  // Check for special case of circular buffer has not yet been filled once
  // If missing records would compose our dead space we don't care
  fseek(fp_buf, 0, SEEK_END);
  iSeek = ftell(fp_buf);
  if ((iMaxRecord-LOOPDEADRECORDS) * iLoopRecordSize > iSeek)
  {
    iMaxRecord = iSeek/iLoopRecordSize;
    iHigh = iMaxRecord;
    iLow = 1;
  }

  *iLoopSize = iMaxRecord;
  if (iFlipRecord < 0 || iFlipRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "GetRecordRange: Invalid index 0 <= %d < %d in %s",
            iFlipRecord, iMaxRecord, idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  //
  // Binary search for last record with End time before tBeginTime
  while (iLow <= iHigh)
  {
    // Get mid point for our binary search test
    iMid = (iHigh+iLow)/2;

    // Convert iMid to a record offset inside circular buffer
    iRecord = (iFlipRecord+iMid) % iMaxRecord;

    // Seek to the record position
    iSeek = iRecord * iLoopRecordSize;
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // Seek should never fail if we set up correctly
      sprintf(looperrstr, "GetRecordRange: Unable to seek to record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to seek to required file buffer position

    // Read in the header only
    if (fread(str_header, FRAME_SIZE, 1, fp_buf) != 1)
    {
      // read should never fail if we set up correctly
      sprintf(looperrstr, "GetRecordRange: Unable to read record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to read record

    // parse out header string
    if (ParseSeedHeader(str_header, recStation, recChan, recLoc,
                &tRecStart, &tRecEnd, &iSeqNum, &iSamples) != NULL)
    {
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // error parsing header

    // See if record End Time is after desired start time
    if ((iCmp=ST_TimeComp2(tRecEnd, tBeginTime)) >= 0)
    {
      // Need to find an earlier record
      iHigh = iMid-1;
    }
    else
    {
      // Need to find a later record
      iLow = iMid+1;
    }
  } // Binary search while loop

  // iHigh now points to first record completely before time span of interest
  iPre = iHigh;

  //
  // Binary search for first record with Start time after tEndTime
  iLow = iPre+1;
  iHigh = iMaxRecord;
  while (iLow <= iHigh)
  {
    // Get mid point for our binary search test
    iMid = (iHigh+iLow)/2;

    // Convert iMid to a record offset inside circular buffer
    iRecord = (iFlipRecord+iMid) % iMaxRecord;

    // Seek to the record position
    iSeek = iRecord * iLoopRecordSize;
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      sprintf(looperrstr, "GetRecordRange: Unable to seek to record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to seek to required file buffer position

    // Read in the header only
    if (fread(str_header, FRAME_SIZE, 1, fp_buf) != 1)
    {
      // read should never fail if we set up correctly
      sprintf(looperrstr, "GetRecordRange: Unable to read record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to read record

    // parse out header string
    if (ParseSeedHeader(str_header, recStation, recChan, recLoc,
                &tRecStart, &tRecEnd, &iSamples, &iSeqNum) != NULL)
    {
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // error parsing header

    // See if record Start time is after desired end time
    if (ST_TimeComp2(tRecStart, tEndTime) > 0)
    {
      // Need to find an earlier record
      iHigh = iMid-1;
    }
    else
      iLow = iMid+1;
  } // Binary search while loop

  // iLow points to first record completely after time span of interest
  iPost = iLow;

  // Check for case of no records within desired time span
  *iCount = iPost - iPre - 1;
  if (*iCount < 1)
  {
    *iFirst = -1;
    *iCount = 0;
  }
  else
  {
    *iFirst = (iPre + 1 + iFlipRecord) % iMaxRecord;
    *iLast = (iPost - 1 + iFlipRecord) % iMaxRecord;
  }

  fclose(fp_buf);
  fclose(fp_idx);

  return NULL;
} // GetRecordRange()

//////////////////////////////////////////////////////////////////////////////
// Print a list of all the spans for the given channel
char *DumpSpans(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  )                       // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  char  str_header[FRAME_SIZE];
  char    recStation[8];
  char    recLoc[4];
  char    recChan[4];
  FILE  *fp_buf;
  FILE  *fp_idx;
  STDTIME2    tRecStart;
  STDTIME2    tRecEnd;
  DELTA_T2    tDeltaT;
  long        lDeltaTMS;
  int   iFlipRecord;
  int   iMaxRecord;
  int   iRecord;
  int   iSeek;
  int   iMid;
  int   iSpanIndex;
  int   bFirst=1;
  int   iCount;
  int   iSeqNum;
  int   iSamples;
  STDTIME2    tSpanStart;
  STDTIME2    tSpanEnd;


  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "DumpSpans: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no last record to return
    return NULL;
  }

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so no last record to return
    fclose(fp_buf);
    return NULL;
  }

  // Load index info
  if (ParseIndexInfo(fp_idx, &iFlipRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "DumpSpans: Data format error in %s",
             idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  if (iFlipRecord < 0 || iFlipRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "DumpSpans: Invalid index 0 <= %d < %d in %s",
            iFlipRecord, iMaxRecord, idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  // Verify what the last record is in case buffer has never been filled
  iMid = LOOPDEADRECORDS+1;
  fseek(fp_buf, 0, SEEK_END);
  iSeek = ftell(fp_buf);
  if ((iMaxRecord-LOOPDEADRECORDS) * iLoopRecordSize > iSeek)
  {
    printf("Circular buffer filling, %d of %d records total filled.\n",
      iSeek/iLoopRecordSize, iMaxRecord);
    iMaxRecord = iSeek/iLoopRecordSize;
    iMid = 1;
  }

  // Loop through all records in file
  bFirst = 1;
  iCount = 0;
  for (; iMid <= iMaxRecord; iMid++)
  {
    // Convert iMid to a record offset inside circular buffer
    iRecord = (iFlipRecord+iMid) % iMaxRecord;
    if (bFirst)
      iSpanIndex = iRecord;

    // Seek to the record position
    iSeek = iRecord * iLoopRecordSize;
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, assume we hit the end of file
      sprintf(looperrstr, "DumpSpans: Unable to seek to header %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to seek to required file buffer position

    // Read in the header only
    if (fread(str_header, FRAME_SIZE, 1, fp_buf) != 1)
    {
      
      sprintf(looperrstr, "DumpSpans: Unable to read header %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to read record

    // parse out header info
    if (ParseSeedHeader(str_header, recStation, recChan, recLoc,
                &tRecStart, &tRecEnd, &iSeqNum, &iSamples) != NULL)
    {
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // error parsing header

    // If not first record, check for gap
    if (!bFirst)
    {
      tDeltaT = ST_DiffTimes2(tRecStart, tSpanEnd);
      lDeltaTMS = ST_DeltaToMS2(tDeltaT);

      if (lDeltaTMS > 10 || lDeltaTMS < -10)
      {
        printf("%s %s/%s Span %s",
            station, loc, chan, ST_PrintDate2(tSpanStart, 1));
        printf(" to %s %d records, start index %d, %ld tms gap\n",
            ST_PrintDate2(tSpanEnd, 1), iCount, iSpanIndex, lDeltaTMS);

        // Remember new span start time
        tSpanStart = tRecStart;
        iSpanIndex = iRecord;
        iCount=0;
      }  // found a gap or overlap
    } // Not the first record
    else
    {
      tSpanStart = tRecStart;
      bFirst = 0;
    } // first record
    
    tSpanEnd = tRecEnd;
    iCount++;
  } // loop through all records

  // Print out last remaining span
  printf("%s %s/%s Span %s",
      station, loc, chan, ST_PrintDate2(tSpanStart, 1));
  printf(" to %s %d records, start index %d\n",
      ST_PrintDate2(tSpanEnd, 1), iCount, iSpanIndex);

  fclose(fp_buf);
  fclose(fp_idx);

  return NULL;
} // DumpSpans()

//////////////////////////////////////////////////////////////////////////////
// Print a list of all the spans for the given channel
char *RangeSpans(
  const char  *station,    // station name
  const char  *chan,       // Channel ID
  const char  *loc,        // Location ID
  int         firstRecord, // First record in range
  int         lastRecord   // Last record in range
  )                        // returns NULL or an error string pointer
{
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  char  idx_filename[2*MAXCONFIGLINELEN+2];
  char  str_header[FRAME_SIZE];
  char    recStation[8];
  char    recLoc[4];
  char    recChan[4];
  FILE  *fp_buf;
  FILE  *fp_idx;
  STDTIME2    tRecStart;
  STDTIME2    tRecEnd;
  DELTA_T2    tDeltaT;
  long        lDeltaTMS;
  int   iFlipRecord;
  int   iMaxRecord;
  int   iRecord;
  int   iSeek;
  int   iMid;
  int   iSpanIndex;
  int   bFirst=1;
  int   iCount;
  int   iSeqNum;
  int   iSamples;
  STDTIME2    tSpanStart;
  STDTIME2    tSpanEnd;


  // The configuration file must be parsed before this routine can work
  if (parse_state == 0)
  {
    sprintf(looperrstr, "RangeSpans: ParseDiskLoopConfig not run yet");
    return looperrstr;
  }

  // Get names of buffer and index files
  // If blank location code, leave off leading location code in filename
  if (loc[0] == ' ' || loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, StripNetworkID(station), chan);
    sprintf(idx_filename, "%s/%s/%s.idx",
        loopDir, StripNetworkID(station), chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, StripNetworkID(station), loc, chan);
    sprintf(idx_filename, "%s/%s/%s_%s.idx",
        loopDir, StripNetworkID(station), loc, chan);
  }

  // Make sure that buffer file exists
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no last record to return
    return NULL;
  }

  // Make sure that index file exists
  if ((fp_idx=fopen(idx_filename, "r")) == NULL)
  {
    // Index file does not exist so no last record to return
    fclose(fp_buf);
    return NULL;
  }

  // Load index info
  if (ParseIndexInfo(fp_idx, &iFlipRecord, &iMaxRecord) != NULL)
  {
    sprintf(looperrstr, "RangeSpans: Data format error in %s",
             idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  if (iFlipRecord < 0 || iFlipRecord >= iMaxRecord)
  {
    sprintf(looperrstr, "RangeSpans: Invalid index 0 <= %d < %d in %s",
            iFlipRecord, iMaxRecord, idx_filename);
    fclose(fp_buf);
    fclose(fp_idx);
    return looperrstr;
  } // error reading index and max record value

  // Loop through all records in file
  bFirst = 1;
  iCount = 0;
  for (iMid=firstRecord; (iMid%iMaxRecord) != ((lastRecord+1)%iMaxRecord);
       iMid++)
  {
    // Convert iMid to a record offset inside circular buffer
    iRecord = iMid % iMaxRecord;
    if (bFirst)
      iSpanIndex = iRecord;

    // Seek to the record position
    iSeek = iRecord * iLoopRecordSize;
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, assume we hit the end of file
      sprintf(looperrstr, "RangeSpans: Unable to seek to header %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to seek to required file buffer position

    // Read in the header only
    if (fread(str_header, FRAME_SIZE, 1, fp_buf) != 1)
    {
      
      sprintf(looperrstr, "RangeSpans: Unable to read header %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // Failed to read record

    // parse out header info
    if (ParseSeedHeader(str_header, recStation, recChan, recLoc,
                &tRecStart, &tRecEnd, &iSeqNum, &iSamples) != NULL)
    {
      fclose(fp_buf);
      fclose(fp_idx);
      return looperrstr;
    } // error parsing header

    // If not first record, check for gap
    if (!bFirst)
    {
      tDeltaT = ST_DiffTimes2(tRecStart, tSpanEnd);
      lDeltaTMS = ST_DeltaToMS2(tDeltaT);

      if (lDeltaTMS > 10 || lDeltaTMS < -10)
      {
        printf("%s %s/%s Span %s",
            station, loc, chan, ST_PrintDate2(tSpanStart, 1));
        printf(" to %s %d records, start index %d, %ld tms gap\n",
            ST_PrintDate2(tSpanEnd, 1), iCount, iSpanIndex, lDeltaTMS);

        // Remember new span start time
        tSpanStart = tRecStart;
        iSpanIndex = iRecord;
        iCount=0;
      }  // found a gap or overlap
    } // Not the first record
    else
    {
      tSpanStart = tRecStart;
      bFirst = 0;
    } // first record
    
    tSpanEnd = tRecEnd;
    iCount++;
  } // loop through all records

  // Print out last remaining span
  printf("%s %s/%s Span %s",
      station, loc, chan, ST_PrintDate2(tSpanStart, 1));
  printf(" to %s %d records, start index %d\n",
      ST_PrintDate2(tSpanEnd, 1), iCount, iSpanIndex);

  fclose(fp_buf);
  fclose(fp_idx);

  return NULL;
} // RangeSpans()

