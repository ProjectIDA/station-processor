/*======================================================================
Author:  Frank Shelly  USGS Albuquerque Seismological Laboratory
Date:    2 January 2008
Purpose: convert IDA isi sever feed to a LISS feed
         Modified from isi general purpose client
         Turned into a multithread application
           Main thread fills shared memory circular buffer with disk loop data
           Listen thread accepts connection requests
           LISS threads send data from shared circular buf to clients
           Number of LISS threads is #define MAXCLIENT limited, usually 1
           NOTE: Only supports single client.  If the client disconnects
             then it will pick up back where it left off when it reconnects
========================================================================*/
#define RELEASE "1.1"
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isi.h"
#include "util.h"
#include "include/netreq.h"
#include "include/dcc_std.h"
#include "include/dcc_time.h"
#include "include/dcc_time_proto2.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#define VERBOSE_REPORT  0x00000001
#define VERBOSE         (flags & VERBOSE_REPORT)

// This code only supports one client
#define MAXCLIENT       1
#define SEED_RECLEN     512

char *VersionIdentString="ida2liss 1.1";
const char *WHOAMI="ida2liss";

#define LOCALBUFLEN SEED_RECLEN

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "localhost"
#endif /* DEFAULT_SERVER */

#define UNKNOWN 0
#define RAW     1

UINT32 flags   = 0;
char *server   = DEFAULT_SERVER;

char *whitelist=NULL;
char *filter=NULL;
int  keepalive=0;

extern int open_socket(int portnum);
extern int accept_client(const char *whitelist, int debug);
extern int send_record(void *record, int size);
extern int WildMatch(char *pattern, char *target);

// Shared memory structure for passing info between threads
// iOldest and iNext are controlled by main program unless
// depth == -1 in which case iOldest is controlled by ListenThread
struct s_mapshm
{
  int  bQuit;          // Tells threads to gracefully terminate
  int  depth;          // depth= parameter from commandline
  int  iDebug;         // Debug verobsity level 0,1,2,3
  int  iPort;          // Port that client will connect to
  int  iRecords;       // Max number of records in seedrec
  int  iOldest;        // Oldest valid seed record index
  int  iNext;          // Next usable seed index, if iOldest==iNext then empty
  pthread_t listen_tid;              // Thread id of ListenThread
  char *seedrec;
}; // struct s_mapshm

//////////////////////////////////////////////////////////////////////////////
// Parses str_header to get station, channel, location
char *SeedHeaderSNLC(
const void    *str_header,  // Header buffer
char          *station,     // return station name
char          *chan,        // return Channel ID
char          *loc          // return Location ID
  )
{
  int         i;  
  seed_header *pheader;

  pheader = (seed_header *)str_header;

  // Parse station name
  for (i=0; i < 5
       && isalnum((int)pheader->station_id_call_letters[i]); i++)
    station[i] = pheader->station_id_call_letters[i];
  station[i] = 0;

  // Parse location name
  for (i=0; i < 2; i++)
    loc[i] = pheader->location_id[i];
  loc[i] = 0;


  // Parse channel name
  for (i=0; i < 3; i++)
    chan[i] = pheader->channel_id[i];
  chan[i] = 0;

  return NULL;
} // SeedHeaderSNLC()


