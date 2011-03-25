//////////////////////////////////////////////////////////////////////////////
//
// ufors
// Collects data from uFORS1 Fiber Optic Rate Sensor and
// archives it in miniseed format for transmission to data collection center
//
// uFors-1 sensor is preset to send data at 640 Hz
// 38400 baud, 8 data bits, no stop bits, no parity
// 5 bytes/data record
//    3 data bytes giving 24 bit signed angular rate, units 2.344*10-6 deg/sec
//    1 status byte
//       Bit 7 = Unknown command
//       Bit 6 = Measurement Range Exceeded
//       Bit 5 = Hardware BIT error
//       Bit 4 = Auxiliary Control Loop Error
//       Bit 3 = Temperature warning
//       Bit 2 = Not used
//       Bit 1 = Reset Acknowledge
//       Bit 0 = NOGO - OR of bits 4 and 5
//    1 checksum byte (sum of all signed 5 bytes should equal 0xff)
//////////////////////////////////////////////////////////////////////////////

// External routines
int openraw(const char *name);

#define MAX_FILE_NAME 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <syslog.h>
#include <time.h>
#include <arpa/inet.h>

#include "libmseed.h"
#include "diskloop.h"
#include "q330arch.h"
#include "netreq.h"
#include "dcc_time_proto2.h"

// Global debug flag
int gDebug=0;

#define TIMEARRAYSIZE 3
#define DATAHZ 640
#define DECAY_RATE 500
#define ROTATION_UNIT 0.000002344

// Variables to keep running average of data rate and record start time
// DECAY_RATE determines the time constant for the running rate average
// measureHZ is the current running average
// measureTime is the timetag for the next seed record
// iMeasureCount is how many samples we've buffered up ready to be sent
// iRunCount is the number of continous seed records we've generated
// iFixCount is the iRunCount we last subtracted ticks from a record
// utcWhen stores shortest record start timetags based upon new data
//         extrapolated back iMeasureCount * measureHZ in time
// iTimeBufSize says how many values are stored in utcWhen
// sample stores the last iMeasureCount sample values
// bNewRun means we need to start timetags etc from scratch
double measureHZ = DATAHZ;
double defaultHZ = DATAHZ;
struct timeval measureTime={0};
long   iMeasureCount=0;
long   iRunCount=0;
long   iFixCount=0;
int    bNewRun=1;  
struct timeval  utcWhen[TIMEARRAYSIZE];
int  iTimeBufSize=0;   // Says how many values have been added to utcWhen
long sample[8192*8];            // Can't pack lower than 1 bit/value

char channel[4];
char location[4];

//////////////////////////////////////////////////////////////////////////////
// ShowUsage()
ShowUsage()
{
  fprintf(stderr,
" Usage: ufors <configuration file> <location/channel> <serial port>\n"
"              [-hz=<samplerate>] [-debug] [-h]\n"
"   <configuration file>  Name of diskloop.config file\n"
"   <location/channel>    How to name the ufors data\n"
"   <serial port>         Unix device name for port connected to unit\n"
" Options:\n"
"   -hz=<samplerate>      Defaults to 640.0 Hz\n"
"   -h                    Prints this help text\n"
"   -debug                Prints additional debugging information\n"
  );

} // ShowUsage()

