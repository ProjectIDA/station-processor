#pragma ident "$Id: ttytst.c,v 1.1 2005/05/26 23:52:35 dechavez Exp $"
/*======================================================================
 *
 *  Test serial lines by writing/reading known packets.
 *
 *  The program loops forever, until SIGINT or SIGTERM is received.
 *
 *  A status report is logged upon receipt of SIGHUP.
 *
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "oldttyio.h"

#define DEF_BAUD  9600
#define DEF_LEN   1024
#define DEF_TO   86400
#define DEF_FLOW     TTYIO_FSOFT
#define FIRST_SEQNO  1
#define MODE_MAN     1
#define MODE_CONT    2
#define MODE_POLE    3
#define MODE_ASCII   4

#define TTYREAD(fd, buffer, n, to) (status = util_read(fd, buffer, n, to))

struct termios old_termios, new_termios;
int  fd;
int mode       = MODE_CONT;
int action     = 0;
char *port     = NULL;
int  pid;
int  sl        = 3;

unsigned short seqno = 0;
unsigned char *buffer;
unsigned short *sbuf;

struct {
    long nrec;
    long corrupt;
    struct {
        long bytes;
        long packets;
        long sync;
    } lost;
} stats;

struct flagmap {
    unsigned long val; 
    char *txt;
};

static void help(myname)
char *myname;
{
    fprintf(stderr, "\n");
    fprintf(stderr, "to read:  %s +r device [options]\n", myname);
    fprintf(stderr, "to write: %s +w device [options]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "baud=speed   - baud rate\n");
    fprintf(stderr, "to=secs      - serial port timeout\n");
    fprintf(stderr, "flow=hard    - hardware flow control\n");
    fprintf(stderr, "flow=soft    - xon/xoff flow control\n");
    fprintf(stderr, "flow=none    - no flow control\n");
    fprintf(stderr, "len=value    - set buffer length\n");
    fprintf(stderr, "log=name     - logfile name\n");
    fprintf(stderr, "debug=val    - initial debug level\n");
    fprintf(stderr, "mode=man     - manual mode\n");
    fprintf(stderr, "mode=cont    - continuous mode\n");
    fprintf(stderr, "mode=pole    - barber pole (requires +w)\n");
    fprintf(stderr, "mode=ascii   - ascii strings (requires +w)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: ");
    fprintf(stderr, "baud=%d ", DEF_BAUD);
    fprintf(stderr, "to=%d ",   DEF_TO);
    fprintf(stderr, "len=%d ",  DEF_LEN);
    fprintf(stderr, "flow=soft ");
    fprintf(stderr, "mode=cont ");
    fprintf(stderr, "\n");
    exit(1);
}

static void log_stats()
{
    if (action == TTYIO_WRITE) {
        util_log(1, "%ld records written", stats.nrec);
    } else {
        util_log(1, "%5ld good packets received", stats.nrec);
        util_log(1, "%5ld sync losses", stats.lost.sync);
        util_log(1, "%5ld dropped bytes", stats.lost.bytes);
        util_log(1, "%5ld corrupt packets", stats.corrupt);
        util_log(1, "%5ld lost packets", stats.lost.packets);
    }
}

static void catcher(int sig)
{
    util_log(1, "%s", util_sigtoa(sig));
    
    if (sig == SIGHUP) {
        ttyio_loginfo(1, port, &new_termios);
        log_stats();
    } else {
        log_stats();

    /*  Close files  */

        close(fd);
        if (sig == SIGINT || sig == SIGTERM) exit(0);
        exit(1);
    }
}

static long sync_to_stream(to)
int to;
{
int status;
unsigned char byte1, byte2;
long dumped = 0;

    if (TTYREAD(fd, &byte1, (long) 1, to) != 1) return -1;
    if (TTYREAD(fd, &byte2, (long) 1, to) != 1) return -1;

    dumped = 0;
    util_log(sl, "searching for sync pattern");
    while (1) {
        if (byte1 == 0 && byte2 == 0xff) {
            util_log(sl, "sync pattern found after %ld bytes", dumped);
            return dumped;
        }
        byte1 = byte2;
        if (TTYREAD(fd, &byte2, (long) 1, to) != 1) return -1;
        if (++dumped % 1000 == 0) {
            util_log(1, "attempting to sync, %ld bytes dumped so far",
                dumped
            );
        }
    }
}

