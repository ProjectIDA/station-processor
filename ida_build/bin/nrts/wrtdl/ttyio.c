#pragma ident "$Id: ttyio.c,v 1.12 2006/02/10 02:07:35 dechavez Exp $"
/*======================================================================
 *
 *  TTY related routines.
 *
 *====================================================================*/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include "wrtdl.h"
#include "nrts.h"
#include "util.h"
#include "ida.h"
#include "oldttyio.h"

#define TTYTAP(buf, n) if (tap > 0) write(tap, buf, n)

static int tap = -1;
static int fd;
static int (*syncfunc)();
static long dumped = 0;
static int timeout;
static long speed;
static struct nrts_ida *info = NULL;
static struct nrts_sys *sys = NULL;
static IDA *ida;
static int flow = TTYIO_FSOFT;
static struct termios old_tty;
static int have_tty = 0;
static int fix_leap_times;

static int ida_rev3sync(int ttyfd, char *obuf, long *start, int timeout)
{
unsigned char byte1, byte2, byte3;
static char *fid = "ida_rev3sync";

    if (util_read(ttyfd, &byte1, (int) 1, timeout) != 1) return -1;
    TTYTAP(&byte1, 1);
    if (util_read(ttyfd, &byte2, (int) 1, timeout) != 1) return -1;
    TTYTAP(&byte2, 1);
    if (util_read(ttyfd, &byte3, (int) 1, timeout) != 1) return -1;
    TTYTAP(&byte3, 1);

    dumped = 0;
    while (!(byte1 == 0x02 && byte2 == 'D' && byte3 == 0xff)) {
        byte1 = byte2;
        byte2 = byte3;
        if (util_read(ttyfd, &byte3, (int) 1, timeout) != 1) return -1;
        TTYTAP(&byte3, 1);
        if (++dumped % 10000 == 0) {
            util_log(2, "%s: attempting to sync, %ld bytes dumped so far",
                     fid, dumped);
            util_log(2,"%s: VERIFY MODEM PORT SPEED IS %ld", fid, speed);
        }
    }
    obuf[0] = byte2;
    obuf[1] = byte3;
    *start = 2;

    return NRTS_OK;

}

static int ida_rev5sync(int ttyfd, char *obuf, long *start, int timeout)
{
char byte1, byte2;
static char *fid = "ida_rev5sync";

    if (util_read(ttyfd, &byte1, (int) 1, timeout) != 1) return -1;
    TTYTAP(&byte1, 1);
    if (util_read(ttyfd, &byte2, (int) 1, timeout) != 1) return -1;
    TTYTAP(&byte2, 1);

    dumped = 0;
    while (byte1 != ~byte2 || ida_rtype(&byte1, ida->rev.value) == IDA_UNKNOWN) {
        byte1 = byte2;
        if (util_read(ttyfd, &byte2, (int) 1, timeout) != 1) return -1;
        TTYTAP(&byte2, 1);
        if (++dumped % 10000 == 0) {
            util_log(2, "%s: attempting to sync, %ld bytes dumped so far",
                     fid, dumped);
            util_log(2,"%s: VERIFY MODEM PORT SPEED IS %ld", fid, speed);
        }
    }
    obuf[0] = byte1;
    obuf[1] = byte2;
    *start  = 2;
    
    return NRTS_OK;
}

static void patch_data(char *buffer, struct nrts_ida *info)
{
static short nsamp = 480;
static char  tqual = 6;

/*  Set tqual and nsamp to legal values  */

    memcpy(buffer +  5, &tqual, 1);
    memcpy(buffer + 42, &nsamp, 2);

/*  Force year in header to be year at init (will need to reset each Jan 1) */

    memcpy(buffer + 52, "NRTS", 4);
    memcpy(buffer + 56, &info->mk4_year, 2);  /* WILL NEED TO DEAL WITH BORDER */

}

