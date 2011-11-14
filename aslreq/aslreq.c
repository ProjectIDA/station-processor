/*
  Network server program to support SAT testing of Q330 boxes from labview
  Accepts SER330 command to change serial number in seneca.config
  Accepts RMSREQ command to get ascii RMS values for a channel
  Accepts ASLREQ command to get ascii time series data for a given channel

  Input formats:

    ASLREQ <station>.[<location>-]<channel> <year>/<month>/<day> <hour>:<min>:<sec> <samples>
  
    SER330 <hex serial number>

    RMSREQ <station>.[<location>-]<channel> <year>/<month>/<day> <hour>:<min>:<sec> <samples> [<loglocation>-]<logchannel>

 */

const char *VersionIdentString = "Release 2.0";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"
#include "steim.h"
#include "steimlib.h"
#include "miniseed.h"

#define MAXSAMPLES 5000000
extern int errno;
void daemonize();

#define perror(ster) fprintf(stderr,"Error: %s (%x)\n",ster,errno)

LONG decode_SEED_micro_header (SEED_data_record *sdr, SHORT *firstframe, SHORT *level, SHORT *flip, SHORT *dframes);
int client_connected=0;
char incmdbuf[80];
char prsbuf[80] ;
char lgstation_id[6] ;
unsigned char lgrec[SEEDRECSIZE];
int fd, sockpath ;
long seqnum ;
char lgframes ;
int lgcount ;
int cmdcnt, cmdlen ;
int gDebug=0;

void close_client_connection()
{
 sleep(2); // Helps some clients who see RESET before last data
 shutdown(sockpath, SHUT_RDWR) ;
 close(sockpath) ;
 client_connected = 0 ;
 fprintf(stderr, "Closing client connection\n");
}

void send_data (void *data)
{
 int len;
 int i, flag, send_attempts ;
 int iSent;
 char *bufptr;

 /* Set up non-blocking write to the socket */
 send_attempts = 0 ;
 iSent = 0;
 bufptr = data;
 len = strlen(data);
 while (send_attempts < 200 && iSent < len)
 {
  ++send_attempts;
  flag = MSG_DONTWAIT ;
  i = send(sockpath, &bufptr[iSent], len-iSent, flag);
//fprintf (stderr, "DEBUG Bytes sent = %i\n", i) ;
  if (i < 0)
  {
   if (errno==EAGAIN)
   {
  fprintf(stderr, "DEBUG Would block error\n") ;
    if (send_attempts>=120)
    {
      fprintf(stderr,"Write timed out - closing connection") ;
      close_client_connection() ;
      return ;
    }
    sleep(1) ;
   }
   else
   {
     if ((errno==ENOTCONN) || (errno==EPIPE))
      fprintf(stderr,"Client Disconnected\n");
     else
      fprintf(stderr,"error on send (%x)\n", errno) ;
     close_client_connection() ;
     return ;
   }
  } // Error return from send
  else
  {
   iSent += i;
   if (iSent < len)
     sleep(1);
  } // send was good
 } // while send not timed out

 // Make sure record was sent
 if (iSent < len)
 {
  fprintf(stderr,"Write too many attempts- closing connection") ;
  close_client_connection() ;
 }
} // send_data()

void add_log_message (char *lgmsg)
{
 int i ;
 i = strlen(lgmsg) ;
 memcpy (&lgrec[lgcount], lgmsg, i) ;
 lgcount = lgcount + i ;
 memcpy (&lgrec[lgcount], "\r", 1) ;
 lgcount = lgcount + 1 ;
 lgframes = lgframes + 1 ;
}

void write_log_record()
{
 /* Copy final (legacy) message to the log record */
 send_data ("Finished\n") ;
 sleep(3);
 fprintf(stderr, "Done processing request\n");
 if (client_connected == 1) close_client_connection() ;
}

