/*
File:       idaapi.h
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Interface to IDA disk loop library
Update History:
mmddyy who Changes
==============================================================================
061207 fcs Creation
******************************************************************************/

#ifndef _IDAAPI_H_
#define _IDAAPI_H_

//////////////////////////////////////////////////////////////////////////////
// Initialize IDA interface
char *idaInit(
  const char  *dlname     // Name of ida disk loop to save to
  );                      // returns NULL or an error string pointer

//////////////////////////////////////////////////////////////////////////////
// Write a record of SEED data
// Hides all of the logic needed to run a circular buffer from main program
char *idaWriteChan(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf,   // Seed record pointer
  const char  *dlname     // Name of ida disk loop to save to
  );                      // returns NULL or an error string pointer

#endif // _IDAAPI_H_ defined