//////////////////////////////////////////////////////////////////////////////
// ListenThread()
//   Accepts new client connections
//   whitelist if not null will limit who can connect
//   Sends data from circular buffer to the LISS client
//   The circular buffer depth is set by depth=argument
//   If depth = -1 then the main program will never overwrite old data
//     In this case it is up to thread to update iOldest index
//   If keepalive > 0 then it will insert dummy keepalive seed records to make
//     sure a packet gets sent every keepalive seconds
void *ListenThread(void *params)
{
  int iReturn;
  int iSend;
  char    station[8];
  char    chan[8];
  char    loc[8];
  char    aliverec[4096];
  struct s_mapshm *mapshm;
  STDTIME2 lastsend;
  STDTIME2 now;
  DELTA_T2 diffTime;
  long     tmsDiff;


  // Get pointer to shared memory area
  mapshm = (struct s_mapshm *)params;

  // open LISS socket
  if (open_socket(mapshm->iPort) < 0)
  {
    fprintf(stderr, "Failed to open socket on port %d\n", mapshm->iPort);
    exit(1);
  }
  if (mapshm->iDebug)
    fprintf(stderr, "Opened socket on port %d\n", mapshm->iPort);

  // Infinite loop accepting new client requests
  while (!mapshm->bQuit)
  {
    // Accept a new client connection
    iReturn = accept_client(whitelist, mapshm->iDebug);

    // If client was not on whitelist then try again
    if (iReturn == 0)
      continue;

    if (iReturn < 0)
    {
      // No longer able to accept connections, exit
      fprintf(stderr, "No longer able to accept connections, exiting!\n");
      mapshm->bQuit = 1;
      exit(1);
    }

    // remember when connection made for keepalive purposes
    lastsend = ST_GetCurrentTime2();
  
    // Send data from buffer until there is a send error
    iReturn = 1;
    while (iReturn > 0 && !mapshm->bQuit)
    {
      // Wait until there is data to send, or keepalive time passes
      while (mapshm->iNext == mapshm->iOldest && iReturn > 0)
      {
        // See if we need to send keepalive packet
        if (keepalive > 0)
        {
          now = ST_GetCurrentTime2();
          diffTime = ST_DiffTimes2(now, lastsend);
          tmsDiff = ST_DeltaToMS2(diffTime);

          if (tmsDiff > keepalive * 10000)
          {
            // We need to send a keepalive packet
            memset(aliverec, ' ', SEED_RECLEN);
            memset(aliverec, '0', 6);
            iReturn = send_record(aliverec, SEED_RECLEN);
            if (mapshm->iDebug)
              fprintf(stdout, "Sent keepalive packet\n");

            // remember when data sent for keepalive purposes
            lastsend = ST_GetCurrentTime2();

          } // send keepalive packet
        }
        usleep(100);
      }

      // send the oldest record
      iSend = mapshm->iOldest;
      iReturn = send_record(&mapshm->seedrec[iSend*SEED_RECLEN],
                            SEED_RECLEN);

      // Update pointers on success
      if (iReturn > 0)
      {
        if (mapshm->iDebug)
        {
          SeedHeaderSNLC(&mapshm->seedrec[iSend*SEED_RECLEN], station,chan,loc);
/* DEBUG
          fprintf(stdout, "Sent %d bytes %s %s/%s %6.6s\n",
                SEED_RECLEN, station, chan, loc, 
                &mapshm->seedrec[iSend*SEED_RECLEN]);
          fprintf(stderr, "Send iOldest=%d, iNext=%d, records=%d\n",
                mapshm->iOldest, mapshm->iNext, mapshm->iRecords);
*/
        } // user wants debug info

        // Update to new oldest record in circular buffer
        mapshm->iOldest = (mapshm->iOldest+1) % mapshm->iRecords;

        // remember when data sent for keepalive purposes
        lastsend = ST_GetCurrentTime2();

      } // No errors sending data
    } // while no read errors and not done
  } // while not done

  return NULL;
} // ListenThread()

