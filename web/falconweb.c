/* Filename: falconweb.c
 *
 * Description:  Creates the web pages used to show falcon alarm status
 *
 * Modification History:
 *    Version 1.0  Nov 3, 2009
 *    Version 1.1  May 20, 2010  Display alarms newest to oldest
 *    Version 2.0  June 22, 2010  Add OFC data charts
 *    Version 2.1  Assume that events within 2 seconds of each other
 *                 are at the same time, prevents restore mixup
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include "../include/dcc_std.h"
#include "../include/dcc_time.h"

#define PROGRAM_NAME "falconweb"
#define VERSION "Version 2.1"
#define EFF_DATE "June 22, 2010"

#define MAX_FILE_NAME 4096

// Store list of ofc channels
struct s_ofc
{
  char     description[12];
  int      has_alarm;
  struct   s_ofc *next;
} S_OFC;

// Stores parsed alarm information
struct s_alarm
{
  char     station[8];
  STDTIME  timetag;
  char     description[12];
  int      channel;
  unsigned char event;
  unsigned char  level;
  struct s_alarm *next;
  struct s_alarm *nextstation;
  struct s_ofc   *ofclist;
} S_ALARM;

// Need a structure to reverse order of alarm messages
struct s_line
{
  char          *msg;
  struct s_line *next;
} S_LINE;

/////////////////////////////////////////////////////////////////////////////
// Turn an event string into a record
// Returns 1 on success, 0 on failure
int ParseEvent(const char *eventstr, struct s_alarm *event)
{
  struct s_alarm holdevent;
  int iArgCount;
  int year, month, dom, hour, minute, second;
  int channel;
  char description[80];
  char alarmtype[80];
  char direction[80];

  if (strlen(eventstr) < 55 || strlen(eventstr) > 80)
    return 0;

  memset(&holdevent, 0, sizeof(holdevent));

  iArgCount = sscanf(eventstr,
         "%s %d/%d/%d %d:%d:%d %s (%d): Alarm event %s %s",
         holdevent.station, &year, &month, &dom, &hour, &minute, &second,
         description, &channel, alarmtype, direction);
//fprintf(stderr, "DEBUG(%d): %s\n", iArgCount, eventstr);
  if (iArgCount != 11)
      return 0;

  holdevent.timetag = ST_CnvJulToSTD(ST_Julian(year, month, dom));
  holdevent.timetag.hour = hour;
  holdevent.timetag.minute = minute;
  holdevent.timetag.second = second;

  if (strlen(description) > 10)
  {
    fprintf(stderr, "Description %s exceeds 8 character limit\n",
            description);
    return 0;
  }
  strncpy(holdevent.description, description, 8);
  holdevent.description[8] = 0;

  holdevent.channel = channel;

  if (strcmp(alarmtype, "On") == 0)
  {
    holdevent.event = 0;
    holdevent.level = 3;
  }
  else if (strcmp(alarmtype, "High1") == 0)
  {
    holdevent.event = 1;
    holdevent.level = 2;
  }
  else if (strcmp(alarmtype, "Low1") == 0)
  {
    holdevent.event = 2;
    holdevent.level = 2;
  }
  else if (strcmp(alarmtype, "High2") == 0)
  {
    holdevent.event = 3;
    holdevent.level = 3;
  }
  else if (strcmp(alarmtype, "Low2") == 0)
  {
    holdevent.event = 4;
    holdevent.level = 3;
  }
  else if (strcmp(alarmtype, "Unknown") == 0)
  {
    holdevent.event = 5;
    holdevent.level = 2;
  }
  else
  {
    fprintf(stderr, "Unrecognized event type string '%s'\n", alarmtype);
    return 0;
  }

  if (strcmp(direction, "triggered") == 0)
  {
    holdevent.event = holdevent.event | 0x80;
  }
  else if (strcmp(direction, "restored") == 0)
  {
    if (holdevent.level <= 3 && holdevent.level > 1)
      holdevent.level--;
    if (holdevent.event == 0)
      holdevent.level = 1;
  }
  else
  {
    fprintf(stderr, "Unrecognized event direction '%s'\n", direction);
    return 0;
  }

  *event = holdevent;

//fprintf(stderr, "DEBUG parsed %s %s event %x  level %d\n '%s'",
//holdevent.station, holdevent.description,
//holdevent.event, holdevent.level, eventstr);

  return 1;
} // ParseEvent()

//////////////////////////////////////////////////////////////////////////////
// Adds s_alarm entry to a linked list of stations, no duplicates
void AddStation(struct s_alarm entry, struct s_alarm **stationlist)
{
  struct s_alarm *newalarm;
  char *errsave;
  newalarm = (struct s_alarm *) malloc(sizeof (struct s_alarm));
  if (newalarm == NULL)
  {
    errsave = strerror(errno);
    fprintf(stderr, "%s() line %d: malloc failed: '%s'\n",
       __FILE__, __LINE__, errsave);
    exit(1);
  }
  *newalarm = entry;

  newalarm->nextstation = *stationlist;
  *stationlist = newalarm;
} // AddStation()

//////////////////////////////////////////////////////////////////////////////
void ProcessOFC(FILE *cFile, struct s_alarm *stationlist,
                struct s_alarm *excludelist)
{
  struct s_ofc *ofcptr;
  struct s_ofc *newofc;
  struct s_alarm *excludeptr;
  int  found;
  char *errsave;
  char name[12];

  // loop through each channel name entry in cFile
  while (!feof(cFile))
  {
    if (fgets(name, 11, cFile) == NULL)
      break;

    if (name[strlen(name)-1] == '\n')
      name[strlen(name)-1] = 0;
    if (name[strlen(name)-1] == '\r')
      name[strlen(name)-1] = 0;
    if (name[strlen(name)-1] == '\n')
      name[strlen(name)-1] = 0;
    if (name[strlen(name)-1] == '\r')
      name[strlen(name)-1] = 0;

    if (strlen(name) < 1 || feof(cFile))
      continue;

    // We have some bad names with more than 6 chars so exclude them
    if (strlen(name) > 6)
    {
      fprintf(stderr, "DEBUG excluded OFC channel '%s', > 6 chars\n",
              name);
      continue;
    }

    // See if name is on the exclude.txt list
    for (found=0, excludeptr=excludelist; !found && excludeptr != NULL; 
         excludeptr = excludeptr->next)
    {
      if (strcmp(name, excludeptr->description) == 0)
      {
        fprintf(stderr, "DEBUG excluded OFC channel '%s' in exclude.txt\n",
              name);
        found = 1;
      }
    }
    if (found)
      continue;

    // Create a new entry for this name
    newofc = (struct s_ofc *) malloc(sizeof (struct s_ofc));
    if (newofc == NULL)
    {
      errsave = strerror(errno);
      fprintf(stderr, "%s() line %d: malloc failed: '%s'\n",
         __FILE__, __LINE__, errsave);
      exit(1);
    }
    strcpy(newofc->description, name);
    newofc->has_alarm = 0;
    newofc->next = NULL;

    if (stationlist->ofclist == NULL)
    {
      stationlist->ofclist = newofc;
    }
    else
    {
      // Add new channel name to end of list
      for (ofcptr = stationlist->ofclist; ofcptr->next != NULL; ofcptr = ofcptr->next)
        ; // find last entry in linked list
      ofcptr->next = newofc;
    }

  } // get list of channel names from cFile
  
} // ProcessOFC()

//////////////////////////////////////////////////////////////////////////////
void ProcessAlarm(struct s_alarm *alarmlist, struct s_alarm alarm, int days,
                  STDTIME today)
{
  struct s_alarm *alarmptr;
  DELTA_T        delta_time;

  int            alarmnum,ptrnum;
  int            alarmdir,ptrdir;
  int            alarmlevel,ptrlevel;
  int            found;
  static char    errstation[8] = {""};

/* Decided to only use tr1 buffer date range, this allows program to work if falcon clock is off
  // First see if this entry is within the last days boundry
  delta_time = ST_DiffTimes(today, alarm.timetag);
//fprintf(stderr, "Testing %s alarm age, delta %d vs max %d\n", alarm.station, delta_time.nday, days);
  if (delta_time.nday >= days)
    return;
*/

  alarmptr = alarmlist->next;

  // See if we can find an alarm entry that matches this one
  found = 0;
  while (alarmptr != NULL)
  {
    // See if description matches
    if (strcmp(alarmptr->description, alarm.description) == 0)
    {
      found = 1;
      alarmnum = alarm.event & 0x7f;
      alarmdir = alarm.event & 0x80;
      if (alarmnum == 0)
      {
        alarmlevel = (alarmdir?3:1);
      }
      else if (alarmnum == 1 || alarmnum == 2)
      {
        alarmlevel = (alarmdir?2:1);
      }
      else if (alarmnum == 3 || alarmnum == 4)
      {
        alarmlevel = (alarmdir?3:2);
      }
      else alarmlevel = 2;

      // See if timetags match
      delta_time = ST_DiffTimes(alarmptr->timetag, alarm.timetag);
      if (ST_DeltaToMS(delta_time) > 0)
      {
        if (strcmp(errstation, alarmptr->station) != 0)
        {
          fprintf(stderr, "%s Alarm entries are not in time sorted order, %s\n",
                  alarmptr->station, ST_PrintDate(alarm.timetag, 1));
          strcpy(errstation, alarmptr->station);
        }
        alarmptr = alarmptr->next;
        continue;
      }
      if ((ST_DeltaToMS(delta_time)/2000) == 0)
      {
        // Timetags match within 2 seconds so determine precedence
        // Need stored alarm info
        ptrnum = alarmptr->event & 0x7f;
        ptrdir = alarmptr->event & 0x80;
        if (ptrnum == 0)
        {
          ptrlevel = (ptrdir?3:1);
        }
        else if (ptrnum == 1 || ptrnum == 2)
        {
          ptrlevel = (ptrdir?2:1);
        }
        else if (ptrnum == 3 || ptrnum == 4)
        {
          ptrlevel = (ptrdir?3:2);
        }
        else ptrlevel = 3;

        // See if exiting alarm is the keeper
        if ((alarmdir && ptrlevel >= alarmlevel) ||
            (!alarmdir && ptrlevel <= alarmlevel) ||
            (ptrdir && !alarmdir))
        {
          // Try next entry
//fprintf(stderr, "Keeping alarm match times for %s %x vs %x\n",
//alarm.description, alarm.event, alarmptr->event);
          alarmptr = alarmptr->next;
          continue;
        }
//fprintf(stderr, "Replacing alarm match times for %s %x vs %x\n",
//alarm.description, alarm.event, alarmptr->event);
      } // simultaneous event changes

//fprintf(stderr, "Updating existing alarm for %s %x vs %x\n",
//alarm.description, alarm.event, alarmptr->event);

      alarmptr->event = alarm.event;
      alarmptr->level = alarmlevel;
      alarmptr->timetag = alarm.timetag;
    } // description matched

    // Try next entry
    alarmptr = alarmptr->next;
  } // loop through existing alarm list

  // See if this is a new description
  if (!found)
  {
    struct s_alarm *newalarm;
    char *errsave;
    // Need to add a new entry to the list
//fprintf(stderr, "Adding new alarm for %s %x\n",
//alarm.description, alarm.event);
    newalarm = (struct s_alarm *) malloc(sizeof (struct s_alarm));
    if (newalarm == NULL)
    {
      errsave = strerror(errno);
      fprintf(stderr, "%s() line %d: malloc failed: '%s'\n",
         __FILE__, __LINE__, errsave);
      exit(1);
    }
    *newalarm = alarm;

    newalarm->next = alarmlist->next;
    alarmlist->next = newalarm;
  } // adding a new alarm type
} // ProcessAlarm()


