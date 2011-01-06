#pragma ident "$Id: version.c,v 1.26 2009/02/04 23:50:49 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "ida.h"

static VERSION version = {4, 3, 2};

/* ida library release notes

4.3.2   02/04/2009
        chnlocmap.c: don't assume array view was set

4.3.1   03/06/2008
        dhead9.c: leave ext time alone when applying TTAG_LEAP_YEAR_PATCH

4.3.0   03/05/2008
        dhead.c: initialize extra.tstamp (to current time) and the new subformat and flags fields
        dhead9.c: added support for TTAG_LEAP_YEAR_PATCH flag

4.2.2   01/25/2008
        timstr.c: fixed bug in sys_timstr() where bogus tic intervals
           in corrupt headers could cause division by zero

4.2.1   01/16/2008
        chnlocmap.c: removed conditional debug code

4.2.0   01/16/2008
        chnlocmap.c: added conditional debug code, fill in missing mapname in idaSetChnLocMap()
        timstr.c: added destination buffers for optional MT-safety

4.1.2   06/30/2007
        chanlocmap.c: replaced string memcpy with strlcpy

4.1.1   06/01/2007
        dhead[2-7].c: use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"

4.1.0   01/25/2007
        dhead7.c: IDA9.x (aka RT593) support
        dhead9.c: ditto
        format.c: ditto

4.0.4   01/08/2007
        chnlocmap.c: strlcpy() instead of strncpy()
        version.c: snprintf() instead of sprintf()

4.0.3   07/10/2006
        dhead5.c: define (unused) station name offset

4.0.2   06/26/2006
        chnlocmap.c: fixed signed/unsigned comparisons
        dhead6.c: included protos.h for prototypes
        protos.h: fixed prototypes

4.0.1   03/13/2006
        filltime.c: removed requirement that IDA handle be non-NULL in ida_filltime()

4.0.0   02/08/2006
        Introduction of the IDA handle and underlying database support.  Too
        many changes to list.  Moved flatfile lookup code over to libdbio.

3.0.2   12/20/2005
        dbdhead7.c: removed attempt to keep year sane based on external time stamp behavior

3.0.1   09/05/2005
        ida.h: added IDA_REV9_TAG
        format.c: initial release

3.0.0   08/25/2005
        db.c: fixed off by one bug in idadb_revdesc
        dbdhead[2-8].c: set (new) extra field to invalid (only valid in 9+)
        dbdhead9.c: support for new rev 9 to (with seqno's and creation time stamps)
        dhlen.c: added rev 9 support

2.2.0   07/24/2005
        db.c: added idaInitDB() and cleaned up integration with DBIO, a bit

2.1.4   06/10/2005 (cvs rtag libida_2_1_4 libida)
        db.c: added IDA_SERIAL_PORT code, but not yet debuged or incorporated 

2.1.3   02/10/2005 (cvs rtag libida_2_1_3 libida)
        db.c: aap win32 portability modes (path delimiter stuff)

2.1.2   08/19/2004 (cvs rtag libida_2_1_2 libida)
        db.c: look for flat files in etc subdir under user provided path

2.1.1   06/24/2004 (cvs rtag libida_2_1_1 libida)
        db.c: trim blanks from chn in idadb_sintmap(), WIN32 port (aap)
        ext1hzoff.c: fixed ancient bug in determining external time in seconds
        
2.1.0   06/21/2004 (cvs rtag libida_2_1_0 libida)
        ida.h: changed time_tag "true" field to "tru"
        dbevtrec5.c: changed time_tag "true" field to "tru"
        dbfilltime.c: changed time_tag "true" field to "tru"
        timstr.c: changed time_tag "true" field to "tru"

2.0.1   01/29/2004 (cvs rtag libida_2_0_1 libida)
        db.c: cleaned up leaking file descriptors

2.0.0   10/16/2003 (cvs rtag libida_2_1_0 libida)
        added version support
  
 */

char *idaVersionString()
{
static char string[] = "ida library version 100.100.100 and slop";

    snprintf(string, strlen(string), "ida library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *idaVersion()
{
    return &version;
}