//////////////////////////////////////////////////////////////////////////////
// record_handler:
// Saves passed miniseed records to the output file.
// Gets called whenever we have a full miniseed records worth of data
static FILE *msfile;
static void
record_handler (char *record, int reclen, void *ptr)
{
  static int iCount=0;
  static int iNudge=0;
  seed_header *seedhdr;
  int iPackCount;
  int i;
  int iTime;
  double calcDelta;
  suseconds_t delta_usec;
  STDTIME2  newtime;

  seedhdr = (seed_header *)record;
  iPackCount = ntohs(seedhdr->samples_in_record);

  // See if smoothed time is drifting from best time by more than .2 millisec
  calcDelta = (utcWhen[1].tv_sec - measureTime.tv_sec)
            + (utcWhen[1].tv_usec - measureTime.tv_usec)/1000000.0;
  if (calcDelta > 0.0001)
  {
    iNudge++;
    if (iNudge > 100)
    {
      // Nudge start time up .1 milliseconds
      // Put new timetag in seed record structure 
      measureTime.tv_usec += 100;
      if (measureTime.tv_usec > 999999)
      {
        measureTime.tv_sec += measureTime.tv_usec/1000000;
        measureTime.tv_usec = measureTime.tv_usec % 1000000;
      }
      if (gDebug)
        fprintf(stdout, 
             "%2.2s %-5.5s %2.2s/%3.3s Adjust clock %d ticks, Rate %.5lf Hz\n",
             seedhdr->seednet, seedhdr->station_id_call_letters,
             seedhdr->location_id, seedhdr->channel_id,
             1, measureHZ);
//    else
//      syslog(LOG_INFO, 
//           "%2.2s %-5.5s %2.2s/%3.3s Adjust clock %d ticks, Rate %.5lf Hz\n",
//           seedhdr->seednet, seedhdr->station_id_call_letters,
//           seedhdr->location_id, seedhdr->channel_id,
//           1, measureHZ);
    } // We've seen enough nudge requests in a row to be convinced
  } // decided to nudge record start time forward 1 tick
  else
  {
    iNudge = 0;
  }

  if (iCount++ % 1200 == 0)
  {
      syslog(LOG_INFO, "%2.2s %5.5s %2.2s/%3.3s Rate %.5lf Hz, drift %.6lf\n",
             seedhdr->seednet, seedhdr->station_id_call_letters,
             seedhdr->location_id, seedhdr->channel_id,
             measureHZ, calcDelta);
  }

  // Store the start time for current record
  memset(&newtime, 0, sizeof(newtime));
  newtime.year = 1970;
  newtime.day = 1;
  newtime = ST_AddToTime2(newtime, 0, 0, 0,
                measureTime.tv_sec, measureTime.tv_usec/100);
  seedhdr->yr = htons(newtime.year);
  seedhdr->jday = htons(newtime.day);
  seedhdr->hr = newtime.hour;
  seedhdr->minute = newtime.minute;
  seedhdr->seconds = newtime.second;
  seedhdr->tenth_millisec = htons(newtime.tenth_msec);

  // Reset record start timetag for new data
  for (i=0; i < iMeasureCount - iPackCount; i++)
    sample[i] = sample[i + iPackCount];
  iMeasureCount -= iPackCount;
  calcDelta = iPackCount / measureHZ;
  delta_usec = calcDelta * 1000000;
  delta_usec += measureTime.tv_usec;
  measureTime.tv_usec = delta_usec % 1000000;
  measureTime.tv_sec += (delta_usec / 1000000);
  iTimeBufSize = 1;
  utcWhen[0] = measureTime;

  // Send seed record to q330arch for archival and data transmission
  // q330arch will merge data from all data loggers into a single stream
  char *retstr;
  while ((retstr = q330SeedSend((void *)record)) != NULL)
  {
    if (gDebug)
      fprintf(stderr, "%s line %d q330SeedSend: %s %s\n", 
             __FILE__, __LINE__, channel, retstr);
    else
      syslog(LOG_ERR, "%s line %d q330SeedSend: %s %s\n",
            __FILE__, __LINE__, channel, retstr);
    sleep(1);
  }
}  /* End of record_handler() */

