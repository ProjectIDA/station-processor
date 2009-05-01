#ifndef FALCON_H
#define FALCON_H

#define FALCON_IDSTRING  "FALC~"
#define FALCON_CHAN      "OFA"
#define FALCON_LOC       "90"

#define DEBUG_ITERATIONS 6
#define DEBUG_INTERVAL 1800 
#define DEBUG_INTERVAL 5 
#define SLEEP_INTERVAL 1800

// Global debug mode flag
extern int gDebug;

void QueueOpaque(
  const void *data,        // Pointer to new data to add
  int data_length,         // Number of bytes in *data
  const char *station,     // The station name
  const char *network,     // Network ID
  const char *channel,     // Channel name
  const char *location,    // Location code
  const char *idstring);   // '~' Terminated ascii id string (keep this short)

void FlushOpaque();

typedef struct {
  const char *station;
  const char *network;
  const char *channel;
  const char *location;
} st_info_t;

#endif

