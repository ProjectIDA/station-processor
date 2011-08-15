/*======================================================================
Author:  Frank Shelly  USGS Albuquerque Seismological Laboratory
Date:    2 January 2008
Purpose: convert IDA isi server feed to a ASL archive disk loop
         Modified from isi general purpose client
Update History:
mmddyy who Changes
==============================================================================
071309 fcs idaapi changed to only log errors or worse Release 1.2
020910 fcs New diskloop.config keywords for falcon
081511 jde NoArchive in diskloop.config means don't add record to Archives
******************************************************************************/

const char *WHOAMI="ida2arch";
const char *VersionIdentString = "Release 1.4";

#define INCLUDE_ISI_STATIC_SEQNOS
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "isi.h"
#include "util.h"
#include "include/netreq.h"
#include "include/diskloop.h"
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

int gDebug=0;

#define LOCALBUFLEN SEED_RECLEN

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "localhost"
#endif /* DEFAULT_SERVER */

#define UNKNOWN 0
#define RAW     1

UINT32 flags   = 0;
char *server   = DEFAULT_SERVER;

char *filter=NULL;

extern int WildMatch(char *pattern, char *target);

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
      if (gDebug)
        fprintf(stderr, "check_filter entry_str '%s' %d > %d+%d+1\n",
              entry_str, strlen(entry_str), i,4);
      else
        syslog(LOG_ERR, "check_filter entry_str '%s' %d > %d+%d+1\n",
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
int save_errno;

    if ((dreq = isiAllocSimpleSeqnoRequest(begseqno, endseqno, SiteSpec)) == NULL) {
        save_errno = errno;
        if (gDebug)
          fprintf(stderr, "isiAllocSimpleSeqnoRequest: %s\n", strerror(save_errno));
        else
          syslog(LOG_ERR, "isiAllocSimpleSeqnoRequest: %s\n", strerror(save_errno));
        exit(1);
    }
    isiSetDatreqCompress(dreq, compress);

    if (dreq->nreq != 1)
    {
      if (gDebug)
        fprintf(stderr, "%s only works on a single station, not %d stations.\n",
                WHOAMI, dreq->nreq);
      else
        syslog(LOG_ERR, "%s only works on a single station, not %d stations.\n",
                WHOAMI, dreq->nreq);
    } // Invalid number of stations given
    return dreq;
} // BuildRawRequest()

