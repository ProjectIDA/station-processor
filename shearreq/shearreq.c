/*
 * CLIENT
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define RECSIZE 512

int on = 1;
int off = 0;
struct linger l;
int rcv;

void Upcase(char *msg)
{
  int i;
  for (i=0; msg[i] != 0; i++)
    msg[i] = (char)toupper(msg[i]);
}

/*
 * We need to define a port number on which the server is listening
 */

int main(int argc,char **argv)
{
  struct sockaddr_in sin;
  struct hostent *hp, *gethostbyname();
  char host[RECSIZE];
  int fd, count, wcount;
  char buf[RECSIZE];
  char savebuf[RECSIZE];
  int port;
  int total=0;

  char cmd[100];
  char chan[100];
  
  if (argc!=8)
  {
    fprintf(stderr,"Usage: %s host port DATREQ station.[location-]channel yyyy/mm/dd hh:mm:ss duration(secs)\n",argv[0]);
    exit(10);
  }

  strcpy(cmd,argv[3]);
  Upcase(cmd);

  strcpy(chan,argv[4]);
  Upcase(chan);

  strcpy(host,argv[1]);

  port = atoi(argv[2]);
  
  /*
   * Translate the hostname into an internet address
   */
  hp = gethostbyname(host);
  if(hp == 0) {
    fprintf(stderr, "%s: unknown host\n",host);
    exit(1);
  }
  /*
   * We now create a socket which will be used for our connection.
   */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd < 0) {
    perror("socket");
    exit(1);
  }
  /*
   *  Before we attempt a connect, we fill up the "sockaddr_in" structure
   */
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_family      = AF_INET;
  sin.sin_port        = htons(port);
  /*
   *  Now attempt to connect to the remote host .....
   */
  if(connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("connect");
    exit(1);
  }
	
  sprintf(buf,"%s %s %s %s %d\n",
	  cmd,chan,argv[5],argv[6],atoi(argv[7]));

  fprintf(stderr,"Command: %s\n",buf);
  count = strlen(buf);
  
  /*
   * Now write to the network
   */

  wcount = write(fd, buf, count);
  if(wcount != count) {
    /* Error on writing to the network */
    perror("Net write");
    exit(1);
  }
  while (1) {
    /*
     * Read from the network
     */
    count = read(fd, buf, sizeof(buf));
    if(count < 0)
    {
      /* Unable to read from the terminal */
      perror("network read");
      exit(1);
    }
    total += count;
			
    if (count==0)
    {
      fprintf(stderr, "Exit on read 0 bytes\n");
      break;
    }

    // Save copy of buffer
    memcpy(savebuf, buf, sizeof(buf));

    /*
     * Now write to the terminal
     */
    wcount = write(1, buf, count);
    if(wcount != count) {
      /* Error on writing to the network */
      perror("Terminal write");
      exit(1);
    }
  } // while full records are read

  fprintf(stderr,"Read %d records\n",
          total/RECSIZE);

  return 0;
} // main()