static void check_year(char *buffer, int rev)
{
short stemp;
long ltemp;
IDA_DHDR dhead;
time_t now;
struct tm *gmt;
short true_year;
static short dl_year = -12345;

/*  Only valid for revs which have year stored at offset 6  */

    if (rev < 5) return;

/*  Determine the current year  */

    now = time(NULL);
    gmt = gmtime(&now);
    true_year = (short) (gmt->tm_year + 1900);
    if (dl_year == -12345) dl_year = true_year;

/*  See what year the data logger thinks it is  */

    if (ida_dhead(ida, &dhead, buffer) != 0) return;

/*  Force the datalogger year to match the real year, if we are not near  */
/*  a year boundary (to avoid dealing with slightly incorrect clocks).    */
/*  Year errors near year boundaries will not be caught as a consequence. */

#define DEC31(a) (a->tm_mon == 11 && a->tm_mday == 31)
#define JAN01(a) (a->tm_mon ==  0 && a->tm_mday ==  1)
 
    if (!DEC31(gmt)) {
        if (dhead.beg.year != true_year) {
            if (dl_year != dhead.beg.year) {
                util_log(2, "INCORRECT YEAR (%hd) forced to %hd",
                         dhead.beg.year, true_year);
            }
            stemp = true_year;
            SSWAB(&stemp, 1);
            memcpy(buffer+6, &stemp, 2);
            dhead.beg.year = true_year;
        } else if (dl_year != dhead.beg.year) {
            util_log(1, "year stamp changed from %hd to %hd",
                     dl_year, dhead.beg.year);
        }
    }

/*  Save current year stamp  */

    dl_year = dhead.beg.year;

/*  Patch Magellan leap year bug, if required  */

    if (fix_leap_times && rev == 8 && dhead.beg.year == 2004) {
        ltemp = dhead.beg.ext + 86400;
        LSWAB(&ltemp, 1);
        memcpy(buffer+14, &ltemp, 4);
        ltemp = dhead.end.ext + 86400;
        LSWAB(&ltemp, 1);
        memcpy(buffer+46, &ltemp, 4);
    }
}

int tty_init(char *name, long baud, int ttyto, struct nrts_sys *system, int ctsrts, int leapfix, char *tapfile, IDA *idaptr)
{
struct termios new_tty;
static char *fid = "tty_init";

    if (tapfile != (char *) NULL) {
        if ((tap = open(tapfile, O_WRONLY | O_CREAT, 0644)) < 0) {
            util_log(1, "%s: %s: %s", fid, tapfile, syserrmsg(errno));
            util_log(1, "ttytap option suppressed");
        } else {
            util_log(1, "TTY data tap to `%s'", tapfile);
        }
    } else {
        tap = -1;
    }

    sys = system;
    if (sys->type == NRTS_IDA) {
        info = (struct nrts_ida *) sys->info;
    } else {
        util_log(1,"%s: unsupported system type %d", fid, sys->type);
        return -1;
    }

    timeout = ttyto;
    util_log(1, "serial port timeout set to %d secs", timeout);

    speed   = baud;
    if (ctsrts) {
        flow = TTYIO_FHARD;
        util_log(1, "hardware flow control enabled");
    } else {
        flow = TTYIO_FSOFT;
        util_log(1, "software flow control enabled");
    }

    fd = ttyio_init(
        name,
        TTYIO_READ,
        baud,
        baud,
        TTYIO_PNONE,
        flow,
        &old_tty,
        &new_tty,
        1
    );
    if (fd < 0) {
        util_log(1, "%s: ttyio_init error %d: %s",
            fid, -fd, syserrmsg(errno)
        );
        return -1;
    }
    util_noblock(fd);

    ttyio_loginfo(1, name, &new_tty);

    have_tty = 1;

/*  Select sync function  */

    if (sys->type == NRTS_IDA) {
        if (ida->rev.value >= 5) {
            syncfunc = ida_rev5sync;
        } else if (ida->rev.value == 3) {
            syncfunc = ida_rev3sync;
        } else {
            util_log(1,"%s: no sync function for IDA rev=%d", ida->rev.value);
            return -1;
        }
    } else {
        util_log(1,"%s: unsupported system type %d", fid, sys->type);
        return -1;
    }

/*  Send a start in case output has been halted  */

    if (tty_reset(fd) != 0 && errno != EWOULDBLOCK) {
        util_log(1,"%s: tty_reset(fd): %s", fid, syserrmsg(errno));
        return -1;
    }

    if (leapfix) {
        fix_leap_times = 1;
        util_log(1, "MK7 Magellan leap year fix enabled");
    } else {
        fix_leap_times = 0;
    }

    ida = idaptr;

    return fd;
}

