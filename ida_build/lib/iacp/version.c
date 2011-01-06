#pragma ident "$Id: version.c,v 1.30 2009/02/03 22:31:44 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "iacp.h"

static VERSION version = {1, 7, 11};

/* iacp library release notes

1.7.11  02/03/2009
        close.c: destroy mutex in iacpClose() (this was a memory leak)

1.7.10  08/20/2008
        This is a build with the beginings of structures to support
        NRTS protocol services, but no actual functionality is in place.
        handshake.c: added branches to handle NRTS protocol handshakes
        nrts.c: preliminary version with stubs for iacpNrtsClientHandshake() and iacpNrtsServerHandshake()
        send.c: stub for iacpSendNrtsFrame() added

1.7.9   08/05/2008
        handshake.c: check for bad paremeter counts from clients

1.7.8   03/26/2007
        recv.c: retry on all ignorable errors in read_to

1.7.9   02/08/2007
        send.c: use send() instead of write()

1.7.8   01/22/2007
        accept.c changed LOG_ERR messages to LOG_INFO
        connect.c changed LOG_ERR messages to LOG_INFO
        handshake.c changed LOG_ERR messages to LOG_INFO
        open.c changed LOG_ERR messages to LOG_INFO
        recv.c changed LOG_ERR messages to LOG_INFO
        send.c changed LOG_ERR messages to LOG_INFO
        server.c changed LOG_ERR messages to LOG_INFO

1.7.7   01/07/2007
        connect.c: fixed bug preserving errno in iacpConnect
        util.c: strlcpy() instead of strcpy()
        version.c: snprintf() instead of sprintf()

1.7.6   09/29/2006
        recv.c: init frame stuff on entry
        send.c: set disabled flag on I/O errors, check for flag and
        fail immediately if set, log each write_to failure

1.7.5   10/09/2005
        close.c, recv.c, send.c: debug tracers removed

1.7.4   10/06/2005
        recv.c: more debug tracers added

1.7.3   09/30/2005
        close.c, recv.c, send.c: debug tracers added

1.7.2   09/30/2005
        connect.c: fixed error message on utilSetHostAddr failure
        free.c: fixed memory leak (free attr.at_dbgpath)
        open.c: improved error message on iacpConnect failure
        util.c: check for NULL iacp in iacpDisconnect()

1.7.1   09/14/2005
        free.c: removed incorrectly added dbgpath free()

1.7.0   05/05/2005 (cvs rtag libiacp_1_7_0 libiacp)
        get.c, recv.c, send.c, util.c: added support for IACP_STATS
        stats.c: created

1.6.0   03/23/2005 (cvs rtag libiacp_1_6_0 libiacp)
        recv.c: set errno and handle error flags with single call
        util.c: fixed memory protection violation (on some platforms) (aap)

1.5.1   02/11/2005 (cvs rtag libiacp_1_5_1 libiacp)
        send.c: win32 portability mods (aap)

1.5.0   01/27/2005 (cvs rtag libiacp_1_5_0 libiacp)
        accept.c: use iacpInitHandle instead of copy to initialize new handle
        close.c: dbgfp support
        free.c: at_dbgpath support
        recv.c: read in chunks which mimic send.c, as aid in debugging
        send.c: wrap all of iacpSendFrame() with handle mutex, use new dump debug funcs
        util.c: added dbgfp and at_dbgpath support, fixed attr initialization error in
            iacpInitHandle, and added new functions iacpDumpDataPreamble(), 
            iacpDumpDataPayload(), and iacpDumpSignaturePreamble

1.4.3   06/18/2004 (cvs rtag libiacp_1_4_3 libiacp)
        iacp.h, connect.c: removed at_myport attribute

1.4.2   06/17/2004 (cvs rtag libiacp_1_4_2 libiacp)
        iacp.h, connect.c: added at_myport attribute for clients

1.4.1   06/04/2004 (cvs rtag libiacp_1_4_1 libiacp)
        recv.c: various AAP windows portability modifications
        util.c: various AAP windows portability modifications

1.4.0   01/29/2004 (cvs rtag libiacp_1_4_0 libiacp)
        connect.c: use iacpErrorLevel() to test for severity
        recv.c: cleaned up logic in read/write loops, log ignored errors
        send.c: cleaned up logic in read/write loops, log ignored errors
        util.c: added iacpErrorLevel()

1.3.4   12/21/2003 (cvs rtag libiacp_1_3_4 libiacp)
        connect.c: expanded list of transient error conditions
        util.c: iacpPeerIdent() NULL arg returns static string instead of NULL ptr

1.3.3   12/09/2003 (cvs rtag libiacp_1_3_3 libiacp)
        server.c: include util.h to calm solaris cc

1.3.2   12/04/2003 (cvs rtag libiacp_1_3_2 libiacp)
        recv.c: added EWOULDBLOCK to list of ignored select failures,
            fixed bug causing unneeded failures for ignored error codes
        send.c: added EWOULDBLOCK to list of ignored select failures

1.3.1   11/19/2003 (cvs rtag libiacp_1_3_1 libiacp)
        close.c: include util.h to calm compiler
        connect.c: include util.h to calm compiler
        handshake.c: unpack UINT32's with proper unpacker (benign)
        recv.c: tuned debug levels
        send.c: tuned debug levels, added some casts to calm compiler
        util.c: fixed iacpInitHandle declaration 

1.3.0   11/03/2003 (cvs rtag libiacp_1_3_0 libiacp)
        Switched units of at_timeo and at_wait to msec
        recv.c: adjust timeout now that units are msec
        send.c: adjust timeout now that units are msec

1.2.1   11/02/2003 (cvs rtag libiacp_1_2_1 libiacp)
        connect.c: use utilCloseSocket() to reset value if sd in IACP handle
        recv.c: tuned logging verbosity

1.2.1   10/16/2003 (cvs rtag libiacp_1_2_0 libiacp)
        Many bug fixes and enhancements, to numerous to mention.

1.1.0   06/30/2003 (cvs rtag libiacp_1_1_0 libiacp)
        Set handle logging threshold to agree with logioOpen debug parameter,
        replaced various macros with mutex locked function calls, rework
        receive logic to support dynamic changes in retry attribute, 
        increased size of maximum payload (subframe data) to 32K, renumber
        NRTS subframe tags.

1.0.0   06/09/2003 (cvs rtag libiacp_1_0_0 libiacp)
        First release
  
 */

char *iacpVersionString()
{
static char string[] = "iacp library version 100.100.100 and slop";

    snprintf(string, strlen(string), "iacp library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *iacpVersion()
{
    return &version;
}
