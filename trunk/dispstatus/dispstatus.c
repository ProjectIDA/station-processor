/*
File:       dispstatus
Copyright:  (C) 2008 by Albuquerque Seismological Laboratory
Purpose:    Displays status of ASL station processor to ACS LCD display

Algorithm:
        1.  Determines list of attached Q330's using directory convention of:
            /etc/q330/DLG[1-9]
            For each DLG? directory the program assumes there is a Q330

        2.  Infinite loop of status display sequences

        3.    Wait for operator to press any display key to start status sequence

        4.    For each type of status display

        5.      Call output routine for that display screen

        6.      Wait for timeout, or keyboard input

        7.      If keyboard up arrow input then go backward one display type, loop to 4
                else Loop back to 4 for next display screen

        8.    Display "Press any key for Status"

        9.  End infinite loop, goto step 2

        Status information from other programs such as q330serv is passed to the
        display program via a Unix shared memory segment defined by shmstatus.h
        The statusutil.c file contains routines useful for those other programs
        to get data stored in this region.

Update History:
mmddyy who Changes
==============================================================================
031208 fcs Creation
040910 fcs Built with new lib330
******************************************************************************/
#define WHOAMI "dispstatus"
const char *VersionIdentString="Release 1.3";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termio.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#include "include/shmstatus.h"
#include "include/dcc_time_proto2.h"
#include "libsupport.h"

// Set this to how long you want each screen to last
#define SCREEN_WAIT  30

#define KEY_UPARROW  6


extern void daemonize();
extern void child_handler(int signum);

// Structure for communicating with keypad thread
struct s_mapkeypad
{
  STDTIME2  timePress;       // Time key was pressed
  int       key;             // 0-6 value of key pressed
                             // -1 set by main program to test for change
  int       fdKeypad;        // File descriptor for keypad
}; // s_mapkeypad
static struct s_mapkeypad *mapkeypad=NULL;
static int bMain = 0;
int    bDebug=0;

/////////////////////////////////////////////////////////////////////////////
// Creates the shared memory segment used for inter fork communications
char *MapKeypadMem(void **mapshm)
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
   if ((shmid = shmget(IPC_PRIVATE, sizeof (struct s_mapkeypad),
         IPC_CREAT | 00666))==-1)
   {
      sprintf(errbuf, "shmget PRIVATE failure, error %d: %s.",
            errno, strerror(errno));
      if (bDebug)
        fprintf(stderr, "%s\n", errbuf);
      else
        syslog(LOG_ERR, "%s\n", errbuf);
      return errbuf;
   }

   if ((int)(unixadr = (char *)shmat(shmid,0,SHM_R | SHM_W))==-1)
   {
      sprintf(errbuf, "%s: shmat PRIVATE failure, error %d: %s.",
            WHOAMI, errno, strerror(errno));
      if (bDebug)
        fprintf(stderr, "%s\n", errbuf);
      else
        syslog(LOG_ERR, "%s\n", errbuf);
      return errbuf;
   }

  *mapshm = unixadr;
  return NULL;
} // MapKeypadMem()

//////////////////////////////////////////////////////////////////////////////
static void sigterm_nodaemon(int sig)
{
  fprintf(stderr, "Exiting dispstatus\n");
  exit(EXIT_SUCCESS);
} // sigterm_nodaemon()

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
  fprintf(stderr, "Usage: %s <serial port> [debug]\n", WHOAMI);
  fprintf(stderr, "  <serial port> usually /dev/ttyS3 or /dev/ttyS0\n");
  fprintf(stderr, "  debug for output to screen, no daemonizing\n");
  fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
}

//////////////////////////////////////////////////////////////////////////////
// Opens a device for serial IO with now ascii data processing
// Returns -1 on error, or file designator
int openraw(
  char *dev_name          // Name of device to open in raw mode
  )
{
  int dev_fdes;           // open file designator
  struct termio dev_termio;     // man termio for usage info

  // Open device for read/write access
  if ((dev_fdes = open(dev_name, O_RDWR)) < 0)
  {
    return dev_fdes;
  } // error opening device


  // Set up for raw terminal IO (man termio)
  memset(&dev_termio, 0, sizeof(dev_termio));
  dev_termio.c_iflag = 0;
  dev_termio.c_oflag = 0;
  dev_termio.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
  dev_termio.c_lflag = 0; // raw mode
  dev_termio.c_line = 0;
  dev_termio.c_cc[VMIN] = 25;  // how many characters to buffer
  dev_termio.c_cc[VTIME] = 3;  // or wait n tenths of a second
  if (ioctl(dev_fdes, TCSETAF, &dev_termio) < 0)
  {
    close(dev_fdes);
    return -1;
  } // ioctl call error

  return dev_fdes;
} // openraw()