void read_socket ()
{
 int count, flag, i ;
 char buf[80];
 int gotcmd ;
 int cbufsiz ;
 int read_attempts ;

 gotcmd=0;
 cbufsiz = 0;
 incmdbuf[cbufsiz] = '\0' ;
 read_attempts = 0 ;
 while(gotcmd==0) {
  /* Set up non-blocking read from the socket */
  flag = MSG_DONTWAIT ;
  i = 1 ;
  count = recv(sockpath, buf, i, flag);
  if (count > 0)
  {
   if (count+cbufsiz>80) {
    fprintf(stderr,"Buffer size exceeded - closing connection\n");
    close_client_connection() ;
    break;
   }
   if (buf[0] < ' ') break ;
   memcpy(&incmdbuf[cbufsiz],buf,count);
   cbufsiz+= count;
   incmdbuf[cbufsiz] = '\0';
  }
  else
  {
   /* Unable to read from the socket */
   if (errno == EAGAIN)
   {
    /* Read would block - exit if byte(s) read
       from the socket, else declare timeout or sleep */
    if (cbufsiz > 0) break ;
    ++read_attempts ;
    if (read_attempts == 100)
     {
      fprintf (stderr, "Read timed out - closing connection\n") ;
      close_client_connection() ;
      break ;
     }
    sleep(1) ;
   }
   else
   {
    fprintf(stderr,"Read error from client - closing connection\n");
    close_client_connection() ;
    break;
   }
  }

 }

}

