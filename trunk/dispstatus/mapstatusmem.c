/*****************************************************************************
File:     mapstatusmem.c
Author:   Frank Shelly
Created:  12 March 2008
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:
Maps shared memory area for status display into current program

Update History:
mmddyy who Changes
==============================================================================
031208 fcs Creation
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "include/shmstatus.h"

#define WHOAMI "mapstatusmem"

//////////////////////////////////////////////////////////////////////////////
// Creates the shared memory segment used for status display info sharing
char *MapStatusMem(void **mapstatus)
{

   int   shmid;
   static char *unixadr = NULL;
   static char errbuf[1024];

   if (unixadr != NULL && unixadr != 0)
   {
     *mapstatus = unixadr;
     return NULL;
   }

   /* Attach the shared memory segment */
   if ((shmid = shmget(SHMKEY_STATUS, sizeof (struct s_mapstatus),
         IPC_CREAT | 00660))==-1)
   {
      sprintf(errbuf, "shmget SHMKEY_STATUS failure, error %d: %s.",
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

  *mapstatus = unixadr;
  return NULL;
} // MapStatusMem()

