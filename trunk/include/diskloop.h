/*
File:       diskloop.h
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Common definitions for working with disk loop archive
Update History:
mmddyy who Changes
==============================================================================
012507 fcs Creation
******************************************************************************/

#ifndef _DISKLOOP_H_
#define _DISKLOOP_H_

#include "include/dcc_std.h"
#include "include/dcc_time.h"

// Ignore any characters in config file after line gets this long
#define MAXCONFIGLINELEN 80

// Number of deadzone records to eliminate/reduce race conditions
// associated with reading records from a circular buffer while writting
#define LOOPDEADRECORDS  2

// When diskloop.config is parsed, the channel buffersize data goes here
struct s_bufconfig
{
  char    station[8];
  char    loc[4];
  char    chan[4];
  int     records;
  int     index;
  struct s_bufconfig *next;
}; // struct s_bufconfig

struct s_mapstation
{
  // Maps q330 station name to final station name
  char    station_q330[8];
  char    station[8];
  struct s_mapstation *next;
}; // struct s_mapstation

// Prototype call definitions
char *ParseDiskLoopConfig(
  const char *filename    // Name of the configuration file
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Returns the directory string where archive files are stored
char *LoopDirectory(
  char *dir               // Should have MAXCONFIGLINELEN+1 chars allocated
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
char *NetBufferSize(
  int *size               // return value for buffer size in bytes
  );

//////////////////////////////////////////////////////////////////////////////
// Returns how many bytes in each disk loop record
char *LoopRecordSize(
  int   *size             // return bytes in each disk loop record
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Return the Station, Network, Channel, Location to use for log messages
char *LogSNCL(char *station, char *network, char *channel, char *location);

//////////////////////////////////////////////////////////////////////////////
// Used to change the defaults returned by LogSNCL
char *SetLogSNCL(const char *station, const char *network,
                 const char *channel, const char *location);

//////////////////////////////////////////////////////////////////////////////
// Returns username to log into falcon, empty means use station name
char *FalconUsername(char *username);

//////////////////////////////////////////////////////////////////////////////
// Returns password to log into falcon, empty means use station name
char *FalconPassword(char *password);

//////////////////////////////////////////////////////////////////////////////
// Returns Falcon IP address
// An IP return of "0.0.0.0" should be interpretted as no IP was set
//   in the diskloop.config file
char *FalconIP(char *ip);

//////////////////////////////////////////////////////////////////////////////
// Returns Falcon port number
char *FalconPort(int *port);

//////////////////////////////////////////////////////////////////////////////
// Returns the TCP port where log messages are sent to
char *LogServerPort(
  int *port               // returns port number
  );                      // returns NULL or an error string pointer

char *WriteChan(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf    // Seed record pointer
  );                      // returns NULL or an error string pointer

char *ReadLast(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf    // Seed record pointer
  );                      // returns NULL or an error string pointer

// Reads the seed record at the specified index location
// Nominal index is 0 .. iLoopSize-1, but code will wrap index to legal value
// Return NULL if no errors
// Returns error string if something bad happened.
char *ReadIndex(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int         index,      // Index of record to read, will wrap index if needed
  char        *databuf    // Seed record pointer
  );                      // returns NULL or an error string pointer

char *GetRecordRange(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  STDTIME2    tBeginTime, // Start time
  STDTIME2    tEndTime,   // End time
  int         *iFirst,    // Returns the first record index within the time
  int         *iLast,     // Returns the last record index within the time
  int         *iCount,    // Returns the total number of records within time
  int         *iLoopSize  // Returns size of circular buffer in records
  );                      // returns NULL or an error string pointer
                          // iFirst == -1 if no records were found
//////////////////////////////////////////////////////////////////////////////
// Returns the number of records for the given channel                       
char *NumChanRecords(             
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int         *records
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Tell whether the given location/channel is on the NoIDA channel list
int CheckNoIDA(
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

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
  );                        // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// The index info is repeated 3 times.  So even if we read it during the
// middle of a write, either the first two, or second two lines should agree.
char *ParseIndexInfo(             
  FILE *fp_idx,       // opened file pointer to index file
  int  *iFlipRecord,  // Returns current position within circular buffer
  int  *iMaxRecord    // Maximum size of circular buffer
  );                  // NULL okay, error string otherwise

//////////////////////////////////////////////////////////////////////////////
// Print a list of all the spans for the given channel
char *DumpSpans(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Print a list of all the spans for the given channel and record range
char *RangeSpans(
  const char  *station,    // station name
  const char  *chan,       // Channel ID
  const char  *loc,        // Location ID
  int         firstRecord, // First record in range
  int         lastRecord   // Last record in range
  );                       // returns NULL or an error string pointer


#endif // _DISKLOOP_H_ defined

