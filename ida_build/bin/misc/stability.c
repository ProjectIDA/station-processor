#pragma ident "$Id: stability.c,v 1.4 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Determine line stability from mcp and (culled) ppp logs..
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

#define OFFSET (10800)
#define DELIMITERS "/-: \n"
#define MAXTOKEN 25

#define UNKNOWN 0
#define UP      1
#define DN    2
#define PPP     1
#define MCP     2

struct timestruct {
    int yr;
    int mo;
    int da;
    int jd;
    int hr;
    int mn;
    int sc;
    int ms;
};

static int interpret(token, ntoken, status, timetag, first, auxtag)
char **token;
int ntoken;
int *status;
long *timetag;
int  first;
long *auxtag;
{
struct timestruct crnt, pppd;
double dtimetag, dcrnt;

    if (ntoken < 7) return *status = UNKNOWN;

    if (strcmp(token[5], "Disconnected") == 0) {
        *status = DN;
    } else if (strcmp(token[6], "connected") == 0) {
        *status = UP;
    } else {
        return *status = UNKNOWN;
    }

    dcrnt = (double) time(NULL) + (double) OFFSET;
    util_tsplit(
        dcrnt, &crnt.yr, &crnt.jd, &crnt.hr, &crnt.mn, &crnt.sc, &crnt.ms
    );
    util_jdtomd(crnt.yr, crnt.jd, &crnt.mo, &crnt.da);

    pppd.mo = atoi(token[0]);
    pppd.da = atoi(token[1]);
    pppd.hr = atoi(token[2]);
    pppd.mn = atoi(token[3]);
    pppd.sc = atoi(token[4]);
    pppd.ms = 0;

    pppd.yr = ((crnt.mo >= pppd.mo) ? crnt.yr : crnt.yr - 1);
    pppd.jd = util_ymdtojd(pppd.yr, pppd.mo, pppd.da);

    dtimetag = util_ydhmsmtod(
        pppd.yr,pppd.jd,pppd.hr,pppd.mn,pppd.sc,pppd.ms
    );
    *timetag = (long) dtimetag;

    if (first) {
        /*pppd.hr = pppd.mn = pppd.sc = 0;*/
        dtimetag = util_ydhmsmtod(
            pppd.yr,pppd.jd,pppd.hr,pppd.mn,pppd.sc,pppd.ms
        );
        *auxtag = (long) dtimetag;
    } else {
        pppd.hr = 23;
        pppd.mn = 59;
        pppd.sc = 59;
        dtimetag = util_ydhmsmtod(
            pppd.yr,pppd.jd,pppd.hr,pppd.mn,pppd.sc,pppd.ms
        );
        *auxtag = (long) dtimetag;
    }

    return *status;
}

int main(int argc, char **argv)
{
#define BUFFERLEN 1023
char buffer[BUFFERLEN+1];
char *token[MAXTOKEN];
int i, ntoken, status;
int prevstat = UNKNOWN;
long timetag, end, duration, lag, auxtag;
float stability, frac;
long prevtime = 0;
long start    = 0;
long uptime   = 0;
long dntime   = 0;
int verbose   = 0;
int dodate    = 0;
int donl      = 1;
int fixw      = 0;
int first     = 1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            dodate = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            fixw = 1;
        } else if (strcmp(argv[i], "-n") == 0) {
            donl = 0;
        }
    }

	while ((utilGetLine(stdin, buffer, BUFFERLEN, '#', NULL)) == 0) {
    	ntoken = utilParse(buffer, token, DELIMITERS, MAXTOKEN, 0);
        if (interpret(token, ntoken, &status, &timetag, first, &auxtag)) {
            if (first) {
                start = end = prevtime = auxtag;
                prevstat = (status == DN) ? UP : DN;
                first = 0;
            }
            if (prevtime) {
                end = timetag;
                lag = timetag - prevtime;
                if (status == UP) {
                    if (prevstat == DN) dntime += lag;
                    if (prevstat == UP) uptime += lag;
                } else if (status == DN) {
                    if (prevstat == UP) uptime += lag;
                    if (prevstat == DN) dntime += lag;
                }
            } else {
                start = end = timetag;
            }
            prevtime = timetag;
            prevstat = status;
        }
    }

    if (start == 0 || start == end) {
        if (fixw) {
            printf("   n/a");
        } else {
            printf("n/a");
        }
        if (donl) printf("\n");
        exit(0);
    }

    end = auxtag;
    lag = auxtag - prevtime;
    if (status == UP) {
        if (prevstat == DN) dntime += lag;
        if (prevstat == UP) uptime += lag;
    } else if (status == DN) {
        if (prevstat == UP) uptime += lag;
        if (prevstat == DN) dntime += lag;
    }

    duration = end - start;
    stability = ((float) uptime / (float) (duration)) * 100.0;

    frac = stability - (float) ((int) stability);
    stability = (float) ((int) stability);
    if (frac >= 0.5) stability = stability + 1.0;

    if (verbose) {
        printf("Start:    %s\n", util_lttostr(start, 0));
        printf("Stop:     %s\n", util_lttostr(end, 0));
        printf("Duration: %s\n", util_lttostr(duration, 2));
        printf("Up:       %s\n", util_lttostr(uptime, 2));
        printf("Down:     %s\n", util_lttostr(dntime, 2));
        printf("Stability: %.0f%%\n", stability);
    } else {
        if (dodate) printf("%s ", util_lttostr(start, 5));
        if (fixw) {
            printf("%5.0f%%", stability);
        } else {
            printf("%.0f%%", stability);
        }
        if (donl) printf("\n");
    }

    exit(0);
}

/* Revision History
 *
 * $Log: stability.c,v $
 * Revision 1.4  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2003/10/16 18:45:36  dechavez
 * re-added
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
