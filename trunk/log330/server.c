/*****************************************************************************
File:     server.c
Author:   Frank Shelly
Created:  26 March 2007
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:
Log message server for q330 applications
Server listens for localhost connections only
Once a connection is made, Seed message blocks are read in. Message blocks
will be buffered for up to 60 seconds 
terminated by a NULL character.  The strings are buffered for up to 60 seconds
or until a seed log record can be filled.  Then output to a circular buffer.

Update History:
mmddyy who Changes
==============================================================================
032607 fcs Creation
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <signal.h>

#include "include/diskloop.h"
#include "include/log330.h"

#define WHOAMI "log330"

static int iSeedRecordSize;
static char errbuf[512];
static struct s_mapshm *mapshm=0;

//////////////////////////////////////////////////////////////////////////////
// Local handler to orderly shut down server and child forks
static void sigterm_server()
{
  int i;
  for (i=0; i<MAX_CLIENTS; i++) 
  {
    if (mapshm->client_pid[i] > 0)
    {
      kill(mapshm->client_pid[i], 15);
    }
  }
  exit(EXIT_SUCCESS);
} // sigterm_server

//////////////////////////////////////////////////////////////////////////////
// Creates the shared memory segment used for inter fork communications
char *MapSharedMem(void **mapshm)
{

   int   shmid;
   static char *unixadr = NULL;
   static char errbuf[1024];

   if (unixadr != NULL && unixadr != 0)
   {
     *mapshm = unixadr;
     return NULL;
   }

   /* Attach the shared memory segment */
   if ((shmid = shmget(SHMKEY_LOG330, sizeof (struct s_mapshm),
         IPC_CREAT | 00660))==-1)
   {
      sprintf(errbuf, "shmget LOG330 failure, error %d: %s.",
            errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

   if ((int)(unixadr = (char *)shmat(shmid,0,060))==-1)
   {
      sprintf(errbuf, "%s: shmat LOG330 failure, error %d: %s.",
            WHOAMI, errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

  *mapshm = unixadr;
  return NULL;
} // MapSharedMem()

//////////////////////////////////////////////////////////////////////////////
// Thread routine for reading messages from clients
// iClient says which of the MAX_CLIENTS this fork handles
void ServerReadThread(int iSocket, int iClient)
{
  int   iCount;
  int   j;
  int   iBuf;
  char  *retmsg;

  // Map in the shared memory area
  if ((retmsg = MapSharedMem((void **)&mapshm)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", WHOAMI, retmsg);
    exit(1);
  }

  // Make sure server has finished initializing us
  while (mapshm->client_pid[iClient] < 0)
    sleep(1);

  if (mapshm->bDebug)
  {
    printf("New read connection %d, pid=%d\n",
        iClient, mapshm->client_pid[iClient]);
  }

  // Infinite loop reading seed message records
  while (1)
  {
    // Read one records worth of data
    iCount = 0;
    iBuf = 1 - mapshm->read_index[iClient];
    while (iCount < iSeedRecordSize)
    {
      j = read(iSocket, &mapshm->buffer[iClient][iBuf][iCount],
              iSeedRecordSize-iCount);
      if (j == 0)
      {
        // Connection closed, shut down this read process
        // Setting client_pid to -1 lets server know this client done
        if (mapshm->bDebug)
        {
          printf("Closing client read thread %d, pid %d\n",
              iClient, mapshm->client_pid[iClient]);
        }
        mapshm->client_pid[iClient] = -1;
        close(iSocket);
        exit(0);
      } // connection closed

      if (mapshm->bDebug)
      {
        printf("Client %d, Read %d+%d/%d bytes from socket\n",
          iClient, j, iCount, iSeedRecordSize);
      }

      // Read until we fill up a full seed record
      iCount = iCount + j;
    } // read until one record has been read in

    // Make sure that server is ready to process another record
    while (mapshm->read_index[iClient] != mapshm->write_index[iClient])
    {
      sleep(1);
    } // while server has not finished writing last record

    // Let server know there is a new data ready to be written
    // And what time it was made available
    mapshm->read_index[iClient] = iBuf;
    mapshm->timetag[iClient] = time(NULL);

  } // loop forever

} // ServerReadThread()

//////////////////////////////////////////////////////////////////////////////
// Entry point for main program to start server listening for log messagges
void StartServer(int con_port, int bDebug)
{
  int   listen_socket;
  int   i;
  char  *errstr;

  struct s_mapshm *mapshm;
  struct sockaddr_in listen_addr; /* Listen address setup */

  // Set up a handler for when this process is terminated
  signal(SIGTERM,sigterm_server); /* Die on SIGTERM */
 
  if ((errstr=LoopRecordSize(&iSeedRecordSize)) != NULL)
  {
    fprintf(stderr, "%s\n", errstr);
    exit(1);
  }

  if (bDebug)
    printf("StartServer(port=%d, debug=%d) size %d\n",
        con_port, bDebug, iSeedRecordSize);

  // Map in the shared memory area
  if ((errstr = MapSharedMem((void **)&mapshm)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", WHOAMI, errstr);
    exit(1);
  }
  mapshm->bDebug = bDebug;
  for (i=0; i<MAX_CLIENTS; i++) 
    mapshm->client_pid[i] = -1;

  /* Create the socket we will listen on error */
  listen_socket = socket(AF_INET, SOCK_STREAM, 0); /* TCP */
  if (listen_socket<0) {
    fprintf(stderr,"StartServer: Cannot open socket (err %d,%d)\n",
	    errno,listen_socket);
    return;
  }

  /* Set up structure and bind to port number */
  // Only local programs can connect via loopback address
  listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons(con_port);

  // Loop forever waiting for bind to succeed
  while (1)
  {

    if (bind(listen_socket, (struct sockaddr *) &listen_addr, 
	     sizeof(listen_addr)) < 0)
    {
      if (errno==EADDRINUSE)
      {
	      fprintf(stderr,"Port %d in use? Trying again in 60 sec\n", con_port);
	      sleep(60);
	      continue;
      }
      fprintf(stderr,"Cannot bind (err %d - %s)\n",errno,strerror(errno));
      exit(10);
    }
    break;
  } // infinite loop
  
  // Set up listener
  if (listen(listen_socket, MAX_CLIENTS) < 0) {
    fprintf(stderr,"dnetport: cannot set depth to %d (%d)\n",
	    MAX_CLIENTS,errno);
    exit(10);
  }
 
  /* Loop forever waiting for connection requests */
  while(1)
  {

    int client_sock;
    struct sockaddr_in addr;
    unsigned int addr_len = sizeof(struct sockaddr_in);
    int client;
    int child_pid;

    // Accept a new connectionr request
    if (bDebug)
    {
      printf("Waiting for connection on port %d\n", con_port);
    }
    client_sock = accept(listen_socket, (struct sockaddr *) &addr, &addr_len);
    if (client_sock<0)
    {
      fprintf(stderr,"accept failed (%d,%d)\n",
	      errno,client_sock);
      sleep(10);
      continue;
    }

    if (getpeername(client_sock, 
		     (struct sockaddr *) &addr, 
		     &addr_len) < 0) {

      fprintf(stderr,"BAD ERROR encountered - error %d - %s\n",
	      errno,
	      strerror(errno));
      close(client_sock);
      sleep(20);
      continue;
    }

    client = -1;
    for (i=0; i<MAX_CLIENTS; i++) 
      if (mapshm->client_pid[i] < 0) client = i;

    if (client<0)
    {  /* No free slots */
      close(client_sock);
      sleep(60);
      continue;
    }
    mapshm->timetag[client] = 0;
    mapshm->read_index[client] = 0;
    mapshm->write_index[client] = 0;

    /* Fork the various service threads */
    if ((child_pid = fork()) == 0)
    {
      // Child thread so execute read code
      ServerReadThread(client_sock, client);
      exit(1);
    } // child process

    if (child_pid < 0)
    {
      sprintf(errbuf, "%s: fork failure, error %d: %s.",
            WHOAMI, errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      sleep(2);
      continue;
    } // error starting child process

    mapshm->client_pid[client] = child_pid;
    if (bDebug)
    {
      printf("New connectionon, started read client %d, pid %d\n",
         client, child_pid);
    }

    sleep(2);
  } // infinite loop
} // StartServer()