///////////////////////////////////////////////////////////////////////////////
void CreateMainFalconPage(
    FILE           *outfile,
    struct s_alarm *stationlist,
    const char     *topdir,
    const char     *createstr)
{
  struct s_alarm *alarmptr;
  struct s_alarm *eventptr;
  struct s_ofc   *ofcptr;
  struct s_line  *linelist;
  struct s_line  *lineptr;
  char fullname[MAX_FILE_NAME];
  char eventstr[160];
  char *class_str[2] = {"r0", "r1"};
  char code_str[160];
  char description_str[160];
  FILE           *aFile;
  int            iMix;
  int            iArg;
  int            iParse;


  fprintf(outfile,"%s",
"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
"    <head>\n"
"        <title>Falcon Health Status</title>\n"
"        <link rel=\"stylesheet\" type=\"text/css\" href=\"falcon.css\"/>\n"
"        <link rel=\"icon\" href=\"favicon.ico\" type=\"image/x-icon\"/>\n"
"        <link rel=\"shortcut icon\" href=\"favicon.ico\" type=\"image/x-icon\"/>\n"
"        <meta http-equiv=\"refresh\" content=\"900\"/>\n"
"        <script type=\"text/javascript\" src=\"falcon.js\"></script>\n"
"    </head>\n"
"    <body onload=\"initialize()\">\n"
);
  fprintf(outfile,
"        <h3 align='center'>Falcon Health Status  %s</h3>\n", createstr);
  fprintf(outfile,
"        <div class=\"main\">\n" );

  // Add list of stations to web page
  for (alarmptr = stationlist; alarmptr != NULL;
       alarmptr = alarmptr->nextstation)
  {
    fprintf(outfile,
"            <div class=\"level-%d\">\n", alarmptr->level-1);
    fprintf(outfile,
"                <div onclick=\"display('%s');\">%s</div>\n",
            alarmptr->station, alarmptr->station);

    for (eventptr=alarmptr->next; eventptr != NULL; eventptr = eventptr->next)
    {
      int found_match;
      // See if this alarm channel has timeseries data too
      for (found_match=0, ofcptr=alarmptr->ofclist; 
           ofcptr != NULL && !found_match; ofcptr = ofcptr->next)
      {
        if (strcmp(ofcptr->description, eventptr->description) == 0)
        {
          found_match=1;
          ofcptr->has_alarm=1;
        }
      }
      if (!found_match)
      {
        // No link to this label
        fprintf(outfile,
"                <span class=\"level-%d\">%s</span>\n",
              eventptr->level-1, eventptr->description);
      }
      else
      {
        // Label links to an ofcweb plot page
        fprintf(outfile,
"                <span class=\"level-%d\"><A HREF=\"ofcweb.html?station=%s&amp;channel=%s\">%s</A></span>\n",
              eventptr->level-1, alarmptr->station, 
              eventptr->description, eventptr->description);
      }
    } // loop through all alarms for this station

    // Make links for any remaing OFC channels
    for (ofcptr=alarmptr->ofclist; 
         ofcptr != NULL; ofcptr = ofcptr->next)
    {
      if (!ofcptr->has_alarm)
      {
        fprintf(outfile,
"                <span class=\"level-N\"><A HREF=\"ofcweb.html?station=%s&amp;channel=%s\">%s</A></span>\n",
              alarmptr->station, ofcptr->description, ofcptr->description);
      } // found one wich hasn't been listed by alarm loop
    } // loop through list of OFC channels

    // Done if no events or data channels for this station
    if (alarmptr->next != NULL || alarmptr->ofclist != NULL)
    {
      fprintf(outfile,
"                <div id='%s' class='alarms'>\n", alarmptr->station);

      // Now let the user see all of the events if they want
      sprintf(fullname, "%s/%s_alarms.txt", topdir, alarmptr->station);
      if ((aFile = fopen(fullname, "r")) == NULL)
      {
        fprintf(stderr, "Unable to open event file '%s'\n", fullname);
      } // failed to open event file for reading
      else
      {
        linelist = NULL;
        while (!feof(aFile))
        {
          fgets(eventstr, 80, aFile);
          if (strlen(eventstr) < 10 || feof(aFile))
            continue;
          if (eventstr[strlen(eventstr)-1] == '\n')
            eventstr[strlen(eventstr)-1] = 0;
          if (eventstr[strlen(eventstr)-1] == '\r')
            eventstr[strlen(eventstr)-1] = 0;
          if (eventstr[strlen(eventstr)-1] == '\n')
            eventstr[strlen(eventstr)-1] = 0;
          if (eventstr[strlen(eventstr)-1] == '\r')
            eventstr[strlen(eventstr)-1] = 0;

          lineptr = malloc(sizeof (struct s_line));
          lineptr->msg = malloc(strlen(eventstr)+1);
          strcpy(lineptr->msg, eventstr);
          lineptr->next = linelist;
          linelist = lineptr;
        } // read until end of file
        fclose(aFile);
      
        // Now print out the reversed lines
        while (linelist != NULL)
        {
          fprintf(outfile,
"                    <div>%s</div>\n", linelist->msg);
          lineptr = linelist;
          linelist = linelist->next;
          free(lineptr->msg);
          free(lineptr);
          lineptr = NULL;
        } // loop through all reversed lines
      } // Able to open file
      if (alarmptr->next == NULL)
      {
        fprintf(outfile,
"                    <div>%s No events</div>\n", alarmptr->station);
      }
      fprintf(outfile,
"                </div>\n");
    } // if this station had events
    else
    {
      fprintf(outfile,
"                <span class=\"level-E\">NONE</span>\n");
    }
      fprintf(outfile,
"            </div>\n");
  } // loop through all stations


  fprintf(outfile,"%s",
"        </div>\n"
         );

  // Now we need a legend section
  fprintf(outfile, "%s\n",
"        <table class=\"legend\">");
  fprintf(outfile, "%s\n",
"            <tr>");
  fprintf(outfile, "%s\n",
"                <th class=\"title\" colspan=\"2\">Legend</th>");
  fprintf(outfile, "%s\n",
"            </tr>");
  fprintf(outfile, "%s\n",
"            <tr>");
  fprintf(outfile, "%s\n",
"                <th>Code</th>");
  fprintf(outfile, "%s\n",
"                <th>Description</th>");
  fprintf(outfile, "%s\n",
"            </tr>");
  // Loop through each line in legends.txt
  sprintf(fullname, "%s/legend.txt", topdir);
  if ((aFile = fopen(fullname, "r")) == NULL)
  {
    fprintf(stderr, "Unable to open legend file '%s'\n", fullname);
  } // failed to open event file for reading
  else
  {
    iMix=0;
    while (!feof(aFile))
    {
      fgets(eventstr, 80, aFile);
      if (strlen(eventstr) < 10 || feof(aFile))
        continue;
      if ((iArg=sscanf(eventstr, "%s : %s\n", code_str, description_str)) != 2)
      {
fprintf(stderr, "DEBUG sscanf %s\n returned %d args,'%s' '%s'\n",
eventstr, iArg, code_str, description_str);
        continue;
      }
      for (iParse=strlen(code_str); iParse < strlen(eventstr) && eventstr[iParse] != ':'; iParse++)
       ; // find :
      for (iParse++; iParse < strlen(eventstr) &&
             (eventstr[iParse] == ' ' || eventstr[iParse] == '\t'); iParse++)
       ; // remove trailing spaces
      strcpy(description_str, &eventstr[iParse]);
      iParse=strlen(description_str) - 1;
      while(description_str[iParse] == '\n' ||
            description_str[iParse] == '\r')
      {
        description_str[iParse] = 0;
        iParse--;
      }
     
  fprintf(outfile, 
"            <tr class=\"%s\">\n", class_str[iMix++%2]);
  fprintf(outfile,
"                <td>%s</td>\n", code_str);
  fprintf(outfile,
"                <td>%s</td>\n", description_str);
  fprintf(outfile, "%s\n",
"            </tr>");
    }
    fclose(aFile);
  }
  fprintf(outfile, "%s\n",
"        </table>");

  fprintf(outfile,"%s",
"    </body>\n"
"</html>\n" );

  return;
} // CreateMainFalconPage()

