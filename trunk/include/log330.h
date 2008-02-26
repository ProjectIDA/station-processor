/*
File:       log330.h
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Common definitions for using log330 message server
Update History:
mmddyy who Changes
==============================================================================
041107 fcs Creation
******************************************************************************/

#ifndef _LOG330_H_
#define _LOG330_H_

#include <time.h>

// server will hold onto message for up to this amount of time in an
// attempt to combine messages to save on disk space
#define BUFFER_TIME_SEC 60

// Some stuff for shared memory comm
// Each client will have 8200 bytes reserved for buffering
#define MAX_CLIENTS 8
#define SHMKEY_LOG330  12

// Server structure for inter fork comm
struct s_mapshm
{
  int     bDebug;
  time_t  timetag[MAX_CLIENTS];
  int     client_pid[MAX_CLIENTS];
  int     read_index[MAX_CLIENTS];
  int     write_index[MAX_CLIENTS];
  char    buffer[MAX_CLIENTS][2][4096];
};

// Routine call prototype definitions
// Calls return NULL on no errors, or pointer to error message string

//////////////////////////////////////////////////////////////////////////////
// Sends a Seed message record to the log330server
char *log330Msg(const char *msg,
                const char *station, const char *network,
                const char *channel, const char *location);

//////////////////////////////////////////////////////////////////////////////
// Close socket port to log330 server
// Normaly only called upon program exit, or can be skipped entirely
char *log330Close();


//////////////////////////////////////////////////////////////////////////////
// Creates the shared memory segment used for inter fork communications
// Used by the server, not the clients
char *MapSharedMem(void **mapshm);


// Routines for creating, combining seed log messages

//////////////////////////////////////////////////////////////////////////////
// Create a Seed record given a null terminated string
// Returns NULL and a filled out recordbuf if okay
// Returns pointer to error string if there was a problem
char *MakeSeedMsg(const char *msg, int seqno,
                  const char *station, const char *network,
                  const char *channel, const char *location,
                  int iSeedRecordSize,
                  char *recordbuf);

//////////////////////////////////////////////////////////////////////////////
// Combine two seed message records together if possible
// Returns true if successful
int CombineSeed(char *msg1, const char *msg2, int iSeedRecordSize);

//////////////////////////////////////////////////////////////////////////////
// Convert a seed record into a null terminated message string
// Caller must supply both a destination buffer and the seed record
void SeedRecordMsg(char *msg, const char *seedrecord,
              char *station, char *network, char *channel, char *location,
              int *year, int *doy, int *hour, int *min, int *sec);

#endif // _LOG330_H_ defined

