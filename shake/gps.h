/*
File:       gps.h
Copyright:  (C) 2011 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Interface to get time from a GPS card

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2011-03-39 FCS - Creation
******************************************************************************/

#ifndef _GSP_H_
#define _GSP_H_

#include <sys/time.h>

// GPS receiver code initialization
// Returns -1 on failure, 0 otherwise
int gpsInit();

// Get the current GPS time from receiver
// -1 = can't communicate
//  0 = Time returned
int gpsTime(
        struct timeval *uxtime,
        unsigned char  *status);

#endif // _GSP_H_ defined

