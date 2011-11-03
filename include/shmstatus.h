/*
File:       shmstatus.h
Copyright:  (C) 2008 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Defines shared memory for status display exchange
Shared data method:
  The data in this shared memory region will have one consumer (dispstatus)
  and two generators of data per data logger.  Too keep everything up to date
  and manage race conditions a tripple buffering scheme is being used.  The
  triple buffering works by keeping an index of the buffer that the consumer
  is currently using, and an index of the buffer that the producer last wrote
  to.  The third unused buffer will be where the next new data will be written
  to by the producer.  When the consumer is ready for new data he sets his
  consumer index to the current producer index marking it as unavailable for
  any new writes.  The producer sets its index after writing a complete set of
  data to the free buffer.

Indexing variables:
  ixWriteData[]     Producer index for data arrival info (one per digitizer)
  ixReadData[]      Consumer index for data arrival info
  ixWriteStatus[]   Producer index for Status information
  ixReadStatus[]    Consumer index for Status information
 

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2008-03-12 FCS - Creation
2010-11-03 FCS - Add place to store vacuum pressure readings
******************************************************************************/

#ifndef _SHMSTATUS_H_
#define _SHMSTATUS_H_

#include "include/dcc_std.h"
#include "include/dcc_time.h"
#include "lib330/libtypes.h"

// Unique unix id for the status display shared memory area
#define SHMKEY_STATUS 11

// How many seconds between screen changes
#define SCREEN_DELAY 5

// Maximum number of data loggers supported
#define MAX_DLG 8

// structure for individual data logger status
struct s_dlgstatus
{
  /////////////////////////
  // First section is data filled out whenever a new seed record is built
  /////////////////////////
  STDTIME2      timeLastData;    // When last data was received from q330


  /////////////////////////
  // Second section is filled out by main program status loop
  /////////////////////////
  unsigned long property_tag;    // Property tag number for Q330
  int           sys_temp;        // Q330 temperature in degrees C
  float         analog_voltage;  // Positive Supply voltage
  float         input_voltage;   // Q330 input voltage
  int           main_cur;        // Main current in ma
  int           boom_pos[6];     // Boom positions (mass position)
  tstat_gps  stat_gps;           // GPS status
  int           pll_state;       // PLL_OFF PLL_HOLD PLL_TRACK PLL_LOCK PLL_AUTO
  float         pll_time_error;  // PLL clock drift
  int           clock_quality;   // 0-100 %
  long          vacuum[3];       // STS1 vacuum pressure readings, -1 = unused
  long          pressure;        // Last pressure reading -1 = no sensor

  // Create some dummy space so we can keep record size constant
  // Otherwise we have to reboot to wipe out memory of shared memory size
  long        dummyl[94];
}; // s_dlgstatus

// Server structure for inter fork comm
struct s_mapstatus
{
  int    iNumDlg;                   // Number of data loggers attached
  enum tlibstate libstate[MAX_DLG]; // No status until q330serv is in LIBSTATE_RUN
  int    ixWriteData[MAX_DLG];      // Producer index for data arrival info
  int    ixReadData[MAX_DLG];       // Consumer index for data arrival info
  int    ixWriteStatus[MAX_DLG];    // Producer index for Status information
  int    ixReadStatus[MAX_DLG];     // Consumer index for Status information
  struct s_dlgstatus dlg[3][MAX_DLG];  // triple buffer for each data logger

  int    ixWriteArch;               // Producer Index q330arch archWatchdog[]
  int    ixReadArch;                // Consumer Index q330arch archWatchdog[]
  STDTIME2 archWatchdog[3];         // Watchdog timer when q330arch last looped

  int    ixWriteServ[MAX_DLG];      // Producer Index q330serv servWatchdog[]
  int    ixReadServ[MAX_DLG];       // Consumer Index q330serv servWatchdog[]
  STDTIME2 servWatchdog[3][MAX_DLG]; // Watchdog timer of when q300serv last looped

  // Create some dummy space so we can keep record size constant
  // Otherwise we have to reboot to wipe out memory of shared memory size
}; // s_mapstatus

//////////////////////////////////////////////////////////////////////////////
// Routine call prototype definitions
// Calls return NULL on no errors, or pointer to error message string


//////////////////////////////////////////////////////////////////////////////
// Creates/points to the shared memory segment for status display
char *MapStatusMem(void **mapStatus);

//////////////////////////////////////////////////////////////////////////////
// Determine index for this q330serv from the configuration directory
// config str can be of the form /etc/q330/DLG1
//   or /etc/q330/DLG1/diskloop.status
// Returns: 0 to MAX_DLG-1 for the index value (0 == DLG1)
//          If it can't figure out the index it assumes index 0
int GetDlgIndex(const char *configstr);

#endif // _SHMSTATUS_H_ defined