//////////////////////////////////////////////////////////////////////////////
// Collect data in endless loop from uFors1 sensors
//
void collectData(
  const char *location,
  const char *channel,
  const char *portname
  )
{
  int filedes;
  int readcnt;
  int rate;
  int synccount;
  int iTime;
  int i;
  double delta_sec;
  suseconds_t delta_usec;
  STDTIME2  newtime;
  struct timeval now;
  static struct timeval last={0};
  struct timeval timetag = {0};

  char cmdstr[4096];
  char buffer[5*DATAHZ*100+4];

  char *save_errstr;
  MSRecord msr;
  MSTraceGroup *mstg = 0;
  MSTrace *mst;

  char seedrec[8192];             // max single seed record size
  int packreclen = 512;           // Use 512 byte miniseed records
  int packencoding=DE_STEIM2;     // Use STEIM2 miniseed compression
  int byteorder=1;                // Use big endian byte order
  int packedrecords;
  int packedsamples;
  int verbose = 0;
  int firstone = 1;

  char log_station[8];
  char log_network[4];
  char log_channel[4];
  char log_location[4];

  // Open portname for reading raw data
  if ((filedes = openraw(portname)) < 0)
  {
    sprintf(cmdstr, "Failed to open %s for raw data reads, ABORT",
              portname);
    if (gDebug)
      fprintf(stderr, "%s\n", cmdstr);
    else
      syslog(LOG_ERR, cmdstr);
    exit(1);
  }

  // Create initial miniseed header
  LogSNCL(log_station, log_network, log_channel, log_location);

  memset(&msr, 0, sizeof(msr));
  memset(seedrec, 0, sizeof(seedrec));
  packreclen = 512;
  msr.reclen = packreclen;
  msr.record = seedrec;
  msr.sequence_number = 1;
  strncpy(msr.station, log_station, 5);
  strncpy(msr.location, location, 2);
  strncpy(msr.channel, channel, 3);
  strncpy(msr.network, log_network, 2);
  msr.dataquality='D';
  memset(&msr.starttime, 0, sizeof(msr.starttime));
  msr.samprate = 640.0;
  msr.encoding = packencoding;
  msr.byteorder = byteorder;
  msr.datasamples = &sample;
  msr.numsamples = 1;
  msr.sampletype = 'i';

  // Infinite loop reading data from serial port
  synccount = 0;
  readcnt = 0;
  iMeasureCount = 0;
  firstone = 1;
  while (1)
  {
    // read a 5 byte data block
    readcnt = 0;
    while (readcnt < 5)
    {
      readcnt += read(filedes, &buffer[readcnt], 5-readcnt);
      gettimeofday(&now, NULL);
      if (firstone && readcnt > 0 && gDebug)
      {
        fprintf(stdout, "First read grabbed %d chars\n", readcnt);
        firstone = 0;
      }

      // Verify checksum
      if (readcnt == 5)
      {
        int checksum=0;
        for (i=0; i < 5; i++)
          checksum += buffer[i]&0xff;
        if ((checksum & 0xff) != 0xff)
        {
          synccount++;
          if (synccount < 10 || iMeasureCount > 0)
          {
            if (gDebug)
              fprintf(stderr, "%s Checksum error %02x %02x %02x %02x %02x = %02x\n",
               channel,
               buffer[0]&0xff, buffer[1]&0xff, buffer[2]&0xff, 
               buffer[3]&0xff, buffer[4]&0xff,
                      checksum & 0xff);
            if (iMeasureCount > 0)
            {
              syslog(LOG_ERR, 
               "%s Checksum error %02x %02x %02x %02x %02x = %02x Data count %d\n",
               channel,
               buffer[0]&0xff, buffer[1]&0xff, buffer[2]&0xff, 
               buffer[3]&0xff, buffer[4]&0xff,
                      checksum & 0xff, iMeasureCount);
               bNewRun = 1;
            }
          } // checksum error debug prints

          // Checksum doesn't match, read chars until it does
          memmove(&buffer[0], &buffer[1], 4);
          readcnt = 4;
        }
      } // loop until we have 5 characters including valid checksum
    } // while need to fill up a 5 byte buffer

    // Extrapolate from now to timetag for first value in samples array
    timetag = now;
    delta_usec = (suseconds_t) ((iMeasureCount * 1000000.0) / measureHZ);
    if ((delta_usec % 1000000) > timetag.tv_usec)
    {
      timetag.tv_usec = 1000000 + timetag.tv_usec - (delta_usec % 1000000);
      timetag.tv_sec = 1 + timetag.tv_sec - (delta_usec / 1000000);
    }
    else
    {
      timetag.tv_usec = timetag.tv_usec - (delta_usec % 1000000);
      timetag.tv_sec -= delta_usec / 1000000;
    }

    // Turn bytes into a sample value
    iMeasureCount++;
    sample[iMeasureCount] = buffer[0] * 0x10000;
    sample[iMeasureCount] += (buffer[1] & 0xff) * 0x100;
    sample[iMeasureCount] += (buffer[2] & 0xff);

    // Handle the various cases
    // 1A -- This is the first data value we have collected
    // 1B -- Too much time has elapsed since last data and need reset
    // 1C -- Checksum error in middle of run should force a reset
    // 2  -- Adding a new value to the existing sample list

    // If this is the first data value
    // If more than 10 seconds since last data value, start record over
    if (timetag.tv_sec - utcWhen[0].tv_sec >= 10 || bNewRun)
    {
      if (gDebug && iMeasureCount > 1)
      {
        fprintf(stdout, "%s Time gap timetag.tv_sec %d - utcWhen[0] %d = %d\n",
        channel, timetag.tv_sec, utcWhen[0].tv_sec, timetag.tv_sec - utcWhen[0].tv_sec);
      }
      else if (!gDebug && iMeasureCount > 1)
      {
        syslog(LOG_INFO, "%s Time gap timetag.tv_sec %d - utcWhen[0] %d = %d\n",
        channel, timetag.tv_sec, utcWhen[0].tv_sec, timetag.tv_sec - utcWhen[0].tv_sec);
      }
      else if (gDebug)
      {
        fprintf(stdout, "%s Data collection start at tv_sec = %d.%06d\n",
        channel, now.tv_sec, now.tv_usec);
      }
      else
      {
        syslog(LOG_INFO, "%s Data collection start at tv_sec = %d.%06d\n",
        channel, now.tv_sec, now.tv_usec);
      }
      bNewRun = 0;
      iRunCount = 0;
      iFixCount = 0;
      iTimeBufSize = 0;
      iMeasureCount = 1;
      measureHZ = defaultHZ;
      timetag = now;
      measureTime = now;
      utcWhen[0] = now;
      memset(&newtime, 0, sizeof(newtime));
      newtime.year = 1970;
      newtime.day = 1;
      newtime = ST_AddToTime2(newtime, 0, 0, 0,
                    measureTime.tv_sec, measureTime.tv_usec/100);
      msr.starttime = ms_time2hptime(newtime.year, newtime.day, newtime.hour,
               newtime.minute, newtime.second, newtime.tenth_msec*100);
    } // Start of a new measuring sequence
    else
    {
      // Here when adding new value to existing sequence
      // Just update the running average
      delta_sec = (double)(now.tv_sec - last.tv_sec) +
                  ((now.tv_usec - last.tv_usec) / 1000000.0);
      if (delta_sec > 0)
      {
        measureHZ = ((measureHZ * (DECAY_RATE - delta_sec)) + 1.0
                    ) / DECAY_RATE;

        // Only allow measureHZ to drift from defaultHZ by 0.05%
        if (measureHZ > defaultHZ*1.0003)
          measureHZ = defaultHZ*1.0003;
        if (measureHZ < defaultHZ*0.9997)
          measureHZ = defaultHZ*0.9997;
      }
      else
      {
        // Want a record of 0 or negative time deltas
        if (gDebug)
          fprintf(stdout, 
             "%s/%s Sample time delta error %.6lf seconds\n",
             location, channel, delta_sec);
        else
          syslog(LOG_INFO, 
             "%s/%s Sample time delta error %.6lf seconds\n",
             location, channel, delta_sec);
      }
    } // Adding new value to existing sequence

    // remember timetag of this sample for next round rate determination
    last = now;

    // Sort the new timetag into the best 
    for (iTime=0; iTime < TIMEARRAYSIZE && iTime < iTimeBufSize; iTime++)
    {
      if ((utcWhen[iTime].tv_sec > timetag.tv_sec) ||
          ((utcWhen[iTime].tv_sec == timetag.tv_sec) &&
           (utcWhen[iTime].tv_usec > timetag.tv_usec)))
      {
        // timetag belogs before current value
        break;
      }
    }
    if (iTime < TIMEARRAYSIZE)
    {
      int iMove;
      for (iMove = TIMEARRAYSIZE-1; iMove > iTime; iMove--)
        utcWhen[iMove] = utcWhen[iMove-1];
      utcWhen[iTime] = timetag;
      if (iTimeBufSize < TIMEARRAYSIZE)
        iTimeBufSize++;
    }

    // Use earliest start time estimate as the real start time
    if (iTime == 0 &&
        (iFixCount != iRunCount || iRunCount < 60*5*4))
    {
      delta_sec = (timetag.tv_sec - measureTime.tv_sec)
                + (timetag.tv_usec - measureTime.tv_usec) / 1000000.0;
      if (delta_sec <= -0.0001)
      {
        // Only allow a single -1 tick correction per record after first 5 minutes
        if (iRunCount > 60*5*4)
          delta_sec = -0.0001;
        delta_usec = (suseconds_t) (-delta_sec * 1000000);
        if ((delta_usec % 1000000) > timetag.tv_usec)
        {
          measureTime.tv_usec = 1000000 + measureTime.tv_usec - (delta_usec % 1000000);
          measureTime.tv_sec = 1 + measureTime.tv_sec - (delta_usec / 1000000);
        }
        else
        {
          measureTime.tv_usec = measureTime.tv_usec - (delta_usec % 1000000);
          measureTime.tv_sec -= delta_usec / 1000000;
        }

        if (gDebug)
          fprintf(stdout, 
             "%2.2s %-5.5s %2.2s/%3.3s Adjust clock %d ticks, Rate %.5lf Hz\n",
             log_network, log_station, location, channel,
             (int)(delta_sec*10000), measureHZ);
        else if (delta_sec < -0.0001)
          syslog(LOG_INFO, 
             "%2.2s %-5.5s %2.2s/%3.3s Adjust clock %d ticks, Rate %.5lf Hz\n",
             log_network, log_station, location, channel,
             (int)(delta_sec*10000), measureHZ);
  
        // Store new timetag
        memset(&newtime, 0, sizeof(newtime));
        newtime.year = 1970;
        newtime.day = 1;
        newtime = ST_AddToTime2(newtime, 0, 0, 0,
                      measureTime.tv_sec, measureTime.tv_usec/100);
//      msr.starttime = ms_time2hptime(newtime.year, newtime.day, newtime.hour,
//               newtime.minute, newtime.second, newtime.tenth_msec*100);
        iFixCount = iRunCount;
      } // time bump was at least 1/10th of a millisecond
    } // We bumped the start time forward
  
    msr.datasamples = sample;
    msr.numsamples = iMeasureCount;
    packedrecords = msr_pack(&msr, &record_handler, NULL, &packedsamples,
                             0, verbose);
    if (packedrecords == -1)
    {
      if (gDebug)
        fprintf(stderr, "Unable to pack records\n");
      else
        syslog(LOG_ERR, "Unable to pack records\n");
    }
    if (packedrecords > 0)
    {
      // Keep library up to date with the latest extrapolated timetag
      memset(&newtime, 0, sizeof(newtime));
      newtime.year = 1970;
      newtime.day = 1;
      newtime = ST_AddToTime2(newtime, 0, 0, 0,
                measureTime.tv_sec, measureTime.tv_usec/100);
//    msr.starttime = ms_time2hptime(newtime.year, newtime.day, newtime.hour,
//              newtime.minute, newtime.second, newtime.tenth_msec*100);
      iRunCount++;
    }

    if (packedrecords > 0 && gDebug)
    {
      double rotation_rate;
      rotation_rate = buffer[0] * 0x10000;
      rotation_rate += (buffer[1] & 0xff) * 0x100;
      rotation_rate += (buffer[2] & 0xff);
      rotation_rate *= ROTATION_UNIT;
fprintf(stdout, "%.6lf Hz, saved/samples %d/%d, angular rate %.9lf deg/sec\n",
measureHZ, packedsamples, packedsamples+iMeasureCount, rotation_rate);
if (buffer[3])
{
  if (buffer[3] & 0x80) fprintf(stdout, "NOGO ");
  if (buffer[3] & 0x40) fprintf(stdout, "Reset ");
  if (buffer[3] & 0x10) fprintf(stdout, "Temp_Warn ");
  if (buffer[3] & 0x08) fprintf(stdout, "AuxErr ");
  if (buffer[3] & 0x04) fprintf(stdout, "BITerr ");
  if (buffer[3] & 0x02) fprintf(stdout, "RangeErr ");
  if (buffer[3] & 0x01) fprintf(stdout, "UnknownCmd ");
  fprintf(stdout, "\n");

}
    } // packed a record and debug is on
  } // loop forever
} // collectData()


