#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "include/diskloop.h"
#include "include/q330arch.h"

static int    iSeqNum=0;
static int    bFirst=1;
static int    iSeedRecordSize=0;
static int    iLogPort=0;
static int    iSocket=-1;
static char   errmsgbuf[512];

//////////////////////////////////////////////////////////////////////////////
// Sends a Seed message record to the q330arch server
char *q330SeedSend(void *seed_record)
{
  char  *errmsg;

  // One time initialization
  if (bFirst)
  {
    if ((errmsg = LoopRecordSize(&iSeedRecordSize)) != NULL)
      return errmsg;
    if ((errmsg = LogServerPort(&iLogPort)) != NULL)
      return errmsg;
    bFirst = 1;
  } // one time initialization

  // Make sure socket is open
  if (iSocket < 0)
  {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // open socket
    iSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (iSocket < 0)
    {
      sprintf(errmsgbuf, "socket call error %d, %s",
        errno, strerror(errno));
      return errmsgbuf;
    }

    // Set up server info
    server = gethostbyname("localhost");
    if (server == NULL)
    {
      close(iSocket);
      iSocket = -1;
      return ("gethostbyname failed to find 'localhost'");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(iLogPort);

    // Make connection
    if (connect(iSocket, (struct sockaddr *)&serv_addr,
        sizeof(serv_addr)) < 0)
    {
      close(iSocket);
      iSocket = -1;
      return "Unable to connect to logserver";
    }
  } // socket needs to be opened

  // Send record
  if (send(iSocket, seed_record, iSeedRecordSize, 0) != iSeedRecordSize)
  {
    sprintf(errmsgbuf, "send q330arch failed, errno %d %s",
      errno, strerror(errno));
    close(iSocket);
    iSocket = -1;
    return errmsgbuf;
  }
  return NULL;
} // q330SeedSend()

//////////////////////////////////////////////////////////////////////////////
// Formats a null terminated string into a SEED message record
// Message is then sent to the q330arch server
char *q330LogMsg(const char *msg,
                const char *station, const char *network,
                const char *channel, const char *location)
{
  char  *errmsg;
  char  seedRecord[8192];    // Buffer to store seed record

  // One time initialization
  if (bFirst)
  {
    if ((errmsg = LoopRecordSize(&iSeedRecordSize)) != NULL)
      return errmsg;
    if ((errmsg = LogServerPort(&iLogPort)) != NULL)
      return errmsg;
  } // one time initialization

  // convert msg into a seed record
  MakeSeedMsg(msg, ++iSeqNum, station, network, channel, location,
                iSeedRecordSize, seedRecord);

  // Call routine to send seed log message
  return q330SeedSend(seedRecord);
} // q330LogMsg()

//////////////////////////////////////////////////////////////////////////////
// Close socket port to q330arch server
char *q330Close()
{
  if (iSocket >= 0)
    close(iSocket);
  iSocket = -1;

  return NULL;
} // closes the q330arch socket connection