static BOOL ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw)
{
  int     status;

    while ((status = isiReadFrame(isi, TRUE)) == ISI_OK)
    {
        if (isi->frame.payload.type != ISI_IACP_RAW_PKT)
        {
            if (gDebug)
              fprintf(stderr, "unexpected type %d packet ignored\n", isi->frame.payload.type);
            else
              syslog(LOG_ERR, "unexpected type %d packet ignored\n", isi->frame.payload.type);
        }
        else
        {
            isiUnpackRawPacket(isi->frame.payload.data, raw);
            return TRUE;
        }
    }

    switch (status) {
      case ISI_DONE:
        if (gDebug)
          fprintf(stderr, "request complete\n");
        else
          syslog(LOG_INFO, "request complete\n");
        break;
        break;

      case ISI_BREAK:
        if (gDebug)
          fprintf(stderr, "server disconnect\n");
        else
          syslog(LOG_INFO, "server disconnect\n");
        break;

      case ISI_TIMEDOUT:
        if (gDebug)
          fprintf(stderr, "ISI_TIMEDOUT\n");
        else
          syslog(LOG_INFO, "ISI_TIMEDOUT\n");
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
  int     save_errno;
  int     *int2x32;
  FILE    *fp_seq;
  char    station[8];
  char    chan[8];
  char    loc[8];
  char    loc_station[16];
  char    sequence_str[(8*3+4)*3+6];
  char    seq_filename[2*MAXCONFIGLINELEN+2];
  char    loopDir[2*MAXCONFIGLINELEN+2];
  char    *retmsg;

    dreq = BuildRawRequest(compress, begseqno, endseqno, SiteSpec);

    if (VERBOSE) {
        fprintf(stderr, "Client side data request\n");
        isiPrintDatreq(stderr, dreq);
    }

    if ((isi = isiInitiateDataRequest(server, par, dreq)) == NULL)
    {
        if (errno == ENOENT) {
            if (gDebug)
              fprintf(stderr, "can't connect to server %s, port %d\n", server, par->port);
            else
              syslog(LOG_ERR, "can't connect to server %s, port %d\n", server, par->port);
        } else {
            save_errno = errno;
            if (gDebug)
              perror("isiInitiateDataRequest");
            else
              syslog(LOG_ERR, "isiInitiateDataRequest: %s", strerror(save_errno));
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

        SeedHeaderSNLC(raw.payload, station,chan,loc);
        if (filter == NULL) {
          // If no filter argument was supplied, use the NoArchive filter(s) 
          // from the config file
          if (CheckNoArchive(station, chan, loc))
            continue;
        }
        else {
          // See if location/channel match our filter string
          sprintf(loc_station, "%2.2s/%3.3s", loc, chan);
          if (check_filter(filter, loc_station) != 1)
            continue;
        }
        int2x32 = (int *)&raw.hdr.seqno.counter;

        if (gDebug)
        {
          fprintf(stderr, "Read %s %s/%s %08x %08x %08x\n",
            station, loc, chan,
            raw.hdr.seqno.signature, int2x32[1], int2x32[0]);
        }

        // Save this record to buffer
        retmsg = WriteChan(station, chan, loc, raw.payload);
        if (retmsg != NULL)
        {
          if (gDebug)
            fprintf(stderr, "WriteChan: %s\n", retmsg);
          else
            syslog(LOG_ERR, "WriteChan: %s\n", retmsg);
        }

        // Save IDA sequence so on a restart, we pick up where we left off
        sprintf(sequence_str,
                "%08x%08x%08x\n"
                "%08x%08x%08x\n"
                "%08x%08x%08x\n",
                raw.hdr.seqno.signature, int2x32[1], int2x32[0],
                raw.hdr.seqno.signature, int2x32[1], int2x32[0],
                raw.hdr.seqno.signature, int2x32[1], int2x32[0]);
        LoopDirectory(loopDir);
        sprintf(seq_filename, "/%s/%s/isi.seq",
                loopDir, SiteSpec);
        if ((fp_seq=fopen(seq_filename, "w+")) == NULL)
        {
          if (gDebug)
            fprintf(stderr, "raw(): failed to create %s",
                  seq_filename);
          else
            syslog(LOG_ERR, "raw(): failed to create %s",
                  seq_filename);
          exit(1);
        } // unable to create index file
        else
        {
          fwrite(sequence_str, strlen(sequence_str), 1, fp_seq);
          fclose(fp_seq);
          fp_seq = NULL;
        } // save latest sequence number to file
    } // While no errors reading from ida disk loop

    if (gDebug)
      fprintf(stderr, "%llu packets received\n", count);
} // raw()

static void ShowUsage(char *myname)
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
"    log=string - turns on library logging using. 'string' is either the\n"
"                 name of the log file, '-' for stdout, or 'syslogd:facility'\n"
"    configdir=string = sets directory where diskloop.config file is\n"
"                       default is /etc/q330/DLG1\n"
"    debug=int - sets the debug level (0, 1, 2)\n"
"    filter=string - What location/channels to archive, default is */*\n"
"                    Format is one or more location/channel entries\n"
"                    separated by commas, * and ? wildcards allowed\n"
"                    For a blank location code start with a /\n"
"                    (If this option is not supplied, the NoArchive entries\n"
"                     from diskloop.config will be used instead)\n"
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
"ida2arch raw=pfo beg=45b566150000000000a613a0 end=45b5661500000000009ea391\n"
"\n";

    fprintf(stderr,"Usage: %s ", myname);
    fprintf(stderr, "[-v] [server=string] [isiport=int] [log=string]\n");
    fprintf(stderr, "  [configdir=string] [filter=string] [debug=int]\n");
    fprintf(stderr, "  raw[=spec] [beg=str] [end=str]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", VerboseHelp);
    fprintf(stderr, "default server is `%s'\n", DEFAULT_SERVER);
    fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);
    exit(1);

} // ShowUsage()

