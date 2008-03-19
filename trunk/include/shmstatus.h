/*
File:       shmstatus.h
Copyright:  (C) 2008 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Defines shared memory for status display exchange
Update History:
mmddyy who Changes
==============================================================================
031208 fcs Creation
******************************************************************************/

#ifndef _SHMSTATUS_H_
#define _SHMSTATUS_H_

#include "include/dcc_std.h"
#include "include/dcc_time.h"

// Unique unix id for the status display shared memory area
#define SHMKEY_STATUS 11

// How many seconds between screen changes
#define SCREEN_DELAY 5

// Maximum number of data loggers supported
// Limited by 8 line display
#define MAX_DLG 8

// structure for individual data logger status
struct s_dlgstatus
{
  STDTIME2    timeLastData;      // When last data was received from q330
}; // s_dlgstatus

// Server structure for inter fork comm
struct s_mapstatus
{
  struct s_dlgstatus dlg[MAX_DLG];  // info for each datalogger
}; // s_mapstatus

//////////////////////////////////////////////////////////////////////////////
// Routine call prototype definitions
// Calls return NULL on no errors, or pointer to error message string


//////////////////////////////////////////////////////////////////////////////
// Creates/points to the shared memory segment for status display
char *MapStatusMem(void **mapStatus);

#endif // _SHMSTATUS_H_ defined