///////////////////////////////////////////////////////////////////////////////
void CreateOfcPages(
    struct s_alarm *stationlist)
{
  struct s_alarm *alarmptr;
  struct s_ofc   *ofcptr;
  char           ofcfilename[MAX_FILE_NAME];
  FILE           *ofcfile;

  // Go through list of stations
  for (alarmptr = stationlist; alarmptr != NULL;
       alarmptr = alarmptr->nextstation)
  {
    // Go through all OFC channel data for this station
    for (ofcptr = alarmptr->ofclist; 
         ofcptr != NULL; ofcptr=ofcptr->next)
    {
      // create web page
      sprintf(ofcfilename, "%s_%s_ofcweb.html",
         alarmptr->station, ofcptr->description);
      if ((ofcfile = fopen(ofcfilename, "w")) == NULL)
      {
        fprintf(stderr, "availweb: failed to create %s, Aborting!\n",
           ofcfilename);
        exit(1);
      }

      // Headers
      fprintf(ofcfile,
"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
"  <head>\n"
"    <title>Falcon %s channel plot</title>\n"
"  </head>\n"
"  <body onload=\"initialize()\">\n", ofcptr->description);

      // Call up ofcweb java applet to do plot
      fprintf(ofcfile,
"    <applet code='ofcweb' archive=\"ofcweb.jar\" width=1000 height=350>\n");
      fprintf(ofcfile,
"      <PARAM NAME=ofcdata VALUE=\"%s_ofc.seed.hist\">\n", alarmptr->station);
      fprintf(ofcfile,
"      <PARAM NAME=channel VALUE=\"%s\">\n", ofcptr->description);
      fprintf(ofcfile,
"      <PARAM NAME=minutespertick VALUE=\"1440\">\n");
      fprintf(ofcfile,
"    </applet>\n");

      // End header
      fprintf(ofcfile,"%s",
"  </body>\n"
"</html>\n" );

      fclose(ofcfile);
    } // loop through each unique OFC channel description
  } // look at all stations

} // CreateOfcPages()

