#pragma ident "$Id: clear.c,v 1.1 2004/04/26 21:12:27 dechavez Exp $"
/*======================================================================
 *
 *  Clear stagnant channels from an NRTS disk loop
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "nrts.h"
#include "util.h"

static int count = 0;

static void help(char *myname)
{
    fprintf(stderr,"usage: %s [options] sys=name date=minimum\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"sys=name     => system name\n");
    fprintf(stderr,"date=minimum => date of oldest last datum permitted\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"-N           => list channels that would be cleared, but don't\n");
    fprintf(stderr,"home=name    => set NRTS_HOME\n");
    fprintf(stderr,"\n");
    fprintf(stderr, "date string is of the form yyyy:ddd-hh:mm:ss.msc, truncation ");
    fprintf(stderr, "from the right is OK.\n");
    exit(1);
}

static BOOL CheckChn(NRTS_STA *sta, NRTS_CHN *chn, double threshold, BOOL Verbose)
{
BOOL result;

    result = (chn->beg < threshold) ? TRUE : FALSE;

    if (result == TRUE && Verbose) {
        printf("%s:%s ", sta->name, chn->name);
        printf("%s ", util_dttostr(chn->beg, 0));
        printf("%s ", util_dttostr(chn->end, 0));
        printf("%5d ", chn->nseg);
        printf("%9s ", nrts_status(chn->status));
        printf("%14s", nrts_latency(time(NULL) - (chn->tread)));
        printf("\n");
    }

    return result;
}

static void ClearChn(NRTS_STA *sta, NRTS_CHN *chn)
{
    if (++count == 1) printf("clearing");

    printf(" %s:%s", sta->name, chn->name);

    chn->status = NRTS_IDLE;
    chn->nread  = 0;
    chn->count  = 0;
    chn->nseg   = 0;
    chn->tread  = NRTS_UNDEFINED_TIMESTAMP;
    chn->beg    = (double) NRTS_UNDEFINED_TIMESTAMP;
    chn->end    = (double) NRTS_UNDEFINED_TIMESTAMP;
    chn->sint   = (float) 0;

}

int main(int argc, char **argv)
{
int i, j;
char *DateString = NULL;
char *home = NULL;
char *sysname  = NULL;
NRTS_SYS *sys;
NRTS_STA *sta;
NRTS_CHN *chn;
struct nrts_files *file;
static struct nrts_mmap map;
BOOL expired, TestOnly = FALSE;
double threshold;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "sys=", strlen("sys=")) == 0) {
            sysname = argv[i] + strlen("sys=");
        } else if (strncmp(argv[i], "date=", strlen("date=")) == 0) {
            DateString = argv[i] + strlen("date=");
        } else if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strcmp(argv[i], "-N") == 0) {
            TestOnly = TRUE;
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

/*  Must specify system name and date */

    if (sysname == NULL || DateString == NULL) help(argv[0]);
    threshold = utilAttodt(DateString);

/*  Get file names  */

    file = nrts_files(&home, sysname);

/* Do it */

    if (nrts_mmap(file->sys, "r+", NRTS_SYSTEM, &map) != 0) {
        fprintf(stderr, "%s: nrts_mmap (%s): %s\n", argv[0], file->sys, strerror(errno));
        exit(1);
    }

    sys = (NRTS_SYS *) map.ptr;

    for (i = 0; i < sys->nsta; i++) {
        sta = &sys->sta[i];
        for (j = 0; j < sta->nchn; j++) {
            chn = &sta->chn[j];
            expired = CheckChn(sta, chn, threshold, TestOnly);
            if (expired && !TestOnly) ClearChn(sta, chn);
        }
    }

    if (count > 0) printf("\n");

    exit(0);
}

/* Revision History
 *
 * $Log: clear.c,v $
 * Revision 1.1  2004/04/26 21:12:27  dechavez
 * initial release
 *
 */
