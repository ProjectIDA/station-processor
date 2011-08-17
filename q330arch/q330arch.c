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
031709 fcs Add pthread_detach calls to free up thread memory after halting
050809 fcs Add Release number to ShowUsage
071309 fcs logapi lowered log message threshold Release 1.3
020910 fcs New diskloop.config keywords for falcon, new lib330_91005
040110 fcs NoIDA in diskloop.config means don't add record to IDA disk loop
042811 fcs Use station name to determine IDA disk loop, not diskloop.config
081511 jde NoArchive in diskloop.config means don't add record to Archives
******************************************************************************/
#define WHOAMI "q330arch"
const char *VersionIdentString = "Release 1.6";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/shm.h>
#include "include/idaapi.h"
#include "include/diskloop.h"
#include "include/shmstatus.h"
#include "include/q330arch.h"
#include "include/netreq.h"
#include "include/dcc_time_proto2.h"

void daemonize();

//////////////////////////////////////////////////////////////////////////////

void *StartServer(void *params);

// Make the server_pid global so it can be killed when program is shut down
int   server_pid=0;
int   g_bDebug=0;
char  g_sIDAname[6];
static struct s_mapshm *mapshm=NULL;
static struct s_mapstatus *mapstatus=NULL;

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
    fprintf(stderr,
"Usage:\n");
    fprintf(stderr,
"  q330arch <configfile> [debug]\n");
    fprintf(stderr,
"    Starts log seed message server as a daemon\n");
    fprintf(stderr,
"      <configfile>\n");
    fprintf(stderr,
"    Usually /etc/q330/DLG1/diskloop.config\n");
    fprintf(stderr,
"      [debug]\n");
    fprintf(stderr,
"    Optionaly start server in debug mode\n");
    fprintf(stderr,
"Version %s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

//////////////////////////////////////////////////////////////////////////////

static void sigterm_q330arch()
{
  // Set flat requesting a graceful program exit
  if (mapshm != NULL)
    mapshm->bQuit = 1;
  else
    exit(0);
} // sigterm_q330arch()

int ChannelControl(char *command)
{
    int i, setResult = 0;
    size_t base = 14;
    char station[6];
    char loc[3];
    char chan[4];
    char c;

    if (command[base] != '-') return -1;
    base++;

    // Copy station name
    for (i=0; i < 5; i++) {
        c = command[base+i];
        if (c == '-') {
          base += i;
          break;
        }
        else if (isalnum(c)) {
          station[i] = c;
        }
        else {
          return -1;
        }
    }

    if (command[base] != '-') return -1;
    base++;

    // Copy location code
    for (i=0; i < 2; i++) {
        c = command[base+i];
        if (c == '-') {
          base += i;
          break;
        }
        else if (isalnum(c)) {
          loc[i] = c;
        }
        else {
          return -1;
        }
    }

    if (command[base] != '-') return -1;
    base++;

    // Copy channel name
    for (i=0; i < 3; i++) {
        c = command[base+i];
        if (c == '-') {
          base += i;
          break;
        }
        else if (isalnum(c)) {
          chan[i] = c;
        }
        else {
          return -1;
        }
    }

    if (command[base] != '-') return -1;
    base++;

    if (strncmp("ARCHIVE-OFF", (char *)(command + base), 11)) {
        setResult = SetChannelToArchive(station, chan, loc, 0);
    }
    else if (strncmp("ARCHIVE-ON", (char *)(command + base), 10)) {
        setResult = SetChannelToArchive(station, chan, loc, 1);
    }
    else if (strncmp("ARCHIVE-DEFAULT", (char *)(command + base), 15)) {
        setResult = DefaultChannelToArchive(station, chan, loc);
    }
    else if (strncmp("IDA-OFF", (char *)(command + base), 7)) {
        setResult = SetChannelToIDA(station, chan, loc, 0);
    }
    else if (strncmp("IDA-ON", (char *)(command + base), 6)) {
        setResult = SetChannelToIDA(station, chan, loc, 1);
    }
    else if (strncmp("IDA-DEFAULT", (char *)(command + base), 11)) {
        setResult = DefaultChannelToIDA(station, chan, loc);
    } else {
        return -1;
    }

    return (setResult > 0);
}

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

  // We write to archive first to give archive max chance of being complete
  retmsg2=NULL;
  if (!CheckNoArchive(station, channel, location))
  {
    retmsg2 = WriteChan(station, channel, location, record);
  }

  // Make sure channel isn't listed with a NoIDA keyword in diskloop.config
  retmsg1=NULL;
  if (!CheckNoIDA(station, channel, location))
  {
    retmsg1 = idaWriteChan(station, channel, location, record, station);
  }

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
int main (int argc, char **argv)
{
  char  station[8];
  char  loc[4];
  char  chan[4];
  char  network[4];
  char  *retmsg;
  char  tempMsg[128];
  int   result;
  int   iSeedRecordSize;
  int   iPort;
  int   iClient;
  int   iBuf;
  int   i;
  int   iDeltaTime;
  int   year, doy, hour, min, sec;
  int   touched;
  int   iWriteIndexArch;

  pthread_t server_tid;

  char   queuebuf[4096];
  char   tempbuf[4096];
  char   *queuemsg=NULL; 
  time_t  queuetimetag=0;

  // Check for right number off arguments
  if (argc != 2 && argc != 3)
  {
    ShowUsage();
    exit(1);
  }

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
    //syslog(LOG_ERR, "%s: %s", WHOAMI, retmsg);
    exit(1);
  }
  LoopRecordSize(&iSeedRecordSize);
  LogSNCL(station, network, chan, loc);
  strncpy(g_sIDAname, station, 5);
  g_sIDAname[5] = 0;

  // Handle command line debug request 
  g_bDebug = 0;
  if (argc == 3)
  {
    if (strcmp(argv[2], "debug") != 0)
    {
      printf("Unknown keyword '%s'\n", argv[2]);
      ShowUsage();
      exit(1);
    }
    g_bDebug = 1;
  } // command line arguments request running server in debug mode

  // Run this program as a daemon unless requested otherwise by user
  if (!g_bDebug)
  {
    daemonize();
  }
  signal(SIGTERM, sigterm_q330arch); // Gracefuly shutdown

  // Let user know what station we are archiving for
  if (g_bDebug)
    fprintf (stderr, "%s archive name %s\n", WHOAMI, g_sIDAname);
  else
    syslog (LOG_INFO, "%s archive name %s", WHOAMI, g_sIDAname);

  // Ignore SIGPIPE when a connection is closed
  signal (SIGPIPE, SIG_IGN);

  // Set up the shared memory segment
  if ((retmsg = MapSharedMem((void **)&mapshm)) != NULL)
  {
    if (g_bDebug)
      fprintf(stderr, "%s:main %s\n", WHOAMI, retmsg);
    else
      syslog(LOG_ERR, "%s:main %s", WHOAMI, retmsg);
    exit(1);
  }
  memset(mapshm, 0, sizeof(struct s_mapshm));

  // Open the dispstatus shared memory region
  retmsg = MapStatusMem((void **)&mapstatus);
  if (retmsg != NULL)
  {
    if (g_bDebug)
      fprintf (stderr, "Error loading dispstatus shared memory: %s\n",
            retmsg);
    else
      syslog (LOG_ERR, "Error loading dispstatus shared memory: %s",
            retmsg);
    exit(1);
  }

  // IDA initialization
  idaInit(station, WHOAMI);

  // Start the server listening for clients in the background
  LogServerPort(&iPort);
  mapshm->iPort = iPort;
  mapshm->bDebug = g_bDebug;
  if (pthread_create(&server_tid, NULL, StartServer, (void *)mapshm))
  {
    if (g_bDebug)
      fprintf(stderr, "%s:main pthread_create StartServer: %s\n",
        WHOAMI, strerror(errno));
    else
      syslog(LOG_ERR, "%s:main pthread_create StartServer: %s",
        WHOAMI, strerror(errno));
    exit(1);
  }
  pthread_detach(server_tid);
  mapshm->listen_tid = server_tid;

  // Infinite loop waiting for buffers to get filled up
  iClient = 0;
  touched = 0;
  while (1)
  {
    // Handle case where quit flag has been set
    if (mapshm->bQuit)
    {
      if (mapshm->listen_tid)
      {
        if (mapshm->bDebug)
          fprintf(stderr, "Quit flag detected, init shutdown %s\n", WHOAMI);

        pthread_kill(mapshm->listen_tid, 15);
        touched = 1;
        mapshm->listen_tid = 0;
      }
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
      {
        if (g_bDebug)
          fprintf(stderr, "Exiting %s via quit flag\n", WHOAMI);
        else
          syslog(LOG_INFO, "Exiting %s", WHOAMI);
        exit(EXIT_SUCCESS);
      }
      usleep(10000);
    }
    touched = 0;

    // Update watchdog timer so dispstatus knows we are alive
    // First determine which ixWriteArch index to use
    if (mapstatus->ixWriteArch != 0 && mapstatus->ixReadArch != 0)
      iWriteIndexArch = 0;
    if (mapstatus->ixWriteArch != 1 && mapstatus->ixReadArch != 1)
      iWriteIndexArch = 1;
    else
      iWriteIndexArch = 2;
    // Save the current time, let others know where updated time is stored
    mapstatus->archWatchdog[iWriteIndexArch] = ST_GetCurrentTime2();
    mapstatus->ixWriteArch = iWriteIndexArch;

    // Check for new messages
    for (i=0; i < MAX_CLIENTS; i++)
    {
      iClient = (iClient+1) % MAX_CLIENTS;
      iBuf = mapshm->read_index[iClient];
      if (mapshm->read_index[iClient] != mapshm->write_index[iClient])
      {
	    touched = 1;

        // Handle channel control commands
        if (strncmp("CHANNELCONTROL", &mapshm->buffer[iClient][iBuf][0], 14) == 0)
        {
          strncpy(tempMsg, &mapshm->buffer[iClient][iBuf], 128);
          for (i=124; i<127; i++) tempMsg[i] = '.';

          result = ChannelControl((char *)&mapshm->buffer[iClient][iBuf]);
          if (result < 1) {
            if (result == 0) {
                if (g_bDebug)
                  fprintf(stderr, "%s: Failed to apply channel control: '%s'\n",
                          WHOAMI, tempMsg);
                else
                  syslog(LOG_ERR, "%s: Failed to apply channel control: '%s'",
                         WHOAMI, retmsg);
            } else {
                if (g_bDebug)
                  fprintf(stderr, "%s: Invalid channel control message: '%s'\n",
                          WHOAMI, tempMsg);
                else
                  syslog(LOG_ERR, "%s: Invalid channel control message: '%s'",
                         WHOAMI, retmsg);
            }
          }
          if (g_bDebug)
            fprintf(stderr, "%s: Channel control applied: '%s'\n",
                    WHOAMI, tempMsg);
          else
            syslog(LOG_ERR, "%s: Channel control applied: '%s'",
                   WHOAMI, retmsg);
          continue;
        } // Channel control command

        // Immediately archive non LOG seed record
        if (strncmp("LOG", &mapshm->buffer[iClient][iBuf][15], 3) != 0)
        {
          if ((retmsg=ArchiveSeed((char *)&mapshm->buffer[iClient][iBuf]) )
              != NULL)
          {
            fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
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