//////////////////////////////////////////////////////////////////////////////
// Routine that processes data request
// Returns the number of samples actually sent
int TransferSamples(
  const char  *rq_station,   // station name
  const char  *rq_chan,      // Channel ID
  const char  *rq_loc,       // Location ID
  STDTIME2    rq_tBeginTime, // Start time
  long        rq_iSamples    // Number of data points to transfer
  )                          // Returns the number of samples actually sent
{
  int   indexFirst;
  int   indexLast;
  int   iCount;
  int   iSample;
  int   iSkip;
  int   iLoopSize;
  int   iLine;
  int   i,j,k;
  int   iEmpty;
  int   iSeek;
  int   iRecord;
  int   iLoopRecordSize;

  long  myData[8192];  // A seed record can never store more than this many values
  long  rectotal;
  static dcptype dcp=NULL;
  short  dframes;
  short  dstat;
  short  firstframe, flip, level;
  long   headertotal;

  STDTIME2    tRecStart;
  DELTA_T2    diffTime;
  double      delta;
  double      dSampleRate;
  int         iRateMult;
  int         iRateFactor;
  double      dTimeCorrection;
  seed_header *pheader;
  char        outline[256];
  short       iYear, iMonth, iDom, iDoy;
  LONG        iJulian;
  char        cSign;


  STDTIME2    rq_tEndTime;
  char  str_record[8192];
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  static char looperrstr[MAXCONFIGLINELEN+2];
  char  loopDir[MAXCONFIGLINELEN+1];
  char  *errmsg;
  FILE  *fp_buf;
  
  // Initialize decompression code
  if (dcp == NULL)
  {
    if ((dcp = init_generic_decompression()) == NULL)
    {
      fprintf(stderr, "TransferSamples: failed call to init_generic_decompression()\n");
      return 0;
    }
  } // first time initialization

fprintf(stderr,
"TransferSamples(%s %s/%s %d,%03d,%02d:%02d:%02d %ld)\n",
rq_station,rq_loc,rq_chan,
rq_tBeginTime.year, rq_tBeginTime.day,
rq_tBeginTime.hour, rq_tBeginTime.minute, rq_tBeginTime.second,
rq_iSamples);

  // Get index of first record for this data request
  rq_tEndTime = ST_AddToTime2(rq_tBeginTime, 0, 0, 0, 1, 0);
  LoopRecordSize(&iLoopRecordSize);
  errmsg = GetRecordRange(rq_station, rq_chan, rq_loc,
             rq_tBeginTime, rq_tEndTime,
             &indexFirst, &indexLast, &iCount, &iLoopSize);
  if (errmsg != NULL)
  {
    fprintf(stderr, "%s\n", errmsg);
    return 0;
  }

  // Make sure there are data records to return
  if (iCount < 1)
    return 0;

  // Get name of buffer file
  // If blank location code, leave off leading location code in filename
  LoopDirectory(loopDir);
  if (rq_loc[0] == ' ' || rq_loc[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, rq_station, rq_chan);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, rq_station, rq_loc, rq_chan);
  }

  // Open the buffer file
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no records to return
    return 0;
  }

  // Loop until we've sent rq_iSamples data points
  iSample = 0;
  i=0;
  iEmpty = 0;
  iLine = 0;
  while (iSample < rq_iSamples)
  {
fprintf(stderr, "DEBUG loop on sample %d of %ld\n", iSample, rq_iSamples);
    iRecord = (i + indexFirst + iEmpty) % iLoopSize;
    iSeek = iRecord * iLoopRecordSize;

    // Seek to the record position
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, we hit end of file, set iHigh
      sprintf(looperrstr, "TransferSamples: Unable to seek to %d in %s",
              iSeek, buf_filename);
      fclose(fp_buf);
      return iSample;
    } // Failed to seek to required file buffer position

    // Read in the data
    if (fread(str_record, iLoopRecordSize, 1, fp_buf) != 1)
    {
      sprintf(looperrstr, "TransferSamples: Unable to read record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      return iSample;
    } // Failed to read record

//fprintf(stderr, "decode_SEED_micro_header\n");
    // Decompress the record
    headertotal = decode_SEED_micro_header ( (SEED_data_record *)str_record, 
                                              &firstframe, &level, &flip, &dframes);
    if ((dframes < 1) || (dframes >= MAXSEEDFRAMES))
    {
      fprintf(stderr, "illegal number of data frames specified!\n");
      exit(0);
    }
    if ((firstframe < 0) || (firstframe > dframes))
    {
      fprintf(stderr,
          "illegal first data frame! Skipping! (firstframe=%d, dframes=%d)\n",
           firstframe,dframes);
      iEmpty++;
      continue;
    }
    if ((level < 1) || (level > 3))
    {
      fprintf(stderr, "illegal compression level!\n");
      exit(0);
    }
//fprintf(stderr, "decompress_generic_record\n");
    rectotal = decompress_generic_record ((generic_data_record *)str_record, myData, &dstat, dcp,
                                           firstframe, headertotal, level, flip, dframes);
fprintf(stderr, "Decompressed %ld records from seed record %d\n", rectotal, i+1);
    // Skip any data points that are prior to start time
    j = 0;  // normaly start at first value in record
    if (i == 0)
    {
      // Get sample rate
      pheader = (seed_header *)str_record;
      iRateFactor = (short)ntohs(pheader->sample_rate_factor);
      iRateMult = (short)ntohs(pheader->sample_rate_multiplier);

      // Get sample rate, See SEED Reference Manual Chp 8
      // Fixed Section of Data Header for formulas
      dSampleRate = 0;
      if (iRateFactor > 0 && iRateMult > 0)
        dSampleRate = (double)(iRateFactor * iRateMult);

      if (iRateFactor > 0 && iRateMult < 0)
        dSampleRate = -((double)iRateFactor / (double)iRateMult);

      if (iRateFactor < 0 && iRateMult > 0)
        dSampleRate = -((double)iRateMult / (double)iRateFactor);

      if (iRateFactor < 0 && iRateMult < 0)
        dSampleRate = 1.0 / (double)(iRateFactor * iRateMult);

      // Parse Record Start time
      tRecStart.year = ntohs(pheader->yr);
      tRecStart.day = ntohs(pheader->jday);
      tRecStart.hour = (int)pheader->hr;
      tRecStart.minute = (int)pheader->minute;
      tRecStart.second = (int)pheader->seconds;
      tRecStart.tenth_msec = ntohs(pheader->tenth_millisec);

      // Get time difference
      diffTime = ST_DiffTimes2(rq_tBeginTime , tRecStart);
      delta = (double)ST_DeltaToMS2(diffTime) / 10000.0;
      iSkip = (int)(delta * dSampleRate);
fprintf(stderr, "Skip %d samples to reach start time, delta=%.4lf rate=%.4lf\n", iSkip, delta, dSampleRate);

      j = iSkip;  // skip this many samples to find first one after start time

      // Get time correction value from seed header
      dTimeCorrection = ntohl(pheader->tenth_msec_correction) /10000.0;

      // See if time correction has allready been applied
      if (pheader->activity_flags & 0x40)
        dTimeCorrection = 0;
fprintf(stderr, "Initial time correction of %.6lf\n", dTimeCorrection);

      // Add to any offset from where we started taking samples
      dTimeCorrection = delta - ((double)iSkip/dSampleRate);
      if (dTimeCorrection < 0)
      {
        cSign = '-';
        dTimeCorrection = -dTimeCorrection;
      }
      else
      {
        cSign = '+';
      }

      // Create header line
      iJulian = ST_GetJulian2(rq_tBeginTime);
      ST_CnvJulToCal2(iJulian, &iYear, &iMonth, &iDom, &iDoy);
      if (rq_loc[0] == ' ' || rq_loc[0] == 0)
      {
        sprintf(outline, "%s.%s %4d/%02d/%02d %02d:%02d:%02d %c%-10.6lf SEC %5.2lf  SPS  UNFILTERED %ld\n",
            rq_station, rq_chan,
            iYear, iMonth, iDom,
            rq_tBeginTime.hour, rq_tBeginTime.minute, rq_tBeginTime.second,
            cSign, dTimeCorrection,
            dSampleRate, rq_iSamples);
      }
      else
      {
        sprintf(outline, "%s.%s-%s %4d/%02d/%02d %02d:%02d:%02d %c%-10.6lf SEC %5.2lf  SPS  UNFILTERED %ld\n",
            rq_station, rq_loc, rq_chan,
            iYear, iMonth, iDom,
            rq_tBeginTime.hour, rq_tBeginTime.minute, rq_tBeginTime.second,
            cSign, dTimeCorrection,
            dSampleRate, rq_iSamples);
      }
      send_data(outline);
      fprintf(stderr, "Final time correction %c%-10.6lf\n",
              cSign, dTimeCorrection);
    } // If first record in

    // If error caused connection to close we are done
    if (!client_connected)
    {
      fclose(fp_buf);
      return iSample;
    }

    // Send data points until record is consumed, or we have sent enough
    for (; j < rectotal && iSample < rq_iSamples;j++,iSample++,k++)
    {
      sprintf(outline, "%5d %ld\n", iLine, myData[j]);
      send_data(outline);
      iLine++;

      // If error caused connection to close we are done
      if (!client_connected)
      {
        fclose(fp_buf);
        return iSample;
      }
    } // for each sample to send from this record

fprintf(stderr, "finished with record\n");
    i++;
  } // while more samples need to be sent

  // Close buffer file
  fclose(fp_buf);

  return iSample;
} // TransferSamples()


