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
#include "sachead.h"
#include "diskloop.h"
#include "q330arch.h"
#include "netreq.h"
#include "dcc_time_proto2.h"

// Global debug flag
int gDebug=0;

// Global data shared between record_handler and collectData
// When record_handler is called when a new seed record is created
// it uses utcWhen[0] as the record timetag
// then it resets iRecordCnt and iTimeBufSize to 0
#define TIMEARRAYSIZE 5
#define DATAHZ 640
#define ROTATION_UNIT 0.000002344

// Stores the 5 shortest record start timetags
struct timeval  utcWhen[TIMEARRAYSIZE];
int  iTimeBufSize=0;   // Says how many values have been added to utcWhen
static int firstone=1;
int  iRecordCnt=0;
long sample[8192*8];            // Can't pack lower than 1 bit/value

// Needed to smooth out time stamping
double measureHZ = DATAHZ;
struct timeval measureTime={0};
long   iMeasureCount=0;

//////////////////////////////////////////////////////////////////////////////
// ShowUsage()
ShowUsage()
{
  fprintf(stderr,
" Usage: ufors <configuration file> <location/channel> <serial port>\n"
"              [-debug] [-h]\n"
"   <configuration file>  Name of diskloop.config file\n"
"   <location/channel>    How to name the ufors data\n"
"   <serial port>         Unix device name for port connected to unit\n"
" Options:\n"
"   -h            Prints this help text\n"
"   -debug        Prints additional debugging information\n"
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
  static int iLastMeasure=0;
  STDTIME2  newtime;
  seed_header *seedhdr;
  int iPackCount;
  int i;
  double calcDelta;
  double calcHZ;

  seedhdr = (seed_header *)record;
  iPackCount = ntohs(seedhdr->samples_in_record);

  // Set measureTime if it is zero
  if (measureTime.tv_sec == 0)
    measureTime = utcWhen[0];

  // Calculate time for new record
  calcDelta = floor(10000.0 * iMeasureCount / measureHZ);
  
  // Smooth adjustment of measureHZ
  if (iMeasureCount > DATAHZ*120)
  {
    calcHZ = (double)(utcWhen[0].tv_sec - measureTime.tv_sec) +
             (utcWhen[0].tv_usec - measureTime.tv_usec) / 1000000.0;
    calcHZ = iMeasureCount / calcHZ;
    measureHZ = (measureHZ * 499 + calcHZ) / 500.0;
    if (!gDebug && ((iMeasureCount - iLastMeasure) / 640) > 300)
    {
      syslog(LOG_INFO, "Measured data rate for %.2s/%.3s is %.4lf Hz\n",
                       &seedhdr->location_id, &seedhdr->channel_id ,calcHZ);
      iLastMeasure = iMeasureCount;
    }
  }

  // Get the record timetag to best of sorted list
  memset(&newtime, 0, sizeof(newtime));
  newtime.year = 1970;
  newtime.day = 1;
  newtime = ST_AddToTime2(newtime, 0, 0, 0,
                    measureTime.tv_sec + (long)(calcDelta/10000),
                    measureTime.tv_usec/100 + (long)fmod(calcDelta, 10000.0));
if (gDebug)
fprintf(stderr, "%d,%d %02d:%02d:%02d.%04d, %.1lf\n",
newtime.year, newtime.day, newtime.hour, newtime.minute,
newtime.second, newtime.tenth_msec, calcDelta);

  // Reset record start timetag for new data
  iTimeBufSize = 0;
  for (i=0; i < iRecordCnt - iPackCount; i++)
    sample[i] = sample[i + iPackCount];
  iRecordCnt -= iPackCount;
  iMeasureCount += iPackCount;

  // Store new timetag
  seedhdr->yr = htons(newtime.year);
  seedhdr->jday = htons(newtime.day);
  seedhdr->hr = newtime.hour;
  seedhdr->minute = newtime.minute;
  seedhdr->seconds = newtime.second;
  seedhdr->tenth_millisec = htons(newtime.tenth_msec);

  // Send seed record to q330arch for archival and data transmission
  // q330arch will merge data from all data loggers into a single stream
  char *retstr;
  while ((retstr = q330SeedSend((void *)record)) != NULL)
  {
    if (gDebug)
      fprintf(stderr, "%s line %d q330SeedSend: %s\n", 
             __FILE__, __LINE__, retstr);
    else
      syslog(LOG_ERR, "%s line %d q330SeedSend: %s\n",
            __FILE__, __LINE__, retstr);
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
  int lastrecordcount;
  suseconds_t delta_usec;
  STDTIME2  newtime;
  struct timeval now;
  struct timeval timetag;
  struct timeval old_timetag;
  static struct timeval last={0};

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
  lastrecordcount = 0;
  iRecordCnt = 0;
  gettimeofday(&last, NULL);
  old_timetag = last;
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
        fprintf(stderr, "First read grabbed %d chars\n", readcnt);
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
          if (synccount < 10 || iRecordCnt > 1)
          {
            if (gDebug)
            fprintf(stderr, "Checksum error %02x %02x %02x %02x %02x = %02x\n",
             buffer[0]&0xff, buffer[1]&0xff, buffer[2]&0xff, 
             buffer[3]&0xff, buffer[4]&0xff,
                    checksum & 0xff);
            if (iRecordCnt > 1)
            {
              syslog(LOG_ERR, 
               "Checksum error %02x %02x %02x %02x %02x = %02x Data count %d\n",
               buffer[0]&0xff, buffer[1]&0xff, buffer[2]&0xff, 
               buffer[3]&0xff, buffer[4]&0xff,
                    checksum & 0xff, iRecordCnt);
            }
          } // checksum error debug prints

          // Checksum doesn't match, read chars until it does
          memmove(&buffer[0], &buffer[1], 4);
          readcnt = 4;
        }
      } // loop until we have 5 characters including valid checksum
    } // while need to fill up a 5 byte buffer

    // Turn now into timetag for start of record
    timetag = now;
    delta_usec = (suseconds_t) ((iRecordCnt * 1000000.0) / DATAHZ);
    if ((delta_usec % 1000000) > timetag.tv_usec)
    {
      timetag.tv_usec = 1000000 + timetag.tv_usec - (delta_usec % 1000000);
    }
    else
    {
      timetag.tv_usec = timetag.tv_usec - (delta_usec % 1000000);
      timetag.tv_sec -= delta_usec / 1000000;
    }

    iRecordCnt ++;

    // If more than 10 seconds since last data value, start record over
    if (timetag.tv_sec - utcWhen[0].tv_sec > 10 && iRecordCnt > 1)
    {
      iRecordCnt = 1;
      iTimeBufSize = 0;
      iMeasureCount = 0;
      measureTime.tv_sec = 0;
      measureHZ = DATAHZ;
      if (gDebug)
      {
        fprintf(stderr, "Time gap timetag.tv_sec %d - utcWhen[0] %d = %d\n",
        timetag.tv_sec, utcWhen[0].tv_sec, timetag.tv_sec - utcWhen[0].tv_sec);
      }
      else
      {
        syslog(LOG_INFO, "Time gap timetag.tv_sec %d - utcWhen[0] %d = %d\n",
        timetag.tv_sec, utcWhen[0].tv_sec, timetag.tv_sec - utcWhen[0].tv_sec);
      }
      timetag = now;
      utcWhen[0] = now;
    }

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
    if (iTime == 0)
    {
      memset(&newtime, 0, sizeof(newtime));
      newtime.year = 1970;
      newtime.day = 1;
      newtime = ST_AddToTime2(newtime, 0, 0, 0,
                            utcWhen[0].tv_sec, utcWhen[0].tv_usec/100);
      msr.starttime = ms_time2hptime(newtime.year,
         newtime.day, newtime.hour,
         newtime.minute, newtime.second,
         newtime.tenth_msec*100);
    }
  
    // Add data to build of new seed record
    sample[iRecordCnt] = buffer[0] * 0x10000;
    sample[iRecordCnt] += (buffer[1] & 0xff) * 0x100;
    sample[iRecordCnt] += (buffer[2] & 0xff);
    msr.datasamples = sample;
    msr.numsamples = iRecordCnt;
    packedrecords = msr_pack(&msr, &record_handler, NULL, &packedsamples,
                             0, verbose);
    if (packedrecords == -1)
    {
      if (gDebug)
        fprintf(stderr, "Unable to pack records\n");
      else
        syslog(LOG_ERR, "Unable to pack records\n");
    }

    if (packedrecords > 0 && gDebug)
    {
      double deltat;
      double rotation_rate;
      deltat = now.tv_sec - last.tv_sec;
      deltat += now.tv_usec/1000000.0 - last.tv_usec/1000000.0;
      last = now;
      rotation_rate = buffer[0] * 0x10000;
      rotation_rate += (buffer[1] & 0xff) * 0x100;
      rotation_rate += (buffer[2] & 0xff);
      rotation_rate *= ROTATION_UNIT;
fprintf(stderr, "%d/%.4lf Hz, count %d, angular rate %.9lf deg/sec\n",
(int)((lastrecordcount)/deltat), measureHZ, packedsamples, rotation_rate);
if (buffer[3])
{
  if (buffer[3] & 0x80) fprintf(stderr, "NOGO ");
  if (buffer[3] & 0x40) fprintf(stderr, "Reset ");
  if (buffer[3] & 0x10) fprintf(stderr, "Temp_Warn ");
  if (buffer[3] & 0x08) fprintf(stderr, "AuxErr ");
  if (buffer[3] & 0x04) fprintf(stderr, "BITerr ");
  if (buffer[3] & 0x02) fprintf(stderr, "RangeErr ");
  if (buffer[3] & 0x01) fprintf(stderr, "UnknownCmd ");
  fprintf(stderr, "\n");

}
//for (iTime = 0; iTime < TIMEARRAYSIZE; iTime++)
//{
//  fprintf(stderr, "TT=%d.%06d\n", 
//          utcWhen[iTime].tv_sec, utcWhen[iTime].tv_usec);
//}
      lastrecordcount = packedsamples;
    }
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
  char location[4];
  char channel[4];

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

