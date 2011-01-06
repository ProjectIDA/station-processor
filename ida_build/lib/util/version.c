#pragma ident "$Id: version.c,v 1.51 2009/03/17 18:17:52 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "util.h"

static VERSION version = {3, 8, 0};

/* util library release notes

3.8.0  03/17/2009
       lock.c: added utilWriteLockWait(), utilWriteLockTry(), utilReadLockWait(),
               utilReadLockTry(), utilReleaseLock(),
               removed utilLockFileWait() and utilUnlockFile()
       log.c: allow specifying log facility with syslog option

3.7.0  10/02/2008
       binprint.c: added utilPrintBinUINT16
       string.c: added utilDotDecimalString

3.6.0  03/13/2008
       lock.c: added utilLockFileWait() and utilUnlockFile()

3.5.0  01/25/2008
       list.c: added listCopy()

3.4.8  01/07/2008
       binprint.c: little-endian fix for utilPrintBinUINT32()
       list.c: made formerly static DestroyNode() public listDestroyNode()

3.4.7  10/30/2007
       peer.c: replace string memcpy w/ strlcpy
       timefunc.c: replace string memcpy w/ strlcpy, add length to utilTimestring() args

3.4.6  10/17/2007
       pack.c: added support for Slate ARM5 cpu REAL64s (fshelly)

3.4.5  09/25/2007
       mkfile.c: allow for zero length files 

3.4.4  09/14/2007
       misc.c: added utilDeleteFile() to unix builds

3.4.3  06/28/2007
       misc.c: removed extra *hp from linux builds
       peer.c: use socklen_t instead of int
       string.c: added strlcpy for systems without HAVE_STRLCPY defined

3.4.2  01/31/2007
       log.c: fixed long standing bug handling "syslogd" specifier

3.4.1  01/07/2007
       Makefile: removed isfloat.o
       etoh.c: strlcpy() instead of strcpy()
       log.c: strlcpy() instead of strcpy(), vsnprintf() instead of vsprintf()
       mkpath.c: strlcpy() instead of strcpy()
       peer.c: strlcpy() instead of strcpy()
       timefunc.c: removed some strcpy()'s, some remain
       version.c: snprintf() instead of sprintf()

3.4.0  12/12/2006
       dir.c: initial release
       io.c: initial release

3.3.0  12/06/2006
       binprint.c: intial release of utilPrintBinUINT8() and utilPrintBinUINT32()
       timefunc.c: allow NULL destination buffers (use mt-unsafe local storage)

3.2.3  11/08/2006
       timefunc.c: cosmetic changes to utilTimeString()

3.2.2  06/26/2006
       lock.c: made separate unix and windows (dummy) functions

3.2.1  06/14/2006
       misc.c: fixed error in Linux gethostbyaddr_r call

3.2.0  05/15/2006
       misc.c: allow for NULL hostname in utilSetHostAddr() (INADDR_ANY)
       timer.c: changed utilStartTimer() interval parameter from int to UINT64

3.1.0  04/27/2006
       timer.c: added utilStartTimer(), utilInitTimer(), utilElapsedTime()
                and utilTimerExpired()

3.0.4  11/03/2005
       list.c: removed unreferenced local variables

3.0.3  09/30/2005
       mkpath.c: 09-30-2005 aap win32 port
       string.c: utilPadString(), utilTrimString()

3.0.2  09/12/2005
       mmap.c: removed MAP_ALIGN option

3.0.1  08/25/2005
       misc.c: added utilDirectoryExists()
       timefunc.c: fixed typo in UTIL_LONGTIME_FACTOR

3.0.0  07/25/2005 (bumped full rev just because nrts and isi went full rev today)
       list.c: added listRemoveNode() and listClear()

2.9.1  07/06/2005
       log.c: added hook to permit use of logio routines
       setid.c: initial release

2.9.0  06/29/2005
       misc.c: set ENOENT errno when utilSetHostAddr() unable to determine address
       timefunc.c: INT64 timestamp support
       timer.c: INT64 timestamp support

2.8.0  05/26/2005
       rwtimeo.c: added utilRead() and utilWrite() (msec timeouts)

2.7.0  05/24/2005 (cvs rtag libutil_2_7_0  libutil)
       mkfile.c: initial release
       mmap.c: initial production release
       query.c: utilPause() to return char that was pressed
 
2.6.3  05/05/2005 (cvs rtag libutil_2_6_3  libutil)
       peer.c: fixed benign #endif;

2.6.2  04/03/2005 (cvs rtag libutil_2_6_2  libutil)
       timefunc.c: oops, didn't get all the gmtimes in 2.6.1.  OK now.

2.6.1  03/31/2005 (cvs rtag libutil_2_6_1  libutil)
       timefunc.c: make utilTsplit and utilDttostr reentrant throught the 
                   use of gmtime_r instead of gmtime.

2.6.0  03/23/2005 (cvs rtag libutil_2_6_0  libutil)
       misc.c: Linux portability mods (aap)
       timefunc.c: added type 14 and 15 strings to utilDttostr (aap)

2.5.1  02/07/2005 (cvs rtag libutil_2_5_1  libutil)
       BufferedStream.c: changed lstrcpy to STRCPY macro for unix portablity

2.5.0  02/07/2005 (cvs rtag libutil_2_5 0  libutil)
       BufferedStream.c: initial release

2.4.1  12/20/2004 (cvs rtag libutil_2_4_1  libutil)
       rwtimeo.c: aap win32 mods

2.4.0  12/09/2004 (cvs rtag libutil_2_4_0  libutil)
       timefunc.c: add support for SAN epoch timestamps to utilDttostr()

2.3.0  09/28/2004 (cvs rtag libutil_2_3_0  libutil)
       connect.c: allow for blocking sockets via negative port numbers in util_connect()

2.2.10 08/24/2004 (cvs rtag libutil_2_2_10 libutil)
       timefunc.c: fixed utilYdhmstod bug in setting up date_time parameters for util_htoe

2.2.9 06/30/2004 (cvs rtag libutil_2_2_9 libutil)
      log.c: remove "obnoxious "logging suspended" message in util_logclose()

2.2.8 06/24/2004 (cvs rtag libutil_2_2_8 libutil)
      list.c: various AAP windows portability modifications
      lock.c: various AAP windows portability modifications
      log.c: various AAP windows portability modifications
      parse.c: various AAP windows portability modifications
      syserr.c: various AAP windows portability modifications

2.2.7 06/10/2004 (cvs rtag libutil_2_2_7 libutil)
      list.c: various AAP windows portability modifications
      log.c: various AAP windows portability modifications
      misc.c: various AAP windows portability modifications
      parse.c: various AAP windows portability modifications
      peer.c: various AAP windows portability modifications

2.2.6 01/29/2004 (cvs rtag libutil_2_2_6 libutil)
      peer.c: fix memory leak in utilPeerAddr()

2.2.5 12/09/2003 (cvs rtag libutil_2_2_5 libutil)
      isfloat.c: added includes to calm solaris cc
      swap.c: renamed swappers to UINTx, INTZx's now being util.h macros

2.2.4 12/04/2003 (cvs rtag libutil_2_2_4 libutil)
      timefunc.c: fixed bizarre bug with utilAttodt return value
          getting lost on x86 gcc builds
      bcd.c compress.c getline.c log.c: removed tabs

2.2.3 11/26/2003 (cvs rtag libutil_2_2_3 libutil)
      bground.c: do not close all open file descriptors

2.2.2 11/21/2003 (cvs rtag libutil_2_2_2 libutil)
      removed fnmatch.c

2.2.1 11/19/2003 (cvs rtag libutil_2_2_1 libutil)
      list.c: included util.h to calm certain compilers
      parse.c: added explicit strtok_r declaration
      swap.c: made util_lswap and util_sswap macros
      
2.2.0 11/14/2003 (cvs rtag libutil_2_2_0 libutil)
      bground.c: recast util_bground() into call to utilBground()
      dump.c: obsoleted
      hexdump.c: initial version
      log.c: removed Sigfunc casts
      parse.c: removed dead code, recast util_parse into call() to utilParse()
      signal.c: removed util_signal and made util_sigtoa() almost reentrant
      timefunc.c: merged in old style functions from timefuncOLD.c
      timefuncOLD.c: removed

2.1.1 11/02/2003 (cvs rtag libutil_2_1_1 libutil)
      list.c: improved error checking in listCreate() and listSetArrayView()
      misc.c: utilCloseSocket() changed to return INVALID_SOCKET

2.1.0
        bground.c: utilBackGround() to return an ENOSYS failure for
            non-unix builds
        dump.c: removed utilLogHexDump() (moved to logio library as
            logioHexDump())
        list.c: imported from ex-liblist
        misc.c: added tests for invalid arguments
        pack.c: added utilPackREAL64(), utilUnpackREAL64(), fixed case
            sensitivity error in various function names revealed for
            LTL_ENDIAN_HOST builds
        parse.c: added utilStringTokenList(), made other functions 
            reentrant via strtok_r instead of strtok
        timer.c: initial release

2.0.0   06/09/2003 (cvs rtag libutil_0_0_0 libutil)
        Initial release
  
 */

char *utilVersionString()
{
static char string[] = "util library version 100.100.100 and slop";

    snprintf(string, strlen(string), "util library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *utilVersion()
{
    return &version;
}
