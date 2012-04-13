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
yyyy-mm-dd WHO - Changes
==============================================================================
2007-06-12 FCS - Creation
2009-07-18 FCS - Make Log threshold LOG_ERR instead of LOG_DEBUG
2011-04-29 FCS - Move to q330arch, add whoami parameter to initialization
                 do a new isidlOpenDiskLoop and isidlCloseDiskLoop on every write
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <ctype.h>
#include <arpa/inet.h>    // Needed for ntohl,ntohs 
#include "include/idaapi.h"
#include "include/map.h"
#include "qdplus.h"
#include "ida10.h"
#include "isi/dl.h"
typedef struct {
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
} LOCALPKT;

typedef struct {
    LOCALPKT pkt;
    LOGIO logio;
    QDPLUS_PAR par;
    ISI_GLOB glob;
    char dlname[ISI_SITELEN+1];
    char site[ISI_SITELEN+1];
} DLINFO;

static UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
static Map *info_map;
static const char *who;

extern int g_bDebug;

//////////////////////////////////////////////////////////////////////////////
// Log function
void logFunc(char *msg)
{
  if (g_bDebug)
  {
    fprintf(stderr, "%s", msg);
  }
  else
  {
    syslog(LOG_INFO, "%s", msg);
  }
} // logFunc

//////////////////////////////////////////////////////////////////////////////
// Local callback
static void mseed(void *arg, QDP_HLP *hlp)
{
  logFunc("idaapi.c: mseed callback, TODO???\n");
} // mseed()

//////////////////////////////////////////////////////////////////////////////
// Diskloop Map Management
void addDiskloop(const char *dlname) 
{
    DLINFO *info;

    if (info_map == 0) {
        info_map = map_new(64, NULL, NULL);
    }

    if ((info = malloc(sizeof(DLINFO))) == 0) {
        if (g_bDebug)
            fprintf(stderr, "%s: memory allocation failed for diskloop '%s' state\n", who, dlname);
        else
            syslog(LOG_ERR, "%s: memory allocation failed for diskloop '%s' state", who, dlname);
        exit(1);
    }

    if (!isidlSetGlobalParameters(NULL, "q330serv", &info->glob))
    {
        if (g_bDebug)
            fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n",
                    who, strerror(errno));
        else
            syslog(LOG_ERR, "%s: isidlSetGlobalParameters failed: %s",
                   who, strerror(errno));
        exit(1);
    }

    strlcpy(info->dlname, dlname, ISI_SITELEN+1);
    info->par.path.meta = "meta";
    info->par.path.state = "state";
    info->par.site = info->dlname;

    if (!qdpInitHLPRules(&info->par.lcq.rules, 512, QDP_HLP_FORMAT_NOCOMP32,
                         mseed, (void *)&(info->pkt), flags))
    {
        //perror("qdpInitHLPRules");
        if (g_bDebug)
            fprintf(stderr, "%s: qdpInitHLPRules: %s\n", who, strerror(errno));
        else
            syslog(LOG_ERR, "%s: qdpInitHLPRules: %s", who, strerror(errno));
        exit(1);
    }

    // Initialize logging
    logioInit(&(info->logio), NULL, logFunc, (char *)who);

    // Optional debug level
    logioSetThreshold(&(info->logio), LOG_ERR);

    fprintf(stderr, "%s: opening diskloop '%s'\n", who, info->dlname);
    if ((info->pkt.dl = isidlOpenDiskLoop(&info->glob, info->par.site, &info->logio, ISI_RDWR)) == NULL)
    {
      if (g_bDebug)
        fprintf(stderr, "%s: isidlOpenDiskLoop failed: %s\n",
                who, strerror(errno));
      else
        syslog(LOG_WARNING, "%s: isidlOpenDiskLoop failed: %s",
                who, strerror(errno));
      return;
    }

    if (!isiInitRawPacket(&info->pkt.raw, NULL, info->pkt.dl->sys->maxlen))
    {
        if (g_bDebug)
            fprintf(stderr, "%s: isiInitRawPacket: %s\n", who, strerror(errno));
        else
            syslog(LOG_ERR, "%s: isiInitRawPacket: %s", who, strerror(errno));
        exit(1);
    }
  
    info->pkt.raw.hdr.len.used = info->pkt.dl->sys->maxlen;
    info->pkt.raw.hdr.len.native = info->pkt.dl->sys->maxlen;
    info->pkt.raw.hdr.desc.comp = ISI_COMP_NONE;
    info->pkt.raw.hdr.desc.type = ISI_TYPE_MSEED;
    info->pkt.raw.hdr.desc.order = ISI_TYPE_UNDEF;
    info->pkt.raw.hdr.desc.size = sizeof(UINT8);

    strcpy(info->pkt.raw.hdr.site, info->pkt.dl->sys->site);

    if ((map_put(info_map, dlname, info)) == 0)
    {
        if (g_bDebug)
            fprintf(stderr, "%s: could not add diskloop '%s' to map\n", who, dlname);
        else
            syslog(LOG_ERR, "%s: could not add diskloop '%s' to map", who, dlname);
        exit(1);
    }
}

DLINFO *getDiskloop(const char *dlname) 
{
    return (DLINFO *)map_get(info_map, dlname);
}

int hasDiskloop(const char *dlname) 
{
    return map_contains(info_map, dlname);
}

void closeDiskloop(DLINFO *info) {
    isidlCloseDiskLoop(info->pkt.dl);
    info->pkt.dl = NULL;
}

void removeDiskloop(const char *dlname) 
{
    DLINFO *info;
    info = getDiskloop(dlname);
    closeDiskloop(info);
    map_remove(info_map, dlname);
}

void callbackRemoveDiskloop(const char *key, const void *value, const void *obj) {
    closeDiskloop((DLINFO *)value);
}

void closeDiskloops() 
{
    map_enum(info_map, callbackRemoveDiskloop, info_map);
    map_delete(info_map);
    info_map = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Initialize IDA interface
// returns NULL or an error string pointer
char *idaInit(const char *dlname, const char *whoami)
{
  who = whoami;

  if (hasDiskloop(dlname)) {
      fprintf(stderr, "%s: already connected to diskloop '%s'\n", whoami, dlname);
      return;
  }

  if (g_bDebug)
    fprintf(stderr, "%s connecting to diskloop '%s'\n", who, dlname);
  else
    syslog(LOG_INFO, "%s connecting to diskloop '%s'", who, dlname);

  addDiskloop(dlname);  
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
  static char lastname[6] = "";
  char *msg;
  BOOL      retflag;
  DLINFO *info;

  if (!hasDiskloop(dlname)) {
      idaInit(dlname, who);
  }
  info = getDiskloop(dlname);

  if (info == 0) {
    if (g_bDebug)
      fprintf(stderr, "%s: could not retrieve info for diskloop '%s'. Exiting!\n", who, dlname);
    else
      syslog(LOG_WARNING, "%s: could not retrieve info for diskloop '%s'. Exiting!", who, dlname);
    return NULL;
  }

  // Copy data to IDA buffer
  memcpy(info->pkt.raw.payload, databuf, 512);

  if (!isidlWriteToDiskLoop(info->pkt.dl, &info->pkt.raw, ISI_OPTION_GENERATE_SEQNO))
  {
    if (g_bDebug)
        fprintf(stderr, "isidlWriteToDiskLoop failed: %s\n", strerror(errno));
    else
        syslog(LOG_ERR, "isidlWriteToDiskLoop failed: %s\n", strerror(errno));
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
