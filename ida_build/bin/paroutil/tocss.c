#pragma ident "$Id: tocss.c,v 1.2 2005/05/27 00:18:14 dechavez Exp $"
/*======================================================================
 *
 * Read from a Paroscientific Digiquartz pressure device and convert to
 * CSS 3.0 format.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1996 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <synch.h>
#include <thread.h>
#include "util.h"
#include "oldttyio.h"
#include "cssio.h"
#include "paro.h"

#define DEF_BAUD  9600
#define DEF_PORT  "/dev/baro"
#define DEF_STA   "PARO"
#define DEF_CHAN  "wdo"
#define WFDISC    "paro.wfdisc"

PARO *pp = (PARO *) NULL;
mutex_t mutex = DEFAULTMUTEX;
mutex_t verbose_mutex = DEFAULTMUTEX;
int     verbose_flag  = 1;

int swap;
int sint = 2;
struct wfdisc wfdisc;

struct {
    FILE *wfdisc;
    FILE *data;
} fp;

void set_verbose(int flag)
{
    mutex_lock(&verbose_mutex);
        verbose_flag = flag;
    mutex_unlock(&verbose_mutex);
}

int verbose()
{
int retval;

    mutex_lock(&verbose_mutex);
        retval = verbose_flag;
    mutex_unlock(&verbose_mutex);

    return retval;
}

void help(char *name)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s ", name);
    fprintf(stderr, "[device baud=speed sta=station_code ");
    fprintf(stderr, "chan=channel_code]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: ");
    fprintf(stderr, "%s baud=%d sta=%s chan=%s\n",
        DEF_PORT, DEF_BAUD, DEF_STA, DEF_CHAN
    );
    exit(1);
}

void write_wfdisc()
{
    rewind(fp.wfdisc);
    if (wwfdisc(fp.wfdisc, &wfdisc) != 0) {
        perror("can't update wfdisc file: ");
        exit(1);
    }
    fflush(fp.wfdisc);
}

void *signal_handler(void *dummy)
{
sigset_t set;
int sig;
long sofar;

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sig = sigwait(&set);
        set_verbose(0);
        printf("%\r%s                                \n", util_sigtoa(sig));

        if (sig == SIGINT) {
            mutex_lock(&mutex);
                sofar = wfdisc.nsamp;
            mutex_unlock(&mutex);
            printf("%ld samples (%.2f secs) acquired so far.\n", sofar,
                (float) sofar / wfdisc.smprate
            );
            if (util_query("Quit now? ")) {
                mutex_lock(&mutex);
                fclose(fp.data);
                write_wfdisc();
                fclose(fp.wfdisc);
                printf("CSS 3.0 wfdisc file is `%s'\n", WFDISC);
                exit(0);
            } else {
                fflush(fp.data);
                write_wfdisc();
            }
        } else if (sig == SIGHUP) {
            fflush(fp.data);
            write_wfdisc();
        } else {
             mutex_lock(&mutex);
             fclose(fp.data);
             write_wfdisc();
             fclose(fp.wfdisc);
             exit(0);
        }
        set_verbose(1);
    }
}

void signals_init()
{
int status;
thread_t tid;
sigset_t set;
static char *fid = "signals_init";

/* Block all signals */

    sigfillset(&set);
    thr_sigsetmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    status = thr_create((void *) NULL, 0, signal_handler, (void *) NULL,
        THR_NEW_LWP | THR_DAEMON | THR_DETACHED, &tid
    );

    if (status != 0) {
        fprintf(stderr, "%s: thr_create: %s\n", fid, syserrmsg(errno));
        exit(1);
    }
}

void print_param(struct paro_param *param)
{
char *unstring[] = {
    "user defined (see UF)",
    "psi",
    "mbar or hPa",
    "bar",
    "kPa",
    "MPa",
    "in Hg",
    "mm Hg or torr",
    "m H20"
};

    printf("Identification parameters\n");
    printf("S/N = %s    Firmware rev = %s\n", param->sn, param->vr);
    printf("\n");
    printf("Configuration parameters\n");
    printf("PR = %6d   TR = %6d   UN = %d ",
        param->pr, param->tr, param->un
    );
    if (param->un == 0) {
        printf("(user defined, UF = %9.7g)\n", param->uf);
    } else {
        printf("(%s)\n", unstring[param->un]);
    }
    printf("\n");
    printf("Calibration parameters\n");
    printf("PA = %9.7g   PM = %9.7g  TC = %9.7g\n",
        param->pa, param->pm, param->tc
    );
    printf("C1 = %9.7g   C2 = %9.7g  C3 = %9.7g\n",
        param->c1, param->c2, param->c3
    );
    printf("D1 = %9.7g   D2 = %9.7g\n",
        param->d1, param->d2
    );
    printf("T1 = %9.7g   T2 = %9.7g  T3 = %9.7g  T4 = %9.7g  T5 = %9.7g\n",
        param->t1, param->t2, param->t3, param->t4, param->t5
    );
    printf("U0 = %9.7g\n",
        param->u0
    );
    printf("Y1 = %9.7g   Y2 = %9.7g  Y3 = %9.7g\n",
        param->y1, param->y2, param->y3
    );
}

