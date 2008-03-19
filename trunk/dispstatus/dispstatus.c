/*
File:       dispstatus
Copyright:  (C) 2008 by Albuquerque Seismological Laboratory
Purpose:    Displays status of ASL station processor to ACS LCD display

Update History:
mmddyy who Changes
==============================================================================
031208 fcs Creation
******************************************************************************/
#define WHOAMI "dispstatus"
#define VERSION_DATE  "12 March 2008"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void daemonize();

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
int main (int argc, char **argv)
{
  int  fdTerm;   // open file designator for terminal device

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

  // Clear display
  acsWrite(fdTerm, "CFF007F");
  acsPrint(fdTerm, "Line 3", 3);
  acsPrint(fdTerm, "Line 1", 1);
  acsPrint(fdTerm, "Line 8", 8);
  acsPrint(fdTerm, "Line 5", 5);

  return 0 ;
} // main()

