#pragma ident "$Id: version.c,v 1.27 2009/02/04 06:09:45 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "ida10.h"

static VERSION version = {2, 7, 5};

/* ida10 library release notes

2.7.5  02/03/2009
       string.c: added alert characters for suspect time PrintStatusFlag() and unused bytes in ida10TSHDRtoString()
       unpack.c: fixed sign problem in sint() (aap)

2.7.4  08/20/2008
       ttag.c: set new "fresh" bit in generic ttag status

2.7.3  04/03/2008
       ttag.c: fixed core dumping typo in ttag.c when printing usupported
        time tags (IDA10_TIMER_GENERIC in this case, which are still unsupported)

2.7.2  04/02/2008
       read.c: added ida10.5 support

2.7.1  01/07/2008
       unpack.c: added ida10PackGenericTtag()

2.7.0  12/21/2007
       unpack.c: added ida10.5 support
       ttag.c:   added IDA10_TIMER_GENERIC support

2.6.9  06/21/2007
       ttag.c: added support for 'derived' time status field

2.6.8  06/01/2007
       unpack.c: use IDA_DEFAULT_EXTRA to initialize ida10CMNHDR "extra"

2.6.7  05/11/2007
       ttag.c: fixed Q330 filter delay compensation

2.6.6  04/18/2007
       string.c:  use static buffer if output not supplied in calls to
        ida10CMNHDRtoString(), ida10TSHDRtoString(), ida10TStoString()

2.6.5  03/06/2007
       ttag.c: fixed bug printing offset in PrintTtag()

2.6.4  01/08/2007
       unpack.c: strlcpy() instead of strncpy()
       version.c: snprintf() instead of sprintf()

2.6.3  01/04/2006
       ttag.c: made ida10TtagIncrErr debug messages a compile time option,
               added support for incerr struct in TS header
       unpack.c: support for nanosecond sint and incerr struct in TS header

2.6.2  12/21/2006
       misc.c: initial release
       string.c: update 10.4 support
       ttag.c: update 10.4 support
       read.c: update 10.4 support
       unpack.c: update 10.4 support

2.6.1  12/08/2006
       string.c: 10.4 support
       ttag.c: preliminary 10.4 support
       unpack.c: preliminary 10.4 support

2.6.0  11/08/2006
       read.c: removed some debug printfs
       reorder.c: added REAL64 support
       string.c: added REAL64 support, generic clock status
       ttag.c: generic timetag support
       unpack.c: updated 10.3 support, added proper REAL64 support

2.5.1  08/18/2006
       read.c: added 10.3 support
       unpack.c: added 10.3 support

2.5.0  08/14/2006
       reorder.c: removed premature 64 bit data type support
       string.c: removed premature 64 bit data type support, added OFIS ttag support
       ttag.c:
       unpack.c:

2.4.1  10/06/2005
       string.c: include format.subformat in ida10CMNHDRtoString()
       unpack.c: include format and subformat in common header, work
                 around bogus nbuytes in early 10.2 CF records

2.4.0  08/25/2005
       read.c: added 10.2 support
       string.c: added 10.2 support
       unpack.c: added 10.2 support

2.3.0  03/23/2005 (cvs rtag libida10_2_3_0 libida10)
       reorder.c: added (untested) support for 64-bit data types
       string.c: added (untested) support for 64-bit data types
       unpack.c: added (untested) support for 64-bit data types

2.2.0   12/09/2004 (cvs rtag libida10_2_2_0 libida10)
        string.c: added ida10TtagToString(), ida10ClockStatusString()
        ttag.c: added ida10SystemTime(), ida10PhaseToUsec(), modified
            ida101HzTime() to use latter

2.1.4   06/25/2004 (cvs rtag libida10_2_1_4 libida10)
        unpack.c: fixed dangling comment in WIN32 ifdef causing nasty ComputeEndTimeTS bug

2.1.3   06/24/2004 (cvs rtag libida10_2_1_3 libida10)
        read.c: removed unnecessary includes (aap)
        sint.c: removed unnecessary includes (aap)
        string.c: removed unnecessary includes (aap)
        ttag.c: removed unnecessary includes (aap)
        unpack.c: removed unnecessary includes, WIN32 port (aap)

2.1.2   12/09/2003 (cvs rtag libida10_2_1_2 libida10)
        sint.c: included math.h
        unpack.c: use sign/unsigned specific utilUnpack...s

2.1.1   11/13/2003 (cvs rtag libida10_2_1_1 libida10)
        ida10.h: removed uneeded includes

2.1.0   10/16/2003 (cvs rtag libida10_2_1_0 libida10)
        reorder.c: initial release
        string.c: added ida10DataTypeString(), and type specific max/min
            in ida10TStoString()
        unpack.c: added support for all (uncompressed) data types in
            ida10UnpackTS()
        version.c: initial release

2.0.0   Initial release
  
 */

char *ida10VersionString()
{
static char string[] = "ida10 library version 100.100.100 and slop";

    snprintf(string, strlen(string), "ida10 library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *ida10Version()
{
    return &version;
}
