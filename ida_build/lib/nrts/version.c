#pragma ident "$Id: version.c,v 1.73 2009/02/03 22:28:32 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "nrts.h"

static VERSION version = {4, 10, 0};

/* nrts library release notes

4.10.0  02/03/2009
        ida10.c: improved logging of rejected packets, cleaned up logic flow a bit
        string.c: initial release

4.9.2   10/10/2008
        ida.c, ida10.c: stopped logging "no chndx" messages

4.9.1   09/11/2008
        ida10decode.c: ingore suspect bit in time status in UnreliableTimeTag()
            test.  This is to prevent apps that "dip" into the NRTS disk loop
            from thinking there is a corrupt packet (specifically nrts_alpha).
            I'm only doing this patently foolish thing to get nrts_alpha for
            KDAK running again.

4.9.0   03/05/2008
        ida.c: added support for NRTS_DL_FLAGS_LEAP_YEAR_CHK, setting the
            IDA9_FLAG_LEAPYEAR_BUG bit if leap year bug detected

4.8.6   02/08/2008
        convert.c: convert samples into network byte order on little-endian systems

4.8.5   02/03/2008
        convert.c: ignore lissUnpackMiniSeed return value and instead 
           check packet decode status to detect non-data packets to ignore

4.8.4   01/25/2008
        convert.c: sync time of next sample to current packet to avoid
                   accumulating one sample drift errors

4.8.3   01/16/2008
        idadecode.c: added new destination buffers to timstr function calls

4.8.2   01/10/2008
        dl.c: added missing return value in CreateIda10ConversionBuffer()

4.8.1   01/08/2007
        convert.c: added missing return value to StreamMatch()

4.8.0   01/07/2007 *** First version with automatic MiniSEED to IDA10 ***
        convert.c: created
        dl.c: added support for IDA10 conversion buffer, fixed
              nrtsWriteToDiskLoop() to ignore everything but explicity accepted packets

4.7.0   09/14/2007
        ida.c: distinguish between complete and partial overlaps
        ida.c: distinguish between complete and partial overlaps,
               improved debug messages

4.6.7   09/06/2007
        bwd.c: added station and channel name to ReduceWfdisc() sanity
            checks, added station/channel name in nsamp error messsage
        systems.c: changed nrts_systems() to ignore etc/Systems entries
            which are not for NRTS disk loops

4.6.6   08/28/2007
        systems.c: ignore systems which lack sys files (ie, non-NRTS)

4.6.5   06/23/2007
        ida10.c: support for NRTS_DL_FLAGS_LAX_TIME

4.6.4   06/21/2007
        ida10.c: added status->avail check to list of conditions that
                 define an unreliable time stamp

4.6.3   06/14/2007
        read.c: fixed shared file descriptor race condition

4.6.2   06/01/2007
        bwd.c: fixed spurious (zero sample) gaps

4.6.1   06/01/2007
        bwd.c: fixed chn nseg on initialization from disk,
               added new (NULL) chn args to nrtsRead
        bwdb.c: made default (dummy) action functions static
        dl.c: added new (NULL) chn args to nrtsRead
        ida.c, ida10.c: added IDA_EXTRA to NRTS_PKT, saved same
        read.c: added optional pointer to NRTS_CHN argument to nrtsRead
               to allow saving of snapshot of state at the time of read,
               changed LOG_ERR to LOG_INFO
        search.c: added new (NULL) chn args to nrtsRead

4.6.0   05/17/2007
        read.c: give busy disk loops up to 10 sec to idle before reading

4.5.1   04/18/2007
        bwd.c: support NRTS_DL_FLAGS_NO_BWD_FF option, use aap formula for
               computing gap size
        dl.c: support NRTS_DL_FLAGS_ALWAYS_TESTWD option in bwd determining
              action argument for nrtsUpdateBwd() in nrtsWriteToDiskLoop()
      
4.5.0   03/26/2007
        bwd.c: call nrtsUpdateBwdDb() if option set in dl flags
        bwdb.c: initial (stubbed) release
        dl.c, ida.c, ida10.c: renamed NRTS_DL_FLAGS_x constants

4.4.1   02/08/2007
        ida.c: ifdef'd out packet header string/status debug messages

4.4.0   01/31/2007
        bwd.c: NRTS_TESTWD support
        dl.c: call nrtsUpdateBwd() with NRTS_TESTWD in the case of a
            fresh start (eliminates extra wfdisc records that don't
            represent a true time tear)
        ida10.c: only check for gaps and overlaps in the case of system
            time tears (eliminates gaps due to erratic GPS)

4.3.2   01/22/2007
        ida10.c: only check GPS time quality in the event of tme tears

4.3.1   01/07/2007
        asldecode.c: strlcpy() instead of strcpy()
        bwd.c: strlcpy() instead of strcpy()
        ida10.c: strlcpy() instead of strcpy()
        idadecode.c: strlcpy() instead of strcpy()
        rcnf.c: strlcpy() instead of strcpy()
        syscode.c: strlcpy() instead of strcpy()
        version.c: snprintf() instead of sprintf()

4.3.0   01/04/2007
        Increased NRTS_MAXCHN to 128

4.2.0   11/08/2006
        ida10.c: generic time stamp/status, support for REAL64
        ida10decode.c: generic time stamp/status

4.1.6   08/14/2006
        bwd.c: fixed error introduced in 4.1.5
        ida10.c: removed premature 64 bit data type support

4.1.5   07/20/2006
        bwd.c: additional uninitialized NRTS_PKT.indx bug fix (GapSearch)

4.1.4   07/19/2006
        bwd.c: additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes
        dl.c: additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes
        search.c: additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes

4.1.3   07/18/2006
        bwd.c: fixed uninitialized beg/end pkt indicies bug in PopulateBwdList (aap)

4.1.2   07/07/2006
        ida.c: mark packets from channels missing from NRTS disk loop as
            NRTS_IGNORE instead of NRTS_REJECT, thus allowing downstream
            systems the choice of including them in their NRTS loops.

4.1.1   06/26/2006
        idadecode.c: removed unreference local variables
        syswd.c: split into unix and windows (dummy) versions

4.1.0   06/19/2006
        isi/dl.h: added indx field to NRTS_PKT and readonly handles to
            NRTS_ DL for smart reads (aap)
        bwd.c: redesigned recursive gap search to minimize stack
           requirements and use new smart nrtsRead for performance
           improvments (aap)
        dl.c: open/close new read-only handles in nrtsOpenDiskLoop and
           nrtsCloseDiskLoop (aap)
        fix.c: fix win32 initialization error (aap)
        read.c: smart read: don't re-read if dest pakcet is already for
           current index (aap)

4.0.3   04/20/2006
        dl.c: don't clopper pid when encountering a locked disk loop
        ida.c: fixed typo in CheckYear (aap)

4.0.2   04/17/2006
        dl.c: zero pid in sys before lock test if it matches current pid

4.0.1   03/13/2006
        dl.c: support changed field names in updated ISI_GLOB

4.0.0   02/08/2006
        asldecode.c: IDA handle and libisidb support (but dropped ASL support elsewhere)
        bwd.c: ISI_STREAM_NAME oldchn -> chnlock, set bwd status while locked
        chksys.c: removed unused IDADB argument
        decode.c: IDA handle support (and dropped ASL support)
        dl.c: IDA handle support
        fixwd.c: IDA handle support
        ida.c: IDA handle support, rework CheckYear for another try, oldchn -> chnloc
        idadecode.c: IDA handle support
        ida10.c: ISI_STREAM_NAME oldchn -> chnlock
        ida10decode.c: IDA handle support
        rcnf.c: removed unneeded includes
        syswd.c: IDA handle support
        wrtdl.c: IDA handle support

3.2.0   12/20/2005
        ida.c: added CheckForYearTransition() test

3.1.9   12/09/2005
        ida.c: patch suspect time stamps with fixed ext/hz time pair (eliminate bogus
               "backward external time jump" errors that previous beg/end pairs were causing)
               return NRTS_NOTDATA for non-data packets (instead of "failing")
        ida10.c: return NRTS_NOTDATA for non-data packets (instead of "failing")

3.1.8   11/03/2005
        bwd.c: explict casts to supress compiler warnings, removed unreferenced local variables
        dl.c: suppress microsoft 4550 compiler warnings, removed unreferenced local variables
        files.c: suppress microsoft 4273 compiler warnings
        ida.c: explict casts to supress compiler warnings, removed unreferenced local variables
        ida10.c: explicit casts to suppress compiler warnings

3.1.7   10/18/2005
        ida.c: SuspectExternalTime() changed to flag all unlocked clocks as suspect

3.1.6   10/09/2005
        ida10.c: fixed error tagging byte order for little-endian systems

3.1.5   10/06/2005
        bwd.c: added missing default return from SanityCheck
        dl.c: included value for new samp field in default NRTS_TOLERANCE
        ida.c: fixed branches in CheckYear that had returns without value
        ida10.c: replaced uninitialized errtol in TimeTearTest() with tol.samp from handle
3.1.4   09/30/2005
        bwd.c: create wfdisc record message demoted to LOG_DEBUG

3.1.3   09/14/2005
        dl.c: check for and quietly fail (ENOENT, LOG_DEBUG message) if not all
              the required NRTS files are present in nrtsOpenDiskLoop()

3.1.2   09/12/2005
        ida10.c: added missing errsmp to arg list for warning message in TimeTearTest()

3.1.1   09/06/2005
        ida.c: fixed SuspectExternalTime() logic

3.1.0   08/25/2005
        ida.c: added ida rev 9 support
        rcnf.c: added ida rev 9 support

3.0.2   07/27/2005
        bwd.c: set severity of create wfdisc record message to LOG_INFO
        dl.c: save hdr and dat offsets in pkt header (which is _essential_ to good bwds!)
        ida.c: set severity of time tear and suspect time messages to LOG_INFO
        ida10.c: set severity of time tear and suspect time messages to LOG_INFO

3.0.1   07/26/2005
        dl.c: flush disk loop on close, fixed SavePacket() bug with new disk loops
        ida10.c: removed leftover debug code

3.0.0   07/25/2005 *** Introduction of MT-safe I/O ***
        bwd.c: initial MT-safe release
        cnf.c: removed (to isidl)
        convert.c: removed (to isidl)
        datreq.c: removed (to isidl)
        dl.c: initial release
        files.c: removed nrtsFiles() 
        ida.c: added strict checks for NRTS_DL output
        ida10.c: added strict checks for NRTS_DL output
        liss.c: switched to current calling syntax (still unsupported, though)
        lookup.c: removed (to isidl)
        oldbwd.c: created from previous bwd.c (MT-unsafe bwd stuff)
        print.c: initial release
        prt.c: changed "latency" column to "last update"
        read.c: incorporated format conversion code from old convert.c
        search.c: removed calls to obsolete nrtsConvert()
        soh.c: removed (to isidl)
        systems.c: removed obsolete nrtsGetSystemList()
        utils.c: removed (to isidl)

2.6.1   06/24/2005 
        datreq.c: added nrtsExpandSeqnoRequest()

2.6.0   03/23/2005 (cvs rtag libnrts_2_6_0 libnrts)
        ida10.c: added (untested) support for 64-bit data types

2.5.0   02/07/2005 (cvs rtag libnrts_2_5_0 libnrts)
        convert.c: set orig len to 1024 for non-MK8 data
        idadecode.c: added nrtsToggleCheckTstampLoggingFlag()

2.4.1   01/19/2005 (cvs rtag libnrts_2_4_1 libnrts)
        idadecode.c: removed external-1Hz comparison between tofs and tols

2.4.0   09/28/2004 (cvs rtag libnrts_2_4_0 libnrts)
        chksys.c: fixed ancient but in nrts_fixsys
        ida10decode.c: improved error log

2.3.4   08/24/2004 (cvs rtag libnrts_2_3_4 libnrts)
        all: rebuild with new NRTS_DEFDBDIR

2.3.3   08/19/2004 (cvs rtag libnrts_2_3_3 libnrts)
        open.c: do not append /etc to db spec

2.3.2   06/30/2004 (cvs rtag libnrts_2_3_2 libnrts)
        lookup.c: set errno for all error branches in nrtsLookupX routines

2.3.1   06/24/2004 (cvs rtag libnrts_2_3_1 libnrts)
        asldecode.c: removed unnecessary includes (aap)
        bwd.c: removed unnecessary includes (aap)
        chksys.c: removed unnecessary includes, WIN32 "port" (aap)
        fixwd.c: removed unnecessary includes (aap)
        getmap.c: removed unnecessary includes (aap)
        ida10decode.c: removed unnecessary includes (aap)
        idadecode.c: removed unnecessary includes (aap)
        indx.c: removed unnecessary includes (aap)
        mmap.c: removed unnecessary includes, WIN32 port (aap)
        prt.c: removed unnecessary includes (aap)
        rcnf.c: removed unnecessary includes (aap)
        stawd.c: removed unnecessary includes (aap)
        syscode.c: removed unnecessary includes, WIN32 port (aap)
        sysident.c: removed unnecessary includes (aap)
        systems.c: removed unnecessary includes (aap)
        syswd.c: removed unnecessary includes, WIN32 "port" (aap)
        time.c: removed unnecessary includes (aap)

2.3.0   06/21/2004 (cvs rtag libnrts_2_3_0 libnrts)
        ida.c: changed time_tag "true" field to "tru
        idadecode.c: changed time_tag "true" field to "tru
        
2.2.1   06/11/2004 (cvs rtag libnrts_2_2_1 libnrts)
        files.c, open.c, systems.c: Add support for (new) home field in NRTS handle
            use nrtsGetSystemList as sole means of reading Systems file
        search.c: initial release

2.2.0   01/29/2004 (cvs rtag libnrts_2_2_0 libnrts)
        wrtdl.c: refuse to write overlapping packets

2.1.3   01/29/2004 (cvs rtag libnrts_2_1_3 libnrts)
        ida10.c: remove temporary variables in nrtsIda10Decoder()
        idadecode.c: initialize input buffer in nrts_idadecode()
        open.c: corrected uninitialized variable in OpenDiskLoops()

2.1.2   12/09/2003 (cvs rtag libnrts_2_1_2 libnrts)
        fixwd.c: improved function declarations to calm solaris cc
        ida.c: casts to calm solaris cc

2.1.1   11/21/2003 (cvs rtag libnrts_2_1_1 libnrts)
        open.c: changed utilParse args to calm gcc 2.X builds

2.1.0   10/16/2003 (cvs rtag libnrts_2_1_0 libnrts)
        cnf.c: initial release
        compress.c: removed
        convert.c: initial release
        datreq.c: initial release
        getsys.c: removed
        hstr.c: removed
        info.c: removed
        ida.c: initial release
        files.c: added nrtsFiles(), nrtsBuildFileNames(), nrtsFreeFileNames()
        lookup.c: initial release
        liss.c: initial (dummy) release
        netio.c: removed
        open.c: initial release
        packet.c: removed
        prt.c: removed nrts_prtreq()
        prtreq.c: removed
        read.c: initial release
        soh.c: initial release
        systems.c: added nrtsGetSystemList()
        tap.c: removed
        utils.c: initial release
        version.c: initial release
        wfdisc.c: initial release

2.0.0   06/09/2003 (cvs rtag libnrts_0_0_0 libnrts)
        Initial release
  
 */

char *nrtsVersionString()
{
static char string[] = "nrts library version 100.100.100 and slop";

    snprintf(string, strlen(string), "nrts library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *nrtsVersion()
{
    return &version;
}
