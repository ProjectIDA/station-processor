/*****************************************************************************
File:     seedmsg.c
Author:   Frank Shelly
Created:  16 April 2007
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:  Library routines for using Seed format message records

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2007-04-16 FCS - Creation
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include "include/netreq.h"

#define WHOAMI "seedmsg"

#define min(a,b) ((a)<(b)?(a):(b))

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
    case 256:
      iRecordLengthCode = 8;
      break;
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
    case 8192:
      iRecordLengthCode = 13;
      break;
    default:
      return "MakeSeedMsg: Invalid seed record length (not 256,512,1024,2048,4096,8192)";
  }

  // We have one blockette 1000 record
  seedrec->number_of_following_blockettes = 1;
  seedrec->first_data_byte = htons(48+8);
  seedrec->first_blockette_byte = htons(48);
  blockette = (blockette_1000 *)&recordbuf[ntohs(seedrec->first_blockette_byte)];
  blockette->type = htons(1000);
  blockette->next_blockette_start = 0;
  blockette->encoding_format = 1;
  blockette->data_record_length = iRecordLengthCode;
  blockette->reserved = 0;

  // Sample rate for LOG messages
  seedrec->samples_in_record = htons(min(strlen(msg), iSeedRecordSize-56-1));
  seedrec->sample_rate_factor = 0;
  seedrec->sample_rate_multiplier = 0;

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

