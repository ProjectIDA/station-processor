/*
File:       q330arch
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Server to merge tcpip seed records from multiple q330serv
            Then it archives the data into IDA and ASL disk loops

Algorithm:  
Starts a message server in a backround thread.
The message server thread accepts connection requests and spawns read threads
Each client read thread reads messages, and copies them to shared mem
A loop in main program checks for new messages in shared mem
  When a new LOG message arrives, combine it with queued LOG message if possible.
    If combine would be too large, send queued message, make new message
       the new queued message
  Once queued LOG message reaches BUFFER_TIME_SEC in age, or non LOG record:
     Archive message, empty queue
end loop

Update History:
mmddyy who Changes
==============================================================================
032607 fcs Creation
020808 fcs Converted fork calls to pthread_create calls
******************************************************************************/
#define WHOAMI "q330arch"
#define VERSION_DATE  "8 February 2008"

#define _REENTRANT
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/q330arch.h"
#include "include/netreq.h"
#include "include/idaapi.h"

void daemonize();
void *StartServer(void *params);

// Make the server_pid global so it can be killed when program is shut down
int   server_pid=0;
int   g_bDebug=0;
char  *g_sIDAname;
static struct s_mapshm *mapshm=NULL;

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
    printf(
"Usage:\n");
    printf(
"  q330arch <configfile> <IDA diskloop name> [debug]\n");
    printf("    Starts log seed message server as a daemon\n");
    printf(
"      <configfile>\n");
    printf("    Name of ASL diskloop configuration file\n");
    printf(
"      <IDA diskloop name>\n");
    printf("    Name of IDA diskloop\n");
    printf(
"      [debug]\n");
    printf("    Optionaly start server in debug mode\n");
} // ShowUsage()

char *ArchiveSeed(char *record)
{
  int  i;
  seed_header *seedrec;
  char station[6];
  char channel[4];
  char location[4];
  char *retmsg1;
  char *retmsg2;

  seedrec = (seed_header *)record;

  memcpy(station, seedrec->station_id_call_letters, 5);
  for (i=0; i < 5 && station[i] != ' '; i++)
    ; // stop at first blank or 5th char
  station[i] = 0;

  memcpy(channel, seedrec->channel_id, 3);
  channel[3] = 0;
  
  memcpy(location, seedrec->location_id, 2);
  location[2] = 0;

  retmsg1 = idaWriteChan(station, channel, location, record, g_sIDAname);
  retmsg2 = WriteChan(station, channel, location, record);

  if (g_bDebug)
  {
    fprintf(stderr, "DEBUG q330arch wrote record %6.6s %2.2s/%3.3s\n",
    &record[0], &record[13], &record[15]);
  }
  if (retmsg1 != NULL)
    return retmsg1;
  else
    return retmsg2;
} // ArchiveSeed()

