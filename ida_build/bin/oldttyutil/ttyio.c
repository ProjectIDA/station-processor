#pragma ident "$Id: ttyio.c,v 1.1 2005/05/26 23:52:35 dechavez Exp $"
/*======================================================================
 *
 *  Copy from tty to stdout or from stdin to tty.
 *
 *  If invoked with the name `rtty' the program will read from
 *  the serial port and write to stdout.
 *
 *  If invoked with the name `wtty' the program will read from
 *  stdin and write to the serial port.
 *
 *  The program loops forever, until SIGINT or SIGTERM is received.
 *
 *  A brief status report is printed upon receipt of SIGUSR1.
 *
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "util.h"
#include "oldttyio.h"

#define MAXTOKEN 1024

#define DEF_BAUD   9600
#define DEF_FLOW   TTYIO_FSOFT
#define DEF_PARITY TTYIO_PNONE
#define DEF_TO     86400
#define RNAME     "rtty"
#define WNAME     "wtty"

struct termios old_termios, new_termios;
int  fd;
long nbytes    = 0;
int action     = 0;
char *port     = NULL;
int  pid;
char *myname   = NULL;

struct flagmap {
    unsigned long val; 
    char *txt;
};

void MSPause( unsigned long interval )
{
int status;
struct timespec rqtp, rmtp;

    rqtp.tv_sec = (time_t) interval / 1000;
    rqtp.tv_nsec = (long) (1000000 * (interval % 1000));
    
    do {
        status = nanosleep(&rqtp, &rmtp);
        if (status != 0) {
            if (errno != EINTR) return;
            rqtp = rmtp;
        }
    } while (status != 0);
}

void help(name)
char *name;
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s device [options]\n", name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "baud=speed   - baud rate\n");
    fprintf(stderr, "flow=hard    - hardware flow control\n");
    fprintf(stderr, "flow=soft    - xon/xoff flow control\n");
    fprintf(stderr, "flow=none    - no flow control\n");
    fprintf(stderr, "parity=odd   - odd parity\n");
    fprintf(stderr, "parity=even  - even parity\n");
    fprintf(stderr, "parity=none  - no parity\n");
    fprintf(stderr, "to=secs      - i/o timeout\n");
    fprintf(stderr, "+/-lock      - set/clear exclusive lock\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: +lock ");
    fprintf(stderr, "baud=%d ", DEF_BAUD);
    fprintf(stderr, "to=<infinite> flow=hard parity=none\n");
    exit(1);
}

static void catcher(int sig)
{
    fprintf(stderr,"%s[%d]: signal ", myname, pid);
    fprintf(stderr,"%s caught\n", util_sigtoa(sig));
    
    if (sig == SIGUSR1) {
        ttyio_prtinfo(stderr, port, &new_termios);
        fprintf(stderr, "bytes:  %ld\n", nbytes);
    } else {
        fprintf(stderr,"%s[%d]: ", myname, pid);
        if (action == TTYIO_READ) {
            fprintf(stderr,"%ld bytes read\n", nbytes);
        } else {
            fprintf(stderr,"%ld bytes written\n", nbytes);
        }
        close(fd);
        fflush(stdout);
        if (sig == SIGINT || sig == SIGTERM) exit(0);
        exit(1);
    }
}

int main(int argc, char **argv)
{
char input, *test;
caddr_t arg;
int i;
int itemp;
int to        = DEF_TO;
int flow      = DEF_FLOW;
int parity    = DEF_PARITY;
speed_t baud  = DEF_BAUD;
int lock      = 1;
int ntoken;
unsigned long delay = 0;
char *token[MAXTOKEN];

/*  Scan command line for default overrides  */

    pid = getpid();

    if ((myname = strdup(argv[0])) == NULL) {
        perror(argv[0]);
        exit(1);
    }

    ntoken = util_sparse(myname, token, "./", MAXTOKEN);
    myname = token[ntoken-1];

    if (strcasecmp(myname, RNAME) == 0) {
        action = TTYIO_READ;
    } else if (strcasecmp(myname, WNAME) == 0) {
        action = TTYIO_WRITE;
    } else {
        fprintf(stderr, "can't interpret `%s'\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++) {

        if (strncasecmp(argv[i], "flow=", strlen("flow=")) == 0) {
            arg = argv[i]+strlen("flow=");
            if (strcasecmp(arg, "soft") == 0) {
                flow = TTYIO_FSOFT;
            } else if (strcasecmp(arg, "hard") == 0) {
                flow = TTYIO_FHARD;
            } else if (strcasecmp(arg, "none") == 0) {
                flow = TTYIO_FNONE;
            } else {
                fprintf(stderr, "illegal flow argument `%s'\n", arg);
                help(myname);
            }

        }else if (strncasecmp(argv[i], "parity=", strlen("parity=")) == 0) {
            arg = argv[i]+strlen("parity=");
            if (strcasecmp(arg, "odd") == 0) {
                parity = TTYIO_PODD;
            } else if (strcasecmp(arg, "even") == 0) {
                parity = TTYIO_PEVEN;
            } else if (strcasecmp(arg, "none") == 0) {
                parity = TTYIO_PNONE;
            } else {
                fprintf(stderr, "illegal parity argument `%s'\n", arg);
                help(myname);
            }

        } else if (strncasecmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = (speed_t) atol(argv[i]+strlen("baud="));

        } else if (strncasecmp(argv[i], "delay=", strlen("delay=")) == 0) {
            delay = (unsigned long) atol(argv[i]+strlen("delay="));

        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i]+strlen("to="));

        } else if (strcasecmp(argv[i], "-lock") == 0) {
            lock = 0;
        } else if (strcasecmp(argv[i], "+lock") == 0) {
            lock = 1;

        } else {
            port = argv[i];
        }
    }

    if (action == 0 || port == NULL) help(myname);

/*  Prepare the serial port */

    fd = ttyio_init(
        port, action, baud, baud, parity,
        flow, &old_termios, &new_termios, lock
    );

    if (fd < 0) {
        fprintf(stderr,"%s[%d]: ttyio_init error %d: ", myname,pid,-fd);
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

/*  Read or write, as appropriate */

    if (action == TTYIO_READ) {

    /* Loop forever, reading from tty and writing to stdout */

        if (delay) fprintf(stderr, "read delay is %lu ms\n", delay);

        while (1) {

            if (util_read(fd, (void *) &input, (long) 1, (int) to) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("read");
                exit(1);
            }

            if (fwrite(&input, sizeof(char), (size_t) 1, stdout) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("fwrite");
                exit(1);
            }
            fflush(stdout);

            ++nbytes;
    
            if (delay) MSPause(delay);
        }
    } else {

    /* Copy input from stdin to tty */

        while (fread(&input, sizeof(char), (size_t) 1, stdin) == 1) {

            if (util_write(fd, &input, 1, to) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("write");
                exit(1);
            }

            ++nbytes;
        }

        if (ferror(stdin)) {
            fprintf(stderr,"%s[%d]: ", myname, pid);
            perror("fread");
            exit(1);
        } else {
            exit(0);
        }
    }
}

/* Revision History
 *
 * $Log: ttyio.c,v $
 * Revision 1.1  2005/05/26 23:52:35  dechavez
 * brought over from original ttyutil
 *
 */
