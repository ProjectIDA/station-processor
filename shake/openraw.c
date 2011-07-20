/* 
Routine:  openraw.c   
Purpose:  Opens a serial port for raw binary data access
**************************** update history ***********************************
mmddyy	who	changes made
*******************************************************************************
102610	fcs	File creation 
*******************************************************************************

Usage:
				
*******************************************************************************
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////
int openraw(
  const char  *devname)
// returns a non-negative fildes on successful completion;  otherwise a -1 is
// returned and errno is set to indicate the error.
{
  int   fildes;
  struct termios rawtty;		// see termio(7)

  if ((fildes = open(devname, O_RDWR | O_NOCTTY)) < 0)
  {
    // error return
    return(fildes);
  }

  // set up terminal device for raw mode operation (see termio(7))
  memset(&rawtty, 0, sizeof(rawtty));
  rawtty.c_iflag = 0;
  rawtty.c_oflag = 0;
  rawtty.c_cflag = CS8 | PARODD | PARENB | CREAD | CLOCAL;
  rawtty.c_lflag = 0;       // raw mode
  rawtty.c_line = 0;
  rawtty.c_cc[VMIN] = 5;  // how many characters to buffer
  rawtty.c_cc[VTIME] = 1;   // or wait n tenths of a second
  cfmakeraw(&rawtty);
  cfsetospeed(&rawtty, B38400);
  cfsetispeed(&rawtty, B38400);
  if (tcsetattr(fildes, TCSANOW, &rawtty) < 0)
  {
    // error return
    close(fildes);
    return(-1);
  }

  ioctl(fildes, TCFLSH, 0);
  return(fildes);
} // openraw()