void parse_request(char srch)
{
 int j ;
 j = 0 ;
 while (cmdcnt < cmdlen)
 {
  if (incmdbuf[cmdcnt] == srch) break ;
  prsbuf[j] = toupper(incmdbuf[cmdcnt]) ;
  ++j ;
  ++cmdcnt ;
 }
 prsbuf[j] = 0;
}

// Process RMSREQ command
// This command returns averaged RMS values for a given channel
void process_rmsreq(
  const char  *rq_station,   // station name
  const char  *rq_chan,      // Channel ID
  const char  *rq_loc,       // Location ID
  STDTIME2    rq_tBeginTime, // Start time
  long        rq_iSamples    // Number of data points to transfer
  )
{
 char loglocation[4] ;
 char logchannel[4] ;
 char str_record[8192];
 char log_msg[8192];
 char msgbuf[512];
 char loopDir[MAXCONFIGLINELEN+1];
 char buf_filename[2*MAXCONFIGLINELEN+2];
 static char looperrstr[MAXCONFIGLINELEN+2];
 char *errmsg;
 char *msgptr;
 int prslen ;
 int badval ;
 int          iLoopRecordSize;
 int          indexFirst;
 int          indexLast;
 int          iStart;
 int          iCount;
 int          iLoopSize;
 int          iSample;
 int          iTry;
 int          iEmpty;
 int          iRecord;
 int          iSeek;
 STDTIME2     rq_tEndTime;
 STDTIME2     startTime;
 FILE         *fp_buf;
 seed_header  *pheader;

 // Parse and validate log [location-]channel
 strcpy(loglocation, "  ") ;
 ++cmdcnt ;
 parse_request(' ') ;
 prslen = strlen(prsbuf) ;
 badval = 0 ;
 switch (prslen)
 {
  case 3 :
   memcpy(logchannel, prsbuf, 4);
   break ;
  case 5 :
   if (prsbuf[1] == '-')
    {
     memcpy(loglocation, prsbuf, 1) ;
     loglocation[1] = 0;
     memcpy(logchannel, &prsbuf[2], 4) ;
    }
   else
    badval = 1 ;
   break ;
  case 6 :
   if (prsbuf[2] == '-')
   {
    memcpy(loglocation, prsbuf, 2) ;
    loglocation[2] = 0;
    memcpy(logchannel, &prsbuf[3], 4) ;
   }
   else
    badval = 1 ;
   break ;
  default :
   badval = 1 ;
 }
 if (badval == 1)
 {
  send_data ("Invalid log [location-]channel name\n");
  return ;
 }

fprintf(stderr, 
"DEBUG process RMSREQ %s.%s-%s in %s-%s %d,%d,%02d:%02d:%02d %ld %s-%s\n",
rq_station,rq_loc,rq_chan, loglocation, logchannel,
rq_tBeginTime.year, rq_tBeginTime.day,
rq_tBeginTime.hour, rq_tBeginTime.minute, rq_tBeginTime.second,
rq_iSamples, loglocation, logchannel);

  // Get index of first record for this data request
  startTime = ST_AddToTime2(rq_tBeginTime, 0, -1, 0, 0, 0);
  rq_tEndTime = rq_tBeginTime;
  rq_tEndTime.year = 3000; // give max year as end time
  LoopRecordSize(&iLoopRecordSize);
  errmsg = GetRecordRange(rq_station, logchannel, loglocation,
             startTime, rq_tEndTime,
             &indexFirst, &indexLast, &iCount, &iLoopSize);
  if (errmsg != NULL)
  {
    sprintf(msgbuf, "%s\n", errmsg);
    send_data(msgbuf);
    return;
  }
fprintf(stderr, "indexFirst=%d, indexLast=%d, iCount=%d, iLoopSize=%d\n",
indexFirst, indexLast, iCount, iLoopSize);

  // Make sure there are data records to return
  if (iCount < 1)
  {
    send_data("No log records found for given day\n");
    return;
  }

  // Get name of buffer file
  // If blank location code, leave off leading location code in filename
  LoopDirectory(loopDir);
  if (loglocation[0] == ' ' || loglocation[0] == 0)
  {
    sprintf(buf_filename, "%s/%s/%s.buf",
        loopDir, rq_station, logchannel);
  }
  else
  {
    sprintf(buf_filename, "%s/%s/%s_%s.buf",
        loopDir, rq_station, loglocation, logchannel);
  }

  // Open the buffer file
  if ((fp_buf=fopen(buf_filename, "r")) == NULL)
  {
    // Buffer file does not exist so no records to return
    sprintf(msgbuf, "Unable to open log file %s\n", buf_filename);
    send_data(msgbuf);
    return;
  }

  // Loop until we've averaged rq_iSamples data points
  // Or the record time is before the requested start time
  iSample = 0;
  iTry = 0;
  while (iSample < rq_iSamples &&
         ((iTry + indexFirst + iEmpty + iLoopSize - 1) % iLoopSize) != indexLast)
  {
    iRecord = (iTry + indexFirst + iEmpty) % iLoopSize;
    iSeek = iRecord * iLoopRecordSize;

    // Seek to the record position
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, we hit end of file
      fprintf(stderr, "process_rms: Unable to seek to %d in %s",
              iSeek, buf_filename);
      fclose(fp_buf);
      sprintf(msgbuf, "Only found %d of requested %ld rms samples\n",
            iSample, rq_iSamples);
      send_data(msgbuf);
      return;
    } // Failed to seek to required file buffer position

    // Read in the data
    if (fread(str_record, iLoopRecordSize, 1, fp_buf) != 1)
    {
      sprintf(looperrstr, "Unable to read record %d in %s\n",
              iRecord, buf_filename);
      fclose(fp_buf);
      send_data(looperrstr);
      return;
    } // Failed to read record

    // Convert the log record into a message string
    pheader = (seed_header *)str_record;
    iStart = min(ntohs(pheader->first_data_byte), 128);
    iCount = min(ntohs(pheader->samples_in_record), 
                 iLoopRecordSize-iStart-1);
    strncpy(log_msg, &str_record[iStart], iCount);
    log_msg[iCount] = 0;

    // Search the log record for rms lines matching requested channel
    msgptr = log_msg;
    while(*msgptr != 0 && iSample < rq_iSamples)
    {
      // Look for {125} messages
      if (strncmp(msgptr, "{125}", 5) == 0)
      {
        // Look for Matching channel and location code
        if (strncmp(&msgptr[27], rq_loc, 2) == 0 &&
            strncmp(&msgptr[30], rq_chan, 3) == 0)
        {
          // The time must be after the start time
          int year=0;
          int month=0;
          int dom=0;
          int hour=0;
          int minute=0;
          int second=0;
          int iCmp;
          STDTIME2 rms_time={0};
          LONG julian;

          if (sscanf(&msgptr[6], "%d-%d-%d %d:%d:%d",
              &year, &month, &dom, &hour,&minute, &second) == 6)
          {
            julian = ST_Julian2(year, month, dom);
            rms_time = ST_CnvJulToSTD2(julian);
            rms_time = ST_AddToTime2(rms_time, 0, hour, minute, second, 0);
          }

          iCmp = ST_TimeComp2(rms_time, rq_tBeginTime);
          if (iCmp >= 0)
          {
            double rms[3];
            if (sscanf(&msgptr[34], "%lf %lf %lf",
                        &rms[0], &rms[1], &rms[2] ) == 3)
            {
              sprintf(msgbuf, "%.3lf\n", rms[1]);
              send_data(msgbuf);
              iSample++;
              if (gDebug)
              {
                fprintf(stderr, "Sample %d: %70.70s\n", iSample, msgptr);
              }
            } // no errors scanning line
          } // Time is after desired start time
        } // RMS line matches desired channel and location
      } // string started with {125}

      // Time to find start of next line
      while (*msgptr != 0 && *msgptr != '\n')
        msgptr++;
      if (*msgptr == '\n') msgptr++;
    } // while we need to check more lines for desired RMS values
    
    iTry++;
  } // while more samples need to found

  // Close buffer file
  fclose(fp_buf);

  if (iSample < rq_iSamples)
  {
    sprintf(msgbuf, "Error, only found %d/%ld samples.\n",
            iSample, rq_iSamples);
    send_data(msgbuf);
  }

  return;
} // process_rmsreq()

