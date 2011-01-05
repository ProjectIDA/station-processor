/*
  Traverses the tr1 buffer directory looking for files matching criteria
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "traversetr1.h"

#define MAX_FILE_NAME 4096

const char *tr1dir = "/tr1/telemetry_days/";
char dirname[MAX_FILE_NAME];

//////////////////////////////////////////////////////////////////////////////
// NextTr1Station()
// int bFirst  True if you want to start at the beginning
// returns next <network_station> directory string from /tr1/telemetry_days/
//////////////////////////////////////////////////////////////////////////////
static struct dirent *nextdir=NULL;
static DIR  *dirptr = NULL;
char *NextTr1Station(int bFirst)
{
  char *savestr;

  if (bFirst && dirptr != NULL)
  {
    closedir(dirptr);
    dirptr = NULL;
  }

  // open station directory if needed
  if (dirptr == NULL && bFirst)
  {
    dirptr = opendir(tr1dir);
    if (dirptr == NULL)
    {
      savestr = strerror(errno);
      fprintf(stderr, "%s() line %d: Unable to open data directory: '%s'\n",
         __FILE__, __LINE__, tr1dir);
      fprintf(stderr, "  opendir failed, errno=%d, '%s'\n", errno, savestr);
      return NULL;
    } // error opening directory
  } // Open directory for the first time

  if (dirptr == NULL)
    return NULL;

  // Get entry from directory
  nextdir=readdir(dirptr);
  if (nextdir == NULL)
  {
    closedir(dirptr);
    dirptr = NULL;
    return NULL;
  }

  return nextdir->d_name;

  return NULL;
} // NextTr1Station()

//////////////////////////////////////////////////////////////////////////////
// NextTr1StationYear()
// int bFirst  True if you want to start at the beginning
// returns next <network_station> directory string from /tr1/telemetry_days/
//////////////////////////////////////////////////////////////////////////////
static struct dirent *nextdaydir=NULL;
static DIR  *daydirptr = NULL;
char *NextTr1StationYearDay(
  int bFirst,                    // True if first time opening this directory
  const char *network_station,   // Station name
  int year,                      // Year to look at
  int doy,                       // Day of year to look at
  char **seed_dir                // returns full path name of directory
  ) // Return pointer to the name of the next file in the directory
{
  *seed_dir = dirname;

  if (bFirst && daydirptr != NULL)
  {
    closedir(daydirptr);
    daydirptr = NULL;
  }

  // open station directory if needed
  if (daydirptr == NULL && bFirst)
  {
    sprintf(dirname, "%s%s/%d/%d_%03d/",
            tr1dir, network_station, year, year,doy);
    daydirptr = opendir(dirname);
    if (daydirptr == NULL)
    {
        // Failure to open is not uncommon since it may not exist
//      char *savestr;
//      savestr = strerror(errno);
//      fprintf(stderr, "%s() line %d: Unable to open data directory: '%s'\n",
//         __FILE__, __LINE__, dirname);
//      fprintf(stderr, "  opendir failed, errno=%d, '%s'\n", errno, savestr);
        return NULL;
    } // error opening directory
  } // Open directory for the first time

  if (daydirptr == NULL)
    return NULL;

  // Get entry from directory
  nextdaydir=readdir(daydirptr);
  if (nextdaydir == NULL)
  {
    closedir(daydirptr);
    daydirptr = NULL;
    return NULL;
  }

  return nextdaydir->d_name;

  return NULL;
} // NextTr1StationYear()
