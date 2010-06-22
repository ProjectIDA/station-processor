/*
  Creates web page for presenting station falcon alarm status
 */
const char *VersionIdentString = "Release 1.0";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <dcc_std.h>
#include <dcc_time.h>

#include "traversetr1.h"

#define MAX_FILE_NAME 4096
char alarmfilename[MAX_FILE_NAME];
char ofcfilename[MAX_FILE_NAME];
char ofafilename[MAX_FILE_NAME];
char ofa_name[MAX_FILE_NAME];
char ofc_name[MAX_FILE_NAME];
char cmdstr[MAX_FILE_NAME];

// Global debug flag
int  iDebug = 0;

extern int errno;

struct s_list
{
  char *name;
  struct s_list *next;
} S_LIST;

void ShowUsage()
{
  fprintf(stderr,
"Usage: falconalarm [-network <network>] [-station <network_station>]\n"
"   [-xstation <network_station> [-days <count>] -dir <directory>\n"
"   [-debug]\n");
  fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

int main(argc,argv)
char *argv[];
int argc;
{
  char *network_station;
  char *seed_name;
  char *seed_dir;
  char *errsave;
  char *dirname=NULL;
  int bFirstDir = 1;
  int bFirstDay = 1;
  int bFoundFalcon = 0;
  int iParseError=0;
  int iParseArg=1;
  int iDays = 30;

  char network[4];
  char station[8];

  struct s_list  *networklist=NULL;
  struct s_list  *stationlist=NULL;
  struct s_list  *stationexcludelist=NULL;
  struct s_list  *newentry=NULL;
  struct s_list  *listptr;

  STDTIME today;
  STDTIME firstday;
  STDTIME workday;
  STDTIME lastOFC;

  WORD out_year, out_mon, out_day, outjday;
  FILE *alarmfile;
  FILE *ofcfile;
  FILE *ofafile;

  if (argc < 2)
    iParseError = 1;
  while(iParseArg < argc && !iParseError)
  {
    if (newentry == NULL)
    {
      newentry = (struct s_list *) malloc(sizeof (struct s_list));
      if (newentry == NULL)
      {
        errsave = strerror(errno);
        fprintf(stderr, "%s() line %d: malloc failed: '%s'\n",
           __FILE__, __LINE__, errsave);
        exit(1);
      }
      memset(newentry, 0, sizeof(struct s_list));
    } // needed another free list entry allocated

    if (strcmp(argv[iParseArg], "-debug") == 0)
    {
      iDebug = 1;
      iParseArg++;
    }
    else if (strcmp(argv[iParseArg], "-days") == 0)
    {
      if (iParseArg+1 == argc)
      {
        iParseError = 1;
        fprintf(stderr, "Missing count argument after -days'\n");
        iParseArg++;
        continue;
      }
      if (argv[iParseArg+1][0] == '-')
      {
        iParseError = 1;
        fprintf(stderr, "Missing count argument after -days'\n");
        iParseArg++;
        continue;
      }
      iDays = atoi(argv[iParseArg+1]);
      if (iDays < 1)
      {
        iParseError = 1;
        fprintf(stderr, "Invalid -days count '%s' argument\n",
          argv[iParseArg+1]);
        iParseArg += 2;
        continue;
      }
      iParseArg += 2;
    } // -days parsed
    else if (strcmp(argv[iParseArg], "-dir") == 0)
    {
      if (iParseArg+1 == argc)
      {
        iParseError = 1;
        fprintf(stderr, "Missing directory argument after -dir'\n");
        iParseArg++;
        continue;
      }
      if (argv[iParseArg+1][0] == '-')
      {
        iParseError = 1;
        fprintf(stderr, "Missing directory argument after -dir'\n");
        iParseArg++;
        continue;
      }
      dirname = argv[iParseArg+1];
      iParseArg += 2;
    } // -dir parsed
    else if (strcmp(argv[iParseArg], "-network") == 0)
    {
      if (iParseArg+1 == argc)
      {
        iParseError = 1;
        fprintf(stderr, "Missing network argument after -network'\n");
        iParseArg++;
        continue;
      }
      if (argv[iParseArg+1][0] == '-')
      {
        iParseError = 1;
        fprintf(stderr, "Missing network argument after -network'\n");
        iParseArg++;
        continue;
      }
      if (strlen(argv[iParseArg+1]) > 2)
      {
        iParseError = 1;
        fprintf(stderr, "-network argument limited to 2 characters\n");
        iParseArg++;
        continue;
      }
      newentry->name = argv[iParseArg+1];
      newentry->next = networklist;
      networklist = newentry;
      newentry = NULL;
      iParseArg += 2;
    } // -network parsed
    else if (strcmp(argv[iParseArg], "-station") == 0)
    {
      if (iParseArg+1 == argc)
      {
        iParseError = 1;
        fprintf(stderr, "Missing station name after -station'\n");
        iParseArg++;
        continue;
      }
      if (argv[iParseArg+1][0] == '-')
      {
        iParseError = 1;
        fprintf(stderr, "Missing station name after -station'\n");
        iParseArg++;
        continue;
      }
      if (strlen(argv[iParseArg+1]) > 8 || strlen(argv[iParseArg+1]) < 3)
      {
        iParseError = 1;
        fprintf(stderr, "-station network_station '%s' is invalid\n",
           argv[iParseArg+1]);
        iParseArg++;
        continue;
      }
      newentry->name = argv[iParseArg+1];
      newentry->next = stationlist;
      stationlist = newentry;
      newentry = NULL;
      iParseArg += 2;
    } // -station parsed
    else if (strcmp(argv[iParseArg], "-xstation") == 0)
    {
      if (iParseArg+1 == argc)
      {
        iParseError = 1;
        fprintf(stderr, "Missing station name after -xstation'\n");
        iParseArg++;
        continue;
      }
      if (argv[iParseArg+1][0] == '-')
      {
        iParseError = 1;
        fprintf(stderr, "Missing station name after -xstation'\n");
        iParseArg++;
        continue;
      }
      if (strlen(argv[iParseArg+1]) > 8 || strlen(argv[iParseArg+1]) < 3)
      {
        iParseError = 1;
        fprintf(stderr, "-xstation network_station '%s' is invalid\n",
           argv[iParseArg+1]);
        iParseArg++;
        continue;
      }
      newentry->name = argv[iParseArg+1];
      newentry->next = stationexcludelist;
      stationexcludelist = newentry;
      newentry = NULL;
      iParseArg += 2;
    } // -xstation parsed
    else
    {
      iParseError = 1;
      fprintf(stderr, "Unrecognized argument %d '%s'\n",
         iParseArg, argv[iParseArg]);
      iParseArg++;
    } // unknown argument
  } // go through all of the command line arguments

  if (newentry != NULL)
  {
    free(newentry);
    newentry = NULL;
  }

  if (dirname == NULL)
  {
    fprintf(stderr, "-dir argument missing\n");
    iParseError = 1;
  }

  if (iParseError)
  {
    ShowUsage();
    exit(100);
  }

  // Find out how far back to search for data
  today = ST_GetCurrentTime();
  firstday = ST_AddToTime(today, -iDays, 0, 0, 0, 0);

  // Now find each station that matches our list
  bFirstDir=1;
  while ((network_station = NextTr1Station(bFirstDir)) != NULL)
  {
    char *charptr;
    int bFound=0;
    bFirstDir=0;

    // Parse network name and station name
    charptr = &network_station[3];
    strncpy(network, network_station, 2);
    network[2] = 0;
    if (network[1] == '_')
    {
      network[1] = 0;
      charptr--;
    }
    if (*(charptr-1) != '_')
      continue;

    if (strlen(charptr) > 5)
      continue;

    strcpy(station, charptr);

    // See if network is on our included list
    for (bFound=0,listptr=networklist; listptr!= NULL && !bFound;
         listptr=listptr->next)
    {
      bFound = (strcmp(network, listptr->name) == 0);
    }

    // See if station is on our included list
    for (listptr=stationlist; listptr!= NULL && !bFound;
         listptr=listptr->next)
    {
      bFound = (strcmp(network_station, listptr->name) == 0);
    }

    // See if station is on our excluded list
    for (listptr=stationexcludelist; listptr!= NULL && bFound;
         listptr=listptr->next)
    {
      if (strcmp(network_station, listptr->name) == 0)
        bFound = 0;
    }

    if (!bFound)
      continue;

    if (iDebug)
    {
//      fprintf(stderr, "DEBUG checking %s\n", network_station);
    }

    // Loop through each day for this station
    workday = firstday;
    lastOFC = ST_AddToTime(firstday, -2, 0, 0, 0, 0);
    bFirstDay = 1;
    bFoundFalcon = 0;
    sprintf(alarmfilename, "%s/%s_alarms.txt", dirname, station);
    sprintf(ofcfilename, "%s/%s_ofc.seed", dirname, station);
    sprintf(ofafilename, "%s/%s_ofa.seed", dirname, station);
    for (workday = firstday; ST_DiffTimes(today, workday).nday >= 0;
             workday = ST_AddToTime(workday, 1, 0, 0, 0, 0))
    {
//fprintf(stderr, "Checking %s %d,%d\n", station, workday.year, workday.day);
      bFirstDay = 1;
      seed_name = NextTr1StationYearDay(bFirstDay, network_station,
                                        workday.year, workday.day,
                                        &seed_dir);
      bFirstDay = 0;

      while (seed_name != NULL)
      {
        // See if this is a data file
        if (strcmp(&seed_name[3], "OFC.512.seed") == 0)
        {
          if (iDebug)
          {
            fprintf(stderr, "DEBUG Found data file for %s %s %d,%d\n",
                 network_station, seed_name, workday.year,workday.day);
          }
          lastOFC = workday;
          if (!bFoundFalcon)
          {
            // Create an empty event file to show that falcon does exist
            bFoundFalcon = 1;
            alarmfile = fopen(alarmfilename, "w");
            fclose(alarmfile);
            ofafile = fopen(ofafilename, "w");
            fclose(ofafile);
            ofcfile = fopen(ofcfilename, "w");
            fclose(ofcfile);
            if (iDebug)
            {
              fprintf(stderr,
                      "DEBUG Found first OFC file, creating empty %s\n",
                      alarmfilename);
            }
          }
          // Need to append new OFC records to the <station>_OFC.seed file
          sprintf(ofc_name, "%s/%s", seed_dir, seed_name);
          sprintf(cmdstr, "cat %s >> %s", ofc_name, ofcfilename);
//fprintf(stderr, "DEBUG appending OFC seed file: \n'%s'\n", cmdstr);
          system(cmdstr);
        } // found an OFC data file

        // See if this is an alarm file
        if (strcmp(&seed_name[3], "OFA.512.seed") == 0)
        {
//        printf("DEBUG Found alarm file for %s %s %d,%d\n",
//             network_station, seed_name, workday.year,workday.day);
          if (!bFoundFalcon)
          {
            if (iDebug)
            {
              fprintf(stderr, 
"DEBUG Found first OFA file, creating empty %s\n", alarmfilename);
            }
            bFoundFalcon = 1;
            alarmfile = fopen(alarmfilename, "w");
            fclose(alarmfile);
            ofcfile = fopen(ofcfilename, "w");
            fclose(ofcfile);
            ofafile = fopen(ofafilename, "w");
            fclose(ofafile);
          }

          // Need to append new alarm messages to the <station>_alarms.txt file
          sprintf(ofa_name, "%s/%s", seed_dir, seed_name);
          sprintf(cmdstr, "ofadump -f %s >> %s", ofa_name, alarmfilename);
//fprintf(stderr, "DEBUG appending alarm text file: \n'%s'\n", cmdstr);
          system(cmdstr);

          // Need to append new OFA records to the <station>_OFA.seed file
          sprintf(ofa_name, "%s/%s", seed_dir, seed_name);
          sprintf(cmdstr, "cat %s >> %s", ofa_name, ofafilename);
          if (iDebug)
          {
            fprintf(stderr, "DEBUG appending OFA seed file: \n'%s'\n", cmdstr);
          }
          system(cmdstr);
        }

        seed_name = NextTr1StationYearDay(bFirstDay, network_station,
                                        workday.year, workday.day,
                                        &seed_dir);
      } // loop through all files for this day

    } // loop through each day of interest
   
    // See if we didn't get any OFC files the last two days
    if (ST_DiffTimes(today, lastOFC).nday > 1 && bFoundFalcon)
    {
      // Generate a custom alarm for no Falcon data
      if (ST_DiffTimes(lastOFC, firstday).nday < 0)
      {
        if (iDebug)
          fprintf(stderr, "DEBUG: Setting lastOFC to first day + 1\n");
        lastOFC = ST_AddToTime(firstday, 1, 0, 0, 0, 0);
      }
      ST_CnvJulToCal(ST_GetJulian(lastOFC), 
                     &out_year, &out_mon, &out_day, &outjday);
      sprintf(cmdstr, "echo '%-5.5s %04d/%02d/%02d 00:00:00 OFC (-1): Alarm event On triggered' >> %s",
             station, out_year, out_mon, out_day,
             alarmfilename);
      system(cmdstr);
      if (iDebug)
      {
        fprintf(stderr, "Generating missing OFC file alarm message.\n%s\n",
                cmdstr);
      }
    } // Check for no OFC data lately
    
    // Done with station, create list of channel names in OFC file
    if (bFoundFalcon)
    {
      sprintf(ofc_name, "%s/%s_chan.txt", dirname, station);
      sprintf(cmdstr, "ofadump -c %s > %s", ofcfilename, ofc_name);
      if (iDebug)
        fprintf(stderr, "DEBUG %s\n", cmdstr);
      system(cmdstr);
    }
  } // check each /tr1/station directory entry

  return 0;
} // main()

