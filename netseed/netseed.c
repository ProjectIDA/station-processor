/*
Network server program to fill SEED data rerequests

Update History:
yyyy-mm-dd WHO - Changes
==============================================================================
2010-02-09 FCS - New diskloop.config keywords for falcon
2011-11-14 JDE - New diskloop.config keywords for telemetry control
******************************************************************************/

const char *VersionIdentString = "Release 1.2";

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"

// The maximum number of channels we could possibly expect from a station
#define MAX_CHAN 500

// Global debug flag
int  iDebug = 0;

extern int errno;
void daemonize();

int WildMatch(char *pattern, char *target);

#define perror(ster) {char *errstr=strerror(errno);fprintf(stderr,"%s: %s(%x)\n",ster,errstr,errno);syslog(LOG_ERR,"%s: %s(%x)\n",ster,errstr,errno);}

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
static struct sockaddr from ;
static socklen_t fromlen = sizeof(struct sockaddr);

/*
  Routines to send seed data out over LISS connection

  int open_socket(int portnum)
    Creates a socket and binds it to portnum.
    Return value is the file descriptor for socket.
    If -1 then there was a system error (see errno)
    If there is a bind error it will try again.
    sets static global fd to the socket fd
    This call only needs to be made once.

  int accept_client(const char *whitelist, int iDebug)
    Accepts a new client connection, waits until one is made
    whitelist is a comma separated string of IP addresses allowed to connect
    iDebug says whether to put out debuging information
     1 = success, global sockpath set to file descriptor
    -1 = failure, see errno

  void close_client_connection()
    closes down the current connection
    sets client_connected = 0 so next send will accept a new connection
*/

void close_client_connection()
{
 sleep(2); // Helps some clients who see RESET before last data
 shutdown(sockpath, SHUT_RDWR) ;
 close(sockpath) ;
 client_connected = 0 ;
fprintf(stderr, "Connection closed\n");
}

