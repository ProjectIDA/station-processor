#pragma ident "$Id: version.c,v 1.42 2009/03/17 17:22:16 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "isi/dl.h"

static VERSION version = {1, 12, 0};

/* isidl library release notes

1.12.0 03/17/2009
       dl.c: reworked disk loop locks to use libutil utilWriteLockWait()
             and utilReadLockWait(), removed mutex in ISI_DL handle

1.11.4  02/23/2009
        dl.c: restored MUTEX_INIT to isidlSnapshot() (dealing with memory
            at the application level)

1.11.3  02/03/2009
        dl.c: added debug messages in isidlReadDiskLoop()
        glob.c: added isidlLoadDebugFlags()

1.11.2  01/26/2009
        dl.c: removed unwise MUTEX_INIT in isidlSnapshot() (cased memory leaks)

1.11.1  12/15/2008
        dl.c: restored O_SYNC when writing NRTS files, set sync flag in all isidlSysSetX calls

1.11.0  01/07/2008 *** First version with automatic MiniSEED to IDA10 ***
        dl.c: automatically convert MiniSEED to IDA10/NRTS, if possible

1.10.4  11/05/2007
        dl.c: add NRTSCompatiblePacket() test before attempting to
              update NRTS disk loop

1.10.3  11/05/2007
        glob.c: backed out short sighted "fix" added in 1.10.2

1.10.2  10/31/2007
        glob.c: removed memory leak in isidlSetGlobalParameters()
        print.c: added buffer length to utilTimeString() calls

1.10.1  10/05/2007
        glob.c: added support for NRTS_HOME environment variable

1.10.0  09/14/2007
        dl.c: removed ISI_DL_FLAGS_TIME_DL_OPS_ENABLED related code, 
              improved debug and log messages, added support for both
              partial and complete overlaps, fixed problem with plugged
              NRTS diskloops following first rejected packet

1.9.5   06/12/2007
        wfdisc.c: added EnforceSampleMaximum() to prevent illegal wfdisc
                  records (more than 99,999,999 samples) from being generated

1.9.4   04/18/2007
        dl.c: note state file in dl flags, if present
        print.c: note state file, if present

1.9.3   02/08/2007
        dl.c: disk loop writers keep files open, added conditional support
        for ISI_DL_FLAGS_TIME_DL_OPS (not compiled in this build)
         
1.9.2   01/31/2007
        wfdisc.c: allow for missing NRTS disk loop

1.9.1   01/22/2007
        dl.c: added support for ISI_DL_FLAGS_IGNORE_LOCKS flag, and changed
        isiReadDiskLoop not choke on indices explicitly set as undefined.
        glob.c: added flags field

1.9.0   01/11/2007
        Renamed all functions to use isidl prefix instead of isi.

1.8.2   01/11/2007
        datreq.c: renamed all the "stream" requests to the more accurate "twind" (time window)

1.8.1   01/08/2007
        datreq.c: switch to size-bounded string operations
        dl.c: switch to size-bounded string operations
        tee.c: switch to size-bounded string operations
        version.c: switch to size-bounded string operations

1.8.0   12/12/2006
        dl.c: added metadata directory support, isidlCrntSeqno()
        meta.c: initial release
        print.c: print availability of metadata

1.7.1   12/08/2006
        print.c: include sequence number of oldest packet in isiPrintDL()

1.7.0   11/08/2006
        dl.c: flock locking only if ENABLE_DISKLOOP_LOCKS defined (and
              it isn't in this build)
        glob.c: added ttag to ISI_DEBUG

1.6.7   09/29/2006
        dl.c: fixed typo in fcntl error reports in LockDiskLoop/UnlockDiskLoop,
        don't attempt to unlock a non-locked file
        util.c: cleared tabs

1.6.6   07/10/2006
        tee.c: prevent open tee files from being copied under Windows

1.6.5   06/26/2006
        cnf.c: use dummy "when" for isiLookupCoords
        master.c: fixed signed/unsigned compares, removed unreferenced local variables

1.6.4   06/20/2006
        utils.c: allow for ISI systems w/o NRTS disk loops in isiLocateNrtsDiskloop()

1.6.3   06/12/2006
        dl.c: added sync arguments to isidlSysX functions such that
            disk syncs didn't happen everytime the structure was
            changed, just after critical updates.
            Added O_BINARY to OpenAndSeek flags for Win32 compatibility (aap)
        sys.c: made sync to disk optional via new "BOOL sync" argument

1.6.2   06/02/2006
        cnf.c: allow for missing NRTS disk loop
        dl.c: wrappped all changes to mapped ISI sys structure with calls
              to isidlSysXXX functions that use msync() to flush to disk
        soh.c: allow for missing NRTS disk loop
        sys.c: initial release

1.6.1   04/20/2006
        dl.c: added isidlUpdateParentID(), check for matching locks

1.6.0   04/07/2006
        dl.c: removed references to obsolete fields in tee handle
        tee.c: remove gzip support (was causing data loss when plug was pulled)

1.5.0   03/15/2006
        dl.c: O_SYNC on write (this was a major oversight(!))
        glob.c: major rework to use libisidb lookups instead of flat file
        print.c: include tee file name in isiPrintDL() output

        tee.c: support changed field names in updated ISI_GLOB 

1.4.0   02/08/2006
        cnf.c: isiLookupSitechan() instead of isiLookupInst()
        dl.c: fixed race condition in LockDiskLoop()
        glob.c: pass LOGIO to dbioOpen
        master.c: warn instead of fail when unable to open a master list site

1.3.4   12/09/2005
        dl.c: recognize NRTS_NOTDATA status from decoder, changed isiWriteDiskLoop()
              to take flags argument and added support for new ISI_REJECT_DUPLICATES option

1.3.3   10/19/2005
        utils.c: fixed uninitialized variable bug in isiLocateNrtsDiskloop()

1.3.2   10/18/2005
        dl.c: log failed fcntl in LockDiskloop closer to event

1.3.1   10/09/2005
        tee.c: gzip by default, require nozip flag file to disable

1.3.0   09/30/2005
        dl.c: changes to tee handle (for gzip), changed name of ReadRaw for Win32 sanity
        glob.c: initialize db isiInitDefaultGlob()
        search.c: deal with requests for packets outside the bounds of the disk loop
        tee.c: support on the fly enable/disable and gzip on/off

1.2.0   09/09/2005
        dl.c: major rework to use explicit hdr and raw file I/O instead of
              mapped memory operations
        glob.c: added support for lock debugger
        print.c: ISI_DL_SYS instead of ISI_DL_HDR

1.1.0   08/25/2005
        dl.c: added tee support
        glob.c: added tee (trecs) support

1.0.2   07/27/2005
        dl.c: only write NRTS_ACCEPTed packets to NRTS disk loop

1.0.1   07/26/2005
        db.c: removed deadlock in isiLookupInst()
        dl.c: include mutex lock in disk loop lock, lock diskloop while closing,
              removed deadlock in isiOpenDiskLoop()

1.0.0   07/25/2005
        cnf.c: initial release
        datreq.c: initial release
        db.c: initial release
        dl.c: initial release
        glob.c: initial release
        master.c: initial release
        print.c: initial release
        search.c: initial release
        soh.c: initial release
        string.c: initial release
        utils.c: initial release
        wfdisc.c: initial release
 */

char *isidlVersionString()
{
static char string[] = "isi library version 100.100.100 and slop";

    snprintf(string, strlen(string), "isi library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *isidlVersion()
{
    return &version;
}
