#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/diskloop.h"
#include "include/log330.h"

int main(int argc, char *argv[])
{
  int i;
  char msg[4096];
  char *errmsg;
  char  station[8];
  char  network[4];
  char  channel[4];
  char  location[4];


  if (argc < 1)
  {
    printf("Usage: msgtest <diskloopconfigfile> \n");
    exit(1);
  }

  // Parse the configuration file, get log SNCL values
  if ((errmsg=ParseDiskLoopConfig(argv[1])) != NULL)
  {
    fprintf(stderr, "%s: %s\n", argv[0], errmsg);
    exit(1);
  } 
  LogSNCL(station, network, channel, location);

  // Send some test messages to the log server
  for (i=1; i < 10; i++)
  {
    sprintf(msg, "Test message %d\r\n", i);
    if ((errmsg=log330Msg(msg, station, network, channel, location)) != NULL)
    {
      fprintf(stderr, "%s\n", errmsg);
      sleep(2);
    }
    else
      fprintf(stderr, "Sent msg: %s", msg);

    sleep(2);
  }
  return 0;
} // main
