#pragma ident "$Id: main.c,v 1.5 2005/06/10 15:33:41 dechavez Exp $"
/*======================================================================
 *
 *  Generate an AutoDRM request based on current disk loop contents
 *
 *====================================================================*/
#include <sys/utsname.h>
#include "isi.h"
#include "util.h"

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "localhost"
#endif /* DEFAULT_SERVER */

#ifndef MAXCHN
#define MAXCHN 64
#endif /* MAXCHN */

#ifndef FUDGE_FACTOR
#define FUDGE_FACTOR 300
#endif /* FUDGE_FACTOR */

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr,"usage: %s [options] sys_name\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"server=string => ISI server name\n");
    fprintf(stderr,"port=int      => ISI port number\n");
    fprintf(stderr,"beg=timestr   => set begin time\n");
    fprintf(stderr,"end=timestr   => set end time\n");
    fprintf(stderr,"reply=e-mail  => return e-mail address\n");
    fprintf(stderr,"log=name      => set log file name\n");
    fprintf(stderr,"debug=level   => set initial debug level\n");
    fprintf(stderr,"maxlen=int    => maximum window length in seconds\n");
    exit(1);
}

static char PrintTimeString(REAL64 value)
{
int yr, da, hr, mn, sc, ms, mo, md;

    utilTsplit(value, &yr, &da, &hr, &mn, &sc, &ms);
    utilJdtomd(yr, da, &mo, &md);
    printf("%04d/%02d/%02d %02d:%02d:%02d", yr, mo, md, hr, mn, sc);
    fflush(stdout);
}

int main (int argc, char **argv)
{
BOOL verbose   = FALSE;
char *server   = DEFAULT_SERVER;
char *reply    = NULL;
char *log, *sta=NULL;
ISI_PARAM par;
int i, port, debug, nchn;
ISI_STREAM_NAME name[MAXCHN];
BOOL NeedBeg=TRUE, NeedEnd=TRUE;
ISI_SOH_REPORT *soh;
REAL64 beg = 0.0;
REAL64 end = 0.0;
REAL64 maxlen = 7200.0;
struct utsname system;

    if (uname(&system) == -1) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("uname");
        exit(1);
    }

    utilNetworkInit();
    isiInitDefaultPar(&par);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "server=", strlen("server=")) == 0) {
            server = argv[i] + strlen("server=");
        } else if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
            isiSetServerPort(&par, port);
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            beg = utilAttodt(argv[i] + strlen("beg="));
            NeedBeg = FALSE;
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            end = utilAttodt(argv[i] + strlen("end="));
            NeedEnd = FALSE;
        } else if (strncmp(argv[i], "maxlen=", strlen("maxlen=")) == 0) {
            maxlen = utilAttodt(argv[i] + strlen("maxlen="));
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
            isiSetDebugFlag(&par, debug);
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
            isiStartLogging(&par, log, NULL, argv[0]);
        } else if (strncmp(argv[i], "reply=", strlen("reply=")) == 0) {
            reply = argv[i] + strlen("reply=");
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (sta == NULL) {
            sta = argv[i];
        } else {
           fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (sta == NULL) help(argv[0]);

    if (verbose) fprintf(stderr, "%s %s\n", argv[0], VersionIdentString);

    if ((soh = isiSoh(server, &par)) == NULL) {
        perror("isiSoh");
        exit(1);
    }

/* Find all channels for the specified station */

    nchn = 0;
    for (i = 0; i < soh->nentry; i++) {
        if (strcmp(soh->entry[i].name.sta, sta) == 0) {
            if (NeedBeg && soh->entry[i].tols.value > 0.0) {
                if (beg == 0.0 || beg > soh->entry[i].tols.value) {
                    beg = soh->entry[i].tols.value;
                }
            }
            if (nchn == MAXCHN) {
                fprintf(stderr, "%s: INCREASE MAXCHN\n", argv[0]);
                exit(1);
            }
            name[nchn++] = soh->entry[i].name;
        }
    }

    if (nchn == 0) {
        fprintf(stderr, "%s: 0 channels found for station %s\n", argv[0], sta);
        exit(1);
    }

    if (NeedEnd) end = (REAL64) (time(NULL) - FUDGE_FACTOR);

    if (end - beg > maxlen) end = beg + maxlen;

/* Print out the request */

    printf("BEGIN GSE2.0\n");
    printf("MSG_TYPE REQUEST\n");
    printf("MSG_ID %s[%d] %s\n", argv[0], getpid(), system.nodename);
    printf("STA_LIST %s\n", sta);
    printf("TIME ");
    PrintTimeString(beg);
    printf(" to ");
    PrintTimeString(end);
    printf("\n");
    printf("CHAN_LIST ");
    for (i = 0; i < nchn; i++) printf(" %s%s", name[i].chn, name[i].loc);
    printf("\n");
    printf("WAVEFORM NATIVE\n");
    if (reply == NULL) {
        printf("FTP %sreply@%s\n", sta, system.nodename);
    } else {
        printf("FTP %s\n", reply);
    }
    printf("STOP\n");

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.5  2005/06/10 15:33:41  dechavez
 * Rename isiSetLogging() to isiStartLogging()
 *
 * Revision 1.4  2004/08/10 23:03:22  dechavez
 * fixed MSG_ID typo
 *
 * Revision 1.3  2004/01/31 23:02:12  dechavez
 * use FTP instead of E-MAIL for reply
 *
 * Revision 1.2  2004/01/31 21:20:13  dechavez
 * added 5 minute fudge factor
 *
 * Revision 1.1  2004/01/31 04:18:59  dechavez
 * initial release
 *
 */
