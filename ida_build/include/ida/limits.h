#pragma ident "$Id: limits.h,v 1.1 2006/02/09 20:09:09 dechavez Exp $"
/*======================================================================
 *
 *  IDA limits
 *  manipulate Project IDA field data.
 *
 *====================================================================*/
#ifndef ida_limits_h_included
#define ida_limits_h_included

#include "platform.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IDA_BUFSIZ      1024 /* Standard record length              */
#define IDA_BUFLEN  IDA_BUFSIZ
#define IDA_HEADLEN      64  /* Standard header length              */
#define IDA_SNAMLEN       8  /* Max station name length (inc. NULL) */
#define IDA_CNAMLEN       8  /* Max channel name length (inc. NULL) */
#define IDA_INAMLEN       8  /* Max instrument name length (inc. NULL) */
#define IDA_MNAMLEN      32  /* Max channel map name len.  (inc. NULL) */
#define IDA_MAXLOGLEN   988  /* Max log message  length (inc. NULL) */
#define IDA_MAXDLEN (IDA_BUFSIZ - IDA_HEADLEN) /* Max size of data area */
#define IDA_MAXSTREAMS   64  /* Max number of defined streams       */
#define IDA_MAXCALCHAN   12  /* Max number of calibratable channels */
#define IDA_MAXOLDCNF   225  /* Max dim of old-style config table   */
#define IDA_MAXOLDHREC   10  /* Max number of old-style H rec types */
#define IDA_MAXEVENTS    27  /* Max number of events in E record    */
#define IDA_PORTNAMLEN   32
#define IDA_DEVNAMLEN    32 
#define IDA_MAXLOGENTRY  40

#define TTAG_MIN_EXTTIM     86400L /* 1 day */
#define TTAG_MAX_EXTTIM  31708799L /* 367 days - 1 sec */

#define IDA_MAX_CAL_EPOCH 64
#define IDA_MAX_ORIENT    64
#define IDA_MAX_CAL_CHAN  48

#define IDA_UNDEFINED_TQUAL -9
#define IDA_EMBEDDED_YEAR 9999 /* when digitizer has included year in secs field */

#ifdef __cplusplus
}
#endif

#endif /* ida_limits_h_included */

/* Revision History
 *
 * $Log: limits.h,v $
 * Revision 1.1  2006/02/09 20:09:09  dechavez
 * initial release
 *
 */
