#pragma ident "$Id: ReleaseNotes.c,v 1.30 2007/01/11 18:06:48 dechavez Exp $"

char *VersionIdentString = "Release 3.1.0";

/* Release Notes

3.1.0   01/11/2007
        NRTS_MAXCHN increased to 128

3.0.1   06/02/2006
        removed UDP support

3.0.0   02/08/2006:
        mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)

2.24.1  09/15/2005:
        Fixed getrec problem for nrts feeds (not sure how it got commented
        out in the first place!)

2.24.0  07/25/2005:
        First build following libnrts 3.0.0 (should have no effect here)

2.23.2  06/24/2005:
        accomodate new design of ISI_DATA_REQUEST structure

2.23.1  06/10/2005:
        Rename isiSetLogging() to isiStartLogging()

2.23.0  05/23/2005:
        Win32 compatibility

2.22.0a 05/06/2005:
        checkpoint build following removal of old raw nrts constructs

2.22.0 04/29/2005:
       added optional logging of ISI:IACP recv stats

2.21.1 03/31/2005:
       relink with reentrant versions of utilDttostr and utilTsplit

2.21.0 01/27/2005:
       Added dbgpath debug support for ISI feeds
       Removed Jan 1 exclusion from check_year (ttyio)

2.20.0 09/28/2004:
       Fixed nasty race condition causing busy diskloops to become
       corrupted on shutdown.
       Relink with cleaned up libxfer (no more error 0x00 disconnects),
       turned down volume a little in the logs

2.19.2 08/19/2004:
       Change NRTS_DEFDB to /usr/nrts

2.19.0 04/26/2004:
       MySQL support added (via dbspec instead of dbdir)

2.18.3 03/01/2004:
       leapfix hack for 2004 added since efi is yet to be updated

2.18.2 02/18/2004: (cvs rtag nrts_wrtdl_2_18_2 nrts_wrtdl)
       Tee packets into $home/$syscode/isp/tee (so that we can
       use unmodified ispPushGz)

2.18.1 02/05/2004: (cvs rtag nrts_wrtdl_2_18_1 nrts_wrtdl)
       Tee packets at IDA_BUFSIZ, regardless of input size

2.18.0 02/05/2004:
       Added option to tee serial data

2.17.2 01/29/2004: (cvs rtag nrts_wrtdl_2_17_2 nrts_wrtdl)
       inits and cleanups to calm purify builds (relink with
       libraries updated in a similar manner)

2.17.1 12/22/2003 (cvs rtag nrts_wrtdl_2_17_1 nrts_wrtdl)
       Link with reentrant version of libxfer

2.17.0 12/21/2003 (cvs rtag nrts_wrtdl_2_17_0 nrts_wrtdl)
       Auto-flush wfdiscs at regular (fwd sec) intervals.
       Moved signal handling into its own thread.
       Relink with updated libraries, primary fix is expanding the list
       of transient network error conditions in order to avoid premature
       shutdowns in presence of network disruption.

2.16.4 12/09/2003 (cvs rtag nrts_wrtdl_2_16_3 nrts_wrtdl)
       Fixed error setting isi request start time when explicitly given
       on command line, relink with a batch of solaris cc calmed libraries

2.16.2 12/05/2003 (cvs rtag nrts_wrtdl_2_16_2 nrts_wrtdl)
       Fixed missing fid's in log message
       Removed some // comments causing old Solaris compiler to complain
       Relink with libisi 1.4.5, libutil 2.2.4, and libiacp 1.3.2, fixing
       various minor problems and some serious problems with x86 builds

2.16.1 11/26/2003 (cvs rtag nrts_wrtdl_2_16_1 nrts_wrtdl)
       Relink with libutil 2.2.3 to correct error when running
       in daemon mode

2.16.0 11/25/2003 (cvs rtag nrts_wrtdl_2_16_0 nrts_wrtdl)
       Added support for ISI input

2.15.0 05/23/2003 (cvs rtag nrts_wrtdl_2_15_0 nrts_wrtdl)
       Added support for UDP input

2.14.0 11/05/2002 (cvs rtag nrts_wrtdl_2_14_0 nrts_wrtdl)
       Added support for "short" IDA10 records

2.12.1 04/29/2002 (cvs rtag nrts_wrtdl_2_12_1 nrts_wrtdl)
       Relink with updated nrts_library where ida10 init routine
       checks disk loop for latest packet and decoder checks for
       overlaps

2.12.0 04/25/2002 (cvs rtag nrts_wrtdl_2_12_0 nrts_wrtdl)
       Relink with update xfer_library to remove potential infinite
       loop in select() based xfer_Read();

2.11.2 10/05/2001 (cvs rtag nrts_wrtdl_2_11_2 nrts_wrtdl)
       Relink with updated libraries allowing upper case channel names

2.11.1 09/08/2001 (cvs rtag nrts_wrtdl_2_11_1 nrts_wrtdl)
       Fixed error with incoming frames with less than 1024 bytes

2.11.0 09/08/2001 (cvs rtag nrts_wrtdl_2_11_0 nrts_wrtdl)
       Relink with updated libraries allowing non-fixed length
       IDA10 data packets

2.10.8 10/12/2000 (cvs rtag nrts_wrtdl_2_10_8 nrts_wrtdl)
       Log request details at level 2.

2.10.7 10/06/2000 (cvs rtag nrts_wrtdl_2_10_7 nrts_wrtdl)
       libutil mods: util_connect to retry at 30 sec intervals when
         gethostbyname() h_errno is TRY_AGAIN.  util_dsteim1() does
         not do sanity checks anymore.
       retry flagged forced on for asl_getrec() and asl2ida_getrec()
       and both to retry util_connect() when return value is 0 (ie,
       when connect() call failed)

2.10.6 09/19/2000 (cvs rtag nrts_wrtdl_2_10_6 nrts_wrtdl)
       libseed mods: seed_minitoida packet resets logged at level 2
                     don't reset connection on ASL packet decode
                     errors (just dump)
       Sleep for 30 seconds after first seed_readmini error in asl2ida

2.10.5 09/19/2000 (cvs rtag nrts_wrtdl_2_10_5 nrts_wrtdl)
       time tears logged at level 2

2.10.4 06/22/2000 (cvs rtag nrts_wrtdl_2_10_4 nrts_wrtdl)
       Fixed bug associated with correcting the problems associated
       with the leap year bug in the DAS.

2.10.3 03/09/2000 (cvs rtag nrts_wrtdl_2_10_3 nrts_wrtdl)
       Check for clobbered sys->pid's and reset if needed.

2.10.2 02/28/2000 (cvs rtag nrts_wrtdl_2_10_2 nrts_wrtdl)
       "leapfix" patch updated for 2000.

2.10.1 02/17/2000 (cvs rtag nrts_wrtdl_2_10_1 nrts_wrtdl)
       Started sane version numbering and release notes after 9 years!
       Improved bogus time tag checks and repair, totally silent by
       default, logged at debug level 2.
*/