int tty_read(int ttyfd, char *obuf, int want, int timeout)
{
int retval;
int i, start;
static int first = 1;
static long sofar = 0;
static char *fid = "tty_read";

/*  Sync to start of stream   */

    retval = (*syncfunc)(ttyfd, obuf, &start, timeout);
    if (retval != NRTS_OK) return retval;

/* Once synced, read an entire record  */

    want -= start;
    if (util_read(ttyfd, obuf + start, want, timeout) != want) return -1;
    TTYTAP(obuf + start, want);

    if (first) {
        util_log(1, "initial sync established, %ld bytes dropped", dumped);
    } else if (dumped) {
        /* Note all sync losses except for extra 2 bytes common for IDA rev 3 */
        if (sys->type != NRTS_IDA || (ida->rev.value != 3 || dumped != 2)) {
            util_log(1,"lost sync, dropped %ld bytes before recovering",
                     dumped);
        }
    }
    first = 0;

/* Tee the raw data immediately, before patches */

    TeePacket(obuf, IDA_BUFSIZ);

/*  Need to patch up rev 3 data (MK4 uplink tap)  */

    if (sys->type == NRTS_IDA && ida->rev.value == 3) patch_data(obuf, info);

/*  Make sure MK6B and later data loggers have realistic years  */

    if (sys->type == NRTS_IDA && ida->rev.value >= 5) check_year(obuf, ida->rev.value);

    errno = 0;

    return NRTS_OK;
}

void tty_close()
{
    if (have_tty) {
        util_log(1, "closing tty port and restoring original settings");
#ifdef TIOCNXCL
        ioctl(fd, TIOCNXCL, NULL);
#endif
        tcsetattr(fd, TCSANOW, &old_tty);
        close(fd);
    }
    CloseTeeFile();
}

int tty_reset(int fd)
{
int restart_output = TCOON;
int restart_input  = TCION;

    if (flow != TTYIO_FSOFT) return 0;

    if (tcflow(fd, TCOON) != 0) return -1;
    return tcflow(fd, TCION);
}

/* Revision History
 *
 * $Log: ttyio.c,v $
 * Revision 1.12  2006/02/10 02:07:35  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.11  2005/05/27 00:26:05  dechavez
 * using oldttyio
 *
 * Revision 1.10  2005/01/28 02:08:55  dechavez
 * Removed Jan 1 exclusion from check_year
 *
 * Revision 1.9  2004/03/01 18:27:31  dechavez
 * updated MK7 leap year hack for 2004
 *
 * Revision 1.8  2004/02/05 22:33:49  dechavez
 * tee packets at fixed length
 *
 * Revision 1.7  2004/02/05 21:08:57  dechavez
 * added support for tee files
 *
 * Revision 1.6  2003/12/10 06:21:17  dechavez
 * fixed check_year type (int to void)
 *
 * Revision 1.5  2003/11/25 20:40:00  dechavez
 * ANSI function calls, made local use only functions static
 *
 * Revision 1.4  2000/06/24 08:22:47  dec
 * #ifdef TIOCNXCL
 *
 * Revision 1.3  2000/06/23 05:43:23  dec
 * fixed bug associated with patching year and leap days
 *
 * Revision 1.2  2000/02/28 18:16:18  dec
 * 2.10.2, leapfix hack changed for 2000.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
