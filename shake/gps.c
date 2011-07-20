/*
File:       gps.c
Copyright:  (C) 2011 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Interface to get time from a GPS card
Update History:
mmddyy who Changes
==============================================================================
033922 fcs Creation
******************************************************************************/

#include "bcuser.h"
#include <sys/time.h>
#include "gps.h"

static BC_PCI_HANDLE hBC_PCI = NULL;

// GPS receiver code initialization
// Returns -1 on failure, 0 otherwise
int gpsInit()
{
  // Start the device
  hBC_PCI = bcStartPci();
  if (!hBC_PCI)
    return -1;

  // GPS mode
  bcSetMode(hBC_PCI, MODE_GPS);

  // We want time in year,day of year format
  bcSetTimeFormat(hBC_PCI, FORMAT_BINARY);

  // Time in UTC time vs GPS
  bcSetGpsTmFmt(hBC_PCI, UTC_TIME_FMT);

  // Year Auto Increment
  bcSetYearAutoIncFlag(hBC_PCI, YEAR_AUTO_ENA);

  return 0;
}

// Get the current GPS time from receiver
// -1 = can't communicate
//  0 = Time returned
int gpsTime(
        struct timeval *uxtime,
	unsigned char  *status)
{
  DWORD maj, minor;
  WORD nano;
  BYTE stat;
  time_t *movetime;

  if ( bcReadBinTimeEx (hBC_PCI, &maj, &minor, &nano, &stat) == FALSE )
  {
    return -1;
  }

  *status = stat;
  uxtime->tv_sec = maj;
  uxtime->tv_usec = minor;
  
  return 0;
} // gpsTime()

