#pragma ident "$Id: ReleaseNotes.c,v 1.68 2009/05/14 16:37:32 dechavez Exp $"

char *VersionIdentString = "Release 3.5.0";

/* Release notes

3.5.0  05/14/2009
       Detached all threads to address isid memory leak, removed DestroySnapshot and
       instead link with libisidl 1.12.0 which has snapshots that don't involve a mutex

3.4.5  02/04/2009
       Use DestroySnapshot to deal avoid isidlSnapshot memory leak
       Added additional debug messages to seqno.c

3.4.4  01/26/2009
       Link with libisidl 1.11.2 to remove memory leak in isidlSnapshot()

3.4.3  01/06/2009
       Fixed unitialized pointer problem in convert.c:BuildNative(), fixed missing
       return value problem in seqno.c:CompareCounters().

3.4.2  10/10/2008
       Plumbing installed to accomodate eventual support of legacy NRTS service

3.4.1  05/24/2008
       Link with libiacp 1.7.9 to check for bad paremeter counts from clients,
       support generic packets of non-INT32 data types.

3.4.0  03/05/2008
       Removed WeirdIndexingProblemCheck() (bad memory leak)
       Added support for IDA9 flags (specifically, ARS leap year bug)

3.3.6  02/07/2008
       Fixed double free() and uninitialized pointer problem, added SIGPIPE
       handler for Slate builds

3.3.5  02/03/2008
       Link with libisi 2.6.1 to fix bug where non-data packets would get
       subjected to the seqno channel selection filter

3.3.4  02/01/2008
       SIGWAIT_THREAD_DOES_NOT_WORK support (from F. Shelly, ASL)

3.3.3  01/18/2008
       Channel selection filter moved to client supplied list

3.3.2  01/17/2008
       Fresh build following code cleanup

3.3.1  01/16/2008
       Fixed -nrts bug when serving compressed data

3.3.0  01/15/2008
       Support for ISI channel filtering via -nrts option.

3.2.1  01/07/2008
       IDA10.5 support confirmed

3.2.0  12/20/2007
       IDA 10.5 support (not tested)

3.1.3  10/02/2007
       Fixed uncompressed data bug (isid would send all data with IDA compression,
       even if the client requested uncompressed packets).
       Support for NRTS_HOME environment variable added via library updates.

3.1.2  06/21/2007
       Increment time stamps in generic packets by sample interval, if there are
       no time tears between IDA10 packets

3.1.1  06/14/2007
       Link with libnrts 4.6.3 to correct nrtsRead race condition
       Link with libisidl 1.9.5 to eliminate wfdisc records that point to more
       than 99,999,999 samples

3.1.0  05/17/2007
       Added -logtt option to track time tears fed to clients
       Link with linbrts 4.6.0 to try to avoid race condition reading fresh data

3.0.5  03/26/2007
       Fixed "lingering" threads on socket write bug

3.0.4  02/08/2007
       Fixed infinite loop bug introduced in 3.0.3

3.0.3  02/06/2007
       Fixed hanging ServiceThreads on abnormal disconnect (ie, timeout)

3.0.2  02/05/2007
       don't core dump on wd requests for a non-NRTS site, 
       fixed bug causing first channel in a request to be ignored, and bug
       causing duplicate packets instead of new channel packets when dl indices
       matched between streams

3.0.1  01/25/2007
       added -nolock option

3.0.0  01/11/2007
       NRTS_MAXCHN increased to 128.  Reworked design to have just one thread
       per client.  Fixed (stack overflow?) problem affecting time window data
       requests on OpenBSD servers.

2.3.1  11/13/2006
       IDA10.3 support, NRTS_MAXCHN increased to 64
       Note: possible hang under load bug evidently introduced in 2.3.0 has
             _not_ been addressed in this release.

2.3.0  09/29/2006
       Rather than have parent thread send heartbeats independent of service
       thread, it just sets a flag which the seqno or stream service threads
       check and act on after each poll sweep.
       Also, linked with libiacp 1.7.6

2.2.2  07/10/2006
       Include IDA rev 6 packets in the Fels' shift group, use calloc()
       instead of malloc() to create client data structures.

2.2.1  06/21/2006
       Fixed error serving stream requests introduced in 2.2.0.

2.2.0  06/19/2006
       Link with updated libnrts 4.1.0 to include intelligent nrtsRead.  Silently
       apply Fels' shifts to all IDA rev 5 packets before serving them.

2.1.1  06/02/2006
       fixed bug handling dp=spec argument
       link with libisidl 1.6.2 to ignore System entries that lack NRTS disk loops

2.1.0  03/13/2006
       replace ini=cfg_file to db=spec for global parameters

2.0.0  02/08/2006
       mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)

1.5.5  10/26/2005
       Eliminated ClientThreadExit() race condition (aap)

1.5.4  10/13/2005
       Don't start status server unless explicitly specified at start up
       Link with updated liblogio that will force out LOG_ERR messages

1.5.3  10/09/2005
       Fixed little-endian ida10 compression bug

1.5.2  9/30/2005
       demoted various messages to LOG_DEBUG
       debug trace stuff added to iacp library
       Fixed bug in ISI seqno search (libisi).
       Check for client termination on each seqno data pass instead of lingering until
       all available data are exhausted.

1.5.1  9/14/2005
       Reworked to use explicit disk I/O for ISI disk loop operations instead of
       mapped memory assignments, in order to remove scaling problem seen when
       number of systems was larger than 6.
       Changed default log to syslogd for daemon runs, stdout for foreground
       Various bug fixes in libiacp and libisi.

1.5.0  8/29/2005
       Use ISI_DL_MASTER instead of obsolete NRTS handle
       Use utilSetIdentity instead of setuid bits to set user
       Force IDA compression in stream requests, regardless of request
       Added gzip compression to seqno requests
       Changed default log spec to "syslogd:local3"

1.5.0(B) 6/29/2005
       Apparently working seqno request support, no compression

1.4.2b 06/10/2005
       Added support for seqno based requests

1.4.2  05/05/2005 (cvs rtag isid_1_4_2 isid)
       Checkpoint build following introduction of data structures to support
       seqno-based requests and raw digitizer packet transfer.  Those features
       are NOT implemented in this rev.

1.4.1  02/09/2005 (cvs rtag isid_1_4_1 isid)
       Fixed x86 compression error (again!)

1.4.0  01/27/2005 (cvs rtag isid_1_4_0 isid)
       Relink with iacp 1.5.0 to remove thread clash with heartbeats and data frames,
       fixed race condition which could cause uneeded heartbeats

1.3.5  09/29/2004 (cvs rtag isid_1_3_5 isid)
       Further attempts at getting x86 byte order issues cleared up

1.3.4  08/19/2004 (cvs rtag isid_1_3_4 isid)
       Relink with updated libraries to correct potential problem with interpretation
       of database specification

1.3.3  06/30/2004 (cvs rtag isid_1_3_3 isid)
       Log nrtsLookupX failures, various minor WIN32 portability modifications

1.3.2  06/21/2004 (cvs rtag isid_1_3_2 isid)
       Fixed uninitialzed pointer problem in BuildGenericUncompressed()
       Fixed problem serving data from NRTS disk loops which lacked implied location
       code (ie, those 3 char channel names).
       Set default database server to localhost (for MYSQL builds)

1.3.1  06/10/2004 (cvs rtag isid_1_3_1 isid)
       Added home directory option and support, use nrtsSearchDiskloop from library

1.3.0  04/26/2004 (cvs rtag isid_1_2_9 isid)
       relinked with MySQL support enabled libraries

1.2.8  01/29/2004 (cvs rtag isid_1_2_8 isid)
       relink with purfied libraries, version 1.4.0 iacp I/O,
       remove race condition in CloseClientConnection() log message

1.2.7  12/21/2003 (cvs rtag isid_1_2_7 isid)
       Include version number in status reports. Relink with updated
       libraries (libiacp, liblogio).

1.2.6  12/16/2003 (cvs rtag isid_1_2_6 isid)
       Fixed error (again) in compression logic on little endian servers

1.2.5  12/09/2003 (cvs rtag isid_1_2_5 isid)
       Yet another attempt to fix compression logic, various minor changes to
       calm solaris cc

1.2.4  12/04/2003 (cvs rtag isid_1_2_4 isid)
       Fix error in compression logic on little endian servers,
       Relink with libisi 1.4.5, libutil 2.2.4, and libiacp 1.3.2

1.2.3  11/26/2003 (cvs rtag isid_1_2_3 isid)
       Fixed error with heartbeats not getting sent in time.
       Fixed dangling data thread error (not exiting when client would declare a timeout).

1.2.2  11/25/2003 (cvs rtag isid_1_2_2 isid)
       Fixed error interpreting nrtsExpandStreamRequest() return value

1.2.1  11/04/2003 (cvs rtag isid_1_2_1 isid)
       Relink with logio 1.2.1

1.2.0  11/04/2003 (cvs rtag isid_1_2_0 isid)
       Fixed search routines for looking for a packet with a
       specific time

1.1.0  11/02/2003 (cvs rtag isid_1_1_0 isid)
       Send expanded data requests back to client as acknowledgement.
       Include client index in reports, for debugging
       Corrected program name in report generator and win32svc.c
       Moved some stuff into library and similar internal cleanup

1.0.0  10/16/2003 (cvs rtag isid_1_0_0 isid)
       Initial release

*/
