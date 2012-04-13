/*
File:       archd.h
Copyright:  (C) 2012 by Albuquerque Seismological Laboratory
Author:     Joel Edwards
Purpose:    Common definitions for using archive server

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2012-04-12 JDE - Creation
 ******************************************************************************/

#ifndef _ARCHD_H_
#define _ARCHD_H_

#include <time.h>
#include <pthread.h>
#include "include/idaapi.h"
#include "libpcomm/pcomm.h"

// server will hold onto message for up to this amount of time in an
// attempt to combine messages to save on disk space
#define BUFFER_TIME_SEC 60

// Each client will have 8200 bytes reserved for buffering
#define MAX_CLIENTS 128

// Message result
#define RESULT_RECORD_SENT      0
#define RESULT_CHAN_CMD_OKAY    1
#define RESULT_CHAN_CMD_FAIL    2
#define RESULT_CHAN_CMD_INVALID 3

// Server structure for inner thread comm
struct s_mapshm
{
    int     bDebug;
    int     bQuit;
    int     iPort;
    pthread_t listen_tid;
    time_t  timetag[MAX_CLIENTS];
    pthread_t client_tid[MAX_CLIENTS];
    int     read_index[MAX_CLIENTS];
    int     write_index[MAX_CLIENTS];
    int     result[MAX_CLIENTS][2];
    char    buffer[MAX_CLIENTS][2][8192];
};

typedef struct archd_context_t {
    pcomm_context *pcomm;

    int server_port;
    int server_socket;

    int debug;
} archd_context;

// Routine call prototype definitions
// Calls return NULL on no errors, or pointer to error message string

//////////////////////////////////////////////////////////////////////////////
// Sends a Seed log message record to the archd server
// Message starts out as just a text string
char *q330LogMsg(const char *msg,
        const char *station, const char *network,
        const char *channel, const char *location);

//////////////////////////////////////////////////////////////////////////////
// Close socket port to archd server
// Normaly only called upon program exit, or can be skipped entirely
char *q330Close();


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

//////////////////////////////////////////////////////////////////////////////
// Send a seed record to the archd server
char *q330SeedSend(void *seed_record);

//////////////////////////////////////////////////////////////////////////////
// Build an opaque seed record or records
// This routine allocates new memory for the seed records, calling program
//   is responsible for freeing up that memory afterwards
// Returns NULL and a filled out recordbuf if okay
// Returns num_records for how many seed records were created
// Returns pointer to error string if there was a problem
char *MakeOpaqueSeed(
        const void *data,        // Pointer to new data to add
        int data_length,         // Number of bytes in *data
        int seqno,               // The sequence number to use
        const char *station,     // The station name
        const char *network,     // Network ID
        const char *channel,     // Channel name
        const char *location,    // Location code
        const char *idstring,    // ~ Terminated ascii id string (keep this short)
        int   iSeedRecordSize,   // Size of seed record to create
        void  **recordbuf,       // Returns pointer to seed record(s) built
        int   *num_records);     // How many seed records it took to store data

//////////////////////////////////////////////////////////////////////////////
// Appends an opaque data array to an existing opaque seed record
// Returns true if successful
int AppendOpaqueSeed(
        char *record1,           // seed record to be appended to
        void *data,              // data to be appended
        int data_length,         // Number of bytes in *data
        int iSeedRecordSize);    // Size of seed records

#endif // _ARCHD_H_ defined

