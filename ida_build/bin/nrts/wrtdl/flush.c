#pragma ident "$Id: flush.c,v 1.4 2005/05/25 23:54:11 dechavez Exp $"
/*======================================================================
 *
 *  Thread to periodically flush wfdiscs
 *
 *====================================================================*/
#include "wrtdl.h"

static THREAD_FUNC FlushThread(VOID *argptr)
{
time_t MaxDelay, LastFlush, now;
struct param *param;
static char *fid = "FlushThread";

    param = (struct param *) argptr;
    MaxDelay = param->fwd;

    LastFlush = time(NULL);
    util_log(1, "flushing wfdiscs at %d second intervals", MaxDelay);

    while (1) {
        now = time(NULL);
        if (now - LastFlush > MaxDelay) {
            nrts_wrtbwd();
            if (param->IsiStatPath != NULL) LogIsiIacpStats(now);
            LastFlush = now;
        }
        sleep(10);
    }
}

VOID StartFlushThread(struct param *param)
{
FILE *fp;
THREAD tid;
/*sigset_t set;*/
static char *header = "#          tstamp        nbyte       nframe       maxlen       minlen       avelen       stddev";
static char *fid = "StartFlushThread";

    if (param->IsiStatPath != NULL) {
        if ((fp = fopen(param->IsiStatPath, "w")) == NULL) {
            util_log(1, "%s: fopen: %s: %s", fid, param->IsiStatPath, strerror(errno));
            param->IsiStatPath = NULL;
        } else {
            fprintf(fp, "%s\n", header);
            fclose(fp);
            util_log(1, "logging ISI:IACP stats to %s\n", param->IsiStatPath);
        }
    }

    if (!THREAD_CREATE(&tid, FlushThread, (void *) param)) {
        util_log(1, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        exit(1);
    }
}

/* Revision History
 *
 * $Log: flush.c,v $
 * Revision 1.4  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.3  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.2  2005/05/06 01:09:29  dechavez
 * added optional logging of ISI:IACP recv stats
 *
 * Revision 1.1  2003/12/22 22:12:48  dechavez
 * created
 *
 */
