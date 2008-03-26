/*
File:       dispstatus
Copyright:  (C) 2008 by Albuquerque Seismological Laboratory
Purpose:    Displays status of ASL station processor to ACS LCD display

Algorithm:
        1.  Determines list of attached Q330's using directory convention of:
            /etc/q330/DLG[1-9]
            For each DLG? directory the program assumes there is a Q330

        2.  Infinite loop start

        3.    Wait for operator to press any display key to start status sequence

        4.    For each type of status display

        5.      Call output routine for that display screen

        6.      Wait for timeout, or keyboard input

        7.    Loop back to 4 for next display screen

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
******************************************************************************/
#define WHOAMI "dispstatus"
#define VERSION_DATE  "12 March 2008"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>

#include "include/shmstatus.h"
#include "include/dcc_time_proto2.h"

void daemonize();

// Structure for communicating with keypad thread
struct s_mapkeypad
{
  STDTIME2  timePress;       // Time key was pressed
  int       key;             // 0-6 value of key pressed
                             // -1 set by main program to test for change
  int       fdKeypad;        // File descriptor for keypad
}; // s_mapkeypad
static struct s_mapkeypad *mapkeypad=NULL;


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
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

   if ((int)(unixadr = (char *)shmat(shmid,0,SHM_R | SHM_W))==-1)
   {
      sprintf(errbuf, "%s: shmat PRIVATE failure, error %d: %s.",
            WHOAMI, errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

  *mapshm = unixadr;
  return NULL;
} // MapKeypadMem()

//////////////////////////////////////////////////////////////////////////////
static void sigterm_nodaemon()
{
  fprintf(stderr, "dispstatus shutting down\n");
  usleep(50000);
  fprintf(stderr, "Exiting dispstatus\n");
  exit(EXIT_SUCCESS);
} // sigterm_nodaemon()

//////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
  fprintf(stderr, "Usage: %s <serial port> [debug]\n", WHOAMI);
  fprintf(stderr, "  <serial port> usually /dev/ttyS0\n");
  fprintf(stderr, "  debug for output to screen, no daemonizing\n");
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

fprintf(stderr, "Send: P%s%s%s\n",
lineStr, "00010", msg);
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
fprintf(stderr, "Failed to read 4 chars from keypad after getting SOH char\n");
      continue;
    }

    // Verify the K character
    if (retstr[0] != 'K')
    {
fprintf(stderr, "Keypad string did not start with K character\n");
      continue;
    }
    // Verify that 4th character is EXT termination character
    if (retstr[3] != 0x03)
    {
fprintf(stderr, "Keypad string did not end with EXT character\n");
      continue;
    }

    // save the time and character
    mapkeypad->timePress = ST_GetCurrentTime2();
    mapkeypad->key = (retstr[1] & 0x07);
fprintf(stderr, "Detected keypress %d\n", mapkeypad->key);
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

    // Get current time
    now = ST_GetCurrentTime2();
  }
  while (seconds < 0 || ST_TimeComp2(now, endTime) <= 0);

if (mapkeypad->key > -1)
fprintf(stderr, "Waitkeypad detected key press %d\n", mapkeypad->key+1);
else
fprintf(stderr, "Waitkeypad timed out after %d seconds\n", seconds);
  
  return mapkeypad->key+1;
} // Waitkeypad

//////////////////////////////////////////////////////////////////////////////
// Display the primary status screen
void PrimaryScreen(
  int fdTerm,                   // file descriptor for status display
  int iNumDig)                  // How many digitizers we have
{
  char  msg[40];
  int   iDig;
  STDTIME2  now;

  // Clear display
  acsClear(fdTerm);

  // Display current unix clock
  now = ST_GetCurrentTime2();
  sprintf(msg, "UNIX %04d,%03d,%02d:%02d:%02d",
          now.year, now.day, now.hour, now.minute, now.second);
  acsPrint(fdTerm, msg, 1);

  // loop through each digitizer
  for (iDig=0; iDig < iNumDig; iDig++)
  {
    sprintf(msg, "DLG%d %04d,%03d,%02d:%02d:%02d", iDig+1,
            now.year, now.day, now.hour, now.minute, now.second);
    acsPrint(fdTerm, msg, 2+iDig);
  } // for each digitizer
} // PrimaryScreen

//////////////////////////////////////////////////////////////////////////////
// Display a digitizer screen
void DigitizerScreen(
  int fdTerm,                   // file descriptor for status display
  int iDig)                     // Which digitizer to display status for
{
  char  msg[40];

  // Clear display
  acsClear(fdTerm);

  // Identify digitizer
  sprintf(msg, "DLG%d status screen", iDig+1);
  acsPrint(fdTerm, msg, 1);

  // Display status information

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

  if (argc < 2 || argc > 3)
  {
    ShowUsage();
    fprintf(stderr, "%s: line %d: %s: %s\n",
        __FILE__, __LINE__, __DATE__, "test");
    exit(1);
  }

  if (argc == 3)
  {
    if (strcmp(argv[2], "debug") != 0)
    {
      ShowUsage();
      exit(1);
    }
  }

  // Set up to run program as a daemon
/* Disable daemonize option until testing is ready for it
  if (argc == 2)
  {
    daemonize();
  }
  else
*/
  {
    signal(SIGINT,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGKILL,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGHUP,sigterm_nodaemon); /* Die on SIGTERM */
    signal(SIGTERM,sigterm_nodaemon); /* Die on SIGTERM */
  }

  // Open the terminal device
  if ((fdTerm = openraw(argv[1])) < 0)
  {
    fprintf(stderr, "%s: line %d: %s: %s\n",
        __FILE__, __LINE__, __DATE__, strerror(errno));
    exit(1);
  }

  // create the keypad shared memory segment
  if ((retmsg = MapKeypadMem((void **)&mapkeypad)) != NULL)
  {
    fprintf(stderr, "%s:main %s\n", WHOAMI, retmsg);
    exit(1);
  }
  memset(mapkeypad, 0, sizeof(struct s_mapkeypad));
  mapkeypad->fdKeypad = fdTerm;

  // Start the keypad read thread
  if (pthread_create(&keypad_tid, NULL, KeypadThread, (void *)mapkeypad))
  {
    fprintf(stderr, "%s:main pthread_create KeypadThread: %s\n",
      WHOAMI, strerror(errno));
    exit(1);
  }

  // Get information about the number of digitizers
  FindDLG(&iNumDig);

  // Infinite loop displaying status
  while (1)
  {
    // Display primary status display
    PrimaryScreen(fdTerm, iNumDig);
    WaitKeypad(fdTerm, 10);

    // Display each of the digitizer status screens
    for (iDig=0; iDig < iNumDig; iDig++)
    {
      DigitizerScreen(fdTerm, iDig);
      WaitKeypad(fdTerm, 10);
    } // for each digitizer detected

    if (mapkeypad->key < 0)
      WaitKeypad(fdTerm, 15);
    acsClear(fdTerm);
    acsPrint(fdTerm, "Press any key", 2);
    acsPrint(fdTerm, "for status", 3);

    // Wait forever for start keypress
    WaitKeypad(fdTerm, -1);
  } // infinite status loop

  return 0 ;
} // main()