//////////////////////////////////////////////////////////////////////////////
void acsWrite(
  int fd,                      // file descriptor for terminal device
  const char *msg)             // String to send to device
{
  write(fd, "\001", 1);
  write(fd, msg, strlen(msg));
  write(fd, "\003", 1);
} // acsWrite()

//////////////////////////////////////////////////////////////////////////////
// Send a print command to ACS display, followed by msg
// line is between 1 and 8 and says which line msg will appear on
// The line is cleared before the message is output
void acsPrint(
  int fd,                      // file descriptor for terminal device
  const char *msg,             // Message string
  int line)                    // line number (1-8) to display message on
{
  int lineMask;
  char lineStr[4];

  // Figure out the correct mask for the line
  if (line < 1) line = 1;
  if (line > 8) line = 8;
  lineMask = 0x01;
  while (line > 1)
  {
    lineMask = lineMask << 1 ;
    line--;
  }
  sprintf(lineStr, "%02X", lineMask);

  // Clear the line
  write(fd, "\001C", 2);
  write(fd, lineStr, 2);
  write(fd, "007F\003", 5);

//fprintf(stderr, "Send: P%s%s%s\n",
//lineStr, "00010", msg);
  // Print message
  write(fd, "\001P", 2);
  write(fd, lineStr, 2);
  write(fd, "00010 ", 6); // Column 0, Font 0, Type 1, left justification
  write(fd, msg, strlen(msg));
  write(fd, "\003", 1);
} // acsPrint()

//////////////////////////////////////////////////////////////////////////////
// Clears the display screen
void acsClear(
  int fd)                      // file descriptor for terminal device
{
  acsWrite(fd, "CFF007F");
} // acsClear()

//////////////////////////////////////////////////////////////////////////////
// Thread to monitor keypad for pressed keys
void *KeypadThread(void *params)
{
  struct s_mapkeypad *mapkeypad;
  char   retstr[8];
  char   inchar;
  int    i;

  mapkeypad = (struct s_mapkeypad *)params;

  // Initialy no key has been pressed
  mapkeypad->key = -1;

  // Infinite loop reading keypad presses
  while (1)
  {
    // Wait for an SOH char to start message
    i=0;
    inchar = 0;
    while (inchar != 0x01)
    {
      read(mapkeypad->fdKeypad, &inchar, 1);
    } // while message start char not received

    // Next 4 chars should be 'K' 2 char key and EXT end message char
    if (read(mapkeypad->fdKeypad, &retstr, 4) != 4)
    {
      if (bDebug)
        fprintf(stderr, "Failed to read 4 chars from keypad after getting SOH char\n");
      else
        syslog(LOG_ERR, "Failed to read 4 chars from keypad after getting SOH char\n");
      continue;
    }

    // Verify the K character
    if (retstr[0] != 'K')
    {
      if (bDebug)
        fprintf(stderr, "Keypad string did not start with K character\n");
      else
        syslog(LOG_ERR, "Keypad string did not start with K character\n");
      continue;
    }
    // Verify that 4th character is EXT termination character
    if (retstr[3] != 0x03)
    {
      if (bDebug)
        fprintf(stderr, "Keypad string did not end with EXT character\n");
      else
        syslog(LOG_ERR, "Keypad string did not end with EXT character\n");
      continue;
    }

    // save the time and character
    mapkeypad->timePress = ST_GetCurrentTime2();
    mapkeypad->key = (retstr[2] & 0x07);
//fprintf(stderr, "Detected keypress %d\n", mapkeypad->key);
  } // loop forever
}


