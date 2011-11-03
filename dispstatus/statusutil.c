/*****************************************************************************
File:     statusutil.c
Author:   Frank Shelly
Created:  12 March 2008
Copyright (c) 2007 Albuquerque Seismological Laboratory
Purpose:
Routines useed by various programs to communicate status information
to the status display program

char *MapStatusMem(void **mapstatus)
Maps shared memory area for status display into current program

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2008-03-12 FCS - Creation
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "include/shmstatus.h"

#define WHOAMI "statusutil"

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

//////////////////////////////////////////////////////////////////////////////
// Determine index for this q330serv from the configuration directory
// config str can be of the form /etc/q330/DLG1
//   or /etc/q330/DLG1/diskloop.status
// Returns -1 for can't figure out the index
// or 0 to MAX_DLG-1 for the index value (0 == DLG1)
int GetDlgIndex(const char *configstr)
{
  int     iDig;
  int     i;

  static  int  rememberdig=-2;

  // Only need to parse this string once
  if (rememberdig > -2)
    return rememberdig;

  iDig = 0;
  rememberdig = 0;

  // Parse /etc
  for (i=0; configstr[i] == '/'; i++)
    ; // skip leading /

  if (strncmp(&configstr[i], "etc", 3) != 0)
    rememberdig;

  // Parse q330
  i += 3;
  if (configstr[i] != '/') return rememberdig;
  for (; configstr[i] == '/'; i++)
    ; // skip separating /

  if (strncmp(&configstr[i], "q330", 4) != 0)
    return rememberdig;

  // Parse DLG
  i += 4;
  if (configstr[i] != '/') return rememberdig;
  for (; configstr[i] == '/'; i++)
    ; // skip separating /

  if (strncmp(&configstr[i], "DLG", 3) != 0)
    return rememberdig;

  i += 3;
  if (!isdigit(configstr[i]))
    return rememberdig;

  // Get index
  iDig = configstr[i] - '0';
  iDig = iDig-1;

  if (iDig < 0) iDig = 0;
  rememberdig = iDig;
  return rememberdig;
} // GetDlgIndex()