int open_socket(int port_number)
{
 struct sockaddr_in sin ;
 struct linger lingeropt ;
 int flag2 ;
 int itry;
 socklen_t j ;

 client_connected = 0 ;

 /* Create the socket to listen on */
 fd = socket(AF_INET, SOCK_STREAM, 0);
 if(fd < 0)
 {
  perror("socket");
  return -1;
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
  lingeropt.l_linger = 0 ;
  j = sizeof(struct linger) ;
  setsockopt(fd, SOL_SOCKET, SO_LINGER, &lingeropt, j) ;
 }

 // Continue to attempt bind for up to 5 minutes (6 sec * 50)
 for (itry=0; itry < 50; itry++)
 {
  if(bind(fd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0)
  {
   syslog(LOG_ERR, "Bind error, retry in 6 seconds.\n");
   fprintf(stderr, "Bind error, retry in 6 seconds.\n");
   sleep(6);
  }
  else break;
 } // for each bind attempt
 if (itry == 50)
 {
  perror("bind");
  return -1;
 }

 /* Set up the pending request depth */
 if( listen(fd, 1) < 0)
 {
  perror("listen");
  return -1;
 }

 return fd;
} // open_socket()

int accept_client(const char *whitelist, int iDebug)
{
  char *checklist;
  char peer_name[16];
  char white_name[16];
  char peer_ip[4];
  char white_ip[4];
  static char *whitebuf=NULL;
  int  found=0;
  int  i,j;
  struct hostent *whitehost;
  static int first=1;

  // One time malloc of buffer space for whitelist
  if (whitelist != NULL && whitebuf == NULL)
  {
   whitebuf = malloc(strlen(whitelist)+1);
  }

  /*
   * Wait for connection requests ......
   */
  if (whitelist && first)
  {
    if (iDebug)
      fprintf(stderr, "Limiting connections to hosts %s\n", whitelist);
    else
      syslog(LOG_INFO, "Limiting connections to hosts %s\n", whitelist);
    first = 0;
  }

  sockpath = accept(fd, &from, &fromlen);
  if(sockpath < 0)
  {
   perror("accept");
   return -1;
  }
  peer_ip[0] = from.sa_data[2];
  peer_ip[1] = from.sa_data[3];
  peer_ip[2] = from.sa_data[4];
  peer_ip[3] = from.sa_data[5];

  // get ip name of client that connected to us
  sprintf(peer_name, "%u.%u.%u.%u",
      (unsigned char)from.sa_data[2],
      (unsigned char)from.sa_data[3],
      (unsigned char)from.sa_data[4],
      (unsigned char)from.sa_data[5]);

  // Valid socket, see if user is on our whitelist
  if (whitelist != NULL)
  {
   found=0;
   checklist = (char *)whitelist;
   while (*checklist != 0 && !found)
   {
     // get next comma separated host into whitebuf, advance checklist
     for(i=0; *checklist != 0 && *checklist != ','; i++,checklist++)
       whitebuf[i] = *checklist;
     whitebuf[i] = 0;
     if (*checklist == ',') checklist++;

     // get ip address for whitebuf hostname
     whitehost = gethostbyname(whitebuf);
     if (whitehost == NULL)
     {
       // Failed to turn host into an IP address
       if (iDebug)
         fprintf(stderr, "Whitelist host '%s' not found\n", whitebuf);
       else
         syslog(LOG_ERR, "Whitelist host '%s' not found\n", whitebuf);
       continue;
     }

     // check whitehost against peer_name
     for (i=0; whitehost->h_addr_list[i] != NULL && !found; i++)
     {
       white_ip[0] = whitehost->h_addr_list[i][0];
       white_ip[1] = whitehost->h_addr_list[i][1];
       white_ip[2] = whitehost->h_addr_list[i][2];
       white_ip[3] = whitehost->h_addr_list[i][3];
       sprintf(white_name, "%u.%u.%u.%u",
           (unsigned char)white_ip[0],
           (unsigned char)white_ip[1],
           (unsigned char)white_ip[2],
           (unsigned char)white_ip[3]
        );

       if (iDebug)
         fprintf(stderr, "Comparing host ip[%d] %s and %s\n",
                 i, white_name, peer_name);
       for (j=0; j < 3 && peer_ip[j] == white_ip[j]; j++)
         ; // looking for first missmatch
       if (j == 3 && (white_ip[3] == 0 || white_ip[3] == peer_ip[3]))
       {
         found = 1;
       }
     } // for each address listed for this host
   } // while there are more hosts to check on whitelist

   // If client not found on whitelist then close clonnection
   if (!found)
   {
     if (iDebug)
       fprintf(stderr,
               "Client %s was not on whitelist '%s', connection closed\n",
               peer_name, whitelist);
     else
       syslog(LOG_INFO,
               "Client %s was not on whitelist '%s', connection closed\n",
               peer_name, whitelist);
     close_client_connection();
     return 0;
   }
  }  // whitelist enabled

  if (iDebug)
    fprintf(stderr, "Client connection accepted from %s\n", peer_name);
  else
    syslog(LOG_INFO, "Client connection accepted from %s\n", peer_name);

  client_connected = 1 ;
  return 1;
} // accept_client()

void send_string (char *msg)
{
 int i, flag, send_attempts ;
 int iSent;
 int iLength;
 char *bufptr;

 /* Set up non-blocking write to the socket */
 send_attempts = 0 ;
 iSent = 0;
 iLength = strlen(msg);
 bufptr = msg;
 while (send_attempts < 200 && iSent < iLength)
 {
  ++send_attempts;
  flag = MSG_DONTWAIT ;
  i = send(sockpath, &bufptr[iSent], iLength-iSent, flag);
//fprintf (stderr, "DEBUG Bytes sent = %i\n", i) ;
  if (i < 0)
  {
   if (errno==EAGAIN)
   {
//fprintf(stderr, "DEBUG Would block error\n") ;
    if (send_attempts>=120)
    {
      syslog(LOG_ERR, "Write timed out - closing connection") ;
      fprintf(stderr, "Write timed out - closing connection") ;
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
   if (iSent < iLength)
     usleep(100000);
  } // send was good
 } // while send not timed out

 // Make sure string was sent
 if (iSent < iLength)
 {
  fprintf(stderr,"Write too many attempts- closing connection") ;
  syslog(LOG_ERR,"Write too many attempts- closing connection") ;
  close_client_connection() ;
 }
} // send_string()

void send_record (void *seedrec)
{
 int i, flag, send_attempts ;
 int iSent;
 char seqbuf[7];
 char *bufptr;

 /* Update SEED sequence number */
 sprintf(seqbuf,"%06ld",seqnum++);
 if (seqnum>999999) seqnum = 1;
 memcpy(seedrec, seqbuf, 6) ;

 /* Set up non-blocking write to the socket */
 send_attempts = 0 ;
 iSent = 0;
 bufptr = seedrec;
 while (send_attempts < 200 && iSent < SEEDRECSIZE)
 {
  ++send_attempts;
  flag = MSG_DONTWAIT ;
  i = send(sockpath, &bufptr[iSent], SEEDRECSIZE-iSent, flag);
//fprintf (stderr, "DEBUG Bytes sent = %i\n", i) ;
  if (i < 0)
  {
   if (errno==EAGAIN)
   {
//fprintf(stderr, "DEBUG Would block error\n") ;
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
   if (iSent < SEEDRECSIZE)
     sleep(1);
  } // send was good
 } // while send not timed out

 // Make sure record was sent
 if (iSent < SEEDRECSIZE)
 {
  fprintf(stderr,"Write too many attempts- closing connection") ;
  close_client_connection() ;
 }
} // send_record()

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
 seed_header lghead ;

 /* Copy final (legacy) message to the log record */
 add_log_message ("I00000End") ;

 /* Set up the SEED header for the log record */
 lghead.seed_record_type = 'D' ;
 lghead.continuation_record = ' ' ;
 memcpy (lghead.station_id_call_letters, lgstation_id, 5) ;
 memcpy (lghead.location_id, "RQ", 2) ;
 memcpy (lghead.channel_id, "LOG", 3) ;
 memcpy (lghead.seednet, "  ", 2) ;
 lghead.yr = 0 ;
 lghead.jday = 0 ;
 lghead.hr = 0 ;
 lghead.minute = 0 ;
 lghead.seconds = 0 ;
 lghead.unused = 0 ;
 lghead.tenth_millisec = 0 ;
 lghead.samples_in_record = 0 ;
 lghead.sample_rate_factor = 0 ;
 lghead.sample_rate_multiplier = 0 ;
 lghead.activity_flags = 0 ;
 lghead.io_flags = 0 ;
 lghead.tenth_msec_correction = 0 ;
 lghead.data_quality_flags = 0 ;
 lghead.number_of_following_blockettes = lgframes ;
 lghead.tenth_msec_correction = 0 ;
 lghead.first_data_byte = 0 ;
 lghead.first_blockette_byte = 64 ;
 lghead.first_blockette_byte = htons(lghead.first_blockette_byte) ;
 memcpy(lgrec, &lghead, 48) ;

 /* Write the log record and close the connection */
 if (client_connected == 1) send_record(lgrec) ;
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

} // read_socket()

//////////////////////////////////////////////////////////////////////////////
// Routine that processes data request
int TransferData(
  const char  *rq_station,   // station name
  const char  *rq_chan,      // Channel ID
  const char  *rq_loc,       // Location ID
  STDTIME2    rq_tBeginTime, // Start time
  int         rq_iSeconds    // Seconds of data desired
  )                          // Returns the number of records actually sent
{
  int   indexFirst;
  int   indexLast;
  int   iCount;
  int   iLoopSize;
  int   i;
  int   iSeek;
  int   iRecord;
  int   iLoopRecordSize;
  STDTIME2    rq_tEndTime;
  char  str_record[8192];
  char  buf_filename[2*MAXCONFIGLINELEN+2];
  static char looperrstr[MAXCONFIGLINELEN+2];
  char  loopDir[MAXCONFIGLINELEN+1];
  char  *errmsg;
  FILE  *fp_buf;
  
  // Get ending time of data request
  rq_tEndTime = ST_AddToTime2(rq_tBeginTime, 0, 0, 0, rq_iSeconds, 0);
/*
fprintf(stderr,
"TransferData(%s %s/%s %d,%03d,%02d:%02d:%02d-%d,%03d,%02d:%02d:%02d\n",
rq_station,rq_loc,rq_chan,
rq_tBeginTime.year, rq_tBeginTime.day,
rq_tBeginTime.hour, rq_tBeginTime.minute, rq_tBeginTime.second,
rq_tEndTime.year, rq_tEndTime.day,
rq_tEndTime.hour, rq_tEndTime.minute, rq_tEndTime.second
); */

  // Get index of first and last record for this data request
  LoopRecordSize(&iLoopRecordSize);
  errmsg = GetRecordRange(rq_station, rq_chan, rq_loc,
             rq_tBeginTime, rq_tEndTime,
             &indexFirst, &indexLast, &iCount, &iLoopSize);
  if (errmsg != NULL)
  {
    fprintf(stderr, "%s\n", errmsg);
    return 0;
  }
//fprintf(stderr, "Returning %d records, index %d..%d, buf length %d, rec %d\n",
//iCount, indexFirst,indexLast,iLoopSize, iLoopRecordSize);

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

  // Loop through all data records
  for (i=0; i < iCount; i++)
  {
    iRecord = (i + indexFirst) % iLoopSize;
    iSeek = iRecord * iLoopRecordSize;

    // Seek to the record position
    fseek(fp_buf, iSeek, SEEK_SET);
    if (iSeek != ftell(fp_buf))
    {
      // If seek failed, we hit end of file, set iHigh
      sprintf(looperrstr, "TransferData: Unable to seek to %d in %s",
              iSeek, buf_filename);
      fclose(fp_buf);
      return i;
    } // Failed to seek to required file buffer position

    // Read in the data
    if (fread(str_record, iLoopRecordSize, 1, fp_buf) != 1)
    {
      sprintf(looperrstr, "TransferData: Unable to read record %d in %s",
              iRecord, buf_filename);
      fclose(fp_buf);
      return i;
    } // Failed to read record

    // Transfer the data to the requester
    if (client_connected == 0)
    {
      // Connection has closed before all of the data was sent
      sprintf(looperrstr, "TransferData: Connection closed on record %d",
              iRecord);
      fclose(fp_buf);
      return i;
    }
    send_record (str_record);
  } // loop through each record index


  // Close buffer file
  fclose(fp_buf);

  return iCount;
} // TransferData()


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

// Process DIRREQ command to get a listing of all seed channels that
// fit the pattern supplied by the command line
// shearreq host port DIRREQ station.[location-]channel <DATE start> <DATE end>
// Where location and channel can be wildcarded with * or ?.
// <DATE> is yyyy/mm/dd hh:mm:ss
// <DATE start> can be replaced with a * for earliest possible start time
// <DATE end> can be replaced with a * to indicate go until end of data
// RETURNS:
// A sequence of newline terminated lines that look like:
// 3 ANMO seed channels match: ANMO.00-BH? * *
// 00/BHZ 2008/09/03 09:55:57  2008/09/08 13:54:17  2599
// 00/BHN 2008/09/03 09:55:57  2008/09/08 13:54:17  2599
//   /BC1 2008/09/03 09:55:57  2008/09/08 13:54:17  2600
// FINISHED
//
// The two dates are the start and end times for data matching request
// The final count is the number of seed records within that time span
// If there are errors parsing anything past the DATREQ keyword you will see:
// Error: 1 parsing DIRREQ: <repeat of line sent>
// ... various messages about what was wrong, match count from above
// FINISHED
void cmd_DIRREQ()
{
 char rqlocation[4] ;
 char rqchannel[4] ;
 char drlocation[4] ;
 char drchannel[4] ;
 char homedir[MAXCONFIGLINELEN+1];
 char stationdir[MAXCONFIGLINELEN*2+1];
 char fullfile[MAXCONFIGLINELEN*2+1];
 char chaninfo[MAX_CHAN][80];
 char str[80];
 char *retstr;
 int  prslen;
 int  badval;
 int  j;
 int  namelength;
 int  iSeek;
 struct dirent *nextdir;
 DIR  *dirptr;
 FILE *fp_buf;

 long rqyear, rqmonth, rqday ;
 int rqhour, rqmin, rqsec ;
 short outyr1, outmon1, outday1, outjday1;
 short outyr2, outmon2, outday2, outjday2;
 STDTIME2 rqstart ;
 STDTIME2 rqfinish ;
 STDTIME2 drstart ;
 STDTIME2 drfinish ;
 int days_mth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;
 int indexFirst;
 int indexLast;
 int iCount;
 int iLoopSize;
 int iRateFactor;
 int iRateMult;
 int iSamples;
 int iSpanDay;
 int iSpanHour;
 int iSpanMin;
 int iSpanSec;
 int iSpanTMSec;
 double dSampleRate;
 seed_header headerstart;
 seed_header headerfinish;
 int   iLoopRecordSize;
 int   iChanCount;

 /* Parse and validate station name*/
 ++cmdcnt ;
 parse_request('.') ;
 prslen = strlen(prsbuf) ;
 if (prslen < 3 || prslen > 5)
 {
  sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
  send_string(str);
  send_string("Invalid or missing station name\n") ;
  send_string("FINISHED\n");
  return ;
 }
 memcpy(lgstation_id, prsbuf, prslen) ;
 lgstation_id[prslen] = 0;

 // Get home data directory
 if ((retstr=LoopDirectory(homedir)) != NULL)
 {
  syslog(LOG_ERR, "netreq:cdm_DIRREQ(): %s\n", retstr);
  sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
  send_string(str);
  send_string("ASP internal error, LoopDirectory() call\n") ;
  send_string("FINISHED\n");
  return;
 }

 // Add Station directory
 strcpy(stationdir, homedir);
 stationdir[strlen(homedir)] = '/';
 strcpy(&stationdir[strlen(homedir)+1], lgstation_id);
 stationdir[strlen(stationdir)+1] = 0;
 stationdir[strlen(stationdir)] = '/';

 /* Parse and validate [location-]channel */
 strcpy(rqlocation, "  ") ;
 ++cmdcnt ;
 parse_request(' ') ;
 prslen = strlen(prsbuf) ;
 badval = 0 ;
 for (j=0; j < prslen && prsbuf[j] != '-'; j++)
  ; // just search
 if (j < prslen)
 {
   // found - separating location from channel
   if (j == 0 || j > 2 || j > prslen-2 || j < prslen-4)
   {
    badval = 1;
   }
   else
   {
    strncpy(rqlocation, prsbuf, j);
    rqlocation[j] = 0;
    strncpy(rqchannel, &prsbuf[j+1], prslen-j-1);
    rqchannel[prslen-j-1] = 0;
   }
 }
 else
 {
  // No '-' specifying a location code
  if (prslen > 3 || prslen < 1)
  {
   badval = 1;
  }
 }
 if (badval == 1)
 {
fprintf(stderr, "DEBUG Invalid [location-]channel name '%s'\n", prsbuf);
  sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
  send_string(str);
  send_string("Invalid [location-]channel name\n");
  send_string("FINISHED\n");
  return ;
 }

 // get record size
 LoopRecordSize(&iLoopRecordSize);

 // open station directory
 dirptr = opendir(stationdir);
 if (dirptr == NULL)
 {
  retstr = strerror(errno);
  syslog(LOG_ERR, "cmd_DIRREQ(): Unable to open data directory: '%s'\n",
         stationdir);
fprintf(stderr, "cmd_DIRREQ(): Unable to open data directory: '%s'\n",
         stationdir);
fprintf(stderr, "opendir failed, errno=%d, '%s'\n", errno, retstr);
   sprintf(str,"Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]); 
   send_string(str);
   send_string("Unable to open station directory\n");
   send_string("FINISHED\n") ;
  return;
 }

 /* Parse and validate start time of yyyy/mm/dd */
 ++cmdcnt ;
 parse_request(' ') ;
 rqyear = 1950 ;
 rqmonth = 1 ;
 rqday = 1 ;
 rqstart = ST_CnvJulToSTD2(ST_Julian2(rqyear, rqmonth, rqday));

 if (strcmp(prsbuf, "*") != 0)
 {
  // Start date is not a * so parse it out
  iCount = sscanf(prsbuf, "%ld/%ld/%ld", &rqyear, &rqmonth, &rqday) ;
  if (iCount != 3) badval = 1;
  if (rqyear < 1980 || rqyear > 3000) badval = 1 ;
  if (rqmonth < 1 || rqmonth > 12) badval = 1 ;
  if (badval == 0)
  {
   if (rqyear % 4 == 0) days_mth[2] = 29 ;
   if (rqday < 1 || rqday > days_mth[rqmonth]) badval = 1 ;
  }
  if (badval == 1)
  {
   sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
   send_string(str);
   send_string("Invalid start date yyyy/mm/dd\n") ;
   send_string("FINISHED\n") ;
   closedir(dirptr);
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
  iCount = sscanf(prsbuf,"%d:%d:%d", &rqhour, &rqmin, &rqsec) ;
  if (iCount < 1) badval = 0;
  if (rqhour < 0 || rqhour > 23) badval = 1 ;
  if (rqmin < 0 || rqmin > 59) badval = 1 ;
  if (rqsec < 0 || rqsec > 59) badval = 1 ;
  if (badval == 1)
  {
   sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
   send_string(str);
   send_string("Invalid start time hh:mm:ss\n") ;
   send_string("FINISHED\n") ;
   closedir(dirptr);
   return ; 
  }
  rqstart.hour = rqhour ;
  rqstart.minute = rqmin ;
  rqstart.second = rqsec ;
  rqstart.tenth_msec = 0 ;
 } // if start date was not a '*'

 // Parse and validate end time of yyyy/mm/dd
 ++cmdcnt ;
 parse_request(' ') ;
 
 rqyear = 3000 ;
 rqmonth = 1 ;
 rqday = 1 ;
 rqfinish = ST_CnvJulToSTD2(ST_Julian2(rqyear, rqmonth, rqday));

 if (strcmp(prsbuf, "*") != 0)
 {
  // End date is not a * so parse it out
  iCount = sscanf(prsbuf, "%ld/%ld/%ld", &rqyear, &rqmonth, &rqday) ;
  if (iCount != 3) badval = 1;
  if (rqyear < 1980 || rqyear > 3000) badval = 1 ;
  if (rqmonth < 1 || rqmonth > 12) badval = 1 ;
  if (badval == 0)
  {
   if (rqyear % 4 == 0) days_mth[2] = 29 ;
   if (rqday < 1 || rqday > days_mth[rqmonth]) badval = 1 ;
  }
  if (badval == 1)
  {
   sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
   send_string(str);
   send_string("Invalid finish date yyyy/mm/dd\n") ;
   send_string("FINISHED\n") ;
   closedir(dirptr);
   return ;
  }
  rqfinish.year = rqyear ;
  /* Calculate days since beginning of year */
  rqfinish.day = 0 ;
  j = 0 ;
  while (j < rqmonth)
  {
   rqfinish.day = rqfinish.day + days_mth[j] ;
   ++j ;
  }
  rqfinish.day = rqfinish.day + rqday ;

  /* Parse and validate time of hh:mm:ss */
  ++cmdcnt ;
  parse_request(' ') ;
  rqhour = 100 ;
  rqmin = 100 ;
  rqsec = 100 ;
  iCount = sscanf(prsbuf,"%d:%d:%d", &rqhour, &rqmin, &rqsec) ;
  if (iCount < 1) badval = 0;
  if (rqhour < 0 || rqhour > 23) badval = 1 ;
  if (rqmin < 0 || rqmin > 59) badval = 1 ;
  if (rqsec < 0 || rqsec > 59) badval = 1 ;
  if (badval == 1)
  {
   sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
   send_string(str);
   send_string("Invalid finish time hh:mm:ss\n") ;
   send_string("FINISHED\n") ;
   closedir(dirptr);
   return ; 
  }
  rqfinish.hour = rqhour ;
  rqfinish.minute = rqmin ;
  rqfinish.second = rqsec ;
  rqfinish.tenth_msec = 0 ;
 } // if start date was not a '*'

//fprintf(stderr, "start %s ", ST_PrintDate2(rqstart, 1));
//fprintf(stderr, "end %s\n", ST_PrintDate2(rqfinish, 1));

 // Get list of directory files
 iChanCount = 0;
 while ((nextdir=readdir(dirptr)) != NULL && iChanCount < MAX_CHAN)
 {
  // Only check files that end in .buf
  namelength = strlen(nextdir->d_name);
  if (namelength < 5) continue;
  if (strcmp(&nextdir->d_name[namelength-4], ".buf") != 0)
   continue;

  // Get location and channel
  if (nextdir->d_name[2] == '_')
  {
    // location code is given
    strncpy(drlocation, nextdir->d_name, 2);
    drlocation[2] = 0;
    strncpy(drchannel, &nextdir->d_name[3], 3);
    drchannel[3] = 0;
  }
  else
  {
    // blank location
    strcpy(drlocation, "  ");
    strncpy(drchannel, nextdir->d_name, 3);
    drchannel[3] = 0;
  }
  
  // Compare location and channel allowing wildcards
  if (!WildMatch(rqlocation, drlocation))
   continue;
  if (!WildMatch(rqchannel, drchannel))
   continue;

  // Location and channel match, check time spans
  retstr = GetRecordRange(lgstation_id, drchannel, drlocation,
           rqstart, rqfinish, 
           &indexFirst, &indexLast, &iCount, &iLoopSize);
  if (retstr != NULL)
  {
   syslog(LOG_ERR, "cmd_DIRREQ(): GetRecordRange: '%s'\n",
          retstr);
   sprintf(str, "Error: 1 parsing DIRREQ: %s\n", &incmdbuf[7]);
   send_string(str);
   send_string("ASP internal error, GetRecordRange() call\n") ;
   send_string("FINISHED\n");
   closedir(dirptr);
   return;
  }
//fprintf(stderr, "Returning %d records, index %d..%d, buf length %d, rec %d\n",
//iCount, indexFirst,indexLast,iLoopSize, iLoopRecordSize);

  // Open the buffer file
  strcpy(fullfile, stationdir);
  strcpy(&fullfile[strlen(stationdir)], nextdir->d_name);
  if ((fp_buf=fopen(fullfile, "r")) == NULL)
  {
    // Unable to open buffer file so have to skip it
    retstr = strerror(errno);
    fprintf(stderr, "Unable to open '%s', %s\n", fullfile, retstr);
    syslog(LOG_ERR, "cmd_DIRREQ(): Unable to open '%s', %s\n",
           fullfile, retstr);
    continue;
  }

  // See if there is data within the requested time span
  if (iCount < 1)
  {
    fclose(fp_buf);
    continue;
  }

  // Get the actual start and finish times of the data records
  iSeek = indexFirst * iLoopRecordSize;
  fseek(fp_buf, iSeek, SEEK_SET);

  if (iSeek != ftell(fp_buf))
  {
    // If seek failed, we hit end of file, set iHigh
    fprintf(stderr, "Unable to seek to %d in '%s'\n", iSeek, fullfile);
    syslog(LOG_ERR, "cmd_DIRREQ(): Unable to seed to %d in '%s'\n", 
           iSeek, fullfile);
    fclose(fp_buf);
    continue;
  } // Failed to seek to required file buffer position

  // Read in the first seed record header
  if (fread(&headerstart, sizeof(headerstart), 1, fp_buf) != 1)
  {
    syslog(LOG_ERR, "cmd_DIRREQ(): Unable to read record %d in %s",
            indexFirst, fullfile);
    fclose(fp_buf);
    continue;
  } // Failed to read record

  // Seek to finish record
  iSeek = indexLast * iLoopRecordSize;
  fseek(fp_buf, iSeek, SEEK_SET);

  if (iSeek != ftell(fp_buf))
  {
    // If seek failed, we hit end of file, set iHigh
    fprintf(stderr, "Unable to seek to %d in '%s'\n", iSeek, fullfile);
    syslog(LOG_ERR, "cmd_DIRREQ(): Unable to seed to %d in '%s'\n", 
           iSeek, fullfile);
    fclose(fp_buf);
    continue;
  } // Failed to seek to required file buffer position

  // Read in the last seed record header
  if (fread(&headerfinish, sizeof(headerfinish), 1, fp_buf) != 1)
  {
    syslog(LOG_ERR, "cmd_DIRREQ(): Unable to read record %d in %s",
            indexLast, fullfile);
    fclose(fp_buf);
    continue;
  } // Failed to read record

  // Done with this file, close it
  fclose(fp_buf);

  // Get start time for start record
  drstart.year = ntohs(headerstart.yr);
  drstart.day = ntohs(headerstart.jday);
  drstart.hour = (int)(headerstart.hr);
  drstart.minute = (int)(headerstart.minute);
  drstart.second = (int)(headerstart.seconds);
  drstart.tenth_msec = ntohs(headerstart.tenth_millisec);

  // Get end time for last record
  drfinish.year = ntohs(headerfinish.yr);
  drfinish.day = ntohs(headerfinish.jday);
  drfinish.hour = (int)(headerfinish.hr);
  drfinish.minute = (int)(headerfinish.minute);
  drfinish.second = (int)(headerfinish.seconds);
  drfinish.tenth_msec = ntohs(headerfinish.tenth_millisec);

  // Calculate time span of last record
  iSamples = (unsigned short)ntohs(headerfinish.samples_in_record);
  iRateFactor = (short)ntohs(headerfinish.sample_rate_factor);
  iRateMult = (short)ntohs(headerfinish.sample_rate_multiplier);

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

  // Get Span time in tenths of milliseconds
  if (iRateFactor != 0 && iRateMult != 0)
    iSpanTMSec = (int)((iSamples / dSampleRate) * 10000.0);
  else
    iSpanTMSec = 0;

  // Add to start time to get end time
  iSpanSec = iSpanTMSec/10000;
  iSpanTMSec = iSpanTMSec % 10000;
  iSpanMin = iSpanSec / 60;
  iSpanSec = iSpanSec % 60;
  iSpanHour = iSpanMin / 60;
  iSpanMin = iSpanMin % 60;
  iSpanDay = iSpanHour / 24;
  iSpanHour = iSpanHour % 24;
  drfinish = ST_AddToTime2(drfinish,
          iSpanDay, iSpanHour, iSpanMin, iSpanSec, iSpanTMSec);
  if (drfinish.tenth_msec > 0)
  {
    drfinish.second++;
    drfinish.tenth_msec = 0;
  }
//fprintf(stderr, "DEBUG %s.%s/%s count %d %s ",
//lgstation_id, drlocation, drchannel, iCount, ST_PrintDate2(drstart, 1));
//fprintf(stderr, " %s\n", ST_PrintDate2(drfinish, 1));

  ST_CnvJulToCal2(ST_GetJulian2(drstart),
     &outyr1, &outmon1, &outday1, &outjday1);
  ST_CnvJulToCal2(ST_GetJulian2(drfinish),
     &outyr2, &outmon2, &outday2, &outjday2);

  sprintf(chaninfo[iChanCount], 
"%2.2s/%3.3s %04d/%02d/%02d %02d:%02d:%02d %04d/%02d/%02d %02d:%02d:%02d %d\n",
         drlocation, drchannel, outyr1, outmon1, outday1,
         drstart.hour, drstart.minute, drstart.second,
         outyr2, outmon2, outday2,
         drfinish.hour, drfinish.minute, drfinish.second,
         iCount);

  iChanCount++;
 } // while more files in directory to check

 // Done with directory so close it
 closedir(dirptr);

 // Send data to the requestor
 sprintf(str, "%d seed channels match: %s\n", iChanCount, &incmdbuf[7]);
 send_string(str);
 for (j=0; j < iChanCount && client_connected; j++)
 {
   send_string(chaninfo[j]);
 }
 if (client_connected)
  send_string("FINISHED\n");
 
// 3 ANMO seed channels match: ANMO.00-BH? * *
} // cmd_DIRREQ()

void process__request()
{
 int j, prslen ;
 int badval, count ;
 char rqlogmsg[120] ;
 char rqstation[6] ;
 char rqlocation[4] ;
 char rqchannel[4] ;
 long rqyear, rqmonth, rqday, rqdur ;
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

 if (iDebug)
  fprintf(stderr, "Request: '%s'\n", incmdbuf);
 // Check that the first word is 'DATREQ' or 'DIRREQ' - if not, 
 // close connection to probers
 cmdcnt = 0 ;
 parse_request(' ') ;
 if (strcmp(prsbuf, "DATREQ") != 0 && strcmp(prsbuf, "DIRREQ") != 0)
 {
  sleep(60);
  close_client_connection() ;
  return ;
 }
 if (!iDebug)
   syslog(LOG_INFO, "Request: '%s'\n", incmdbuf);

 
 // Handle DIRREQ command seperately
 if (strcmp(prsbuf, "DIRREQ") == 0)
 {
   cmd_DIRREQ();
   close_client_connection() ;
   return;
 }

 /* Parse and validate station name*/
 ++cmdcnt ;
 parse_request('.') ;
 prslen = strlen(prsbuf) ;
 if (prslen < 3 || prslen > 5)
 {
  syslog(LOG_ERR, "Invalid or missing station name");
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
  syslog(LOG_ERR, "Invalid [location-]channel name") ;
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
 if (rqyear < 1980 || rqyear > 3000) badval = 1 ;
 if (rqmonth < 1 || rqmonth > 12) badval = 1 ;
 if (badval == 0)
 {
  if (rqyear % 4 == 0) days_mth[2] = 29 ;
  if (rqday < 1 || rqday > days_mth[rqmonth]) badval = 1 ;
 }
 if (badval == 1)
 {
  syslog(LOG_ERR, "Invalid time yyyy/mm/dd") ;
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
  syslog(LOG_ERR, "Invalid time hh:mm:ss") ;
  add_log_message("Invalid time hh:mm:ss") ;
  write_log_record() ;
  return ; 
 }
 rqstart.hour = rqhour ;
 rqstart.minute = rqmin ;
 rqstart.second = rqsec ;
 rqstart.tenth_msec = 0 ;

 /* Parse and validate duration  */
 ++cmdcnt ;
 parse_request(' ') ;
 rqdur = 0 ;
 count = sscanf(prsbuf,"%ld", &rqdur) ;
 if (rqdur < 1)
 {
  syslog(LOG_ERR, "Invalid duration") ;
  add_log_message("Invalid duration") ;
  write_log_record() ;
  return ;
 }

 /* Call the diskloop handler to find and send any available data
    Pass rqstation, rqlocation, rqchannel, rqstart, rqdur */
 TransferData(rqstation,rqchannel,rqlocation, rqstart,rqdur);

 if (client_connected == 0) return ;
 if (seqnum == 1)
  add_log_message ("No data was available") ;
 else
  add_log_message ("Request for data was fufilled") ;
 write_log_record() ;
} // process__reqeust()

void ShowUsage()
{
  fprintf(stderr,"Usage: netseed <port> [debug]\n");
  fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
} // ShowUsage()

int main(argc,argv)
char *argv[];
int argc;
{
 int port_number ;
 char *retmsg ;
 char *whitelist = NULL;
 int  iReturn;

 if (argc < 2 || argc > 3)
 {
  ShowUsage();
  exit(100);
 }

 if (argc == 3 && strcmp(argv[2], "debug") != 0)
 {
  ShowUsage();
  exit(100);
 }
 if (argc == 3)
  iDebug = 1;

 port_number = atol(argv[1]);

 client_connected = 0 ;

 // Parse the q330driver configuration file
 if ((retmsg=ParseDiskLoopConfig(
              "/etc/q330/DLG1/diskloop.config")) != NULL)
 {
  if (iDebug)
    fprintf(stderr, "%s: %s\n", argv[0], retmsg);
  else
    syslog(LOG_ERR, "%s[Exit]: %s\n", argv[0], retmsg);
  exit(1);
 }

 signal(SIGPIPE, SIG_IGN);

 // Set up to run program as a daemon
 if (!iDebug)
   daemonize();

 // open request socket
 if (open_socket(port_number) < 0)
 {
   fprintf(stderr, "Failed to open socket on port %d\n", port_number);
   exit(1);
 }

 // endless loop waiting for connection requests
 while(1)
 {
  /*
   * Wait for connection requests ......
   */
    iReturn = accept_client(whitelist, iDebug);

    // If client was not on whitelist then try again
    if (iReturn == 0)
      continue;

    if (iReturn < 0)
    {
      // No longer able to accept connections, exit
      fprintf(stderr, "No longer able to accept connections, exiting!\n");
      syslog(LOG_ERR, "No longer able to accept connections, exiting!\n");
      exit(1);
    }

  read_socket();
  if (client_connected == 1) process__request();
 }
} // main()