//////////////////////////////////////////////////////////////////////////////
// Wait for user to press a keypad button
int WaitKeypad(
  int fd,                      // file descriptor for terminal device
  int  seconds)                // How many seconds to wait, -1 = forever
  // returns                   +number for which key was pressed
  //                           0 for timeout
{
  STDTIME2  startTime;
  STDTIME2  endTime;
  STDTIME2  now;
  int       i;
  char      retstr[40];
  char      inchar;

  // Get time period for request
  startTime = ST_GetCurrentTime2();
  endTime   = ST_AddToTime2(startTime, 0, 0, 0, seconds, 0);

  // Overwrite any prior keypad detection
  mapkeypad->key = -1;

  do
  {
    // Test for keypress
    if (mapkeypad->key > -1)
      break;

    // Keep backlight on while waiting
    if ( seconds > 0)
      acsWrite(fd, "b2");

    // Don't hog up the CPU
    usleep(100000);

    // Get current time
    now = ST_GetCurrentTime2();
  }
  while (seconds < 0 || ST_TimeComp2(now, endTime) <= 0);

//if (mapkeypad->key > -1)
//fprintf(stderr, "Waitkeypad detected key press %d\n", mapkeypad->key+1);
//else
//fprintf(stderr, "Waitkeypad timed out after %d seconds\n", seconds);
  
  return mapkeypad->key+1;
} // Waitkeypad

//////////////////////////////////////////////////////////////////////////////
// Display the primary status screen
void PrimaryScreen(
  int fdTerm,                           // file descriptor for status display
  struct s_dlgstatus dlg[MAX_DLG],      // status for each data logger
  STDTIME2    watchdogServ[MAX_DLG],    // watchdog timer for q330serv
  STDTIME2           watchdogArch,      // watchdog timer for q330arch program
  enum tlibstate     libstate[MAX_DLG], // the run state that lib330 is in
  int iDlg,                             // Even index 0,2,4,...
  int iNumDig)                          // How many digitizers we have
{
  char  msg[40];
  STDTIME2  now;
  STDTIME2  timetag;
  int       iDig;
  DELTA_T2  diffTime;
  long      tmsDiff;


  // Clear display
  acsClear(fdTerm);

  // Display current unix clock
  now = ST_GetCurrentTime2();
  sprintf(msg, "UNIX %04d,%03d,%02d:%02d:%02d",
          now.year, now.day, now.hour, now.minute, now.second);
  acsPrint(fdTerm, msg, 1);

  // loop through each digitizer
  for (iDig=iDlg; iDig < iDlg+2 && iDig < iNumDig; iDig++)
  {
    // First verify that watchdog timer is reasonably current
    diffTime = ST_DiffTimes2(now, watchdogServ[iDig]);
    tmsDiff = ST_DeltaToMS2(diffTime);

    if (tmsDiff > 60*10000)
    {
      sprintf(msg, "DLG%d q330serv down", iDig+1);
    } // q330serv is not running
    else
    {
      // Message is dependent upon run state
      switch(libstate[iDig])
      {
        case LIBSTATE_IDLE:    // Not connected to Q330
          sprintf(msg, "DLG%d LIBSTATE IDLE", iDig+1);
          break;
        case LIBSTATE_TERM:    // Terminated
          sprintf(msg, "DLG%d LIBSTATE TERM", iDig+1);
          break;
        case LIBSTATE_PING:    // Un-registered Ping, returns to LIBSTATE_IDLE when done
          sprintf(msg, "DLG%d LIBSTATE PING", iDig+1);
          break;
        case LIBSTATE_REG:     // Requesting Registration
          sprintf(msg, "DLG%d Re Register", iDig+1);
          break;
        case LIBSTATE_READCFG: // Reading Configuration
          sprintf(msg, "DLG%d LIBSTATE READCFG", iDig+1);
          break;
        case LIBSTATE_READTOK: // Reading Tokens
          sprintf(msg, "DLG%d LIBSTATE READTOK", iDig+1);
          break;
        case LIBSTATE_DECTOK:  // Decoding Tokens and allocating structures
          sprintf(msg, "DLG%d LIBSTATE DECTOK", iDig+1);
          break;
        case LIBSTATE_RUNWAIT: // Waiting for command to run
          sprintf(msg, "DLG%d Wait RUN State", iDig+1);
          break;
        case LIBSTATE_DEALLOC: // De-allocating structures
          sprintf(msg, "DLG%d De-allocating", iDig+1);
          break;
        case LIBSTATE_DEREG:   // De-registering
          sprintf(msg, "DLG%d De-registering", iDig+1);
          break;
        case LIBSTATE_WAIT:    // Waiting for a new registration
          sprintf(msg, "DLG%d Wait register", iDig+1);
          break;
        case LIBSTATE_RUN:
          timetag = dlg[iDig].timeLastData;
          sprintf(msg, "DLG%d %04d,%03d,%02d:%02d:%02d", iDig+1,
                  timetag.year, timetag.day, timetag.hour, timetag.minute, timetag.second);
          break;
        default:
          sprintf(msg, "DLG%d LIBSTATE %d", iDig+1, libstate[iDig]);
      } // switch libstate
    } // else q330serv is running

    // Output whatever message we came up with
    acsPrint(fdTerm, msg, 2+(iDig%2));
  } // for each digitizer

  // Display q330arch status
  diffTime = ST_DiffTimes2(now, watchdogArch);
  tmsDiff = ST_DeltaToMS2(diffTime);
  if (tmsDiff > 60*10000)
  {
    sprintf(msg, "q330arch down");
  } // q330arch is not running
  else
  {
    sprintf(msg, "q330arch up");
  } // q330arch running
  acsPrint(fdTerm, msg, 4);

  // If the first q330 is down we have no GPS status
  diffTime = ST_DiffTimes2(now, watchdogServ[iDlg]);
  tmsDiff = ST_DeltaToMS2(diffTime);
  if (tmsDiff > 60*10000)
  {
    sprintf(msg, "GPS q330serv down");
    acsPrint(fdTerm, msg, 5);
    return;
  } // if q330serv that we get GPS status from is down

  // Display GPS status
  sprintf(msg, "GPS sats %d/%d",
          dlg[iDlg].stat_gps.sat_used, dlg[iDlg].stat_gps.sat_view);
  acsPrint(fdTerm, msg, 5);
  sprintf(msg, "Quality: %d%%", dlg[iDlg].clock_quality);
  acsPrint(fdTerm, msg, 6);
  switch(dlg[iDlg].pll_state)
  {
    case PLL_HOLD:
     sprintf(msg, "PLL state: Hold");
      break;
    case PLL_TRACK:
     sprintf(msg, "PLL state: Track");
      break;
    case PLL_LOCK:
     sprintf(msg, "PLL state: Lock");
      break;
    default:
     sprintf(msg, "PLL state: Unknown");
  }
  acsPrint(fdTerm, msg, 7);
  if (dlg[iDlg].vacuum[0] >= 0 ||
      dlg[iDlg].vacuum[1] >= 0 ||
      dlg[iDlg].vacuum[2] >= 0)
  {
    sprintf(msg, "Vac %d %d %d",
            dlg[iDlg].vacuum[0], dlg[iDlg].vacuum[1], dlg[iDlg].vacuum[2]);
    acsPrint(fdTerm, msg, 8);
  }

} // PrimaryScreen

