/*
File:       ida2chan
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Purpose:    Breaks apart an ida disk loop into seed channel data

Update History:
mmddyy who Changes
==============================================================================
080207 fcs Creation
******************************************************************************/
#define WHOAMI "ida2chan"
const char *VersionIdentString = "Release 1.1";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>    // Needed for ntohl,ntohs
#include "qdplus.h"
#include "ida10.h"
#include "isi.h"
#include "isi/dl.h"
#include "include/netreq.h"

#define MAXCONFIGLINELEN  78

typedef struct {
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
} LOCALPKT;

static UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
static LOCALPKT local;
static QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
static ISI_DL snap;
static ISI_DL_SYS sys;

LOGIO logio, *lp = NULL;

//////////////////////////////////////////////////////////////////////////////
// Local callback
static void mseed(void *arg, QDP_HLP *hlp)
{
  fprintf(stderr, "idaapi.c: mseed callback, TODO???\n");
}

//////////////////////////////////////////////////////////////////////////////
// Initialize IDA interface
char *idaInitDL(
    const char  *dlName
  )                       // returns NULL or an error string pointer
{
  static char *fid = "OpenDiskLoop";
  static ISI_GLOB glob;
  FILE *fp=NULL;
  char logFileName[MAXPATHLEN+1];

  par.path.meta = "meta";
  par.path.state = "state";
  if (par.site == NULL) par.site = (char *)dlName;

  if (!qdpInitHLPRules(&par.lcq.rules, 512, QDP_HLP_FORMAT_NOCOMP32,
                  mseed, (void *)&local, flags))
  {
    perror("qdpInitHLPRules");
    exit(1);
  }


  if (!isidlSetGlobalParameters(NULL, "q330driver", &glob))
  {
    fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n",
          fid, strerror(errno));
    exit(1);
  }

  // Initialize logging
  sprintf(logFileName, "%s/%s", glob.root, "log/ida2chan.log");
fprintf(stderr, "Opening log file logFileName '%s'\n", logFileName);
  logioInit(&logio, logFileName, NULL, "ida2chan");
  lp = &logio;
  // Optional debug level
  logioSetThreshold(&logio, LOG_DEBUG);

  if ((local.dl = isidlOpenDiskLoop(&glob, par.site, lp, ISI_RDONLY)) == NULL)
  {
    fprintf(stderr, "%s: isidlOpenDiskLoop failed: %s\n",
          fid, strerror(errno));
    exit(1);
  }

  if (!isiInitRawPacket(&local.raw, NULL, local.dl->sys->maxlen))
  {
    fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
    exit(1);
  }
  strcpy(local.raw.hdr.site, local.dl->sys->site);
  local.raw.hdr.len.used = local.dl->sys->maxlen;
  local.raw.hdr.len.native = local.dl->sys->maxlen;
  local.raw.hdr.desc.comp = ISI_COMP_NONE;
  local.raw.hdr.desc.type = ISI_TYPE_MSEED;
  local.raw.hdr.desc.order = ISI_TYPE_UNDEF;
  local.raw.hdr.desc.size = sizeof(UINT8);


  return NULL;
} // idaInitDL()

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
  fprintf(stderr,
"Usage: %s <idadlname>\n", WHOAMI);
  fprintf(stderr,
"  <idadlname> id of ida disk loop\n");
  fprintf(stderr,
" Appends seed records from ida disk loop into indivdual channel\n");
  fprintf(stderr,
" files in the working directory.\n");
  fprintf(stderr,"%s  %s\n", VersionIdentString, __DATE__);
}


int main (int argc, char **argv)
{
  char    *retmsg;
  UINT32  index;
  UINT32  count;
  UINT32  iBegin;
  UINT32  iEnd;
  UINT32  iWrap;
  FILE    *fp_buf;
  char    seedrec[512];
  char    station[8];
  char    chan[8];
  char    loc[8];
  char    buf_filename[2*MAXCONFIGLINELEN+2];
  char    *loopDir=".";
  int     year, doy, hour, minute, second, tmsec;

  if (argc < 2 || argc > 2)
  {
    ShowUsage();
    exit(1);
  }

  // Initialize ida interface
  if ((retmsg = idaInitDL(argv[1])) != NULL)
  {
    fprintf(stderr, "%S: idaInitDL: %s\n",
        WHOAMI, retmsg);
  }

  // Get start, stop indexes, wrap point
  if (!isidlSnapshot(local.dl, &snap, &sys))
  {
        fprintf(stderr, "isidlSnapshot failed for site=%s\n", argv[1]);
        exit(1);
  }

  iWrap  = snap.sys->numpkt;
  iBegin = snap.sys->index.oldest;
  iEnd   = (snap.sys->index.yngest+1) % iWrap;

  if (snap.sys->index.oldest > snap.sys->index.yngest)
    count = snap.sys->index.yngest+iWrap - snap.sys->index.oldest;
  else
    count = snap.sys->index.yngest - snap.sys->index.oldest + 1;
  printf("Saving %d seed records to channel files\n", count);

  // Go through all of the disk loop records
  for (count=1, index = iBegin;
       index != iEnd; index = (index+1) % iWrap, count++)
  {
    // Get data from the IDA disk loop
    if (!isidlReadDiskLoop(local.dl, &local.raw, index))
    {
      fprintf(stderr, "isidlReadDiskLoop failed for site=%s index=%lu\n",
              local.dl->sys->site, index);
      exit(1);
    }

    // Copy the data
    memcpy(seedrec, local.raw.payload, 512); 

    // Parse out channel info needed to select filename
    ParseSeedHeader(seedrec, station, chan, loc);

    // Get names of buffer and index files
    // If blank location code, leave off leading location code in filename
    if (loc[0] == ' ' || loc[0] == 0)
    {
      sprintf(buf_filename, "%s/%s_%s.seed",
          loopDir, station, chan);
    }
    else
    {
      sprintf(buf_filename, "%s/%s_%s_%s.seed",
          loopDir, station, loc, chan);
    }

    // Append open seed channel file
    if ((fp_buf=fopen(buf_filename, "a")) == NULL)
    {
      // Buffer file does not exist so start a new set
      fprintf(stderr, "%s: failed to create %s",
              WHOAMI, buf_filename);
      exit(1);
    }

    // Write record
    if (fwrite(seedrec, 512, 1, fp_buf) != 1)
    {
      fprintf(stderr, "%s: Unable to write record %d in %s",
             WHOAMI, count, buf_filename);
      exit(1);
    }
    fclose(fp_buf);

    // Debug log ISI sequence number, channel, time
    seed_header *pheader;
    pheader = (seed_header *)seedrec;
    year = ntohs(pheader->yr);
    doy = ntohs(pheader->jday);
    hour = (int)pheader->hr;
    minute = (int)pheader->minute;
    second = (int)pheader->seconds;
    tmsec = ntohs(pheader->tenth_millisec);
/*
    int *int2x32;
    int2x32 = (int *)&local.raw.hdr.seqno.counter;
    printf("%08x %08x %08x %s %s/%s %04d,%03d,%02d:%02d:%02d.%04d\n",
          local.raw.hdr.seqno.signature, int2x32[1], int2x32[0],
          station, loc, chan, year, doy, hour, minute, second, tmsec);
*/

  } // for each disk loop record

  printf("Done");
  return 0 ;
} // main()