//////////////////////////////////////////////////////////////////////////////
// Creates the shared memory segment used for inter fork communications
char *MapSharedMem(int iSize, struct s_mapshm **mapshm)
{
    
   int   shmid;
   static char *unixadr = NULL;
   static char errbuf[1024];
     
   if (unixadr != NULL && unixadr != 0)
   {
     *mapshm = (struct s_mapshm *)unixadr;
     return NULL; 
   }
    
   /* Attach the shared memory segment */
   if ((shmid = shmget(IPC_PRIVATE, iSize,
         IPC_CREAT | 00666))==-1)
   {
      sprintf(errbuf, "shmget Q330ARCH failure, error %d: %s.",
            errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

   if ((int)(unixadr = (char *)shmat(shmid,0,SHM_R | SHM_W))==-1)
   {
      sprintf(errbuf, "%s: shmat Q330ARCH failure, error %d: %s.",
            WHOAMI, errno, strerror(errno));
      fprintf(stderr, "%s\n", errbuf);
      return errbuf;
   }

  *mapshm = (struct s_mapshm *)unixadr;
  return NULL;
} // MapSharedMem()

//////////////////////////////////////////////////////////////////////////////
// Compare a location/channel string against the filter string
// filter_str contains a comma separated list of location/channel pairs
// chan_str contatins a single location/channel
// Wildcards *,? are allowed in filter_str, not in chan_str
// Returns 1 if filter_str is NULL
// Returns 1 if chan_str matches one of the filters
// Returns 0 if chan_str does not match one of the filters
// Returns -1 if there is a syntax error in filter_str
int check_filter(const char *filter_str, const char *chan_str)
{
  char *entry_ptr;
  char entry_str[16];
  char pattern_str[4];
  char target_str[4];
  int  i,j;

  if (filter_str == NULL)
    return 1;

  // Loop through each entry in filter_str
  for (entry_ptr = (char *)filter_str; *entry_ptr != 0;) 
  {
    // Copy entry
    for (i=0; *entry_ptr != 0 && *entry_ptr != ','; i++, entry_ptr++)
    {
      entry_str[i] = *entry_ptr;
      if (i > 5) return -1; // loc/channel is never more than 6 chars
    }
    if (*entry_ptr == ',') entry_ptr++;
    entry_str[i] = 0;
    if (i < 2) return -1;  // shortest legal filter is "/*"

    // Compare locations
    strncpy(target_str, chan_str, 2);
    target_str[2] = 0;

    for (i=0; i < 2; i++)
    {
      if (entry_str[0] == '/')
      {
        strcpy(pattern_str, "  ");
        j = 2;
        break;
      }

      if (entry_str[i] != '/')
      {
        pattern_str[i] = entry_str[i];
      }

      if (entry_str[i] == '/') break;
    } // Copy pattern location
    pattern_str[i] = 0;
    if (entry_str[i] != '/') return -1;  // Syntax error in filter location

    if (!WildMatch(pattern_str, target_str)) continue;

    // Match channel strings
    strncpy(target_str, &chan_str[3], 3);
    target_str[3] = 0;

    strncpy(pattern_str, &entry_str[i+1], 3);
    pattern_str[3] = 0;
    if (strlen(entry_str) - i > 4)
    {
      fprintf(stderr, "check_filter entry_str '%s' %d > %d+%d+1\n",
              entry_str, strlen(entry_str), i,4);
      return -1;  // channel name too long
    }

    if (WildMatch(pattern_str, target_str))
    {
     if (VERBOSE)
       fprintf(stderr, "Matched filter '%s' against '%s'\n",
              entry_str, chan_str);
      return 1;
    }
  } // loop through all filter pairs

  // If we get here then there was no match
  return 0;
} // check_filter()

static ISI_DATA_REQUEST *BuildRawRequest(int compress, ISI_SEQNO *begseqno, ISI_SEQNO *endseqno, char *SiteSpec)
{
ISI_DATA_REQUEST *dreq;

    if ((dreq = isiAllocSimpleSeqnoRequest(begseqno, endseqno, SiteSpec)) == NULL) {
        fprintf(stderr, "isiAllocSimpleSeqnoRequest: %s\n", strerror(errno));
        exit(1);
    }
    isiSetDatreqCompress(dreq, compress);

    return dreq;
} // BuildRawRequest()

static BOOL ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw)
{
  int     status;

    while ((status = isiReadFrame(isi, TRUE)) == ISI_OK)
    {
        if (isi->frame.payload.type != ISI_IACP_RAW_PKT)
        {
            fprintf(stderr, "unexpected type %lud packet ignored\n", isi->frame.payload.type);
        }
        else
        {
            isiUnpackRawPacket(isi->frame.payload.data, raw);
            return TRUE;
        }
    }

    switch (status) {
      case ISI_DONE:
        fprintf(stderr, "request complete\n");
        break;

      case ISI_BREAK:
        fprintf(stderr, "server disconnect\n");
        break;

      default:
        perror("isiReadFrame");
        break;
    }

    return FALSE;
} // ReadRawPacket()

static void raw(char *server, ISI_PARAM *par, int compress, ISI_SEQNO *begseqno, ISI_SEQNO *endseqno, char *SiteSpec, struct s_mapshm *mapshm)
{
  ISI *isi;
  ISI_RAW_PACKET raw;
  ISI_DATA_REQUEST *dreq;
  UINT8 buf[LOCALBUFLEN];
  UINT64 count = 0;
  char    station[8];
  char    chan[8];
  char    loc[8];
  char    loc_station[16];

    dreq = BuildRawRequest(compress, begseqno, endseqno, SiteSpec);

    if (VERBOSE) {
        fprintf(stderr, "Client side data request\n");
        isiPrintDatreq(stderr, dreq);
    }

    if ((isi = isiInitiateDataRequest(server, par, dreq)) == NULL) {
        if (errno == ENOENT) {
            fprintf(stderr, "can't connect to server %s, port %d\n", server, par->port);
        } else {
            perror("isiInitiateDataRequest");
        }
        exit(1);
    }

    isiFreeDataRequest(dreq);

    if (VERBOSE) {
        fprintf(stderr, "Server expanded data request\n");
        isiPrintDatreq(stderr, &isi->datreq);
    }

    while (ReadRawPacket(isi, &raw))
    {
        ++count;
//      if (VERBOSE) fprintf(stderr, "%s\n", isiRawHeaderString(&raw.hdr, buf));
        if (!isiDecompressRawPacket(&raw, buf, LOCALBUFLEN)) {
            fprintf(stderr, "isiDecompressRawPacket error\n");
        } // else if (VERBOSE) {
//            utilPrintHexDump(stderr, raw.payload, 64);
//      }

        // See if location/channel match our filter string
        SeedHeaderSNLC(raw.payload, station,chan,loc);
        sprintf(loc_station, "%2.2s/%3.3s", loc, chan);
        if (check_filter(filter, loc_station) != 1)
          continue;

        if (mapshm->iDebug)
        {
          fprintf(stderr, "Read iOldest=%d, iNext=%d, records=%d, %s %s/%s\n",
             mapshm->iOldest, mapshm->iNext, mapshm->iRecords, station, loc, chan);
        }

        // See if we are going to be overwriting old data
        if ( ((mapshm->iNext+1) % mapshm->iRecords) == mapshm->iOldest)
        {
          // See if we overwrite or pause
          if (mapshm->depth < 0)
          {
            // Pause until adding record won't cause overwrite
            if (mapshm->iDebug && ( ((mapshm->iNext+1) % mapshm->iRecords) 
                                    == mapshm->iOldest) )
              fprintf(stderr, "Blocking on full circular buffer\n");
            while ( ((mapshm->iNext+1) % mapshm->iRecords) == mapshm->iOldest)
              sleep(1);
          } // we pause
          else // we are in overwrite mode
          {
            // Remove oldest record
            // Worst case is a race condition could cause us to remove more
            // than one old record so I don't worry about it
            if (mapshm->iDebug)
              fprintf(stderr, "Overwriting oldest record %d\n", mapshm->iOldest);
            mapshm->iOldest = (mapshm->iOldest+1) % mapshm->iRecords;
          }
        } // if a data overwrite was going to occur

        // Save this record to buffer
        memcpy(&mapshm->seedrec[mapshm->iNext * SEED_RECLEN], raw.payload,
               SEED_RECLEN);
        mapshm->iNext = (mapshm->iNext+1) % mapshm->iRecords;

        // Give send process a chance to start send operation
        usleep(200);

    } // While no errors reading from ida disk loop

    fprintf(stderr, "%llu packets received\n", count);
} // raw()

static void help(char *myname)
{
static char *VerboseHelp = 
"Program to serve a single liss feed using IDA disk loop as a source.\n"
"The IDA disk loop must contain 512 byte seed records\n"
"\n"
"The arguments in [ square brackets ] are optional:\n"
"\n"
"   -v - turns on verbose isi output\n"
"    server=string - sets the name of the server to the specified string.\n"
"    isiport=int - sets the isi server port number\n"
"    lissport=int - sets the liss port number\n"
"    log=string - turns on library logging using. 'string' is either the\n"
"                 name of the log file, '-' for stdout, or 'syslogd:facility'\n"
"    debug=int - sets the debug level (0, 1, 2)\n"
"    depth=int - -1 default means buffer depth is as deep as ida disk loop\n"
"                only buffer <depth> records with oldest being discarded\n"
"    filter=string - What location/channels to send, default is */*\n"
"                    Format is one or more location/channel entries\n"
"                    separated by commas, * and ? wildcards allowed\n"
"                    For a blank location code start with a /\n"
"    whitelist=string - hosts that are allowed to connect\n"
"                       Empty (default) whitelist means all hosts\n"
"                       A zero in last ip address means subnet access allowed\n"
"                       Use comma to separate multiple hosts/addresses\n"
"    keepalive=int - Send a keepalive seed packet every int seconds\n"
"                    If argument is not present, none get sent\n"
"\n"
"Data feeds are done using 'raw' ISI (sequence number based) disk loops.\n"
"A 'raw' feed defaults to all packets from all data sources, starting with the\n"
"most recent packet and continuing indefinitely.  The optional '=spec' suffix\n"
"allows you to select specific data sources.  For station systems this is not\n"
"usually needed, but it is if requesting data from a hub and you only want to\n"
"get packets from a particular station.  You can specify start and end points\n"
"of the request using the optional 'beg=str' and 'end=str' arguments.  In this\n"
"case the format of the 'str' string is the desired sequence number given in\n"
"hex. For example:\n"
"\n"
"ida2liss raw=pfo beg=45b566150000000000a613a0 end=45b5661500000000009ea391\n"
"\n";

    fprintf(stderr,"Usage: %s ", myname);
    fprintf(stderr, "[ -v server=string isiport=int log=string debug=int ] ");
    fprintf(stderr, "[ depth=int filter=string whitelist=string keepalive=int]");
    fprintf(stderr, "lissport=int raw[=spec] [beg=str end=str]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", VerboseHelp);
    fprintf(stderr, "default server is `%s'\n", DEFAULT_SERVER);
    fprintf(stderr, "Version %s  %s\n", RELEASE, __DATE__);
    exit(1);

} // help()

int main(int argc, char **argv)
{
int i, request, isiport;
int debug = 0;
int lissport=4001;
int depth=-1;
int iRecords;
ISI_PARAM par;
char *req = NULL;
char *log;
char *SiteSpec = NULL;
char *begstr = NULL;
char *endstr = NULL;
char *retstr = NULL;
ISI_SEQNO begseqno = ISI_NEWEST_SEQNO;
ISI_SEQNO endseqno = ISI_NEVER_SEQNO;
char *tmpstr;
int compress  = ISI_COMP_NONE;
int format    = ISI_FORMAT_GENERIC;
struct s_mapshm *mapshm=NULL;

    utilNetworkInit();
    isiInitDefaultPar(&par);

    signal(SIGPIPE, SIG_IGN); // client connection is closed

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            server = argv[i] + strlen("server=");
        } else if (strncmp(argv[i], "isiport=", strlen("isiport=")) == 0) {
            isiport = atoi(argv[i] + strlen("isiport="));
            isiSetServerPort(&par, isiport);
        } else if (strncmp(argv[i], "lissport=", strlen("lissport=")) == 0) {
            lissport = atoi(argv[i] + strlen("lissport="));
        } else if (strncmp(argv[i], "depth=", strlen("depth=")) == 0) {
            depth = atoi(argv[i] + strlen("depth="));
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            begstr = argv[i] + strlen("beg=");
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            endstr = argv[i] + strlen("end=");
        } else if (strncmp(argv[i], "format=", strlen("format=")) == 0) {
            tmpstr = argv[i] + strlen("format=");
            if (strcasecmp(tmpstr, "native") == 0) {
                format = ISI_FORMAT_NATIVE;
            } else if (strcasecmp(tmpstr, "generic") == 0) {
                format = ISI_FORMAT_GENERIC;
            } else {
                fprintf(stderr, "%s: unsupported format type '%s'\n", argv[0], tmpstr);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "comp=", strlen("comp=")) == 0) {
            tmpstr = argv[i] + strlen("comp=");
            if (strcasecmp(tmpstr, "none") == 0) {
                compress = ISI_COMP_NONE;
            } else if (strcasecmp(tmpstr, "ida") == 0) {
                compress = ISI_COMP_IDA;
            } else if (strcasecmp(tmpstr, "gzip") == 0) {
                compress = ISI_COMP_GZIP;
            } else {
                fprintf(stderr, "%s: unsupported comp type '%s'\n", argv[0], tmpstr);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
            isiSetDebugFlag(&par, debug);
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            isiStartLogging(&par, log, NULL, argv[0]);
        } else if (strncmp(argv[i], "whitelist=", strlen("whitelist=")) == 0) {
            whitelist = argv[i] + strlen("whitelist=");
        } else if (strncmp(argv[i], "filter=", strlen("filter=")) == 0) {
            filter = argv[i] + strlen("filter=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            isiSetDbgpath(&par, argv[i] + strlen("dbgpath="));
        } else if (strncmp(argv[i], "keepalive=", strlen("keepalive=")) == 0) {
            keepalive = atoi(argv[i] + strlen("keepalive="));
        } else if (strcmp(argv[i], "-v") == 0) {
            flags |= VERBOSE_REPORT;
        } else if (req == NULL) {
            req = argv[i];
        } else {
           fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (req == NULL) help(argv[0]);

    // If we are not in debug mode, run program as a daemon
    if (debug == 0)
      daemonize();

    if (VERBOSE) fprintf(stderr, "%s %s\n", argv[0], VersionIdentString);

    request = UNKNOWN;
    if (strcasecmp(req, "raw") == 0) {
        request = RAW;
        SiteSpec = NULL;
    } else if (strncasecmp(req, "raw=", strlen("raw=")) == 0) {
        request = RAW;
        SiteSpec = req + strlen("raw=");
        if (strlen(SiteSpec) == 0) SiteSpec = NULL;
    } else {
        help(argv[0]);
    }

    // Check filter string for easy to find syntax errors
    if (check_filter(filter, "##/###") == -1)
    {
      fprintf(stderr, "Syntax error in filter string '%s'\n", filter);
      exit(1);
    }

    switch (request) {
      case RAW: 
        if (VERBOSE) fprintf(stderr, "%s raw data request\n", server);

        // Create shared memory region
        if (depth < 4) iRecords = 4;
        else iRecords = depth+1;
        retstr = MapSharedMem(sizeof(struct s_mapshm) + iRecords*SEED_RECLEN,
                 &mapshm);
        if (retstr != NULL)
        {
          fprintf(stderr, "%s:main %s\n", WHOAMI, retstr);
          exit(1);
        }
        memset(mapshm, 0, sizeof(struct s_mapshm));
        mapshm->depth = depth;
        mapshm->iDebug = VERBOSE || debug;
        mapshm->iPort = lissport;
        mapshm->iRecords = iRecords;
        mapshm->iOldest = 0;
        mapshm->iNext = 0;
        mapshm->seedrec = (char *)((char *)mapshm + sizeof(struct s_mapshm));

        // Start Listen server thread
        if (pthread_create(&mapshm->listen_tid, NULL,
              ListenThread, (void *)mapshm))
        {
          fprintf(stderr, "%s:main pthread_create StartServer: %s\n",
            WHOAMI, strerror(errno));
          exit(1);
        }
        pthread_detach(mapshm->listen_tid);

        if (begstr != NULL && !isiStringToSeqno(begstr, &begseqno)) {
            fprintf(stderr, "illegal beg seqno '%s'\n", begstr);
            exit(1);
        }
        if (endstr != NULL && !isiStringToSeqno(endstr, &endseqno)) {
            fprintf(stderr, "illegal end seqno '%s'\n", endstr);
            exit(1);
        }
        raw(server, &par, compress, &begseqno, &endseqno, SiteSpec, mapshm);
        break;
      default:
        help(argv[0]);
    } // switch request

    exit(0);
} // main()