//////////////////////////////////////////////////////////////////////////////
// Display a digitizer screen
void DigitizerScreen(
  int fdTerm,                   // file descriptor for status display
  int iDig,                     // Which digitizer to display status for
  STDTIME2    watchdogServ[MAX_DLG],    // watchdog timer for q330serv
  enum tlibstate     libstate[MAX_DLG], // the run state that lib330 is in
  struct s_dlgstatus dlg[MAX_DLG])      // datalogger status information
{
  char  msg[40];
  STDTIME2  nowTime;
  DELTA_T2  diffTime;
  long      tmsDiff;
  static int bValidStatus=0;  // Set to 1 once we have good status data

  // Clear display
  acsClear(fdTerm);

  // If the q330serv is down and we've never had full status show nothing
  nowTime = ST_GetCurrentTime2();
  diffTime = ST_DiffTimes2(nowTime, watchdogServ[iDig]);
  tmsDiff = ST_DeltaToMS2(diffTime);
  if (tmsDiff > 60*10000 && !bValidStatus)
  {
    sprintf(msg, "DLG%d q330serv down", iDig+1);
    acsPrint(fdTerm, msg, 1);
    return;
  } // the q330serv for this digitizer is down

  // Remember if we've had at least one valid status come through
  if (dlg[iDig].property_tag > 0)
    bValidStatus = 1;

  // Stop at showing digitizer state if we've never had full status before
  if (!bValidStatus)
  {
    sprintf(msg, "DLG%d status wait", iDig+1);
    acsPrint(fdTerm, msg, 1);
    return;
  } // Don't have any valid status to display yet

