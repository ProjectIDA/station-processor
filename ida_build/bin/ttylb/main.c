#pragma ident "$Id: main.c,v 1.3 2005/05/26 23:48:43 dechavez Exp $"
/*======================================================================
 *
 *  Simple minded tty loop back test code.
 *
 *====================================================================*/
#include "ttylb.h"

#define DEF_BAUD 9600
#define DEF_SBITS 2
#define DEF_FLOW TTYIO_FLOW_HARD

static void help(myname)
char *myname;
{
    fprintf(stderr, "\n");
    fprintf(stderr, "usage:  %s device [options]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "baud=speed   - baud rate\n");
    fprintf(stderr, "flow=hard    - hardware flow control\n");
    fprintf(stderr, "flow=soft    - xon/xoff flow control\n");
    fprintf(stderr, "flow=none    - no flow control\n");
    fprintf(stderr, "sbits=1|2    - number of stop bits\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "defaults: ");
    fprintf(stderr, "baud=%d ", DEF_BAUD);
    fprintf(stderr, "sbits=%d ", DEF_SBITS);
    fprintf(stderr, "flow=hard ");
    fprintf(stderr, "\n");
    exit(1);
}

main(int argc, char **argv)
{
long baud = DEF_BAUD;
int i, sig, flow = DEF_FLOW, sbits = DEF_SBITS;
char *port = (char *) NULL, *arg;
sigset_t set;

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "baud=", strlen("baud=")) == 0) {
            baud = atol(argv[i]+strlen("baud="));

        } else if (strncmp(argv[i], "speed=", strlen("speed=")) == 0) {
            baud = atol(argv[i]+strlen("speed="));

        } else if (strncmp(argv[i], "sbits=", strlen("sbits=")) == 0) {
            sbits = atoi(argv[i]+strlen("sbits="));

        } else if (strncmp(argv[i], "flow=", strlen("flow=")) == 0) {
            arg = argv[i]+strlen("flow=");
            if (strcmp(arg, "soft") == 0) {
                flow = TTYIO_FLOW_SOFT;
            } else if (strcmp(arg, "hard") == 0) {
                flow = TTYIO_FLOW_HARD;
            } else if (strcmp(arg, "none") == 0) {
                flow = TTYIO_FLOW_NONE;
            } else {
                fprintf(stderr, "illegal flow argument `%s'\n", arg);
                help(argv[0]);
            }
        } else {
            port = argv[i];
        }
    }

    if (port == (char *) NULL || baud < 1) help(argv[0]);

/* Start reading */

    StartReader(port, baud, flow, sbits);

/* Start writing */

    StartWriter(port, baud, flow, sbits);

/* Wait until we are done */

    pause();
}

/* Revision History 
 *
 * $Log: main.c,v $
 * Revision 1.3  2005/05/26 23:48:43  dechavez
 * switch to new ttyio calls
 *
 * Revision 1.2  2003/12/10 06:31:26  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1  2000/05/15 22:35:10  dec
 * import existing sources
 *
 */
