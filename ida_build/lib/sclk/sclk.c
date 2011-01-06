#pragma ident "$Id: sclk.c,v 1.3 2005/05/27 00:29:43 dechavez Exp $"
/*======================================================================
 *
 *  Routines to interact with serial clocks.  While the API is intended
 *  to permit use with any clock that sends a time string out the serial
 *  port, for the moment (and forever?) things are coded explicitly for
 *  the IDA Magellan GPS clocks.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Author: David Chavez (dec@essw.com)
 *         Engineering Services & Software
 *         San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "util.h"
#include "oldttyio.h"
#include "sclk.h"

/* Connect to the device */

SCLK *sclk_open(char *port, int speed, struct sclk_param *param)
{
int fd;
SCLK *cp, *null = (SCLK *) NULL;;

/* Create and initialize the object */

    if ((cp = (SCLK *) malloc(sizeof(SCLK))) == (SCLK *) NULL) return null;

    memset(cp->buf, 0, SCLK_BUFLEN);

    cp->n       = 0;
    cp->to      = SCLK_TO;
    cp->dropped = 0;

/* Connect to the device */

    cp->fd = ttyio_init(
        port,
        TTYIO_READ,
        speed,
        speed,
        TTYIO_PNONE,
        TTYIO_FNONE,
        (struct termios *) NULL,
        (struct termios *) NULL,
        1
    );
    if (cp->fd < 0) {
        free(cp);
        return null;
    }
    util_noblock(cp->fd);

    return cp;
}

/* Close the device */

void sclk_close(SCLK *cp)
{
    if (cp != (SCLK *) NULL) close(cp->fd);
    free(cp);
}

/* Fake the current time */

static int fake_gettime(SCLK *cp, struct sclk_tstamp *tstamp)
{
int waiting;
static struct sclk_tstamp prev = {0.0, -1};
time_t next, now;
 
    if (prev.qual == -1) {
        tstamp->time = time(NULL);
        tstamp->qual = 'M';
    } else {
        next = prev.time + 1;
        waiting = 1;
        while (waiting) {
            if ((now = time(NULL)) >= next) {
                tstamp->time = now;
                tstamp->qual = 'M';
                waiting = 0;
            }
        }
    }
 
    prev = *tstamp;
    return 0;
}

/* Read the current time */

int sclk_gettime(SCLK *cp, struct sclk_tstamp *tstamp)
{
int status, done = 0;
char new;
int yr, da, mo, hr, mn, sc;
static char beg = '$';
static char end = 0x0a;
static int complete = 55;
 
    if (cp == (SCLK *) NULL) return fake_gettime(cp, tstamp);
    tstamp->qual = 0x0f;

/* Read one time string */

    while (!done) {
        if (util_read(cp->fd, &new, 1, cp->to) != 1) return -10;

        if (cp->n == 0 && new != beg) {
            cp->dropped++;
        } else if (cp->n != 0 && new == beg) {
            cp->dropped += cp->n;
            cp->n = 1;
        } else if (cp->n == complete) {
            if (new == end) {
                done  = 1;
                cp->n = 0;
            } else {
                cp->dropped += cp->n;
                cp->n = 1;
            }
        } else {
            cp->buf[cp->n++] = new;
        }
    }

/* Decode the time string */

    hr = (util_ctoi(cp->buf[10]) * 10) + util_ctoi(cp->buf[11]);
    if (hr < 0 || hr > 23) {
        errno = EINVAL;
        return -20;
    }

    mn = (util_ctoi(cp->buf[12]) * 10) + util_ctoi(cp->buf[13]);
    if (mn < 0 || mn > 59) {
        errno = EINVAL;
        return -30;
    }

    sc = (util_ctoi(cp->buf[14]) * 10) + util_ctoi(cp->buf[15]);
    if (sc < 0 || sc > 59) {
        errno = EINVAL;
        return -40;
    }

    da = (util_ctoi(cp->buf[17]) * 10) + util_ctoi(cp->buf[18]);
    if (da < 1 || da > 31) {
        errno = EINVAL;
        return -50;
    }

    mo = (util_ctoi(cp->buf[20]) * 10) + util_ctoi(cp->buf[21]);
    if (mo < 1 || mo > 12) {
        errno = EINVAL;
        return -60;
    }

    yr = (util_ctoi(cp->buf[23]) * 1000) +
         (util_ctoi(cp->buf[24]) *  100) +
         (util_ctoi(cp->buf[25]) *   10) +
         (util_ctoi(cp->buf[26]));
    if (yr < 1997 || yr > 2037) {
        errno = EINVAL;
        return -70;
    }

/* Get the day of year from the year, month, and day of month */

    da = util_ymdtojd(yr, mo, da);

/* Convert to epoch time */

    tstamp->time = (long) util_ydhmsmtod(yr, da, hr, mn, sc, 0);
    tstamp->qual = cp->buf[28] == '1' ? 'M' : '?';

    return 0;
}

#ifdef DEBUG_TEST

main(int argc, char **argv)
{
static char *port = "/dev/gps";
int speed = 9600;
struct sclk_param param;
struct sclk_tstamp tstamp;
SCLK *cp;

    if (argc == 3) {
        port  = argv[1];
        speed = atoi(argv[2]);
    } else if (argc != 1) {
        fprintf(stderr, "usage: %s [port speed]\n", argv[0]);
        fprintf(stderr, "default: %s %d\n", port, speed);
        exit(1);
    }

    if ((cp = sclk_open(port, speed, &param)) == (SCLK *) NULL) {
        perror(port);
        exit(1);
    }

    while (1) {
        if (sclk_gettime(cp, &tstamp) == 0) {
            printf("%s `%c' %d\n",
                util_lttostr(tstamp.time, 0), tstamp.qual, cp->dropped
            );
            cp->dropped = 0;
        } else {
            perror("sclk_gettime");
            exit(1);
        }
    }
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: sclk.c,v $
 * Revision 1.3  2005/05/27 00:29:43  dechavez
 * using oldttyio
 *
 * Revision 1.2  2001/05/07 22:41:24  dec
 * change errno to EINVAL from EBADMSG, for improved portability
 *
 * Revision 1.1.1.1  2000/02/08 20:20:38  dec
 * import existing IDA/NRTS sources
 *
 */