void write_sample(long sample, time_t tstamp)
{

    mutex_lock(&mutex);
        if (swap) util_lswap(&sample, 1);
        if (fwrite(&sample, sizeof(long), 1, fp.data) != 1) {
            perror("can't update waveform file: ");
            exit(1);
        }
        ++wfdisc.nsamp;
    mutex_unlock(&mutex);
}

void save(long sample, time_t tstamp)
{
static int first = 1;
time_t new_sample_time;
int missed;
static time_t prev_tstamp = 0;
static long   prev_sample = 0;
struct tm *tm;

    wfdisc.endtime = (double) tstamp;

    if (first) {
        wfdisc.time = (double) tstamp;
        tm = gmtime(&tstamp);
        tm->tm_year += 1900;
        ++tm->tm_yday;
        wfdisc.jdate = (tm->tm_year * 1000) + tm->tm_yday;
        write_sample(sample, tstamp);
        prev_tstamp = tstamp;
        prev_sample = sample;
        first = 0;
        return;
    }

    new_sample_time = prev_tstamp + (time_t) sint;
    if (tstamp < new_sample_time) return;

    missed = (tstamp - prev_tstamp) / sint;

    if (missed == 1) {
        write_sample((prev_sample + sample) / 2, new_sample_time);
        new_sample_time += sint;
    } else {
        printf("\nWARNING: missed %d samples!\n", missed); fflush(stdout);
    }
    tstamp = new_sample_time;

    write_sample(sample, tstamp);
    prev_tstamp = new_sample_time;
    prev_sample = sample;
}

main(int argc, char **argv)
{
int i, failed;
long sample;
time_t tstamp;
speed_t baud   = DEF_BAUD;
char *port     = DEF_PORT;
char *sta      = DEF_STA;
char *chan     = DEF_CHAN;
char *dir      = ".";
char *dfile    = "paro.w";
char *instype  = "PS6016B";
char *datatype = "s4";
char *reply;
struct paro_param param;

/* Scan command line for default overrides */

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = (speed_t) atol(argv[i]+strlen("baud="));
        } else if (strncasecmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta  = (argv[i] + strlen("sta="));
        } else if (strncasecmp(argv[i], "chan=", strlen("chan=")) == 0) {
            chan = (argv[i] + strlen("chan="));
        } else if (strncasecmp(argv[i], "-h", strlen("-h")) == 0) {
            help(argv[0]);
        } else {
            port = argv[i];
        }
    }

    swap = (util_order() != BIG_ENDIAN_ORDER);

/* Initialize wfdisc */

    wfdisc = wfdisc_null;
    sprintf(wfdisc.dir, "%s", dir);
    sprintf(wfdisc.dfile, "%s", dfile);
    sprintf(wfdisc.sta, "%s", sta);
    sprintf(wfdisc.chan, "%s", chan);
    sprintf(wfdisc.instype, "%s", instype);
    sprintf(wfdisc.datatype, "%s", datatype);
    wfdisc.calib   = 0.10;
    wfdisc.calper  = 100.;
    wfdisc.nsamp   = 0;
    wfdisc.smprate = 1.0 / (float) sint;
    wfdisc.foff    = 0;

/* Open output files */

    if ((fp.wfdisc = fopen(WFDISC, "w")) == (FILE *) NULL) {
        fprintf(stderr, "can't open wfdisc file: ");
        perror(WFDISC);
        exit(1);
    }

    if ((fp.data = fopen(dfile, "w")) == (FILE *) NULL) {
        fprintf(stderr, "can't open waveform file: ");
        perror(dfile);
        exit(1);
    }

/* Connect to the device */

    printf("Connecting to device at `%s', speed %d... ", port, baud);
    fflush(stdout);
    if ((pp = paro_open(port, baud, &param)) == (PARO *) NULL) {
        perror(port);
        exit(1);
    }
    printf("done\n");
    usleep(250000);

    printf("Memory test ");
    printf("%s\n\n", (failed = paro_test(pp)) ? "FAILED" : "completed OK");

    if (!failed) {
        usleep(250000);
        print_param(&param);
        printf("\n");
    }

/*  Set up exit handlers  */

    signals_init();

/* Turn on continuous sampling */

    paro_command(pp, PARO_P4);

/* Acquire data forever */

    printf("Begin continuous %d second/sample data acquisition.\n",
        sint
    );
    printf("Type ctrl-C when you want to flush or quit.\n");
    while (1) {
        if ((reply = paro_read(pp)) == (char *) NULL) {
            printf("\nbarometer read error: %s\n",
                syserrmsg(errno)
            );
            fflush(stdout);
            exit(1);
        } else {
            tstamp = time(NULL);
            sample = atol(reply);
            if (verbose()) {
                printf("Current pressure = %.3f mbar     \r",
                    sample / 1000.0
                );
                fflush(stdout);
            }
            save(sample, tstamp);
        }
    }
}

/* Revision History
 *
 * $Log: tocss.c,v $
 * Revision 1.2  2005/05/27 00:18:14  dechavez
 * using oldttyio
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
