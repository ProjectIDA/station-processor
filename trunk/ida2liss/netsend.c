/*
  Routines to send seed data out over LISS connection

  int open_socket(int portnum)
    Creates a socket and binds it to portnum.
    Return value is the file descriptor for socket.
    If -1 then there was a system error (see errno)
    If there is a bind error it will try again.
    sets static global fd to the socket fd
    This call only needs to be made once.

  int send_record(void *record, int size)
    Sends size bytes from record pointer to fd socket
     0 = client was not connected to start with, call accept_client
     1 = success
    -1 = socket closed or timed out

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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "include/dcc_std.h"
#include "include/dcc_time_proto2.h"
#include "include/netreq.h"

extern int errno;

#define perror(ster) fprintf(stderr,"Error: %s (%x)\n",ster,errno)

int client_connected=0;
static int fd, sockpath ;
static long seqnum=1;
static struct sockaddr from ;
static socklen_t fromlen = sizeof(struct sockaddr);

void close_client_connection()
{
 shutdown(sockpath, SHUT_RDWR) ;
 close(sockpath) ;
 client_connected = 0 ;
}

int send_record (void *record, int size)
{
 int i, flag, send_attempts ;
 int iSent;
 char seqbuf[7];
 char *bufptr;

 // Check for closed connection
 if (client_connected == 0) return 0;

 /* Overwrite SEED sequence number */
 sprintf(seqbuf,"%06ld",seqnum);
 if (seqnum>999999) seqnum = 1;
 memcpy(record, seqbuf, 6) ;

 /* Set up non-blocking write to the socket */
 send_attempts = 0 ;
 iSent = 0;
 bufptr = record;
 while (send_attempts < 200 && iSent < size)
 {
  ++send_attempts;
  flag = MSG_DONTWAIT ;
  i = send(sockpath, &bufptr[iSent], size-iSent, flag);
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
      return -1;
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
     return -1;
   }
  } // Error return from send
  else
  {
   iSent += i;
   if (iSent < size)
     usleep(1000);
  } // send was good
 } // while send not timed out

 // Make sure record was sent
 if (iSent < size)
 {
  fprintf(stderr,"Write too many attempts- closing connection") ;
  close_client_connection() ;
  return -1;
 }

 seqnum++;
 return 1;
} // send_record()

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

  // One time malloc of buffer space for whitelist
  if (whitelist != NULL && whitebuf == NULL)
  {
   whitebuf = malloc(strlen(whitelist)+1);
  }

  /*
   * Wait for connection requests ......
   */
  if (iDebug && whitelist)
    fprintf(stderr, "Limiting connections to hosts %s\n", whitelist);
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
       {
         fprintf(stderr, "Whitelist host '%s' not found\n", whitebuf);
       }
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
         fprintf(stderr, "Comparing host ip[%d] %s and %s\n", i, white_name, peer_name);
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
     {
       fprintf(stderr, "Client %s was not on whitelist '%s', connection closed\n",
         peer_name,
         whitelist);
     }
     close_client_connection();
     return 0;
   }
  }  // whitelist enabled

  if (iDebug)
    fprintf(stderr, "Client connection accepted from %s\n", peer_name);

  client_connected = 1 ;
  return 1;
} // accept_client()