//////////////////////////////////////////////////////////////////////////////

main (argc, argv)
int argc;
char *argv[];
{
  // command line parsing
  char *configfile = NULL;
  char *loc_chan = NULL;
  char *portname = NULL;
  char *retmsg;

  int  bParseError=0;
  int  bShowUsage=0;
  int  iParseArg;
  int  iLength;
  int  i;
 
  /* miniseed data */
  long int num_msrec;           // Number of miniseed records
  char *miniseed=NULL;          // Points to miniseed records created

	/* output file name construction */
	char laststa[8 + 1];						/* name of last station read */
	char lastchan[8 + 1];						/* name of last compnt read */
	int counter;								/* number of segments in wf */
  FILE *outfile;

  
	// parse command line
  for (iParseArg=1; iParseArg < argc; iParseArg++)
  {
    if (strcmp(argv[iParseArg], "-h") == 0)
    {
      bShowUsage = 1;
    }
    else if (strncmp(argv[iParseArg], "-hz=", 4) == 0)
    {
      measureHZ = atof(&argv[iParseArg][4]);
      defaultHZ = measureHZ;
    }
    else if (strcmp(argv[iParseArg], "-debug") == 0)
    {
      gDebug = 1;
    }
    else if (argv[iParseArg][0] == '-')
    {
      fprintf(stderr, "Unrecognized switch '%s'\n", argv[iParseArg]);
      bParseError = 1;
    }
    else
    {
      if (configfile == NULL)
      {
        configfile = argv[iParseArg];
      }
      else if (loc_chan == NULL)
      {
        loc_chan = argv[iParseArg];
      }
      else if (portname == NULL)
      {
        portname = argv[iParseArg];
      }
      else
      {
         fprintf(stderr, "Extra argument %d '%s'\n",
                 iParseArg, argv[iParseArg]);
         bParseError = 1;
      }
    }
  } // check all command line arguments

  if (bParseError)
  {
    ShowUsage();
    exit(1);
  }

  if (bShowUsage)
  {
    ShowUsage ();
    exit(0);
  }

  if (configfile == NULL)
  {
    fprintf(stderr, "Missing configuration file argument!\n");
    ShowUsage();
    exit(1);
  }

  if (loc_chan == NULL)
  {
    fprintf(stderr, "Missing location/channel argument!\n");
    ShowUsage();
    exit(1);
  }

  if (portname == NULL)
  {
    fprintf(stderr, "Missing serial port argument!\n");
    ShowUsage();
    exit(1);
  }

  // Parse the configuration file
  if ((retmsg=ParseDiskLoopConfig(configfile)) != NULL)
  {
    fprintf(stderr, "%s: %s\n", configfile, retmsg);
    exit(1);
  }

  // Parse location and channel names
  if (loc_chan[0] != '/')
  {
    strncpy(location, loc_chan, 2);
    location[2] = 0;
    strncpy(channel, &loc_chan[3], 3);
  }
  else
  {
    strcpy(location, "  ");
    strncpy(channel, &loc_chan[1], 3);
  }
  channel[3] = 0;
  
  // Run as a daemon
  if (!gDebug)
  {
    daemonize();
  }

  // Run loop to endlessly collect and store data from uFors1 sensors
  collectData(location, channel, portname);

  return (0);
} // main()

