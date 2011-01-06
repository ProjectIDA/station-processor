#pragma ident "$Id: main.c,v 1.4 2005/10/06 22:11:58 dechavez Exp $"
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
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include "util.h"
#include "ttyio.h"

extern char *VersionIdentString;

#define MAXTOKEN 1024

#define DEF_LOCK   TRUE
#define DEF_BAUD   38400
#define DEF_FLOW   TTYIO_FLOW_HARD
#define DEF_PARITY TTYIO_PARITY_NONE
#define DEF_TO     86400000
#define DEF_PIPE   0
#define DEF_SBITS  2
#define RNAME     "rtty"
#define WNAME     "wtty"

#define ACTION_UNDEFINED  0
#define ACTION_READ  1
#define ACTION_WRITE 2

void help(char *name)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "%s %s\n", name, VersionIdentString);
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
    fprintf(stderr, "sbits=1|2    - number of stop bits\n");
    fprintf(stderr, "to=msecs     - i/o timeout\n");
    fprintf(stderr, "pipe=nbytes  - size of pipe for buffered input\n");
    fprintf(stderr, "+/-lock      - set/clear exclusive lock\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: +lock ");
    fprintf(stderr, "baud=%d ", DEF_BAUD);
    fprintf(stderr, "to=<infinite> flow=none parity=none sbits=2\n");
    exit(1);
}

main(int argc, char **argv)
{
int i;
TTYIO *tp;
char *myname = NULL, *arg;
BOOL lock  = DEF_LOCK;
INT32 to   = DEF_TO;
INT32 pipe = DEF_PIPE;
int flow   = DEF_FLOW;
int parity = DEF_PARITY;
INT32 baud = DEF_BAUD;
int sbits  = DEF_SBITS;
char *port = NULL;
int action = ACTION_UNDEFINED;
int  pid;
int ntoken;
UINT8 input;
char *token[MAXTOKEN];

/*  Scan command line for default overrides  */

    pid = getpid();

    if ((myname = strdup(argv[0])) == NULL) {
        perror(argv[0]);
        exit(1);
    }

    ntoken = utilParse(myname, token, "./", MAXTOKEN, (char) NULL);
    myname = token[ntoken-1];

    if (strcasecmp(myname, RNAME) == 0) {
        action = ACTION_READ;
    } else if (strcasecmp(myname, WNAME) == 0) {
        action = ACTION_WRITE;
    } else {
        fprintf(stderr, "can't interpret `%s'\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++) {

        if (strncasecmp(argv[i], "flow=", strlen("flow=")) == 0) {
            arg = argv[i]+strlen("flow=");
            if (strcasecmp(arg, "soft") == 0) {
                flow = TTYIO_FLOW_SOFT;
            } else if (strcasecmp(arg, "hard") == 0) {
                flow = TTYIO_FLOW_HARD;
            } else if (strcasecmp(arg, "none") == 0) {
                flow = TTYIO_FLOW_NONE;
            } else {
                fprintf(stderr, "illegal flow argument `%s'\n", arg);
                help(myname);
            }

        } else if (strncasecmp(argv[i], "parity=", strlen("parity=")) == 0) {
            arg = argv[i]+strlen("parity=");
            if (strcasecmp(arg, "odd") == 0) {
                parity = TTYIO_PARITY_ODD;
            } else if (strcasecmp(arg, "even") == 0) {
                parity = TTYIO_PARITY_EVEN;
            } else if (strcasecmp(arg, "none") == 0) {
                parity = TTYIO_PARITY_NONE;
            } else {
                fprintf(stderr, "illegal parity argument `%s'\n", arg);
                help(myname);
            }

        } else if (strncasecmp(argv[i], "sbits=", strlen("sbits=")) == 0) {
            sbits = atoi(argv[i]+strlen("sbits="));
            if (sbits != 1 && sbits != 2) {
                fprintf(stderr, "illegal '%s'\n", argv[i]);
                help(myname);
            }

        } else if (strncasecmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = atoi(argv[i]+strlen("baud="));

        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i]+strlen("to="));

        } else if (strncasecmp(argv[i], "pipe=", strlen("pipe=")) == 0) {
            pipe = atoi(argv[i]+strlen("pipe="));

        } else if (strcasecmp(argv[i], "-lock") == 0) {
            lock = FALSE;

        } else if (strcasecmp(argv[i], "+lock") == 0) {
            lock = TRUE;

        } else if (strcasecmp(argv[i], "-h") == 0) {
            lock = TRUE;

        } else if (strcasecmp(argv[i], "-help") == 0) {
            lock = TRUE;

        } else {
            port = argv[i];
        }
    }

    if (action == 0 || port == NULL) help(myname);

/*  Prepare the serial port */

    tp = ttyioOpen(
        port,
        lock,
        baud,
        baud,
        parity,
        flow,
        sbits,
        to,
        pipe,
        NULL
    );
    if (tp == NULL) {
        fprintf(stderr,"%s[%d]: ttyioOpen: ", myname, pid);
        perror(port);
        exit(1);
    }

/*  Read or write, as appropriate */

    if (action == ACTION_READ) {

    /* Loop forever, reading from tty and writing to stdout */

        while (1) {

            if (ttyioRead(tp, &input, 1) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("ttyioRead");
                exit(1);
            }

            if (fwrite(&input, sizeof(char), (size_t) 1, stdout) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("fwrite");
                exit(1);
            }
            fflush(stdout);
        }
    } else {

    /* Copy input from stdin to tty */

        while (fread(&input, sizeof(char), (size_t) 1, stdin) == 1) {

            if (ttyioWrite(tp, &input, 1) != 1) {
                fprintf(stderr,"%s[%d]: ", myname, pid);
                perror("ttyioWrite");
                exit(1);
            }
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
 * $Log: main.c,v $
 * Revision 1.4  2005/10/06 22:11:58  dechavez
 * added LOGIO (NULL) now required by ttyioOpen()
 *
 * Revision 1.3  2005/06/10 15:40:43  dechavez
 * removed obsolete ibuf and obuf parameters from ttyioOpen
 *
 * Revision 1.2  2005/06/01 18:51:33  dechavez
 * 1.2.0
 *
 * Revision 1.1  2005/05/26 23:49:38  dechavez
 * switch to new ttyio library
 *
 */