static void WritePole(int fd, int to)
{
char c;

    while (1) {
        for (c = 0x20; c <= 0x7e; c++) util_write(fd, &c, 1, to);
        ++stats.nrec;
    }
}

static void WriteAscii(int fd, int to, int baud)
{
char string[1024];

    while (1) {
        sprintf(string, "tty write test, baud=%d, record=%d\r\n", baud, ++stats.nrec);
        util_write(fd, string, strlen(string), to);
        sleep(1);
    }
}

int main(int argc, char **argv)
{
unsigned long count, speed = 0;
char input, *test;
caddr_t arg;
int  i, status, level, rl;
int  debug  = 1;
int  flow   = DEF_FLOW;
int  to     = DEF_TO;
long blen   = DEF_LEN;
long slen   = DEF_LEN;
int  baud   = DEF_BAUD;
long nlost, dumped;
int  corrupt;
int  swap;
unsigned short value, key, prev_key;
unsigned char newline = '\n';
char *log     = NULL;

char *version = "version 2.0 (5/26/2004)";

    assert(sizeof(short) == 2);

/*  Scan command line for default overrides  */

    pid = getpid();

    for (i = 1; i < argc; i++) {

        if (strcmp(argv[i], "+r") == 0) {
            action = TTYIO_READ;

        } else if (strcmp(argv[i], "+w") == 0) {
            action = TTYIO_WRITE;

        } else if (strncmp(argv[i], "mode=", strlen("mode=")) == 0) {
            arg = argv[i] + strlen("mode=");
            if (strcmp(arg, "cont") == 0) {
                mode = MODE_CONT;
            } else if (strcmp(arg, "man") == 0) {
                mode = MODE_MAN;
            } else if (strcmp(arg, "pole") == 0) {
                mode = MODE_POLE;
            } else if (strcmp(arg, "ascii") == 0) {
                mode = MODE_ASCII;
            } else {
                fprintf(stderr, "illegal mode argument `%s'\n", arg);
                help(argv[0]);
            }

        } else if (strncmp(argv[i], "flow=", strlen("flow=")) == 0) {
            arg = argv[i]+strlen("flow=");
            if (strcmp(arg, "soft") == 0) {
                flow = TTYIO_FSOFT;
            } else if (strcmp(arg, "hard") == 0) {
                flow = TTYIO_FHARD;
            } else if (strcmp(arg, "none") == 0) {
                flow = TTYIO_FNONE;
            } else {
                fprintf(stderr, "illegal flow argument `%s'\n", arg);
                help(argv[0]);
            }

        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");

        } else if (strncmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = (speed_t) atol(argv[i]+strlen("baud="));

        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));

        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i]+strlen("to="));

        } else if (strncmp(argv[i], "len=", strlen("len=")) == 0) {
            blen = atol(argv[i]+strlen("len="));

        } else {
            port = argv[i];
        }
    }

    if (action == 0 || port == NULL) help(argv[0]);

    if ((mode == MODE_POLE || mode == MODE_ASCII) && action != TTYIO_WRITE) {
        fprintf(stderr, "mode=pole and mode=ascii require +w\n");
        exit(1);
    }

/* Prepare buffer space  */

    slen = blen / 2;
    blen = slen * 2; /* make sure it is a multiple of 2 */

    if (blen <= 16) {
        fprintf(stderr,"ttytst[%d]:", pid);
        fprintf(stderr," illegal len=%ld\n", blen);
        exit(1);
    }

    if ((buffer = (unsigned char *) malloc(blen+2)) == NULL) {
        fprintf(stderr,"ttytst[%d]: ", pid);
        perror("malloc");
        exit(1);
    }

/*  Open port  */

    fd = ttyio_init(port,action,baud,baud,TTYIO_PNONE,flow,&old_termios,&new_termios,1);
    if (fd < 0) {
        fprintf(stderr,"ttytst[%d]: ttyio_init error %d: ", pid, -fd);
        perror(port);
        exit(1);
    }

