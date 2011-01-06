#pragma ident "$Id: globals.c,v 1.89 2009/02/04 23:52:07 dechavez Exp $"
/*======================================================================
 *
 * Declaration of isp library globals, and release notes.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "isp.h"

char *isp_release = "IDA Station Processor Release 4.4.2";
 
/* Release notes

4.4.2  02/04/2009
       Link with libida 4.3.2 to fix channel map lookup bug that has been
       causing config operations to fail since at least 4.3.5!

4.4.1  12/15/2008
       Link with libisidl 1.11.1 in include sync'd mmaps and disk loop writes

4.4.0  08/21/2008
       Added iboot support

4.3.6  01/07/2007
       Use correct paths for less and ls in isp/display in non-Solaris builds

4.3.5  01/07/2007
       Linked with updated libraries that support all sorts of ISI stuff
       not used here, but we bump the version number all the same in case
       there are side effects that haven't shown up.

4.3.4  10/31/2007
       Link with updated libraries with additional use of strlcpy, snprintf
       (retrofit by no means complete)

4.3.3  02/08/2007
       Link with libnrts4.4.1 for improved IDA10 time tag gap logic and
       cleaner wfdisc managment

4.3.2  01/25/2007
       RT593 support

4.3.1  Don't reject unreliable time tagged IDA10 packets from NRTS unless
       they are after a system time tear.

4.3.0  01/04/2007
       increase NRTS_MAXCHN to 128, removed digital power monitor and tape

4.2.1  11/08/2006
       increase NRTS_MAXCHN to 64, support for generic time stamp and status

4.2.0  10/01/2006
       Do not require that DAS detector match ISP expected.

4.1.5  07/07/2006
       Link with libnrts 4.1.2 for fix allow streams absent in local
       NRTS disk loop to still be considered for inclusion in downstream
       systems.

4.1.4  06/19/2006
       Link with libnrts 4.1.0 for smart nrtsRead and gap search

4.1.3  06/12/2006
        Link with libisi 1.6.3 for more rational synchronized mmap I/O

4.1.2  06/05/2006
        Link with libisi 1.6.2 for synchronized memory mapped updates
        Use liboldmsgg

4.1.1  04/17/2006
       Relink with libnrts 4.0.2 to eliminate bogus locks after system crash

4.1.0  04/07/2006
       Relink with libisidl 1.6.0 (eliminated .gz tee file support)

4.0.1  03/15/2006
       Relink with liboldparo (name change only)

4.0.0  03/15/2006
       Support for libida 4.+ and libisidb 1.+ (IDA handle, MySQL database)
       O_SYNC on write for isi disk loop

3.1.6  12/20/2005
       Link with updated ida and nrts libraries that attempt to handle year
       transitions better, spurious year increments in particular.

3.1.5  11/03/2005
       Don't core dump on undecipherable peer connects.
       Fixed problem of SAN data gaps on startup, introduced in 3.1.0

3.1.4  10/18/2005
       Link with libnrts 3.1.7, trying to keep bad time stamps out of NRTS disk loop

3.1.3  10/17/2005
       Link with liblogio 2.1.1, fixing LOG_ERR and LOG_WARN filtering problem

3.1.2  10/11/2005
       gzip tee files by default, support for updated stage/burn/stat scripts

3.1.1  09/30/2005
       fixed bug in MK8 CF 10.1 nbytes field
       support for on the fly ISI tee+gzip added

3.1.0  09/09/2005
       switched to explicit ISI disk I/O

3.0.1  09/06/2005
       fixed error detecting IDA9 time tag errors

3.0.0  09/05/2005
       first release with ISI/NRTS/IDA9/IDA10.2 support

2.4.6  04/05/2005 (cvs rtag isp 2_4_6 isp)
       debugged version of 2.4.5 (duh)

2.4.5  04/04/2005 (cvs rtag isp 2_4_5 isp)
       relink with thread-safe utilDttostr and utilTsplit

2.4.4  02/07/2005 (cvs rtag isp 2_4_4 isp)
       nrtsToggleCheckTstampLoggingFlag on SIGUSR2 (ispd)
       
2.4.3  01/19/2005 (cvs rtag isp_2_4_3 isp)
       Relink with libnrts to supress external/1Hz tofs vs tols check

2.4.2  12/16/2004 (cvs rtag isp_2_4_2 isp)
       Auto-flush wfdiscs at regular intervals

2.4.1  09/29/2004 (cvs rtag isp_2_4_1 isp)
       Improved log updates on VT100 displays
       Default MK7 dbpsec is /usr/nrts

2.4.0  04/26/2004 (cvs rtag isp_2_3_4 isp)
       MySQL support added

2.3.3  01/29/2004 (cvs rtag isp_2_3_3 isp)
       Relink with purified code

2.3.2  12/09/2003 (cvs rtag isp_2_3_2 isp)
       Relink with cleaned up libraries

2.3.1  10/02/2003 (cvs rtag isp_2_3_1 isp)
       Fixed cdrom eject problem for systems running vold, fixed
       isp display console hang, added display timeout, improved
       (a bit) display refresh for serial consoles

2.3.0  11/05/2002 (cvs rtag isp_2_3_0bis isp)
       SAN Release 1.11.3 compatibility mods: remove IDA8 only
       restriction on tape output, added NRTS recognition of
       "short" records.

2.2.9  10/09/2002 (cvs rtag isp_2_2_9 isp)
       Formal 2.2.9 release

2.2.9rc2 09/12/2002 (cvs rtag isp_2_2_9rc2 isp)
       Release candidate, DPM support 

2.2.8z 09/09/2002 (cvs rtag isp_2_2_8z isp)
       Checkpoint build with generalized aux device support, including
       new DPM support

2.2.8  05/15/2002 (cvs rtag isp_2_2_8 isp)
       Fixed buffer overflow problem with DAS config using long
       channel names, fixed missing "Output Buffer" data in DAS
       isp tape status display.

2.2.7  03/16/2002 (cvs rtag isp_2_2_7 isp)
       Added IDA10.x support (variable length records), and changed
       isp (monitor) to only run on localhost and to consult system
       run file for ispd c&c port.

2.2.6  02/26/2002 (cvs rtag isp_2_2_6 isp)
       Fixed setuid related problems with executing CD-R related
       scripts when ispd started by root at boottime.

2.2.5  02/21/2002 (cvs rtag isp_2_2_5 isp)
       Added support for CD-R output in MK-7 systems. Fixed error
       with using alternate IDA channel maps.  Fixed problem with
       premature ISO image flushes in CD-R systems.

2.2d   12/20/2001 (cvs rtag isp_2_2D isp)
       CDROM support and linked with modified libxfer which eliminates
       setjmp, longjump and, (knock, knock) fixes the problems of
       spurious returns from the msgq routines.  ISP console (isp)
       supports CDROM commands for SAN systems only.

2.2c   10/08/2001 (cvs rtag isp_2_2C isp)
       Add support for systems with no output media selected

2.2b   09/08/2001 (cvs rtag isp_2_2B isp)
       Suppress printing of ignored SOH fields

2.2a   05/31/2001 (cvs rtag isp_2_2A isp)
       Relink with libsanio 1.1.1 (recognize SAN_CONFIG_ASNPARAMS)
       
2.2    05/21/2001 (cvs rtag isp_2_2 isp)
       Fixed ispd core dump when SAN digitizer reboots.
       Rework to use platform.h MUTEX, THREAD, and SEMAPHORE macros.
 
2.1g   05/07/2001 (cvs rtag isp_2_1_G isp)
       Relaxed definition of corrupt time stamp to permit the "normal"
       Magellan missed and auto-incremented values.

2.1f   05/02/2001 (cvs rtag isp_2_1_F isp)
       Added support for SAN20xx rev 1.3.0, force flush of SAN command
       queue if filled.

2.1e   04/19/2001 (cvs rtag isp_2_1_E isp)
       ISP_DEBUG_BARO and ISP_DEBUG_CLOCK commands added.
       Fixed bug introduced in 2.1d which resulted in uninitialized
       internal Hz times.
       SAN ADC timeout audible alarm in isp. 

2.1d   02/23/2001 (cvs rtag isp_2_1_D isp)
       2.1c was a bad idea ("ct" would block on the message queue).
       Reverted to 2.1b and then added a check to eject flag when ignoring
       flushes due to 0 output records.
       Eliminated time offset tolerance test in read_extclock().

2.1c   02/23/2001 (cvs rtag isp_2_1_C isp)
       Allow 0 record massio flushes, to prevent inadvertent delayed tape
       ejects due to change tape operations commenced before any data
       are present in the output buffer.

2.1b   01/11/2001 (cvs rtag isp_2_1_B isp)
       Set SO_REUSEADDR socket option for server threads in ispd.

2.1a   11/08/2000 (cvs rtag isp_2_1_A isp)
       Fix bug in determining SAN uptime following SAN reboot.
       Fix libsanio bug in decoding SAN filter table.

2.1    11/08/2000 (cvs rtag isp_2_1_2 isp)
       First production release with full SAN support.
    
2.0(b6) 11/02/2000 (cvs rtag isp_2_0_B6 isp)
       Everything except for CF encapsulation.

2.0(b4) 11/02/2000 (cvs rtag isp_2_0_B4 isp)
       Beta release with full SAN support.

2.0.5  10/19/2000 (cvs rtag isp_2_0_5 isp)
       isp<->ispd protocol to include handshake with digitizer type
       socket i/o to use util pack/unpack routines
       isp begining to be digitizer aware

2.0.4  09/19/2000 (cvs rtag isp_2_0_4 isp)
       Initial SAN 20xx support.

1.2i   02/17/2000 (cvs rtag isp_1_2i isp)
       Improved bogus time tag handling in NRTS code.

1.2h   11/22/1999
       Modified barometer facility to detect incorrect parameters
       and to reconfigure the device if necessary.  Default PR and
       UF parameters can be overridden in the run file.

1.2g   8/21/1999
       Corrected bug in isp "vc" command that caused display to be
       suppressed when detector was disabled.  Modified channel name
       display in nrts summary so that columns lined up OK when short
       names were being used.
       
1.2f   6/03/1999
       Added tee support.  Creating a "tee" subdirectory will cause
       all Data packets to get written to files in the subdirectory.
       Each hour the files are flushed and a new one started.
       NO AUTOMATIC CLEANUP OF THESE FILES DONE IN ISPD.

       Modified idaddecode.c in libnrts to no longer print all those
       corrupt/suspect record detected messages when doing the strict
       time stamp checks.

1.2e   2/17/1999
       Fixed bug in nrts library that caused core dump when printing
       error message about corrupt packet header.
       Modified ida library to support 24-bit filter 20 and so prevent
       the above "error" condition from occuring in the first place.

1.2d   5/21/1998
       Fixed bug in patch.c that was kept the Data packets from getting
       the updated station code.  Fixed a minor bug in decode.c that
       could cause DAS alarm to get cleared when a barometer packet
       came in.  Added code to insure idents are always at the front
       of a tape, code which used to exist and somehow got lost it
       the shuffle (probably between 1.1f and 1.2).
 
1.2c   5/4/1998
       Check for EINTER errors from accept().
 
1.2b   4/3/1998
       Log transitions in DAS status flags (calib, trigger on/off).
       Make msgq_init smarter, and not memset the mmap()'d massio
       files unless they are too small.  This significantly improves
       startup speed.
 
1.2a   4/2/1998
       Various bug fixes to get 1.2 to really work as advertised.

1.2    3/31/1998
       Major redesign of output subsystem.  First, modified msgq to
       to support mmap()'d buffers.  Defined a new Heap/Q pair (obuf)
       that handles a variable number of buffers for massio (defined
       by new parameter, numobuf).  Massio thread now simply grabs
       empty (mmap()'d) buffers from the Heap, fills them with packets
       and releases them downstream when full or on demand.  The tape_write
       function is now a thread that pulls new buffers from massio and
       dumps them to tape.  This should permit a significant reduction in
       the size of the packet Heap, as everything should just whiz through
       the system, and it allows greater buffering of data in case media
       errors cause the system to temporarily lose the output device.

1.2    3/31/1998
       Modified msgq to support mmap()'d buffers and allow multiple
       massio output buffers to exist.  A separate thread pulls full
       buffers out of a new message queue and writes them to the mass
       storage.

1.1f   3/30/1998
       ispd
          Include and isplog and inject record counters in packet clear
          command.
          tapeio and massio to gracefully give up when an I/O error
          occurs... this is to prevent msgq buffer overflows.

1.1e   3/16/1998
       Corrected serious memory overrun problem in ispd (was using
       a 1024 byte buffer to send the channel map which is easily
       exceeds that).  Resolved by sending the individual map
       entries as independent messages.  Corresponding code changes
       in include/isp.h, lib/isp/socket.c, and isp.  Now during the
       configuration phase the working streams get the mapped names
       assigned OK.

       Increased NRTS_MAXCHN in nrts.h from 24 to 32.  Big consequences
       for existing disk loops, as the mmap()'d sys file has now changed
       size.

1.1d   3/12/1998
       ispd:
           Automatic DAS reboot when having difficulty syncing to
           the uplink stream (defined by the amount of data dumped
           so far... currently hard-coded to dumped % 8192)

1.1c   3/10/1998
       ispd:
           Fixed bug in tapeio.c that caused 1st record on a pre-recorded
           tape to be preserved even when ERASE option was selected (I had
           forgotten to rewind after doing the test read).
       isp:
           Modified isplog command in isp to deal with active nrtslog file.
           Need to to a soft link from nrtslog to isplog/active for this
           to work.

1.1b   3/6/1998
       Added daslog and isplog commands to isp.

1.1a   3/6/1998
       Begining of log support.  Defined the isp_inject service and
       IDA_ISPLOG record type and added appropriate code to libraries,
       isp, and ispd.  Created two new programs: isp_logpkt to format
       stdin to IDA_ISPLOG records, and isp_inject to take such packets
       from stdin and inject these into the running ispd via the isp_inject
       service (which is supported by a new server thread in ispd).
       Ispd now does on the fly decoding of DAS log messages and writes
       them to files in ~nrts/sta/isp/daslog/yyyydddd where the filename
       gives the date the record was received by the ISP.  It is asssumed
       that an external process takes care of managing these log files
       and deletes them from time to time (ie, the script isp_flushlog)

       p.s. it appears that release 1.0.5e cured (knock, knock) the
            core dumps in ispd.
       
1.0.5e 3/03/1998
      Essentially stabs in the dark trying to understand the more
      than occasional "unsupported protocol" core dumps in ispd.
      However, there was a bug in ispd:
          changed strcpy(dbdir, token[1]) to dbdir = strdup(token[1])
          which is certain to at least permit problems!
          Also, ifdef'd out the dynamic client allocation in serve.c
          and hard coded 10 simultaneous clients max.

1.0.5d 2/27/1998
      isp.h:
        Added last.change and last.write status parameters
      libisp.a:
        Send and receive last.change and last.write in socket.c
      ispd:
        Update last.change and last.write as required
      isp:
        Display last.change and last.write
        highlight non-standard states of Trg/Cal/Cnf status

1.0.5c 2/24/1998
      isp:
        GPS clock format changed to year:day-hh:mm:sec
        audible alarms highlighted in the alarm display

1.0.5b 2/23/1998
      isp:
        improved audible alarm handling
      ispd:
        decoupled clock from barometer in init (before, if you didn't have
        a barometer, it would not attempt to read the aux clock)

1.0.5 2/20/1998
      isp:
        various changes to user interface as per requests from IGPP
      libttyio.a: added parity selection as a ttyio_init option
      libparo.a: added parity selection to ttyio_init call
      libsclk.a: added parity selection to ttyio_init call
      ispd:
        dasio.c: added parity to ttyio_init call

1.0.4 1/6/1998
      ispd:
        decode.c: added code so that barometer packets would not get
          patched (the year patch for GPS encoded years in the DAS was
          causing the barometer time stamps to have year 1997)
        nrts.c: modified nrts_iniwrtdl call to include new argmuent
          (the "opt" parameter, hard coded to zero (tic tolerance))

      libnrts.a: tic tolerance in idadecode (requires extra argument
          to nrts_iniwrtdl).

1.0.3 12/02/1997
      ispd:
        fixed typo in msgq.c that could potentially cause a core dump
        while trying to report a problem with the queue logic

1.0.2 11/13/1997
      ispd:
        fixed spurious "waiting for calibration start time" message
        when requesting immediate calibs

        silently send SOH requests following calib start/stop requests
        to reduce the delay in updating the cal status in the isp display
 
1.0.1 11/12/1997
      isp:
        calibrate: remove user defined calibration parameter option

      ispd:
        Makefile: -lseed -lm because of asldecode in libnrts.a
        das_write: recognize preset calibration commands
        serve.c: set initilized flag when client array is valid, and
                 then test in set_cnflag

      libnrts.a: ASL decode support included

1.0 - 11/4/1997 Initial Production Release

*/