//////////////////////////////////////////////////////////////////////////////
// Local handler to orderly shut down server and child forks
static void sigterm_server()
{
  if (mapshm == NULL) exit(1);
  mapshm->bQuit = 1;
  return;
} // sigterm_server

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  char  station[8];
  char  loc[4];
  char  chan[4];
  char  network[4];
  char  *retmsg;
  int   iSeedRecordSize;
  int   iPort;
  int   iClient;
  int   iBuf;
  int   i;
  int   iDeltaTime;
  int   year, doy, hour, min, sec;
  int   touched;

  pthread_t server_tid;

  char   queuebuf[4096];
  char   tempbuf[4096];
  char   *queuemsg=NULL; 
  time_t  queuetimetag=0;

  // Check for right number off arguments
  if (argc != 3 && argc != 4)
  {
    ShowUsage();
    exit(1);
  }
  g_sIDAname = argv[2];

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
    exit(1);
  }
  LoopRecordSize(&iSeedRecordSize);

  // Handle command line debug request 
  g_bDebug = 0;
  if (argc == 4)
  {
    if (strcmp(argv[3], "debug") != 0)
    {
      printf("Unknown keyword '%s'\n", argv[3]);
      ShowUsage();
      exit(1);
    }
    g_bDebug = 1;
  } // command line arguments request running server in debug mode

  // Run this program as a daemon unless requested otherwise by user
  if (argc == 3)
    daemonize();

  // Set up the shared memory segment
  if ((retmsg = MapSharedMem((void **)&mapshm)) != NULL)
  {
    fprintf(stderr, "%s:main %s\n", WHOAMI, retmsg);
    exit(1);
  }
  memset(mapshm, 0, sizeof(struct s_mapshm));

  // Set up a handler for when this process is terminated
  signal(SIGTERM,sigterm_server); /* Die on SIGTERM */

  // Start the server listening for clients in the background
  LogServerPort(&iPort);
  mapshm->iPort = iPort;
  mapshm->bDebug = g_bDebug;
  if (pthread_create(&server_tid, NULL, StartServer, (void *)mapshm))
  {
    fprintf(stderr, "%s:main pthread_create StartServer: %s\n",
      WHOAMI, strerror(errno));
    exit(1);
  }
  mapshm->listen_tid = server_tid;

  // Infinite loop waiting for buffers to get filled up
  iClient = 0;
  touched = 0;
  while (1)
  {
    if (mapshm->bQuit)
    {
      if (mapshm->listen_tid)
      {
        pthread_kill(mapshm->listen_tid, 15);
        touched = 1;
      }
      mapshm->listen_tid = 0;
      for (i=0; i<MAX_CLIENTS; i++)
      {
        if (mapshm->client_tid[i] > 0)
        {
          touched = 1;
          pthread_kill(mapshm->client_tid[i], 15);
          mapshm->client_tid[i] = 0;
        }
      }
    } // quit flag was set

    // Notice if we don't get any data this loop
    if (!touched)
    {
      if (mapshm->bQuit)
        exit(EXIT_SUCCESS);
      usleep(10000);
    }
    touched = 0;

    // Check for new messages
    for (i=0; i < MAX_CLIENTS; i++)
    {
      iClient = (iClient+1) % MAX_CLIENTS;
      iBuf = mapshm->read_index[iClient];
      if (mapshm->read_index[iClient] != mapshm->write_index[iClient])
      {
	touched = 1;

        // Immediately archive non LOG seed record
        if (strcmp("LOG", &mapshm->buffer[iClient][iBuf][15]) != 0)
        {
          if ((retmsg=ArchiveSeed((char *)&mapshm->buffer[iClient][iBuf]) )
              != NULL)
          {
            fprintf(stderr, "q330arch: %s\n", retmsg);
          }
          // Indicate that we have processed message
          mapshm->write_index[iClient] = iBuf;
          continue;
        } // Not a LOG record

        // See if there is a message waiting to be sent out
        if (queuemsg != NULL)
        {
          // Try to combine the two records to save space
          if (CombineSeed(queuemsg, mapshm->buffer[iClient][iBuf],
                          iSeedRecordSize))
          {
            // Indicate that we have processed message and that's all
            mapshm->write_index[iClient] = iBuf;
            continue;
          }
          else
          {
            // Send queued message since we can't combine the two
            SeedRecordMsg(tempbuf, queuebuf, station, network, chan, loc,
                        &year, &doy, &hour, &min, &sec);
            if ((retmsg=ArchiveSeed(queuemsg)) != NULL)
            {
              fprintf(stderr, "q330arch: %s\n", retmsg);
            }
            queuemsg = NULL;
          } // combine failed
        } // we have a waiting message to possibly combine with

        // Copy new message to free up shared memory buffer
        memcpy(queuebuf, mapshm->buffer[iClient][iBuf], iSeedRecordSize);
        queuemsg = queuebuf;
        queuetimetag = mapshm->timetag[iClient];

        // Indicate that we have processed message
        mapshm->write_index[iClient] = iBuf;
      } // new message from this client
    } // check each possible client

    // See if oldest message is more than BUFFER_TIME_SEC old
    // Or if this is not a log message
    iDeltaTime = time(NULL) - queuetimetag;
    if (queuemsg != NULL &&
        (iDeltaTime < 0 || iDeltaTime >= BUFFER_TIME_SEC))
    {
      SeedRecordMsg(tempbuf, queuebuf, station, network, chan, loc,
                    &year, &doy, &hour, &min, &sec);
      if ((retmsg=ArchiveSeed(queuemsg)) != NULL)
      {
        fprintf(stderr, "q330arch: %s\n", retmsg);
      }
      queuemsg = NULL;
    } // timeout elapsed

  } // loop forever
} // main()