  // Identify digitizer
  sprintf(msg, "DLG%d # %d", iDig+1, dlg[iDig].property_tag);
  acsPrint(fdTerm, msg, 1);
  sprintf(msg, "Temp: %dC", dlg[iDig].sys_temp);
  acsPrint(fdTerm, msg, 2);
  sprintf(msg, "Analog Supply: %.2fV", dlg[iDig].analog_voltage);
  acsPrint(fdTerm, msg, 3);
  sprintf(msg, "Input Voltage: %.2fV", dlg[iDig].input_voltage);
  acsPrint(fdTerm, msg, 4);
  sprintf(msg, "Main Current: %dma", dlg[iDig].main_cur);
  acsPrint(fdTerm, msg, 5);
  sprintf(msg, "Boom 1-3: %d %d %d",
      dlg[iDig].boom_pos[0], dlg[iDig].boom_pos[1], dlg[iDig].boom_pos[2]);
  acsPrint(fdTerm, msg, 6);
  sprintf(msg, "Boom 4-6: %d %d %d",
      dlg[iDig].boom_pos[3], dlg[iDig].boom_pos[4], dlg[iDig].boom_pos[5]);
  acsPrint(fdTerm, msg, 7);

  // If six minutes between Unix clock and last GPS mark then GPS is down
  if ((now() - dlg[iDig].stat_gps.last_good) > 360.0)
    sprintf(msg, "GPS down          ");
  else
    sprintf(msg, "GPS Quality: %d%%", dlg[iDig].clock_quality);
  if (bDebug)
    fprintf(stderr, "DEBUG %s\n", msg);
  acsPrint(fdTerm, msg, 8);
} // DigitizerScreen()

