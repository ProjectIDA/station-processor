/*
File:       idaapi.c
Copyright:  (C) 2007 by Albuquerque Seismological Laboratory
Author:     Frank Shelly
Purpose:    Routines for interfacing to IDA disk loop library
Routines:
            idaInit
              First time initialization
            idaWrite
              Write a new seed record to ida disk loop

Update History:
mmddyy who Changes
==============================================================================
061207 fcs Creation
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>    // Needed for ntohl,ntohs 
#include "include/idaapi.h"
#include "qdplus.h"
#include "ida10.h"
#include "isi/dl.h"
typedef struct {
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
} LOCALPKT;

static UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
static LOCALPKT local;
static QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;

LOGIO logio, *lp = NULL;

//////////////////////////////////////////////////////////////////////////////
// Local callback
static void mseed(void *arg, QDP_HLP *hlp)
{
  fprintf(stderr, "idaapi.c: mseed callback, TODO???\n");
}

//////////////////////////////////////////////////////////////////////////////
// Initialize IDA interface
char *idaInit(const char *dlname
  )                       // returns NULL or an error string pointer
{
  static char *fid = "OpenDiskLoop";
  static ISI_GLOB glob;
  static char  site[ISI_SITELEN+1];
  FILE *fp=NULL;
  char logFileName[MAXPATHLEN+1];

  par.path.meta = "meta";
  par.path.state = "state";
  strlcpy(site, dlname, ISI_SITELEN+1);
  par.site = site;

  if (!qdpInitHLPRules(&par.lcq.rules, 512, QDP_HLP_FORMAT_NOCOMP32,
                  mseed, (void *)&local, flags))
  {
    perror("qdpInitHLPRules");
    exit(1);
  }


  if (!isidlSetGlobalParameters(NULL, "q330serv", &glob))
  {
    fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n",
          fid, strerror(errno));
    exit(1);
  }

  // Initialize logging
  sprintf(logFileName, "%s/%s.log", glob.root, dlname);
  logioInit(&logio, logFileName, NULL, dlname);
  lp = &logio;
  // Optional debug level
  logioSetThreshold(&logio, LOG_DEBUG);

  if ((local.dl = isidlOpenDiskLoop(&glob, par.site, lp, ISI_RDWR)) == NULL)
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
} // idaInit()

//////////////////////////////////////////////////////////////////////////////
// Write a record of SEED data
// Hides all of the logic needed to run a circular buffer from main program
char *idaWriteChan(
  const char  *station,   // station name
  const char  *chan,      // Channel ID
  const char  *loc,       // Location ID
  char        *databuf,   // Seed record pointer
  const char  *dlname     // Name of ida disk loop to save to
  )                       // returns NULL or an error string pointer
{
  static int firstcall=1;
  char *msg;
  BOOL      retflag;

  // one time initialization
  if (firstcall)
  {
    firstcall = 0;
    msg = idaInit(dlname);
    if (msg != NULL)
      return msg;
  }

  // Copy data to IDA buffer
  memcpy(local.raw.payload, databuf, 512);

  if (!isidlWriteToDiskLoop(local.dl, &local.raw, ISI_OPTION_GENERATE_SEQNO))
  {
        fprintf(stderr, "isidlWriteToDiskLoop failed: %s\n", strerror(errno));
        exit(1);
  }

  return NULL;
} // idaWriteChan()

//////////////////////////////////////////////////////////////////////////////
// Linux doesn't have this routine so fake it
/*
size_t strlcpy(char *dst, const char *src, size_t dstsize)
{
  size_t len,tlen;
  len = strlen(src);
  tlen = len;
  if (tlen >= dstsize)
    tlen = dstsize-1;
  strncpy(dst,src, tlen);
  dst[tlen] = 0;
  return len;
}
*/
