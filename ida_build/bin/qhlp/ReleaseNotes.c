#pragma ident "$Id: ReleaseNotes.c,v 1.22 2009/05/14 18:11:55 dechavez Exp $"

char *VersionIdentString = "Release 1.6.0";

/* Release notes

1.6.0  05/14/2009
       Include sequence number in main() event warning messages
       Added support for /usr/nrts/etc/debug toggling

1.5.0  01/26/2009
       Link with libisidl 1.11.2 to remove isidlSnapshot() memory leak, added
       liblogio 2.4.1 SIGHUP handler for watchdog logging.

1.4.2  01/15/2009
       added logioUpdateWatchdog() tracers

1.4.1  12/16/2008
       Fixed empty qhlp state file bug

1.4.0  12/14/2008
       Handle corrupt state files gracefully.
       Link with libisidl x.x.x in include sync'd mmaps and disk loop writes

1.3.6  10/10/2008
       Link with libida 4.9.2 to eliminate the "no chndx" messages

1.3.5  03/05/2008
       CNP316 (aux packet) support

1.3.4  01/07/2008 
       *** little-endian support ***

1.3.3  12/20/2007
       Link with libq330 1.0.5 to accept calib entries in the cfg file
       (not yet using this information)

1.3.2  12/14/2007
       Link wth libqdp 1.0.5 to correct signed/unsigned datum bug

1.3.1  09/25/2007
       Continually update pre-event memory (even during detections), link with
       libdetect 1.0.2 to permit immediate triggers following post event dump

1.3.0  09/25/2007
       Rework trigger framework into a more general "process" path which currently
       just runs the event detector.  The event detector has been generalized into
       an opaque structure managed by libdetect.  All detector parameters are 
       specfied via the q330.cfg file.  Linked with libdetect 1.0.0 which includes
       a simple STA/LTA trigger.

1.2.0  09/14/2007
       Fixed problem saving/recovering state on shutdown/startup
       Set ISI dl options to reject overlapping packets from ISI disk loop
       Fixed problem with NRTS disk loop getting plugged on Q330 restarts
       Enabled station system checks to detect short packets and keep them out of NRTS
       Added framework for event detector and implemented manual trigger
       Added -dbgpkt, -dbgttag, -dbgbwd, -dbgdl, -dbglock and -notrig command line options

1.1.0  09/07/2007
       Set user id to "nrts" or that which is specified via user= on the command line
       Specify Q330 digitizer parameters via name (look up in cfg= config file)

1.0.2  06.26.2007
       Fixed missing state file bug.
       Set "station system" flag for NRTS disk loop so that strict packet checks
       would detect short packets (and reject them from NRTS disk loop).

1.0.1  06.26.2007
       Fixed fixed decompression error (libqdp 1.0.2)

1.0.0  06/14/2007
       Initial production release

*/
