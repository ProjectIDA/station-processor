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
#include <sys/ioctl.h>
#include "include/idaapi.h"
#include "include/map.h"
#include "include/prioqueue.h"
#include "libpcomm/pcomm.h"

// server will hold onto messages for up to this amount of time in an
// attempt to combine messages to save on disk space
#define BUFFER_TIME_SEC 60

#define ASYNC_TIMEOUT_SECONDS       5
#define ASYNC_TIMEOUT_MICROSECONDS  0
#define BIND_RETRY_INTERVAL         15

#define MAX_SERVERS 128
#define MAX_CLIENTS 16
#define MAX_RECORD_SIZE 8192

#define SOCKET_KEY_SIZE 32

// Client timeouts are in microseconds
#define CLIENT_FULL_TIMEOUT      15 * 1000000
#define CLIENT_PARTIAL_TIMEOUT   60 * 1000000
#define CLIENT_EMPTY_TIMEOUT    300 * 1000000

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
    time_t  timetag[MAX_SERVERS];
    pthread_t client_tid[MAX_SERVERS];
    int     read_index[MAX_SERVERS];
    int     write_index[MAX_SERVERS];
    int     result[MAX_SERVERS][2];
    char    buffer[MAX_SERVERS][2][MAX_RECORD_SIZE];
};

// data record wrapper
typedef struct DATA_RECORD
{
    struct timeval receive_time;
    uint8_t *data;

    int priority;
    int ref_count;
}
data_record_t;

// structure for tracking the context of each server connected to archd
typedef struct SERVER_CONTEXT
{
    struct timeval connect_time;
    char key[SOCKET_KEY_SIZE];
    int socket;

    uint8_t buffer[MAX_RECORD_SIZE];
    size_t length;

    uint64_t received;
    uint64_t confirmed;

    queue_t reply_queue;
}
server_context_t;

// structure for tracking the context of each telemetry client connected to archd
typedef struct CLIENT_CONTEXT
{
    struct timeval connect_time;
    struct timeval last_write;
    char key[SOCKET_KEY_SIZE];
    int socket;

    data_record_t *partial;
    size_t offset;

    uint64_t sent;
    queue_t telemetry_queue;
}
client_context_t;

// structure containing reply messages for servers
typedef struct REPLY_MESSAGE
{
    uint8_t *message;
    size_t   length;

    int priority;
    int sent;
}
reply_message_t;

// structure for tracking archd daemon state
typedef struct ARCHD_CONTEXT
{
    // asynchronous I/O library context
    pcomm_context_t *pcomm;

    // memory tracker
    Map *memory;

    // quick access to timezone info
    struct timezone tz_info;
    struct timeval timeout;

    // shared memory region for updating dispstatus
    struct s_mapstatus *status_map;
    int write_index;

    // keep the queued log message here
    data_record_t *log_message;

    // server socket management
    int server_port;
    int server_socket;
    size_t max_servers;
    size_t server_count;
    Map *server_map;

    // client socket management
    int client_port;
    int client_socket;
    size_t max_clients;
    size_t client_count;
    Map *client_map;
    Map *client_resume_map;

    // priority queue of records to be archived
    queue_t record_queue;
    int64_t records_received;
    int64_t records_archived;
    int64_t log_record_count;

    // priority queue of record to be telemetered
    queue_t telemetry_queue;
    int     telemetry_depth;

    int debug;
    int record_size;
    char ida_name[6];
}
archd_context_t;

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

