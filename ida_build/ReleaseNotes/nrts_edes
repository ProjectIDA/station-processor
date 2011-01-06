#pragma ident "$Id: ReleaseNotes.c,v 1.26 2009/03/20 21:05:51 dechavez Exp $"

char *VersionIdentString = "Release 3.3.1";

/* Release Notes

3.3.1   03/06/2009
        Add support for log=syslgod:facility

3.3.0   09/11/2008
        Work around FreeBSD off the bat core dump by dynamically allocating EdesParams
        in main.c.  Note that this version is linked with libnrts 4.9.1 which ignores
        the suspect bit in the time stamps.  Who knows what havoc that will cause.

3.2.6   03/06/2008
        Link with libida 4.3.1 to deal with ARS leap year patch

3.2.5   01/15/2008
        Link with libdbio 3.3.0 and libisidb 1.3.4 to correct errors loading sint
        flatfile.  Fail with server fault when sint data are incomplete.

3.2.4   01/8/2008
        IDA10.5 support

3.2.3   08/28/2007
        Link with librts 4.6.6 so that Systems files with non-NRTS disk loops don't
        cause us to choke

3.2.2   03/06/2007
        Increment time stamps in generic packets by sample interval, if there are
        no time tears

3.2.1   02/14/2007
        Patch time stamps in generic packets if GPS is suspect but system time
        increments OK

3.2.0   01/11/2007
        NRTS_MAXCHN increased to 128, isidb function names

3.1.2   12/06/2006
        Only support 2 and 4 byte data types, since that is all the protocol can support

3.1.1   11/13/2006
        IDA10.3 support, NRTS_MAXCHN increased to 64

3.1.0   08/14/2006
        pause no more than 1 sec between packets

3.0.0   02/06/2006
        mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)

2.17.0  09/06/2005
        IDA9 support

2.16.0a 05/06/2005
        checkpoint build following removal of old raw nrts constructs

2.16.0 09/28/2004
       Don't reject requests which are legal but for unsupported station/channels.
       Send heartbeats instead and maybe we won't get persistent reconnects from
       misconfigured clients.
       Improved debug logging

2.15.1 09/17/2004
       Fix off-by-one-day bug in serving generic MK7 data

2.15.0 09/16/2004
       Fix bug with resolving station string

2.14.1 05/18/2004
       Suppress logging of EWB.PTWC.NOAA.GOV connections.

2.14.0 04/26/2004
       MySQL support added (via dbspec instead of dbdir)
       Various verbose debugging statements added to help diagnose NEIC firewall
       problem and left in place because I don't want to mess with this program
       anymore.

2.13.1 12/09/2003 (cvs rtag nrts_edes_2_13_1 nrts_edes)
       relink with a bunch of solaris cc calmed libraries

2.13.0 05/05/2003 (cvs rtag nrts_edes_2_13_0 nrts_edes)
       Send XFER_FINISHED in a slow loop when done.  Leave it up to the
       client to notice and disconnect

2.12.5 03/13/2003 (cvs rtag nrts_edes_2_12_5 nrts_edes)
       Refuse to compress corrupt ida10 records (ida10_compress mod), 
       fixed bug reverting to uncompressed data when ida10 compression
       failed to reduce size

2.12.4 01/29/2003 (cvs rtag nrts_edes_2_12_4 nrts_edes)
       Link with version of libxfer that addresses the broken(?) getdtablesize
       problem worked around in src/lib/xfer/common.c for Solaris 9

2.12.3 09/09/2002 (cvs rtag nrts_edes_2_12_3 nrts_edes)
       Explicitly set sockets to non-blocking (not really required, this
       is just left over from debugging Solaris 9 build errors which
       had nothing to do with this but it is harmless to leave in)

2.12.2 04/25/2002 (cvs rtag nrts_edes_2_12_2 nrts_edes)
       Relink with libxfer.a to eliminate potential infinite loops

2.12.1 04/01/2002 (cvs rtag nrts_edes_2_12_1 nrts_edes)
       Reject malformed requests (nsta or nchan too big),
       log requested stations and channels

2.12.0 03/11/2002 (cvs rtag nrts_edes_2_12_0 nrts_edes)
       Optionally dump bad ida packets to disk

2.11.5 02/18/2002 (cvs rtag nrts_edes_2_11_5 nrts_edes)
       Seed output option splits 5-char chans into chan/loc

2.11.4 10/24/2001 (cvs rtag nrts_edes_2_11_4 nrts_edes)
       Relink with setjmp/longjump free xfer library

2.11.3 09/08/2001 (cvs rtag nrts_edes_2_11_3 nrts_edes)
       Relink with updated libraries allowing non-fixed length
       IDA10 data packets

2.11.2 02/08/2001 (cvs rtag nrts_edes_2_11_2 nrts_edes)
       Fixed search bug when target time lands on the edge of the
       disk loop.

2.11.1 06/07/2000 (cvs rtag nrts_edes_2_11_1 nrts_edes)
       Relink with corrected libida that fixes problem with looking
       up parameters in sint table.

2.11.0 03/15/2000 (cvs rtag nrts_edes_2_11_0 nrts_edes)
       Removed reliance on frozen configuration file in favor of new
       lookup table (sint).  Added SIGHUP handler to print peer coordinates
       and list of requested stations.

2.10.1 02/17/2000 (cvs rtag nrts_edes_2_10_1 nrts_edes)
       Started sane version numbering and release notes after 9 years!
*/
