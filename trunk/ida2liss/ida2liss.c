#pragma ident "$Id: main.c,v 1.25 2007/06/28 19:45:51 dechavez Exp $"
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
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isi.h"
#include "util.h"
#include "include/netreq.h"
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
static char *StandardOutput = "stdout";

extern int open_socket(int portnum);

// Shared memory structure for passing info between threads
// iOldest and iNext are controlled by main program unless
// depth == -1 in which case iOldest is controlled by single LissThread
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
  pthread_t client_tid[MAXCLIENT];   // Thread id of LissThread
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

} // SeedHeaderSNLC()

//////////////////////////////////////////////////////////////////////////////
// LissThread()
//   Sends data from circular buffer to the LISS client
//   The circular buffer depth is set by depth=argument
//   If depth = -1 then the main program will never overwrite old data
//     In this case it is up to LissThread to update iOldest index
//     If depth = -1 then only one client will be allowed to connect
void *LissThread(void *params)
{
  fprintf(stderr, "DEBUG entering LissThread()\n");
  while (1)
    sleep(1);
  return NULL;
} // LissThread()


//////////////////////////////////////////////////////////////////////////////
// ListenThread()
//   Accepts new client connections and spawns a LissThread for each one
//   Connections are limited by #define MAXCLIENT
//   whitelist if not null will limit who can connect
void *ListenThread(void *params)
{
  struct s_mapshm *mapshm;


  // Get pointer to shared memory area
  mapshm = (struct s_mapshm *)params;
  
  // open LISS socket
  if (mapshm->iDebug)
  {
    fprintf(stderr, "Entering ListenThread()\n");
    fprintf(stderr, "About to open socket on port %d\n", mapshm->iPort);
  }

  if (open_socket(mapshm->iPort) < 0)
  {
    fprintf(stderr, "Failed to open socket on port %d\n", mapshm->iPort);
    exit(1);
  }
  fprintf(stderr, "DEBUG opened socket on port %d\n", mapshm->iPort);

  while (1)
    sleep(1);
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
            fprintf(stderr, "unexpected type %d packet ignored\n", isi->frame.payload.type);
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

static void raw(char *server, ISI_PARAM *par, int compress, ISI_SEQNO *begseqno, ISI_SEQNO *endseqno, char *SiteSpec)
{
ISI *isi;
ISI_RAW_PACKET raw;
ISI_DATA_REQUEST *dreq;
UINT8 buf[LOCALBUFLEN];
UINT64 count = 0;

char    station[8];
char    chan[8];
char    loc[8];
int     iReturn;

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

    while (ReadRawPacket(isi, &raw)) {
        ++count;
        if (VERBOSE) fprintf(stderr, "%s\n", isiRawHeaderString(&raw.hdr, buf));
        if (!isiDecompressRawPacket(&raw, buf, LOCALBUFLEN)) {
            fprintf(stderr, "isiDecompressRawPacket error\n");
        } else if (VERBOSE) {
            utilPrintHexDump(stderr, raw.payload, 64);
        }
        if (VERBOSE) fprintf(stderr, "New record %6.6s\n", raw.payload);

        // Send the data
        // Return 0 means no connection
        // Return -1 means error, connection closed
        // Return 1 means success
        iReturn = send_record(raw.payload, raw.hdr.len.used);

        // If no connection keep trying until there is one
        while (iReturn < 1)
        {
          iReturn = accept_client();
          if (iReturn < 1)
          {
            // No longer able to accept connections, exit
            fprintf(stderr, "No longer able to accept connections, exiting!\n");
            exit(1);
          }

          // We now have somebody connected, retry send
          if (VERBOSE) fprintf(stderr, "new connection accepted\n");
          iReturn = send_record(raw.payload, raw.hdr.len.used);
        } // while unable to send record

        // ASL code to write data to LISS connection
        SeedHeaderSNLC(raw.payload, station, chan, loc);
        fprintf(stdout, "Sent %d bytes %s %s/%s %6.6s\n",
                raw.hdr.len.used, station, chan, loc, &raw.payload[0]);

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
"    whitelist=string - hosts that are allowed to connect\n"
"                       Empty (default) whitelist means all hosts\n"
"                       A zero in last ip address means subnet access allowed\n"
"                       Use comma to separate multiple hosts/addresses\n"
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

    fprintf(stderr,"usage: %s ", myname);
    fprintf(stderr, "[ -v server=string isiport=int log=string debug=int ] ");
    fprintf(stderr, "[ depth=int filter=string whitelist=string ] ");
    fprintf(stderr, "raw[=spec] [beg=str end=str]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", VerboseHelp);
    fprintf(stderr, "default server is `%s'\n", DEFAULT_SERVER);
    exit(1);

} // help()

int main(int argc, char **argv)
{
int i, request, isiport;
int debug = 1;
int lissport=4001;
int depth=-1;
int iRecords;
char *whitelist=NULL;
char *filter=NULL;
ISI_PARAM par;
char *req = NULL;
char *log;
char *StreamSpec = NULL;
char *SiteSpec = NULL;
char *begstr = NULL;
char *endstr = NULL;
char *retstr = NULL;
REAL64 begtime = ISI_NEWEST;
REAL64 endtime = ISI_KEEPUP;
ISI_SEQNO begseqno = ISI_NEWEST_SEQNO;
ISI_SEQNO endseqno = ISI_NEVER_SEQNO;
char *tmpstr;
int maxdur    = 0;
int compress  = ISI_COMP_NONE;
int format    = ISI_FORMAT_GENERIC;
char *dbspec  = NULL;
struct s_mapshm *mapshm;

    utilNetworkInit();
    isiInitDefaultPar(&par);

    // Ignore broken pipe signal from closed network connection
    signal(SIGPIPE, SIG_IGN);

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
        } else if (strncmp(argv[i], "maxdur=", strlen("maxdur=")) == 0) {
            maxdur = atoi(argv[i] + strlen("maxdur="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            isiStartLogging(&par, log, NULL, argv[0]);
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            isiSetDbgpath(&par, argv[i] + strlen("dbgpath="));
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

    switch (request) {
      case RAW: 
        if (VERBOSE) fprintf(stderr, "%s raw data request\n", server);

        // Create shared memory region
        if (depth < 1) iRecords = 2;
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
        mapshm->iDebug = debug;
        mapshm->iPort = lissport;
        mapshm->iRecords = iRecords;
        mapshm->iOldest = 0;
        mapshm->iNext = 0;
        mapshm->seedrec = (char *)(mapshm + sizeof(struct s_mapshm));

        // Start Listen server thread
        if (pthread_create(&mapshm->listen_tid, NULL,
              ListenThread, (void *)mapshm))
        {
          fprintf(stderr, "%s:main pthread_create StartServer: %s\n",
            WHOAMI, strerror(errno));
          exit(1);
        }

        if (begstr != NULL && !isiStringToSeqno(begstr, &begseqno)) {
            fprintf(stderr, "illegal beg seqno '%s'\n", begstr);
            exit(1);
        }
        if (endstr != NULL && !isiStringToSeqno(endstr, &endseqno)) {
            fprintf(stderr, "illegal end seqno '%s'\n", endstr);
            exit(1);
        }
        raw(server, &par, compress, &begseqno, &endseqno, SiteSpec);
        break;
      default:
        help(argv[0]);
    } // switch request

    exit(0);
} // main()