int main(int argc, char **argv)
{
int i, request, isiport;
int depth=-1;
int iRecords;
ISI_PARAM par;
char *req = NULL;
char *log;
char *SiteSpec = NULL;
char *begstr = NULL;
char *endstr = NULL;
char *retstr = NULL;
char *configdir = "/etc/q330/DLG1";
ISI_SEQNO begseqno = ISI_OLDEST_SEQNO;
ISI_SEQNO endseqno = ISI_NEVER_SEQNO;
char *tmpstr;
int compress  = ISI_COMP_NONE;
int format    = ISI_FORMAT_GENERIC;
char configfile[2*MAXCONFIGLINELEN+2];
char seq_filename[2*MAXCONFIGLINELEN+2];
char loopDir[2*MAXCONFIGLINELEN+2];
char sequence_str[3][28];
int *int2x32;
char station[8];
char network[4];
char channel[4];
char location[4];
FILE *fp_seq;

    utilNetworkInit();
    isiInitDefaultPar(&par);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            server = argv[i] + strlen("server=");
        } else if (strncmp(argv[i], "isiport=", strlen("isiport=")) == 0) {
            isiport = atoi(argv[i] + strlen("isiport="));
            isiSetServerPort(&par, isiport);
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            begstr = argv[i] + strlen("beg=");
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            endstr = argv[i] + strlen("end=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            gDebug = atoi(argv[i] + strlen("debug="));
            isiSetDebugFlag(&par, gDebug);
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            isiStartLogging(&par, log, NULL, argv[0]);
        } else if (strncmp(argv[i], "filter=", strlen("filter=")) == 0) {
            filter = argv[i] + strlen("filter=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            isiSetDbgpath(&par, argv[i] + strlen("dbgpath="));
        } else if (strncmp(argv[i], "configdir=", strlen("configdir=")) == 0) {
            configdir = argv[i] + strlen("configdir=");
        } else if (strcmp(argv[i], "-v") == 0) {
            flags |= VERBOSE_REPORT;
        } else if (req == NULL) {
            req = argv[i];
        } else {
           if (gDebug)
           {
              fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
              ShowUsage(argv[0]);
           }
        }
    }

    if (req == NULL)
    {
      ShowUsage(argv[0]);
      exit(1);
    }

    // If we are not in debug mode, run program as a daemon
    gDebug = gDebug || VERBOSE;
    if (gDebug == 0)
      daemonize();

    // Open diskloop.config file
    sprintf(configfile, "%s/diskloop.config", configdir);
    if ((retstr=ParseDiskLoopConfig(configfile)) != NULL)
    {
      if (gDebug)
        fprintf(stderr, "%s: %s\n", WHOAMI, retstr);
      else
        syslog(LOG_ERR, "%s\n", retstr);
      exit(1);
    }
    LogSNCL(station, network, channel, location);

    if (VERBOSE)  fprintf(stderr, "%s  %s\n", VersionIdentString, __DATE__);

    request = UNKNOWN;
    if (strcasecmp(req, "raw") == 0) {
        request = RAW;
        SiteSpec = station;
    } else if (strncasecmp(req, "raw=", strlen("raw=")) == 0) {
        request = RAW;
        SiteSpec = req + strlen("raw=");
        if (strlen(SiteSpec) == 0) SiteSpec = station;
    } else {
        ShowUsage(argv[0]);
    }

    // Check filter string for easy to find syntax errors
    if (check_filter(filter, "##/###") == -1)
    {
      fprintf(stderr, "Syntax error in filter string '%s'\n", filter);
      exit(1);
    }

    // Get beginning sequence number from continuity file
    if (begstr == NULL)
    {
        LoopDirectory(loopDir);
        sprintf(seq_filename, "/%s/%s/isi.seq",
                loopDir, SiteSpec);
        if ((fp_seq=fopen(seq_filename, "r")) == NULL)
        {
          if (gDebug)
            fprintf(stderr, "%s not found, starting at oldest ISI record\n",
                  seq_filename);
          else
            syslog(LOG_INFO, "%s not found, starting at oldest ISI record\n",
                  seq_filename);
        } // unable to find index file
        else
        {
          fgets(sequence_str[0], 28, fp_seq);
          fgets(sequence_str[1], 28, fp_seq);
          fgets(sequence_str[2], 28, fp_seq);
          if (strlen(sequence_str[0]) >= 24 &&
              strlen(sequence_str[1]) >= 24)
          {
            // if fist two lines agree, use them
            begstr = sequence_str[0];
            if (strncmp(sequence_str[0], sequence_str[1], 24) != 0)
            {
              // Otherwise use the 3rd line if it is valid length
              if (strlen(sequence_str[2]) >= 24)
                begstr = sequence_str[2];
              else
                begstr = NULL;
            } // first two lines did not aggree
          } // first two lines have enough characters

          // Check to see if we did not parse a valid resume index
          if (begstr == NULL)
          {
            if (gDebug)
              fprintf(stderr,
                    "%s contents invalid, starting at oldest ISI record\n",
                    seq_filename);
            else
              syslog(LOG_ERR,
                    "%s contents invalid, starting at oldest ISI record\n",
                    seq_filename);
          } // invalid resume index
          else
          {
            // Convert string into an index
            begstr[24] = 0;
            if (!isiStringToSeqno(begstr, &begseqno))
            {
              if (gDebug)
                fprintf(stderr, "illegal beg seqno '%s'\n", begstr);
              else
                syslog(LOG_ERR, "illegal beg seqno '%s'\n", begstr);
              exit(1);
            }
 
            // increment index by one to get next unsaved record
            begseqno.counter++;

            int2x32 = (int *)&begseqno.counter;
            sprintf(begstr, "%08x%08x%08x",
                    begseqno.signature, int2x32[1], int2x32[0]);
          } // else we have what could be an index string
        } // found index file so load last index
    } // if user did not specify a beginning sequence number

    switch (request) {
      case RAW: 
        if (VERBOSE) fprintf(stderr, "%s raw data request\n", server);

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
        ShowUsage(argv[0]);
    } // switch request

    exit(0);
} // main()

