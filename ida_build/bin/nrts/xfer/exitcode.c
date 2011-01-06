#pragma ident "$Id: exitcode.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Exit handlers.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "nrts_xfer.h"
#include "util.h"

static struct {
    long nrec;   /* total number of records transferred */
    long nbytes; /* total number of bytes transferred   */
    double min;  /* minimum transfer rate, bytes/second */
    double ave;  /* average transfer rate, bytes/second */
    double max;  /* maximum transfer rate, bytes/second */
} rate;

int catch_signal(int sig)
{
char pwd[MAXPATHLEN+1];

    switch (sig) {

      case SIGHUP:
      case SIGUSR1:
      case SIGUSR2:
        util_log(1, "%s", util_sigtoa(sig));
        util_log(1, "signal ignored");
        return;

      case SIGTERM:
      case SIGINT:
      case SIGPIPE:
        util_log(1, "going down on %s", util_sigtoa(sig));
        die(0);

      default:
        util_log(1, "%s", util_sigtoa(sig));
        util_log(1, "abort - core dumped (pwd = %s)", getwd(pwd));
        abort();

    }

}

int exitcode(void)
{

    if (signal(SIGTERM, (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGINT,  (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGBUS,  (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGSEGV, (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGPIPE, (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGFPE,  (Sigfunc *) catch_signal) == SIG_ERR) return -1;
    if (signal(SIGILL,  (Sigfunc *) catch_signal) == SIG_ERR) return -1;

    rate.nrec   = -1;
    rate.nbytes = -1;

    return 0;
}

void init_rate()
{
    if (rate.nrec == -1) {
        rate.nrec   = 0;
        rate.nbytes = 0;
        rate.ave    = 0.0;
    }
}

void do_rate(struct timeval *begtm, struct timeval *endtm, int len, char *client)
{
char *type;
double beg, end, elapsed, xferrate;
double nbytes, min, ave, max;
static char *fid = "do_rate";

    if (begtm != NULL && endtm != NULL && len > 0) {
        if (
            begtm->tv_sec  == endtm->tv_sec  &&
            begtm->tv_usec == endtm->tv_usec
        ) {
            elapsed = 0.0001;
        } else {
            beg =  (double) begtm->tv_sec + 
                  ((double) begtm->tv_usec / (double) 1000000);
            end =  (double) endtm->tv_sec +
                  ((double) endtm->tv_usec / (double) 1000000);
            elapsed = end - beg;
        }
        rate.nbytes += len;
        xferrate = (double) (len) / elapsed;
        if (++rate.nrec == 1) {
            rate.min = rate.max = rate.ave = xferrate;
        } else {
            if (xferrate < rate.min) rate.min = xferrate;
            if (xferrate > rate.max) rate.max = xferrate;
            rate.ave += xferrate;
        }

    } else if (rate.nrec > 0) {

        if (rate.nbytes < BYTES_PER_KBYTE) {
            nbytes = (double) rate.nbytes;
            type  = "bytes";
        } else if (rate.nbytes < BYTES_PER_MBYTE) {
            nbytes = (double) rate.nbytes / (double) BYTES_PER_KBYTE;
            type  = "Kbytes";
        } else if (rate.nbytes < BYTES_PER_GBYTE) {
            nbytes = (double) rate.nbytes / (double) BYTES_PER_MBYTE;
            type  = "Mbytes";
        } else {
            nbytes = (double) rate.nbytes / (double) BYTES_PER_GBYTE;
            type  = "Gbytes";
        }

        rate.ave /= (double) rate.nrec;

        min = rate.min / (double) BYTES_PER_KBYTE;
        ave = rate.ave / (double) BYTES_PER_KBYTE;
        max = rate.max / (double) BYTES_PER_KBYTE;

        util_log(2, "%ld records (%.2lf %s) sent to %s",
            rate.nrec, nbytes, type, client
        );
        util_log(3, "rate = %.2lf/%.2lf/%.2lf Kbytes/s", min, ave, max);

        rate.nrec = rate.nbytes = 0;

    } else if (rate.nrec == 0) {

        util_log(1, "0 records transferred to %s", client);
    }
}

void die(int status)
{
    util_log(2, "exit %d", status);
    exit(status);
}

/* Revision History
 *
 * $Log: exitcode.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
