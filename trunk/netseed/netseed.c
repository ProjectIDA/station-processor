/*
  Network server program to fill SEED data rerequests
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>
#include "include/diskloop.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"

extern int errno;
void daemonize();

#define perror(ster) fprintf(stderr,"Error: %s (%x)\n",ster,errno)

int client_connected=0;
char incmdbuf[80];
char prsbuf[80] ;
char lgstation_id[5] ;
unsigned char lgrec[SEEDRECSIZE];
int fd, sockpath ;
long seqnum ;
char lgframes ;
int lgcount ;
int cmdcnt, cmdlen ;

void close_client_connection()
{
 sleep(2); // Helps some clients who see RESET before last data
 shutdown(sockpath, SHUT_RDWR) ;
 close(sockpath) ;
 client_connected = 0 ;
}

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

}

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
);
*/
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
 for (j=0; j<5; j++) lgstation_id[j] = ' ' ;
 for (j=lgcount; j<SEEDRECSIZE; j++) lgrec[j] = ' ' ; 
 memcpy (rqlogmsg, "Request of \"", 12) ;
 cmdlen = strlen(incmdbuf) ;
 memcpy (&rqlogmsg[12], incmdbuf, cmdlen) ;
 j = cmdlen + 12 ;
 strcpy (&rqlogmsg[j], "\" received") ;
 add_log_message(rqlogmsg) ;

 /* Check that the first word is 'DATREQ' - if not, close connection
  to probers */
 cmdcnt = 0 ;
 parse_request(' ') ;
 if (strcmp(prsbuf, "DATREQ") != 0)
 {
  close_client_connection() ;
  return ;
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
 memcpy(lgstation_id, prsbuf, prslen) ;
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
     memcpy(rqchannel, &prsbuf[2], 4) ;
    }
   else
    badval = 1 ;
   break ;
  case 6 :
   if (prsbuf[2] == '-')
   {
    memcpy(rqlocation, prsbuf, 2) ;
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

 /* Parse and validate duration (limit of four hours of data) */
 ++cmdcnt ;
 parse_request(' ') ;
 rqdur = 0 ;
 count = sscanf(prsbuf,"%ld", &rqdur) ;
 if (rqdur < 1 || rqdur > 14400)
 {
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

 if (argc!=2) {
  fprintf(stderr,"Missing port argument\n");
  exit(100);
 }
 port_number = atol(argv[1]);
 fprintf(stderr,"Starting on port number %s\n", argv[1]);

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
  lingeropt.l_linger = 0 ;
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
  /* fprintf(stderr,"Client connection accepted\n"); */
  client_connected = 1 ;
  read_socket();
  if (client_connected == 1) process__request();
 }
} // main()

