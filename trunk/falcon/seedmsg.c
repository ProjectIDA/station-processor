/*****************************************************************************
File:     seedmsg.c
Author:   Frank Shelly
Created:  16 April 2007
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:  Library routines for building opaque seed records

Update History:
mmddyy who Changes
==============================================================================
041709 fcs Creation
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "include/netreq.h"
#include "include/q330arch.h"

#define WHOAMI "seedmsg"

#define min(a,b) ((a)<(b)?(a):(b))

static char errmsg[256];

static int iMultiRecordSeqNo=0;

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
  int   *num_records)      // How many seed records it took to store data
{
  char  line[80];
  seed_header *seedrec;
  blockette_1000 *b1000;
  blockette_2000 *b2000;
  struct tm utc;
  time_t  now;
  char    *recptr;
  char    *dataptr;
  int     iRecordLengthCode;
  int     iFreeSpace;
  int     i,j;

  // Figure out how many records it will take to store this data
  iFreeSpace = iSeedRecordSize - 48 - sizeof(blockette_1000)
             - 15 - strlen(idstring);
  *num_records = (data_length + iFreeSpace-1) / iFreeSpace;

  // Allocate memory for the records
  *recordbuf = malloc(iSeedRecordSize * *num_records);
  if (*recordbuf == NULL)
  {
    sprintf(errmsg, "FATAL %s[%d]: malloc(%d) failed.",
            __FILE__, __LINE__, iSeedRecordSize * *num_records);
    *num_records = 0;
    return errmsg;
  }
  recptr = *recordbuf;

  // Keep seperate seqno for Multi record 0paque seed blocks
  if (*num_records > 1)
    iMultiRecordSeqNo++;

  // Default header SNCL chars are blank, rest is zero
  memset(recptr, 0, iSeedRecordSize * *num_records);
  for (i=0; i < *num_records; i++)
    memset(&recptr[iSeedRecordSize*i], ' ', 20);

  // Loop through each seed record filling it out
  for (i=0; i < *num_records; i++)
  {
    seedrec = (seed_header *)&recptr[i*iSeedRecordSize];

    // Limit sequence number to 6 digits
    seqno = seqno % 1000000;
    sprintf(line, "%06d", seqno);
    memcpy(seedrec->sequence, line, 6);

    // SNCL
    seedrec->seed_record_type = 'D';
    memcpy(seedrec->station_id_call_letters, station, min(strlen(station),5));
    memcpy(seedrec->location_id, location, min(strlen(location),2));
    memcpy(seedrec->channel_id, channel, min(strlen(channel),3));
    memcpy(seedrec->seednet, network, min(strlen(network),2));
  
    // Fill in timetag
    time(&now);
    gmtime_r(&now, &utc);
    seedrec->yr = htons(utc.tm_year + 1900);
    seedrec->jday = htons(utc.tm_yday);
    seedrec->hr = utc.tm_hour;
    seedrec->minute = utc.tm_min;
    seedrec->seconds = utc.tm_sec;
    seedrec->tenth_millisec = htons(0);

    switch(iSeedRecordSize)
    {
      case 512:
        iRecordLengthCode = 9;
        break;
      case 1024:
        iRecordLengthCode = 10;
        break;
      case 2048:
        iRecordLengthCode = 11;
        break;
      case 4096:
        iRecordLengthCode = 12;
        break;
      default:
        return "MakeOpaqueSeed: Invalid seed record length (not 512,1024,2048,4096)";
    }

    // We have one blockette 1000 record
    seedrec->number_of_following_blockettes = 2;
    seedrec->first_data_byte = htons(0);
    seedrec->first_blockette_byte = htons(48);
    b1000 = (blockette_1000 *)&recptr[i*iSeedRecordSize
                               + ntohs(seedrec->first_blockette_byte)];
    b1000->type = htons(1000);
    b1000->next_blockette_start = htons(48+sizeof(blockette_1000));
    b1000->encoding_format = 0;
    b1000->word_order = 1;
    b1000->data_record_length = iRecordLengthCode;
    b1000->reserved = 0;

    // Blockette 2000 for opaque blockettes
    b2000 = (blockette_2000 *)&recptr[i*iSeedRecordSize
                               + ntohs(b1000->next_blockette_start)];
    b2000->type = htons(2000);
    b2000->next_blockette_start = htons(0);
    if (i+1 < *num_records)
      b2000->data_record_length = htons(iFreeSpace);
    else
      b2000->data_record_length = htons(data_length - (iFreeSpace*i));
    b2000->data_offset = htons(15 + strlen(idstring));
    if (*num_records > 1)
      b2000->record_number = htonl(iMultiRecordSeqNo);
    else
      b2000->record_number = htonl(0);
    b2000->word_order = 1;
    if (*num_records > 1)
    {
      if (i == 0)
        b2000->data_flags = 0x10;  // First in series of multiple records
      else if (i+1 < *num_records)
        b2000->data_flags = 0x30;  // Middle in series
      else
        b2000->data_flags = 0x20;  // Last in series
    }
    else
    {
      b2000->data_flags = 0x00;    // Self contained record
    }
    b2000->number_header_fields = 0;
    for (j=0; idstring[j] != 0; j++)
    {
      if (idstring[j] == '~')
        b2000->number_header_fields++;
    } // count ~ chars in idstring
    if (j > 0 && idstring[j-1] != '~')
    {
      return "idstring parameter in MakeOpaqueSeed must end in a ~ char";
    }
    if (b2000->number_header_fields > 0)
      memcpy(&b2000->data_header_fields, idstring, strlen(idstring));

    // Sample rate for opaque messages
    seedrec->samples_in_record = 0;
    seedrec->sample_rate_factor = 0;
    seedrec->sample_rate_multiplier = 1;

    // fill in message section
    dataptr = (char *)data;
    memcpy(&recptr[iSeedRecordSize*i
                   + ntohs(b1000->next_blockette_start)
                   + ntohs(b2000->data_offset)],
           &dataptr[i*iFreeSpace], ntohs(b2000->data_record_length));
  } // for each seed record we need to create

  return NULL;
} // MakeOpaqueSeed()

//////////////////////////////////////////////////////////////////////////////
// Appends an opaque data array to an existing opaque seed record
// Returns true if successful
int AppendOpaqueSeed(
  char *record1,           // seed record to be appended to
  void *data,              // data to be appended
  int data_length,         // Number of bytes in *data
  int iSeedRecordSize)     // Size of seed records
{
  seed_header *seedrec;
  blockette_1000 *b1000;
  blockette_2000 *b2000;
  char    *dataptr;
  int iFreeSpace;
  int iDataIndex;
  int i;

  // Allow us to see data as a seed record header
  seedrec = (seed_header *)record1;
  if (seedrec->number_of_bollowing_blockettes == 0)
  {
    // No blockette 1000 so we can't do anything with this record
    return 0;
  }

  // All records we generate havea blockette 1000 as the first blockette
  b1000 = (blockette_1000 *)&record1[ntohs(seedrec->first_blockette_byte)];
  if (ntohs(b1000->type) != 1000)
  {
    // Can't append if we don't start with a blockette 1000
    return 0;
  }

  // Only allow between 1 and 127 blockettes
  if (seedrec->number_of_bollowing_blockettes < 1 ||
      seedrec->number_of_bollowing_blockettes >= 127)
  {
    // Outside of signed 1 byte integer so don't add any more blockettes
    return 0;
  }

  // Determine how much free space is left 
  iDataIndex = ntohs(b1000->next_blockette_start);

  // Loop through any additional opaque blockettes
  for (i=1; i < seedrec->number_of_following_blockettes; i++)
  {
    b2000 = (blockette_2000 *)&record1[iDataIndex];
    if (ntohs(b2000->type) != 2000)
    {
      // Can't handle records composed of other than blockette 2000 here
      return 0;
    }

    if (i <  seedrec->number_of_following_blockettes-1) 
    {
      iDataIndex = ntohs(b2000->next_blockette_start);
    }
    else
    {
      iDataIndex+=ntohs(b2000->data_offset)+ntohs(b2000->data_record_length);
      // Round up next blockette start to an 4 byte boundry
      iDataIndex = ((iDataIndex+3) / 4) * 4;
    }
  } // loop through all opaque blockette 2000 records
  iFreeSpace = iSeedRecordSize - iDataIndex;

  // return false if there is not enough room
  if (iFreeSpace < data_length + 15)
  {
    return 0;
  } // not enough room

   
  // Update count and pointer to this new opaque blockette
  seedrec->number_of_following_blockettes++;
  b2000->next_blockette_start = (htons(iDataIndex));

  // add new opaque blockette
  b2000 = (blockette_2000 *)&record1[iDataIndex];
  b2000->type = htons(2000);
  b2000->next_blockette_start = htons(0);
  b2000->data_record_length = htons(data_length);
  b2000->data_offset = htons(15);
  b2000->record_number = htonl(0);
  b2000->word_order = 1;
  b2000->data_flags = 0x00;    // Self contained record
  b2000->data_header_fields = 0;
  iDataIndex += ntohs(b2000->data_offset);

  dataptr = (char *)data;
  memcpy(&record1[iDataIndex], dataptr, data_length);

  // Success, so return 1
  return 1;
} // AppendOpaqueSeed()


//////////////////////////////////////////////////////////////////////////////
// Create a Seed record given a null terminated string
// Returns NULL and a filled out recordbuf if okay
// Returns pointer to error string if there was a problem
char *MakeSeedMsg(const char *msg, int seqno,
                  const char *station, const char *network,
                  const char *channel, const char *location,
                  int iSeedRecordSize,
                  char *recordbuf)
{
  char  line[80];
  seed_header *seedrec;
  blockette_1000 *blockette;
  struct tm utc;
  time_t  now;
  int     iRecordLengthCode;

  // Default header SNCL chars are blank, rest is zero
  memset(recordbuf, 0, iSeedRecordSize);
  memset(&recordbuf[0], ' ', 20);
  seedrec = (seed_header *)recordbuf;

  // Limit sequence number to 6 digits
  seqno = seqno % 1000000;
  sprintf(line, "%06d", seqno);
  memcpy(seedrec->sequence, line, 6);

  // SNCL
  seedrec->seed_record_type = 'D';
  memcpy(seedrec->station_id_call_letters, station, min(strlen(station),5));
  memcpy(seedrec->location_id, location, min(strlen(location),2));
  memcpy(seedrec->channel_id, channel, min(strlen(channel),3));
  memcpy(seedrec->seednet, network, min(strlen(network),2));
  
  // Fill in timetag
  time(&now);
  gmtime_r(&now, &utc);
  seedrec->yr = htons(utc.tm_year + 1900);
  seedrec->jday = htons(utc.tm_yday);
  seedrec->hr = utc.tm_hour;
  seedrec->minute = utc.tm_min;
  seedrec->seconds = utc.tm_sec;
  seedrec->tenth_millisec = htons(0);

  switch(iSeedRecordSize)
  {
    case 512:
      iRecordLengthCode = 9;
      break;
    case 1024:
      iRecordLengthCode = 10;
      break;
    case 2048:
      iRecordLengthCode = 11;
      break;
    case 4096:
      iRecordLengthCode = 12;
      break;
    default:
      return "MakeSeedMsg: Invalid seed record length (not 512,1024,2048,4096)";
  }

  // We have one blockette 1000 record
  seedrec->number_of_following_blockettes = 1;
  seedrec->first_data_byte = htons(48+8);
  seedrec->first_blockette_byte = htons(48);
  blockette = (blockette_1000 *)&recordbuf[ntohs(seedrec->first_blockette_byte)];
  blockette->type = htons(1000);
  blockette->next_blockette_start = 0;
  blockette->encoding_format = 0;
  blockette->word_order = 1;
  blockette->data_record_length = iRecordLengthCode;
  blockette->reserved = 0;

  // Sample rate for LOG messages
  seedrec->samples_in_record = htons(min(strlen(msg), iSeedRecordSize-56-1));
  seedrec->sample_rate_factor = 0;
  seedrec->sample_rate_multiplier = htons(1);

  // fill in message section
  memcpy(&recordbuf[ntohs(seedrec->first_data_byte)], msg, 
          ntohs(seedrec->samples_in_record));

  // Null terminate
  recordbuf[ntohs(seedrec->first_data_byte)
           +ntohs(seedrec->samples_in_record)] = 0;

  return NULL;
} // MakeSeedMsg()

//////////////////////////////////////////////////////////////////////////////
// Combine two seed message records together if possible
// Returns true if successful
int CombineSeed(char *msg1, const char *msg2, int iSeedRecordSize)
{
  seed_header *seedrec1;
  seed_header *seedrec2;

  seedrec1 = (seed_header *)msg1;
  seedrec2 = (seed_header *)msg2; 
  // make sure location codes match before combining
  if (strncmp(seedrec1->location_id, seedrec2->location_id, 2) != 0)
    return 0;

  // See if send message added to first will be too large
  if (ntohs(seedrec1->samples_in_record) + ntohs(seedrec2->samples_in_record) 
      + ntohs(seedrec1->first_data_byte) > iSeedRecordSize)
  {
    return 0;
  } // not enough room

  // Append second message to the first
  memcpy(&msg1[ntohs(seedrec1->first_data_byte) 
            + ntohs(seedrec1->samples_in_record)],
          &msg2[ntohs(seedrec2->first_data_byte)], 
          ntohs(seedrec2->samples_in_record));
  seedrec1->samples_in_record = htons(ntohs(seedrec1->samples_in_record)
                              + ntohs(seedrec2->samples_in_record));

  // Success, so return 1
  return 1;
} // CombineSeed()

//////////////////////////////////////////////////////////////////////////////
// Convert a seed record into a null terminated message string
// Caller must supply both a destination buffer and the seed record
void SeedRecordMsg(char *msg, const char *seedrecord,
              char *station, char *network, char *channel, char *location,
              int *year, int *doy, int *hour, int *min, int *sec)
{
  seed_header *seedrec;
  int         iCount;
  int         iStart;
  int         i;

  seedrec = (seed_header *)seedrecord;

  // copy and null terminate message text only
  iStart = min(ntohs(seedrec->first_data_byte), 128);
  iCount = min(ntohs(seedrec->samples_in_record), 4095-iStart);
  strncpy(msg, &seedrecord[iStart], iCount);
  msg[iCount] = 0;

  memcpy(station, seedrec->station_id_call_letters, 5);
  for (i=0; i < 5 && station[i] != ' '; i++)
    ; // stop at first blank or 5th char
  station[i] = 0;

  memcpy(network, seedrec->seednet, 2);
  network[2] = 0;

  memcpy(channel, seedrec->channel_id, 3);
  channel[3] = 0;

  memcpy(location, seedrec->location_id, 2);
  location[2] = 0;

  *year = ntohs(seedrec->yr);
  *doy = ntohs(seedrec->jday);
  *hour = seedrec->hr;
  *min = seedrec->minute;
  *sec = seedrec->seconds;
} // SeedRecordMsg()