///////////////////////////////////////////////////////////////////////////////
void ShowUsage()
{
  printf("\n%s %s %s\n", PROGRAM_NAME, VERSION, EFF_DATE); 
  printf(
"Usage: %s -d <directory of web pages> -t <days back>\n",
         PROGRAM_NAME);
  printf("Where:\n");
  printf("\t-d      Head directory path where avail files start\n");
  printf("\t-t      How many days to look back for events (default 30)\n");
  exit(10);
} // ShowUsage()

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  extern char *optarg;
  extern int optind;
  extern int opterr;
  int        priorInd;
  int        daysBack = 30;
  int        lineno;
  int        iParse;
  int        sorted;

  char *savestr;
  char outfilename[MAX_FILE_NAME];
  char excludefilename[MAX_FILE_NAME];
  char createstr[MAX_FILE_NAME];
  char fullname[MAX_FILE_NAME];
  char eventstr[160];
  char channel[160];

  int     opt;
  struct s_alarm newalarm;
  struct s_alarm stationalarm;
  struct s_alarm *stationlist=NULL;
  struct s_alarm *alarmptr;
  struct s_alarm *eventptr;
  struct s_alarm *excludelist=NULL;
  struct s_alarm *excludeptr;
  STDTIME today;
  int     curYear_work;
  int     curDoy_work;

  char *topdir;
  FILE *outfile;
  FILE *excludefile;

  if (argc < 2)
  {
    ShowUsage();
    exit(1);
  }

  outfile = NULL;
  topdir = NULL;
  opterr=0;
  optind=1;
  priorInd=optind;
  daysBack = 30;

  struct dirent *direntry=NULL;
  DIR           *dirptr=NULL;
  FILE          *aFile=NULL;

  // Parse the command line arguments
  while ((opt = getopt(argc, argv, "d:t:")) != EOF)
  {
    switch (opt)
    {
      case 'd':
        // specify head directory where everything starts
        if (topdir != NULL)
        {
          fprintf(stderr, "-d head directory specified twice, aborting!\n");
          exit(1);
        }
        topdir = optarg;
//fprintf(stderr, "Opt '%c':, arg '%s', index %d\n", opt, optarg, optind);
        break;

      case 't':
        // Specify the number of days we want to go back
        daysBack = atoi(optarg);
        if (daysBack < 1)
        {
          fprintf(stderr, "-t <days back> option must be > 0, aborting!\n");
          exit(1);
        }
        break;
        
      default:
        fprintf(stderr, "Invalid option: '%c'\n", opt);
        ShowUsage();
        exit(1);
        break;
    } // switch

    // remember arg index before it gets changed
    priorInd=optind;
  } // while more command line options

  // Check for bogus option
  if (optind < argc && argv[optind][0] != '-')
  {
    fprintf(stderr, "Unrecognized argument %d\n", optind);
    exit(1);
  }

  // Verify that all arguments were provided
  if (topdir == NULL)
  {
    fprintf(stderr, "Missing -d directory argument!\n");
    exit(1);
  }

  // read list of channel names to exclude from exclude.txt
  sprintf(excludefilename, "%s/exclude.txt", topdir);
  if ((excludefile = fopen(excludefilename, "r")) != NULL)
  {
fprintf(stderr, "DEBUG opened %s\n", excludefilename);
    lineno=0;
    while (!feof(excludefile))
    {
      fgets(eventstr, 80, excludefile);
      if (feof(excludefile))
        continue;
      lineno++;

      if (eventstr[0] == '#')
        continue;
      channel[0] = 0;
      sscanf(eventstr, "%s", channel);
      if (strlen(channel) > 0 && strlen(channel) <= 8)
      {
        alarmptr = (struct s_alarm *) malloc(sizeof (struct s_alarm));
        if (alarmptr == NULL)
        {
          char *errsave;
          errsave = strerror(errno);
          fprintf(stderr, "%s() line %d: malloc failed: '%s'\n",
             __FILE__, __LINE__, errsave);
          exit(1);
        }
        strcpy(alarmptr->description, channel);
fprintf(stderr, "DEBUG added exclude channel '%s'\n", channel);
        alarmptr->next = excludelist;
        excludelist = alarmptr;
      } // found a channel name to exclude
    } // read all lines in exclude.txt
    fclose(excludefile);
  } // found exclude.txt

  // Get working year, month, day of year, julian day
  today = ST_GetCurrentTime();
  curYear_work = today.year;
  curDoy_work = today.day;

  // Creation date string
  sprintf(createstr, "%d,%03d %02d:%02d UTC", curYear_work, curDoy_work,
          today.hour, today.minute);

  // Parse each event file in topdir
  dirptr = opendir(topdir);
  if (dirptr == NULL)
  {
    savestr = strerror(errno);
    fprintf(stderr, "%s() line %d: Unable to open data directory: '%s'\n",
       __FILE__, __LINE__, topdir);
    fprintf(stderr, "  opendir failed, errno=%d, '%s'\n", errno, savestr);
    exit(1);
  } // troubles opening data file directory

  // Loop through each event file <station>_alarms.txt
  do
  {
    direntry = readdir(dirptr);
    if (direntry != NULL)
    {
       char station[8];
       memset(station, 0, 8);
       if (strlen(direntry->d_name) > strlen("XXXXX_alarms.txt"))
         continue;
       if ((strcmp("_alarms.txt",
           &direntry->d_name[strlen(direntry->d_name)-11]) == 0) &&
            (strlen(direntry->d_name) > 11))
       {
         strncpy(station, direntry->d_name, strlen(direntry->d_name)-11);
//fprintf(stderr, "Found file '%s' station %s\n", direntry->d_name, station);
         sprintf(fullname, "%s/%s", topdir, direntry->d_name);
         if ((aFile = fopen(fullname, "r")) == NULL)
         {
           fprintf(stderr, "Unable to open event file '%s'\n", fullname);
           continue;
         } // failed to open event file for reading
         station[5] = 0;

         // Add this to list of stations
         memset(&stationalarm, 0, sizeof(stationalarm));
         strcpy(stationalarm.station, station);
         stationalarm.level = 0;
         AddStation(stationalarm, &stationlist);

         // Parse through all of the lines in the file
         lineno = 0;
         while (!feof(aFile))
         {
            fgets(eventstr, 80, aFile);
            if (feof(aFile))
              continue;
            lineno++;
            if (!(iParse=ParseEvent(eventstr, &newalarm)) &&
                 strlen(eventstr) > 1 && eventstr[0] != '#')
            {
              fprintf(stderr, "Failed to parse line %d in %s\n'%s'\n",
                   lineno, direntry->d_name, eventstr);
            }

            // see if we should exclude this alarm
            for (excludeptr=excludelist; iParse && excludeptr != NULL; 
                 excludeptr = excludeptr->next)
            {
              if (strcmp(newalarm.description, excludeptr->description) == 0)
              {
fprintf(stderr, "DEBUG exclude alarm for channel %s '%s'\n", 
newalarm.station, excludeptr->description);
                iParse = 0;
              }
            }

            if (iParse)
            {
              // Incorporate this new alarm
              ProcessAlarm(stationlist, newalarm, daysBack, today);
            } // parsed line
         } // read until end of file
         fclose(aFile);

         // get list of channel descriptors in OFC files
         sprintf(fullname, "%s/%s_chan.txt", topdir, station);
         if ((aFile = fopen(fullname, "r")) == NULL)
         {
           fprintf(stderr, "Unable to channnel data file '%s'\n", fullname);
           continue;
         } // failed to open event file for reading
         ProcessOFC(aFile, stationlist, excludelist);

       } // file name matches correct format
    } // found a file in the directory
  } while (direntry != NULL);

  // Done going through files, close directory
  closedir(dirptr);
  dirptr = NULL;

  // Find alert level for each station
  for (alarmptr = stationlist; alarmptr != NULL;
       alarmptr = alarmptr->nextstation)
  {
    int templevel = 1;
    for (eventptr=alarmptr->next; eventptr != NULL; eventptr = eventptr->next)
    {
      if (eventptr->level > templevel)
        templevel = eventptr->level;
    } // loop through all alarms for this station
    alarmptr->level = templevel;
  } // loop through all stations

  // Sort list by station level, then station name
  sorted = 0;
  while (!sorted)
  {
     struct s_alarm *previous;
     sorted = 1;
     previous = NULL;
     for (alarmptr = stationlist; alarmptr != NULL && sorted;
          previous = alarmptr, alarmptr = alarmptr->nextstation)
     {
       if (alarmptr->nextstation == NULL)
         continue;

       // Check station alarm level
       if ( (alarmptr->level < alarmptr->nextstation->level) ||
           ((alarmptr->level == alarmptr->nextstation->level)
            && strcmp(alarmptr->station, alarmptr->nextstation->station) > 0))
       {
         struct s_alarm *ptr;
         sorted = 0;
         ptr = alarmptr->nextstation;
         alarmptr->nextstation = alarmptr->nextstation->nextstation;
         ptr->nextstation = alarmptr;
         if (alarmptr == stationlist)
         {
           stationlist = ptr;
         }
         else
         {
           previous->nextstation = ptr;
         }
       } // stations needed to be switched
     } // loop through all stations
  } // continue until no changes are made

  // create web page
  sprintf(outfilename, "%s/falcon.html", topdir);
  if ((outfile = fopen(outfilename, "w")) == NULL)
  {
    fprintf(stderr, "availweb: failed to create %s, Aborting!\n",
       outfilename);
    exit(1);
  }

  CreateMainFalconPage(outfile, stationlist, topdir, createstr);
  fclose(outfile);

  // Create OFC channel web pages
// seperate OFC channel web pages are no longer needed
//  CreateOfcPages(stationlist);

  exit(0);
} // main()