//////////////////////////////////////////////////////////////////////////////
// Traverses /etc/q330/DLG? tree to find how many q330's we have
void FindDLG(int *piNumDlg)
{
  int iDir;
  char dirname[256];
  int fd;

  *piNumDlg = 0;

  // Find out how many /etc/q330/DLG? entries we can match
  for (iDir=0; iDir < MAX_DLG; iDir++)
  {
    sprintf(dirname, "/etc/q330/DLG%d", iDir+1);
    if ((fd=open(dirname, O_RDONLY)) < 0)
      break;

    close(fd);
  } // loop through all directory name matches
//fprintf(stderr, "DEBUG FindDLG got %d digitizers\n", iDir);
  // Close directory
  *piNumDlg = iDir;
} // FindDLG()

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  int  fdTerm;   // open file designator for terminal device
  char *retmsg;
  pthread_t keypad_tid;
  int  iNumDig;
  int  iDig;
  int  iScreen;
  int  iKey;
  int  index;
  struct s_mapstatus *mapstatus;
  struct s_dlgstatus dlg[MAX_DLG]={};
  STDTIME2           watchdogServ[MAX_DLG]={};
  STDTIME2           watchdogArch={};

  if (argc < 2 || argc > 3)
  {
    ShowUsage();
    exit(1);
  }

  if (argc == 3)
  {
    if (strcmp(argv[2], "debug") != 0)
    {
      ShowUsage();
      exit(1);
    }
    bDebug = 1;
  }

  // Set up to run program as a daemon
  if (argc == 2)
  {
    daemonize();
  }
  else
  {
    signal(SIGINT,sigterm_nodaemon);  // Die on SIGINT
    signal(SIGHUP,sigterm_nodaemon);  // Die on SIGHUP
    signal(SIGTERM,sigterm_nodaemon); // Die on SIGTERM
    signal(SIGCHLD,child_handler);    // Ignore SIGCHLD
    signal(SIGUSR1,child_handler);    // Ignore SIGUSR1
    signal(SIGALRM,child_handler);    // Ignore SIGALRM
  }

  // Open the terminal device
  if ((fdTerm = openraw(argv[1])) < 0)
  {
    if (bDebug)
    {
      fprintf(stderr, "%s: line %d: %s: %s\n",
        __FILE__, __LINE__, __DATE__, strerror(errno));
    }
    else
    {
      syslog(LOG_ERR, "%s: line %d: %s: %s\n",
        __FILE__, __LINE__, __DATE__, strerror(errno));
    }
    exit(1);
  }

  // create the status shared memory segment
  if ((retmsg = MapStatusMem((void **)&mapstatus)) != NULL)
  {
    if (bDebug)
      fprintf(stderr, "%s:main:MapStatusMem %s\n", WHOAMI, retmsg);
    else
      syslog(LOG_ERR, "%s:main:MapStatusMem %s\n", WHOAMI, retmsg);
    exit(1);
  }

  // create the keypad shared memory segment
  if ((retmsg = MapKeypadMem((void **)&mapkeypad)) != NULL)
  {
    if (bDebug)
      fprintf(stderr, "%s:main:MapKeypadMem %s\n", WHOAMI, retmsg);
    else
      syslog(LOG_ERR, "%s:main:MapKeypadMem %s\n", WHOAMI, retmsg);
    exit(1);
  }
  memset(mapkeypad, 0, sizeof(struct s_mapkeypad));
  mapkeypad->fdKeypad = fdTerm;

  // Start the keypad read thread
  if (pthread_create(&keypad_tid, NULL, KeypadThread, (void *)mapkeypad))
  {
    if (bDebug)
      fprintf(stderr, "%s:main pthread_create KeypadThread: %s\n",
        WHOAMI, strerror(errno));
    else
      syslog(LOG_ERR, "%s:main pthread_create KeypadThread: %s\n",
        WHOAMI, strerror(errno));
    exit(1);
  }

  // Remember that we are the main program for signal handler
  bMain = 1;

  // Get information about the number of digitizers
  FindDLG(&iNumDig);

  // Infinite loop displaying status
  while (1)
  {
    // Get latest status for all of the data loggers
    for (iDig=0; iDig < iNumDig; iDig++)
    {
      mapstatus->ixReadStatus[iDig] = mapstatus->ixWriteStatus[iDig];
      dlg[iDig] = mapstatus->dlg[mapstatus->ixReadStatus[iDig]][iDig];

      mapstatus->ixReadData[iDig] = mapstatus->ixWriteData[iDig];
      dlg[iDig].timeLastData = 
         mapstatus->dlg[mapstatus->ixReadData[iDig]][iDig].timeLastData;

      mapstatus->ixReadServ[iDig] = mapstatus->ixWriteServ[iDig];
      watchdogServ[iDig] = mapstatus->servWatchdog[mapstatus->ixReadServ[iDig]][iDig];

      mapstatus->ixReadArch = mapstatus->ixWriteArch;
      watchdogArch = mapstatus->archWatchdog[mapstatus->ixReadArch];

//fprintf(stderr, "Dig %d Read[%d][%d] Time: %s %s\n",
//iDig, mapstatus->ixReadStatus[iDig], mapstatus->ixReadData[iDig],
//dlg[iDig].stat_gps.date, dlg[iDig].stat_gps.time);
    } // loop through all of the digitizers

    // Loop through each display screen
    for (iScreen=0; iScreen < (iNumDig+1)/2 + iNumDig; iScreen++)
    {
      // Going back from first screen gets new status if available
      if (iScreen < 0) break;

      // Display primary status display
      if (iScreen < (iNumDig+1)/2)
      {
        PrimaryScreen(fdTerm, dlg, watchdogServ, watchdogArch, 
                      mapstatus->libstate, iScreen*2, iNumDig);
        iKey = WaitKeypad(fdTerm, SCREEN_WAIT);
        if (iKey == KEY_UPARROW)
        {
          iScreen -= 2;
          continue;
        }
      } // Primary status display

      // One of the individual digitizer screens
      if (iScreen >= ((iNumDig+1)/2) && iScreen < iNumDig + (iNumDig+1)/2)
      {
        iDig = iScreen - (iNumDig+1)/2;
        DigitizerScreen(fdTerm, iDig, watchdogServ, mapstatus->libstate, dlg);
        iKey = WaitKeypad(fdTerm, SCREEN_WAIT);
        if (iKey == KEY_UPARROW)
        {
          iScreen-=2;
          continue;
        }
      } // individual digitizer screen

    } // for each screen

    if (iScreen < 0)
    {
      // User went backwards from first screen, get new status
      continue;
    }

    if (mapkeypad->key < 0)
      WaitKeypad(fdTerm, 5);
    acsClear(fdTerm);
    acsPrint(fdTerm, "Press any key", 2);
    acsPrint(fdTerm, "for status", 3);

    // Wait forever for start keypress
    WaitKeypad(fdTerm, -1);
  } // infinite status loop

  return 0 ;
} // main()

