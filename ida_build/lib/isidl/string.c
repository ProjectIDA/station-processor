#pragma ident "$Id: string.c,v 1.2 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  ISI disk loop related string functions
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"

static char *IndexString(UINT32 value, char *format, char *buf)
{
    if (value != ISI_UNDEFINED_INDEX) {
        sprintf(buf, format, value);
    } else {
        sprintf(buf, "<undefined>");
    }

    return buf;
}

char *isidlIndexString(ISI_INDEX *index, char *buf)
{
char tmp[64];

    buf[0] = 0;

   sprintf(buf+strlen(buf), "active=%s", IndexString(index->active, "%lu", tmp));
   sprintf(buf+strlen(buf), " oldest=%s", IndexString(index->oldest, "%lu", tmp));
   sprintf(buf+strlen(buf), " yngest=%s", IndexString(index->yngest, "%lu", tmp));
   sprintf(buf+strlen(buf), " lend=%s", IndexString(index->lend, "%lu", tmp));

    return buf;
}

char *isidlStateString(int state)
{
static char *inactive = "inactive";
static char *idle     = "idle";
static char *active   = "active";
static char *update   = "update";
static char *unknown  = "UNKNOWN???";

    switch (state) {
      case ISI_INACTIVE: return inactive;
      case ISI_IDLE:     return idle;
      case ISI_ACTIVE:   return active;
      case ISI_UPDATE:   return update;
    }

    return unknown;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.2  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
