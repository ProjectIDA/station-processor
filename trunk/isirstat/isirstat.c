/*
File:       isirstat
Copyright:  (C) 2009 by Albuquerque Seismological Laboratory USGS
Purpose:    Displays status information about a remote station

Update History:
mmddyy who Changes
==============================================================================
020509 fcs Creation
******************************************************************************/
#define WHOAMI "isirstat"
#define RELEASE "1.1"

#define INCLUDE_ISI_STATIC_SEQNOS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>    // Needed for ntohl,ntohs
#include "qdplus.h"
#include "ida10.h"
#include "isi.h"
#include "isi/dl.h"
#include "util.h"
#include "include/netreq.h"
#include "include/dcc_std.h"
#include "include/dcc_time.h"
#include "include/dcc_time_proto2.h"

#define MAXCONFIGLINELEN  78
#define VERBOSE_REPORT  0x00000001
#define VERBOSE         (flags & VERBOSE_REPORT)
#define SEED_RECLEN     512
#define LOCALBUFLEN SEED_RECLEN
#define UNKNOWN 0
#define RAW     1
#define BUFLEN  256

char *server   = NULL;

int iDebug;

static UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
static QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
static ISI_DL *dl, snap;
static ISI_DL_SYS sys;

LOGIO logio, *lp = NULL;

//////////////////////////////////////////////////////////////////////////////
// Local callback
static void mseed(void *arg, QDP_HLP *hlp)
{
  fprintf(stderr, "idaapi.c: mseed callback, TODO???\n");
}

//////////////////////////////////////////////////////////////////////////////
// Parses str_header to get station, channel, location
char *ParseSeedHeader(
const char    *str_header,  // Header buffer
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

} // ParseSeedHeader()

void ShowUsage()
{
  fprintf(stderr,"Usage: %s isi=<host> [port=<port>] [<site>]\n", WHOAMI);
  fprintf(stderr," Displays status information about a remote isi disk loop\n");
  fprintf(stderr," port defaults to 39136\n");
  fprintf(stderr," site defaults to the name reported by the remote server\n");
  fprintf(stderr," Only works with 512 byte miniseed based isi disk loop\n");
  fprintf(stderr,"Version %s  %s\n", RELEASE, __DATE__);
}


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
  ISI_GLOB glob;
  
  ISI *isi;
  ISI_RAW_PACKET raw;
  ISI_DATA_REQUEST *dreq;
  UINT8 buf[LOCALBUFLEN];
  UINT64 count = 0;
  char    station[8];
  char    chan[8];
  char    loc[8];
  char    loc_station[16];
  char    seedrec[SEED_RECLEN];
  char    datebuf[BUFLEN];
  int     year, doy, hour, minute, second, tmsec;

  if (!isidlSetGlobalParameters(NULL, WHOAMI, &glob))
  {
     fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", WHOAMI, strerror(errno));
     exit(1);
  }

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

        // Copy the data
        memcpy(seedrec, raw.payload, SEED_RECLEN); 

        // Parse out channel info needed to select filename
        ParseSeedHeader(seedrec, station, chan, loc);

        // Debug log ISI sequence number, channel, time
        seed_header *pheader;
        pheader = (seed_header *)seedrec;
        year = ntohs(pheader->yr);
        doy = ntohs(pheader->jday);
        hour = (int)pheader->hr;
        minute = (int)pheader->minute;
        second = (int)pheader->seconds;
        tmsec = ntohs(pheader->tenth_millisec);
        int *int2x32;
        int2x32 = (int *)&raw.hdr.seqno.counter;
        printf("%-4s %s/%s %04d,%03d,%02d:%02d:%02d.%04d  index %08x %08x %08x\n",
              station, loc, chan, year, doy, hour, minute, second, tmsec,
              raw.hdr.seqno.signature, int2x32[1], int2x32[0]);

        // Open the disk loop for the site this packet comes from
/*
        if ((dl = isidlOpenDiskLoop(&glob, raw.hdr.site, NULL, ISI_RDONLY)) == NULL)
        {
          fprintf(stderr, "%s: isidlOpenDiskLoop failed for site=%s\n", WHOAMI, raw.hdr.site);
          exit(1);
        }

        // Get snapshot of the disk loop
        if (!isidlSnapshot(dl, &snap, &sys))
        {
          fprintf(stderr, "%s: isidlSnapshot failed for site=%s\n",
                  WHOAMI, raw.hdr.site);
          exit(1);
        }

        // Print additional status info
        printf("     count=%llu, numpkt=%d, update=%s\n",
               sys.count, sys.numpkt,
               utilTimeString(sys.tstamp.write, 100, datebuf, BUFLEN));
*/
    } // While no errors reading from ida disk loop

} // raw()

int main(int argc, char **argv)
{
int i, request, isiport;
int debug = 0;
ISI_PARAM par;
char *req = NULL;
char *log;
char *SiteSpec = NULL;
char *begstr = NULL;
char *endstr = NULL;
char *retstr = NULL;
ISI_SEQNO begseqno = ISI_NEWEST_SEQNO;
ISI_SEQNO endseqno = ISI_NEWEST_SEQNO;
char *tmpstr;
int compress  = ISI_COMP_NONE;
int format    = ISI_FORMAT_GENERIC;

  utilNetworkInit();
  isiInitDefaultPar(&par);

  // Loop through command line arguments
  for (i=1; i < argc; i++)
  {
    if (strncmp(argv[i], "isi=", strlen("isi=")) == 0)
    {
        server = argv[i] + strlen("isi=");
    }
    else if (strncmp(argv[i], "port=", strlen("port=")) == 0)
    {
        isiport = atoi(argv[i] + strlen("port="));
        isiSetServerPort(&par, isiport);
    }
    else if (strlen(argv[i]) <= 5 && argv[i][0] != '?' && 
             strcmp(argv[i], "help") != 0 && strcmp(argv[i], "-help") != 0)
    {
      // Should be a site name
      if (SiteSpec != NULL)
      {
        fprintf(stderr, "Only one site name argument supported.\n");
        fprintf(stderr, "Argument %d makes 2\n", i);
        exit(1);
      }
      SiteSpec = argv[i];
    }
    else
    {
      fprintf(stderr, "Argument %d not recognized\n", i);
      ShowUsage();
      exit(1);
    }
 
  } // for all command line arguments

  if (server == NULL)
  {
    ShowUsage();
    exit(1);
  }

  raw(server, &par, compress, &begseqno, &endseqno, SiteSpec);

  exit(0);

} // main()

