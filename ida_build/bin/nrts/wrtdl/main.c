#pragma ident "$Id: main.c,v 1.8 2005/05/23 20:56:51 dechavez Exp $"
/*======================================================================
 *
 *  Read data from one of several sources and write to a NRTS disk loop.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "wrtdl.h"
#include "nrts.h"
#include "util.h"

int main(int argc, char **argv)
{
size_t len;
int status, oldpid, mypid, error;
char *temp, *prev, *crnt;
struct param param;
struct nrts_header *hd;
struct nrts_sys *sys;
int (*getrec)();
static char *fid = "main";

    utilNetworkInit();

    if (init(argc, argv, &param) != 0) {
        fprintf(stderr, "%s: failed to init\n", argv[0]);
        util_log(1, "%s: abort: failed to init", fid);
        if (param.sys != NULL) param.sys->pid = 0;
        exit(1);
    }

    mypid = getpid();

    len    =  param.len;
    prev   =  param.prev;
    crnt   =  param.crnt;
    getrec =  param.read_func;
    sys    =  param.sys;
    hd     =  NULL;

    memset(prev, 0, len);

    while (1) {

        /* Read a record from input */

        if ((status=(*getrec)(crnt, len, &hd)) == NRTS_OK) {

            /* Write to disk if unique, discard it if it's a duplicate */

            if (memcmp(prev, crnt, len) == 0) {
                util_log(1, "duplicate record dropped (len=%d)", len);
            } else {
                LockDiskloop();
                    error = nrts_wrtdl(crnt, &hd);
                UnlockDiskloop();
                if (error) {
                    util_log(1, "abort: fatal nrts_wrtdl error");
                    die(NRTS_ERROR);
                }
            }
            temp = prev;
            prev = crnt;
            crnt = temp;

            /* If pid was clobbered (why?) reset it now */

            LockDiskloop();
                if ((oldpid = sys->pid) != mypid) {
                    util_log(1, "sys->pid clobbered? Was %d, reset to %d.", oldpid, mypid);
                    sys->pid = mypid;
                }
            UnlockDiskloop();

        } else if (status == NRTS_EOF) {
            util_log(1, "end of input");
            die(NRTS_OK);
        } else if (status == NRTS_ERROR) {
            die(NRTS_OK);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.8  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.7  2004/09/28 23:27:30  dechavez
 * use disk loop lock/unlock guards to prevent corruption due to race
 * condition between main and wfdisc update threads
 *
 * Revision 1.6  2004/01/29 18:52:36  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.5  2003/12/22 22:12:30  dechavez
 * removed obsoleted flush wfdisc stuff
 *
 * Revision 1.4  2003/11/25 20:40:44  dechavez
 * ANSI function declarations
 *
 * Revision 1.3  2002/04/25 20:20:27  dec
 * changed main to int to quiet gcc compiler
 *
 * Revision 1.2  2000/03/09 17:36:10  dec
 * check for clobbered sys->pid and reset if needed
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
