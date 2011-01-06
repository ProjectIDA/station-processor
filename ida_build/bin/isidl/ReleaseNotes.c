#pragma ident "$Id: ReleaseNotes.c,v 1.78 2009/05/14 16:42:58 dechavez Exp $"

char *VersionIdentString = "Release 1.19.5";

/* Release Notes

1.19.5  05/14/2009
        Changed missed barometer sample from -2 to 0xFFFFFFFF, changed paroOpen() to
        conform with libparo 1.2.1

1.19.4  12/15/2008
        Link with libisidl 1.11.1 in include sync'd mmaps and disk loop writes

1.19.3  12/09/2008
        Link with libqdp 1.3.1 to eliminate "out of window" messages

1.19.2  10/10/2008
        Link with libida 4.9.2 to eliminate the "no chndx" messages

1.19.1  03/06/2008
        link with libida 4.3.1 to correct bug in ARS leap year bug patch

1.19.0  03/05/2008
        Added support for Q330 aux packets (via libqdp 1.2.0)
        Added optional "intercept" processing (-intercept) and
        ARS leap year bug check (-leap)

1.18.3  02/08/2008
        Link with libnrts 4.8.6 to correct IDA10.5 sample data byte order
        problem on little endian systems.  Add SIGPIPE handler for Slate builds.
       
1.18.2  02/03/2008
        Link with libliss 1.2.2 to correct bug where non-data packets were
        not making it into the disk loop

1.18.1  02/02/2008
        Fixed incorrect name of exit handler for ARM_SLATE builds

1.18.0  01/25/2008
        Add support for scl (stream control list).  Link with libnrts 4.8.4
        to include smooth LISS time of next sample updates to eliminate
        spurious 1 sample blocking errors due to clock drift (when building
        IDA10 from MiniSEED)

1.17.4  01/14/2008
        Suppress signal handling thread for those platforms that have problems
        with sigwait receiving signals sent to the controlling process (ie, Slate)

1.17.3  01/08/2008
        Link with libliss 1.2.1 to address bus errors on Solaris builds when
        trying to convert ISI supplied LISS packets to NRTS

1.17.2  01/08/2008
        Link with libnrts 4.8.1 to fix spurious failures with liss conversion

1.17.1  01/07/2008
        MiniSEED to IDA10 conversion into library so that ISI feeds from
        LISS sites can also drive NRTS disk loops

1.17.0  12/21/2007
        Added NRTS disk loop support for MiniSEED packets from LISS feeds
        via repackaging as IDA10.5 packets (original data still available
        via ISI disk loop).

1.16.3  12/20/2007
        Fixed bug where 16-bit streams in RT593 equipped ARS systems were
        getting flagged as Ida9 instead of IDA9.

1.16.2  10/31/2007
        Lookup Q330_ADDR instead of serialno and authcode, link with various
        updated libraries that included strlcpy instead of blanket memcpy's
        (fixes some non-Solaris core dumps)

1.16.1  10/05/2007
        Link with libdbio 3.2.6, libdbio 1.3.2 and libisidl 1.10.1 for
        support for NRTS_HOME environment variable

1.16.0  09/14/2007
        Link with libnrts 4.7.0 and libisidl 1.10.0 for changes in overlap
        definition and response

1.15.0  09/07/2007
        Use Q330 config file via cfg option to specify digitizer addresses

1.14.3  06/23/2007
        Added -lax option to ignore suspicious bit

1.14.2  06/01/2007
        link with libnrts 4.6.2 to fix fixed spurious (zero sample) gaps

1.14.1  06/01/2007
        link with libnrts 4.6.1 to fix NRTS bwd nseg at startup error

1.14.0  05/03/2007
        Added LISS support

1.13.1  04/18/2007
        Added -gsras option to set NRTS DL flags for default wfdisc
        managment in Obninsk

1.13.0  03/28/2007
        RT593 option verified.

1.12.2x 03/26/2007
        Added RT593 option.  Remains to be tested, so do NOT deploy
        1.12.2x at sites equipped with RT593 digitizer boards.

1.12.2  02/19/2007
        Set log tag for single station isi sites to site instead of server.
        Fixed "reconnect at beginning" bug for isi feeds with specific beg seqno.

1.12.1  02/08/2007
        CompleteIDA10Header calls ida10InsertSeqno32().
        NRTS_MAXCHN increased to 128. LOG_ERR and LOG_WARN to LOG_INFO.
        libnrts4.4.0 for improved wfdisc support and IDA10 packet gaps fixed
        Local packet support via LOCALPKT handle contaning dl, mutex and pkt

1.12.0  12/12/2006
        Use QDP library callback for transparent handling of metadata

1.11.1  12/06/2006
        Preliminary 10.4 support (underlying library not fully tested).
        Use the streamlined QDPLUS_PKT
        
1.11.0  11/13/2006
        Updated 10.3 support, NRTS_MAXCHN increased to 64, added various
        -dbgxxx command line options

1.10.0  09/29/2006
        Default to 64Kbyte TCP I/O buffers

1.9.2c  08/18/2006
        Preliminary 10.3 support (underlying library not fully tested)
        Ignore IACP signatures

1.9.2   07/07/2006
        Better IDA5/6 support, link with libisidl 1.6.6 to copy lock tee
        files under Windows.

1.9.1   07/07/2006
        Link with libnrts 4.1.2 for fix allow streams absent in local
        NRTS disk loop to still be considered for inclusion in downstream
        systems.

1.9.0   06/30/2006
        Replaced message queues for handling locally acquired data with static
        buffers, eliminating the randcom core dumps on Q330 reconnect problem.

1.8.7   06/23/2006
        Added client side port parameter to Q330 argument list

1.8.6   06/20/2006
        Fixed Win32 build, link with libisi2.4.1

1.8.5   06/19/2006
        Proper handling of IDA 5 packets, link with libnrts4.1.0 for smart
        nrtsReads and more efficient bwd construction at initialization.

1.8.4   06/14/2006
        Fixed bogus failure in Q330 init code

1.8.3   06/12/2006
        Link with libisidl 1.6.3 for more rational synchronized mmap I/O

1.8.2   06/07/2006
        This time really fixed the uppercase ARS station names in IDA9 packets.

1.8.1   06/07/2006
        Fixed some QDP barometer bugs.  Unresolved crashes in Q330/barometer test.

1.8.0   06/02/2006
        Initial Q330/QDP support
        Link with libisi 1.6.2 for synchronized memory mapped updates

1.7.3   04/20/2006
        Fix bug allowing multiple instances to "lock" a single disk loop

1.7.2   04/17/2006
        Relink with libnrts 4.0.2 to eliminate bogus locks after system crash

1.7.1   04/07/2006
        Fixed initialization problem with systems that have no barometers

1.7.0   04/07/2006
        Link with libisidl 1.6.0 (remove gzip tee file support)

1.6.0   04/03/2006
        Added Paroscientific barometer support.  Redesigned ARS input to allow
        the inclusion of one or more barometer streams into an ARS system, or
        simply act as a standalone barometer station.  Change the design for
        station systems to use a message queue for generating new packets, and
        added starter hooks for eventual inclusion of an arbitrary number of Q330s.

1.5.0   03/13/2006
        Link with libisidl 1.5.0 (O_SYNC on write) !!!
        Replaced ini=file command line option with db=spec for global init
        Bracket ISI_DL access with mutexes to prevent race condition in shutdown
        from closing active disk loops (messy, probably needs to be addressed in
        the handle).

1.4.0   02/08/2006
        link with libida 4.0.0, libisidb 1.0.0 and neighbors (MySQL support)

1.3.0   12/20/2005
        Link with updated ida and nrts libraries that attempt to handle year
        transitions better, spurious year increments in particular

1.2.9   12/14/2005
        Fixed bug that clobbered ISI sequence numbers in ARS feeds, introduced
        in 1.2.8

1.2.8   12/09/2005
        Fixed duplicate packet problem from ARS feeds, fixed off by 1000 bug in
        serial I/O timeout

1.2.7   10/18/2005
        Link with libnrts 3.1.7, trying to keep bad time stamps out of NRTS disk loop

1.2.6   10/17/2005
        Link with liblogio 2.1.1, fixing LOG_ERR and LOG_WARN filtering problem

1.2.5   10/11/2005
        link with repaired isiClose()
        allow comma delimiter in tty spec (eg, "/dev/term/d5001,19200")

1.2.4   10/09/2005
        Fixed infinite loop bug in data request phase of reconnect to ISI server

1.2.3   10/06/2005
        Fixed bug clobbering common header of non-DATA records when reading tty data.

1.2.2   09/30/2005
        Fixed msec vs sec confusion in specifying timeout (user specifies seconds)
        Toggle IACP packet dumps on/off with SIGHUP

1.2.1   09/15/2005
        Fixed core dump on server disconnect bug

1.2.0   09/13/2005
        Reworked to use explicit disk I/O for ISI disk loop operations instead of
        mapped memory assignments.
        Changed default log to syslogd for daemon runs, stdout for foreground.
        Fixed bug causing core dump from ReadFromISI() when server disconnected.
        Include build date/time in version string output
        Added dbgpath option.
        Abandon ISI reconnects in case of server fault and other critical errors,
        otherwise sleep for a progressively increasing longer interval between attempts.

1.1.1   09/06/2004
        fixed bug in verifying IDA9 time offsets between packets

1.1.0   09/05/2005
        Convert tty input to ISI_TYPE_IDA9
        Changed default log spec to syslogd:user

1.0.1   07/27/2005
        Fixed problem with foff's in wfdiscs, better logging verbosity

1.0.0   07/25/2005
        Initial release
 */
