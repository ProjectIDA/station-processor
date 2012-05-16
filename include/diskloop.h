/*
File:       diskloop.h
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Common definitions for working with disk loop archive

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2007-012-5 FCS - Creation
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

// 
#define PARSE_NONE          0
#define PARSE_BUFFER        1
#define PARSE_NO_ARCHIVE    2
#define PARSE_NO_IDA        3

#define INDEX_BEFORE_EXCLUSIVE -2
#define INDEX_BEFORE_INCLUSIVE -1
#define INDEX_WITHIN            0
#define INDEX_AFTER_INCLUSIVE   1
#define INDEX_AFTER_EXCLUSIVE   2

typedef struct DISKLOOP_CONTEXT
{
    char        key[20];        // key for this context
    char        loop_name[2*MAXCONFIGLINELEN+2];  // name of diskloop file
    char        index_name[2*MAXCONFIGLINELEN+2]; // name of index file
    FILE       *loop_fp;        // file descriptor for the diskloop
    FILE       *index_fp;       // file descriptor for the index file

    int         record_size;    // size of archive records
    int         capacity;       // how much space is allocated for this diskloop
    int         size;           // how much space is already used by this diskloop
    int         index;          // index of the last record

    int         has_written;    // this context has written data

    int      last_record_seqnum;  // sequence numer of last written record
    int      last_record_samples; // samples in last written record
    STDTIME2 last_record_start; // start timestamp of last written record
    STDTIME2 last_record_end;   // end timestamp of last written record
    struct timeval last_flush;  // last time writes were flushed
    struct timeval last_index;  // last time the index file was written
}
diskloop_context_t;

typedef struct RECORD_INFO
{
    // the record
    char buffer[8192];

    char station[8];
    char location[4];
    char channel[4];
    STDTIME2 start_time;
    STDTIME2 end_time;
    int seqnum;
    int samples;
    int rate_factor;
    int index;
}
record_info_t;

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
// Returns the UDP port where channel set commands are sent to
char *ChannelSetPort(
  int *port               // returns port number
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Returns the TCP port where log messages are sent to
char *LogServerPort(
  int *port               // returns port number
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Returns the TCP port to which telemetry clients will connect
char *TelemetryPort(
  int *port // returns port number
  );        // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Returns the maximum number of records for the in-memory telemetry buffer
char *TelemetryBufferDepth(
  int *depth // returns port number
  );         // returns NULL or an error string pointer


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
// Add a channel to the Archive bypass map
int SetChannelToArchive(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int          send       // Save to archive
  );

//////////////////////////////////////////////////////////////////////////////
// Check whether this channel should be archived
int CheckChannelToArchive(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

//////////////////////////////////////////////////////////////////////////////
// Remove a channel from the Archive bypass map
int DefaultChannelToArchive(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

//////////////////////////////////////////////////////////////////////////////
// Add a channel to the IDA bypass map
int SetChannelToIDA(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int          send       // Save to IDA diskloop
  );

//////////////////////////////////////////////////////////////////////////////
// Check whether this channel should be added to the IDA diskloop
int CheckChannelToIDA(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

//////////////////////////////////////////////////////////////////////////////
// Remove a channel from the IDA bypass map
int DefaultChannelToIDA(
  const char  *station,   // Station Name
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

//////////////////////////////////////////////////////////////////////////////
// Tell whether the given station-location/channel is on the supplied channel list
int CheckChannelList(
  const char          *station,   // Station Name
  const char          *chan,      // Channel ID
  const char          *loc,       // Location ID
  struct s_bufconfig  *ptr,       // List of entries
  struct s_bufconfig  **entry     // Matching entry if found
  );

//////////////////////////////////////////////////////////////////////////////
// Returns the number of records for the given channel                       
char *NumChanRecords(             
  const char  *station,
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  int         *records
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Tell whether the given location/channel is on the NoArchive channel list
int CheckNoArchive(
  const char  *station,
  const char  *chan,      // Channel ID
  const char  *loc        // Location ID
  );

//////////////////////////////////////////////////////////////////////////////
// Tell whether the given location/channel is on the NoIDA channel list
int CheckNoIDA(
  const char  *station,
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

//////////////////////////////////////////////////////////////////////////////
// Remaps the station name in the seeed record if diskloop.config
// has an applicable MapStation: entry
// Returns 1 on a remap, 0 on no remap
int RemapStationName(
  char *str_header);       // seed record pointer


char *adl_print_all();
char *adl_flush_all();
char *adl_write_all_indices();
char *adl_close_all();
void  adl_debug(int debug);

#endif // _DISKLOOP_H_ defined