// Process SER330 command
// This changes the serial number in seneca.config file
// then restarts the q330serv program to use the new serial number
void process_ser330()
{
 int64_t serno;
 int     count;
 char    cmdstr[80];
 fprintf(stderr, "Processing %s\n", incmdbuf);

 // Validate the serial number
 ++cmdcnt ;
 parse_request(' ') ;
 serno = 0 ;
 count = sscanf(prsbuf,"%Lx", &serno) ;
 if (count != 1)
 {
   send_data ("Unable to parse serial number\n") ;
   return;
 }

 // Use a shell script to make changes, restart q330serv
 sprintf(cmdstr, "newser330 %016Lx", serno);
 if (system(cmdstr) == -1)
 {
   if (gDebug)
     fprintf(stderr, "system(%s) returned error\n", cmdstr);
   else
     syslog(LOG_ERR, "system(%s) returned error\n", cmdstr);
   send_data ("newser330 command returned error, status unknown\n") ;
   return;
 }
 
} // process_ser330()

void process__request()
{
 int j, prslen ;
 int badval, count ;
 char rqlogmsg[120] ;
 char rqcmd[8] ;
 char rqstation[6] ;
 char rqlocation[4] ;
 char rqchannel[4] ;
 long rqyear, rqmonth, rqday, rqsamples ;
 int rqhour, rqmin, rqsec ;
 STDTIME2 rqstart ;
 int days_mth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;

 /* Initialize log record and add request received message */
 seqnum = 1 ;
 lgframes = 0 ;
 lgcount = 64 ;
 strcpy(lgstation_id, "     ");
 for (j=lgcount; j<SEEDRECSIZE; j++) lgrec[j] = ' ' ; 
 memcpy (rqlogmsg, "Request of \"", 12) ;
 cmdlen = strlen(incmdbuf) ;
 memcpy (&rqlogmsg[12], incmdbuf, cmdlen) ;
 j = cmdlen + 12 ;
 strcpy (&rqlogmsg[j], "\" received") ;
 add_log_message(rqlogmsg) ;

 /* Check for valid request keyword 'ASLREQ' or 'SER330' or 'RMSREQ'
  - if not, close connection to probers */
 cmdcnt = 0 ;
 parse_request(' ') ;
 if (strcmp(prsbuf, "ASLREQ") != 0 && 
     strcmp(prsbuf, "SER330") != 0 &&
     strcmp(prsbuf, "RMSREQ") != 0)
 {
  close_client_connection() ;
  return ;
 }
 fprintf(stderr, "DEBUG msg '%s'\n", rqlogmsg);
 strncpy(rqcmd, prsbuf, 6);
 rqcmd[6] = 0;

 // Handle SER330 request seperately
 if (strcmp(prsbuf, "SER330") == 0)
 {
  process_ser330();
  if (client_connected == 0) return;
  write_log_record();
  return;
 }

 /* Parse and validate station name*/
 ++cmdcnt ;
 parse_request('.') ;
 prslen = strlen(prsbuf) ;
 if (prslen < 3 || prslen > 5)
 {
  add_log_message("Invalid or missing station name") ;
  write_log_record() ;
  return ;
 }
 memcpy(lgstation_id, prsbuf, prslen + 1) ;
 memcpy(rqstation, prsbuf, prslen + 1) ;

 /* Parse and validate [location-]channel */
 strcpy(rqlocation, "  ") ;
 ++cmdcnt ;
 parse_request(' ') ;
 prslen = strlen(prsbuf) ;
 badval = 0 ;
 switch (prslen)
 {
  case 3 :
   memcpy(rqchannel, prsbuf, 4);
   break ;
  case 5 :
   if (prsbuf[1] == '-')
    {
     memcpy(rqlocation, prsbuf, 1) ;
     rqlocation[1] = 0;
     memcpy(rqchannel, &prsbuf[2], 4) ;
    }
   else
    badval = 1 ;
   break ;
  case 6 :
   if (prsbuf[2] == '-')
   {
    memcpy(rqlocation, prsbuf, 2) ;
    rqlocation[2] = 0;
    memcpy(rqchannel, &prsbuf[3], 4) ;
   }
   else
    badval = 1 ;
   break ;
  default :
   badval = 1 ;
 }
 if (badval == 1)
 {
  add_log_message("Invalid [location-]channel name") ;
  write_log_record() ;
  return ;
 }

 /* Parse and validate time of yyyy/mm/dd */
 ++cmdcnt ;
 parse_request(' ') ;
 rqyear = 0 ;
 rqmonth = 0 ;
 rqday = 0 ;
 count = sscanf(prsbuf, "%ld/%ld/%ld", &rqyear, &rqmonth, &rqday) ;
 if (rqyear < 2005 || rqyear > 2050) badval = 1 ;
 if (rqmonth < 1 || rqmonth > 12) badval = 1 ;
 if (badval == 0)
 {
  if (rqyear % 4 == 0) days_mth[2] = 29 ;
  if (rqday < 1 || rqday > days_mth[rqmonth]) badval = 1 ;
 }
 if (badval == 1)
 {
  add_log_message("Invalid time yyyy/mm/dd") ;
  write_log_record() ;
  return ;
 }
 rqstart.year = rqyear ;
 /* Calculate days since beginning of year */
 rqstart.day = 0 ;
 j = 0 ;
 while (j < rqmonth)
 {
  rqstart.day = rqstart.day + days_mth[j] ;
  ++j ;
 }
 rqstart.day = rqstart.day + rqday ;

 /* Parse and validate time of hh:mm:ss */
 ++cmdcnt ;
 parse_request(' ') ;
 rqhour = 100 ;
 rqmin = 100 ;
 rqsec = 100 ;
 count = sscanf(prsbuf,"%d:%d:%d", &rqhour, &rqmin, &rqsec) ;
 if (rqhour < 0 || rqhour > 23) badval = 1 ;
 if (rqmin < 0 || rqmin > 59) badval = 1 ;
 if (rqsec < 0 || rqsec > 59) badval = 1 ;
 if (badval == 1)
 {
  add_log_message("Invalid time hh:mm:ss") ;
  write_log_record() ;
  return ; 
 }
 rqstart.hour = rqhour ;
 rqstart.minute = rqmin ;
 rqstart.second = rqsec ;
 rqstart.tenth_msec = 0 ;

 /* Parse and validate number of samples */
 ++cmdcnt ;
 parse_request(' ') ;
 rqsamples = 0 ;
 count = sscanf(prsbuf,"%ld", &rqsamples) ;
 if (rqsamples < 1 || rqsamples > MAXSAMPLES)
 {
  add_log_message("invalid number of samples") ;
  write_log_record() ;
  return ;
 }

 // Handle RMSREQ seperately
 if (strcmp(rqcmd, "RMSREQ") == 0)
 {
  process_rmsreq(rqstation, rqchannel, rqlocation, rqstart, rqsamples);
  if (client_connected == 0) return;
  write_log_record();
  return;
 }

 /* Call the diskloop handler to find and send any available data
    Pass rqstation, rqlocation, rqchannel, rqstart, rqsamples */
 TransferSamples(rqstation,rqchannel,rqlocation, rqstart, rqsamples);

 if (client_connected == 0) return ;
 if (seqnum == 1)
  add_log_message ("No data was available") ;
 else
  add_log_message ("Request for data was fufilled") ;
 write_log_record() ;
} // process__reqeust()

