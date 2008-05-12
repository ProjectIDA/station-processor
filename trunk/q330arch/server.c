/*****************************************************************************
File:     server.c
Author:   Frank Shelly
Created:  26 March 2007
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:
Log message server for q330 applications
Seed message server for q330 applications
Server listens for localhost connections only
Once a connection is made, Seed message blocks are read in. LOG Message blocks
will be buffered for up to 60 seconds or until a seed log record can be filled.  Then output to a circular buffer.
Any other seed message blocks are put directly into the buffer.
The main program takes seed records from the buffer and archives them
Update History:
mmddyy who Changes
==============================================================================
032607 fcs Creation
111907 fcs Modified to work with all seed records, not just log messages
020808 fcs Use pthread_create instead of fork calls
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
#include <syslog.h>

#include "include/diskloop.h"
#include "include/q330arch.h"

#define WHOAMI "q330arch"

static int iSeedRecordSize;
static struct s_mapshm *mapshm=0;

// structure for passing parameters to ServerReadThread
struct s_readthread
{
  int iClient;
  int iSocket;
  struct s_mapshm *mapshm;
};

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
   if ((shmid = shmget(IPC_PRIVATE, sizeof (struct s_mapshm),
         IPC_CREAT | 00666))==-1)
   {
      sprintf(errbuf, "shmget Q330ARCH failure, error %d: %s.",
            errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

   if ((int)(unixadr = (char *)shmat(shmid,0,SHM_R | SHM_W))==-1)
   {
      sprintf(errbuf, "%s: shmat Q330ARCH failure, error %d: %s.",
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
void *ServerReadThread(void *params)
{
  int   iClient;
  int   iSocket;
  int   iCount;
  int   j;
  int   iBuf;
  struct s_readthread *thread_args;

  thread_args = (struct s_readthread *) params;
  iClient = thread_args->iClient;
  iSocket = thread_args->iSocket;

  mapshm = (struct s_mapshm *) thread_args->mapshm;


  // Make sure server has finished initializing us
  while (mapshm->client_tid[iClient] == 0)
    sleep(1);

  if (mapshm->bDebug)
  {
    printf("New read connection %d, thread id=%lu\n",
        iClient, mapshm->client_tid[iClient]);
  }
  else
  {
    syslog(LOG_INFO, "New read connection %d, thread id=%lu\n",
        iClient, mapshm->client_tid[iClient]);
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
        // Setting client_tid to 0 lets server know this client done
        if (mapshm->bDebug)
        {
          printf("Closing client read thread %d, thread id %lu\n",
              iClient, mapshm->client_tid[iClient]);
        }
        else
        {
          syslog(LOG_INFO, "Closing client connection %d, thread id=%lu\n",
              iClient, mapshm->client_tid[iClient]);
        }
        mapshm->client_tid[iClient] = 0;
        close(iSocket);
        return NULL;
      } // connection closed

//      if (mapshm->bDebug)
//      {
//        printf("Client %d, Read %d+%d/%d bytes from socket\n",
//          iClient, j, iCount, iSeedRecordSize);
//      }

      // Read until we fill up a full seed record
      iCount = iCount + j;
    } // read until one record has been read in

    // Make sure that server is ready to process another record
    while (mapshm->read_index[iClient] != mapshm->write_index[iClient])
    {
      usleep(100);
    } // while server has not finished writing last record

    // Let server know there is a new data ready to be written
    // And what time it was made available
    mapshm->read_index[iClient] = iBuf;
    mapshm->timetag[iClient] = time(NULL);

    // Wait until the message is processed
    while (mapshm->read_index[iClient] != mapshm->write_index[iClient])
    {
      usleep(100);
    } // while server has not finished writing last record

    // Send acknowledgement response back to client
    send(iSocket, &mapshm->buffer[iClient][iBuf][5], 1, 0);

  } // loop forever

} // ServerReadThread()

//////////////////////////////////////////////////////////////////////////////
// Entry point for main program to start server listening for log messages
void *StartServer(void *params)
{
  int   listen_socket;
  int   i;
  char  *errstr;
  struct s_readthread read_params;

  struct s_mapshm *mapshm;
  struct sockaddr_in listen_addr; /* Listen address setup */

  // Get thread arguments
  mapshm = (struct s_mapshm *) params;

  if ((errstr=LoopRecordSize(&iSeedRecordSize)) != NULL)
  {
    fprintf(stderr, "%s\n", errstr);
    mapshm->bQuit = 1;
    return NULL;
  }

  if (mapshm->bDebug)
    printf("StartServer(port=%d, debug=%d) size %d\n",
        mapshm->iPort, mapshm->bDebug, iSeedRecordSize);

  // Map in the shared memory area
  for (i=0; i<MAX_CLIENTS; i++) 
    mapshm->client_tid[i] = 0;

  /* Create the socket we will listen on error */
  listen_socket = socket(AF_INET, SOCK_STREAM, 0); /* TCP */
  if (listen_socket<0)
  {
    fprintf(stderr,"StartServer: Cannot open socket (err %d,%d)\n",
	    errno,listen_socket);
    mapshm->bQuit = 1;
    return NULL;
  }

  /* Set up structure and bind to port number */
  // Only local programs can connect via loopback address
  listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons(mapshm->iPort);

  // Loop forever waiting for bind to succeed
  while (1)
  {

    if (bind(listen_socket, (struct sockaddr *) &listen_addr, 
	     sizeof(listen_addr)) < 0)
    {
      if (errno==EADDRINUSE)
      {
        fprintf(stderr,"Port %d in use? Trying again in 30 sec\n",
        mapshm->iPort);
        sleep(30);
        continue;
      }
      fprintf(stderr,"Cannot bind (err %d - %s)\n",errno,strerror(errno));
      mapshm->bQuit = 1;
      return NULL;
    }
    break;
  } // infinite loop
  
  // Set up listener
  if (listen(listen_socket, MAX_CLIENTS) < 0) {
    fprintf(stderr,"dnetport: cannot set listen depth to %d (%d)\n",
	    MAX_CLIENTS,errno);
    mapshm->bQuit = 1;
    return NULL;
  }
 
  /* Loop forever waiting for connection requests */
  while(1)
  {

    int client_sock;
    struct sockaddr_in addr;
    unsigned int addr_len = sizeof(struct sockaddr_in);
    int client;
    pthread_t child_tid;

    // Accept a new connection request
    if (mapshm->bDebug)
    {
      printf("Waiting for connection on port %d\n", mapshm->iPort);
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
      if (mapshm->client_tid[i] == 0) client = i;

    if (client<0)
    {  /* No free slots */
      close(client_sock);
      if (mapshm->bDebug)
      {
        printf("All client connections are used up, waiting for one to free up...\n");
      }
      sleep(60);
      continue;
    }
    mapshm->timetag[client] = 0;
    mapshm->read_index[client] = 0;
    mapshm->write_index[client] = 0;
    read_params.iSocket = client_sock;
    read_params.iClient = client;
    read_params.mapshm = mapshm;
    if (mapshm->bDebug)
    {
      printf("q330serv connected and attached to client handler %d, socket %d\n",
        read_params.iClient, read_params.iSocket);
    }

    /* Fork the various service threads */
    if (pthread_create(&child_tid, NULL, ServerReadThread, &read_params))
    {
      fprintf(stderr, "%s:StartServer pthread_create ServerReadThread: %s\n",
        WHOAMI, strerror(errno));
      mapshm->bQuit = 1;
      return NULL;
    } // error starting child thread

    mapshm->client_tid[client] = child_tid;
    if (mapshm->bDebug)
    {
      printf("New connectionon, started read client %d, thread id %lud\n",
         client, child_tid);
    }

    sleep(2);
  } // infinite loop
} // StartServer()