/*  Set up exit handlers  */

    signal(SIGHUP,  catcher);
    signal(SIGINT,  catcher);
    signal(SIGTERM, catcher);
    signal(SIGQUIT, catcher);
    signal(SIGUSR1, catcher);
    signal(SIGUSR2, catcher);

/* Start logging */

    util_logopen(log, 1, 9, debug, NULL, argv[0]);
    util_log(1, "current port parameters");
    ttyio_loginfo(1, port, &new_termios);

/* Determine byte order */

    swap = (util_order() == LTL_ENDIAN_ORDER) ? 1 : 0;

/*  Read or write, as appropriate */

    stats.nrec         = 0;
    stats.corrupt      = 0;
    stats.lost.sync    = 0;
    stats.lost.bytes   = 0;
    stats.lost.packets = 0;
    prev_key           = 0;

    if (action == TTYIO_READ) {

        if (flow == TTYIO_FSOFT) tcflow(fd, TCION);

        sbuf = (unsigned short *) buffer;

        util_log(1, "begining read loop");

    /* Loop forever, reading from tty and writing to stdout */

        while (1) {

            if ((dumped = sync_to_stream(to)) < 0) {
                util_log(1, "sync_to_stream: %s", syserrmsg(errno));
                exit(1);
            }
    
            if (stats.nrec == 0) {
                util_log(1, "intial sync established, %ld bytes dropped",
                    dumped
                );
            } else if (dumped != 0) {
                util_log(1, "lost sync, %ld bytes dropped before recovering",
                    dumped
                );
                stats.lost.bytes += dumped;
                ++stats.lost.sync;
            }
            
            if (util_read(fd, (void *) buffer, blen, to) != blen) {
                util_log(1, "util_read: %s", syserrmsg(errno));
                exit(1);
            }

            if (swap) util_sswap((short *) sbuf, slen);
            key = sbuf[0];

            corrupt = 0;
            for (corrupt = 0, i = 1; i < slen; i++) {
                if (sbuf[i] != key) {
                    util_log(1, "corrupt: at index %d expected 0x%04x, got 0x%04x",
                        i, key, sbuf[i]
                    );
                    i = slen + 1;
                    corrupt = 1;
                    sl = 1;
                    ++stats.corrupt;
                }
            }

            if (corrupt == 0) {
                rl = (sl < 3) ? 1 : 2;
                util_log(rl, "received record 0x%04x intact", key);
                if (stats.nrec > 0 && prev_key != 0xfffe) {
                    nlost = (long) key - (long) (prev_key + 1);
                    if (nlost != 0) {
                        util_log(1, "%d records lost (0x%04x thru 0x%04x)",
                            nlost, (long) prev_key + 1, (long) key - 1
                        );
                        stats.lost.packets += nlost;
                    }
                }
                sl = 3;
                prev_key = key;
                ++stats.nrec;
            }
        }

    } else {

        if (mode == MODE_POLE) WritePole(fd, to);
        if (mode == MODE_ASCII) WriteAscii(fd, to, baud);

        sbuf = (unsigned short *)
              ((unsigned short *) buffer + sizeof(unsigned short));
        blen += 2;

    /* Loop forever, creating and writing packets */

        seqno = FIRST_SEQNO;
        level = (mode == MODE_MAN) ? 1 : 2;

        memset(buffer + 0, 0x00, 1);
        memset(buffer + 1, 0xff, 1);
        util_log(1, "begining write loop");

        while (1) {

            value = seqno;
            if (swap) util_sswap((short *) &value, 1);

            for (i = 0; i < slen; i++) sbuf[i] = value;

            if (mode == MODE_MAN) {
                if (!utilQuery("Send next packet [y/n]? ")) {
                    kill(getpid(), SIGTERM);
                }
            }

            if (util_write(fd, (void *) buffer, blen, to) != blen) {
                util_log(1, "util_write: %s", syserrmsg(errno));
                exit(1);
            }

            util_log(level, "sent record 0x%04x", seqno);

            ++stats.nrec;
            if (++seqno == (unsigned short) 0xffff) seqno = FIRST_SEQNO;
        }
    }
}

/* Revision History
 *
 * $Log: ttytst.c,v $
 * Revision 1.1  2005/05/26 23:52:35  dechavez
 * brought over from original ttyutil
 *
 */