int main(argc,argv)
char *argv[];
int argc;
{
 struct sockaddr_in sin ;
 struct sockaddr from ;
 struct linger lingeropt ;
 socklen_t fromlen = sizeof(struct sockaddr);
 int flag2 ;
 socklen_t j ;
 long port_number ;
 char *retmsg ;
 char peer_name[16];

 if (argc < 2 || argc > 3 || (argc == 3 && strcmp(argv[2], "debug") != 0))
 {
  fprintf(stderr, "Usage:  %s <portnumber> [ debug ]\n", argv[0]);
  fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
  exit(100);
 }
 port_number = atol(argv[1]);
 fprintf(stderr,"Starting on port number %s\n", argv[1]);

 if (argc == 3)
  gDebug = 1;
 client_connected = 0 ;

 // Parse the q330driver configuration file
 if ((retmsg=ParseDiskLoopConfig(
              "/etc/q330/DLG1/diskloop.config")) != NULL)
 {
  fprintf(stderr, "%s: %s\n", argv[0], retmsg);
  exit(1);
 }

 signal(SIGPIPE, SIG_IGN);

 // Set up to run program as a daemon
 if (!gDebug)
  daemonize();

 /* Create the socket to listen on */
 fd = socket(AF_INET, SOCK_STREAM, 0);
 if(fd < 0) {
  perror("socket");
  exit(1);
 }
 /* Set up the structure, set the socket option(s) and bind to the port */
 memset (&sin, 0, sizeof(struct sockaddr_in)) ;
 sin.sin_addr.s_addr = INADDR_ANY;
 sin.sin_family      = AF_INET;   /* Internet address format */
 sin.sin_port        = htons(port_number);
 flag2 = 1 ;
 j = sizeof(int) ;
 setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag2, j) ;
 j = sizeof(struct linger) ;
 getsockopt(fd, SOL_SOCKET, SO_LINGER, &lingeropt, &j) ;
 if (lingeropt.l_onoff)
 {
  fprintf(stderr, "Setting linger option\n") ;
  lingeropt.l_onoff = 0 ;
  lingeropt.l_linger = 20 ;
  j = sizeof(struct linger) ;
  setsockopt(fd, SOL_SOCKET, SO_LINGER, &lingeropt, j) ;
 }
 if(bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0) {
  perror("bind");
  exit(1);
 }
 /* Set up the pending request depth */
 if( listen(fd, 1) < 0) {
  perror("listen");
  exit(1);
 }
 while(1) { /* Do this forever */
  /*
   * Wait for connection requests ......
   */
  sockpath = accept(fd, &from, &fromlen);
  if(sockpath < 0) {
   perror("accept");
   exit(1);
  }
  // get ip name of client that connected to us
  sprintf(peer_name, "%u.%u.%u.%u",
      (unsigned char)from.sa_data[2],
      (unsigned char)from.sa_data[3],
      (unsigned char)from.sa_data[4],
      (unsigned char)from.sa_data[5]);

  fprintf(stderr,"Client connection accepted from %s\n", peer_name);
  client_connected = 1 ;
  read_socket();
  if (client_connected == 1)
  {
   process__request();
   close_client_connection();
  }
 }
} // main()

