#pragma ident "$Id: wdreq.c,v 1.3 2002/11/20 01:16:10 dechavez Exp $"
/*======================================================================
 *
 *  Service a request for wfdiscs.
 *
 *====================================================================*/
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "nrts.h"
#include "nrts_xfer.h"
#include "cssio.h"
#include "util.h"

int do_wdreq(char *client, char *home, int ifd, int ofd, int to)
{
u_long lval;
u_short sval;
int maxdur = HARDCODED_MAXDUR_PARAMETER;
long i, count, nrec, flush, len;
char *syscode, *buffer;
char sta[NRTS_SNAMLEN+1];
struct wfdisc *wfdisc = NULL;
struct nrts_mmap map;
static char *fid = "do_wdreq";

    util_log(2, "wfdisc request from %s", client);

/* Get station code and flush flag */

    if (util_read(ifd, sta, NRTS_SNAMLEN, to) != NRTS_SNAMLEN) {
        util_log(1, "%s not responding: %s", client, syserrmsg(errno));
        die(1);
    }

    if (util_read(ifd, (char *) &sval, sizeof(sval), to) != sizeof(sval)) {
        util_log(1, "%s not responding: %s", client, syserrmsg(errno));
        die(1);
    }

    flush = (int) ntohs(sval);

/*  Get wfdiscs for the system which contains this station  */

    if ((count = nrts_stawd(home, util_ucase(sta), &wfdisc, flush, maxdur, NULL)) < 0) {
        util_log(1, "%s: nrts_stawd: %s", syserrmsg(errno));
        die(1);
    }

/*  Send over the records for this station  */

    lval = htonl((u_long) count);
    len  = sizeof(u_long);
    if (util_write(ofd, (char *) &lval, len, to) != len) {
        util_log(1, "%s not responding: %s", client, syserrmsg(errno));
        die(1);
    }

    util_log(2, "sending %ld %s wfdisc records to %s", count, sta, client);
    for (i = 0; i < count; i++) {
        buffer = wdtoa(wfdisc + i);
        if ((len = strlen(buffer)+1) != WFDISC_SIZE) {
            util_log(1, "wfdisc length `%d' != `%d'", len, WFDISC_SIZE);
            die(1);
        }
        util_log(3, "%s", buffer);
        if (util_write(ofd, (char *) buffer, len, to) != len){
            util_log(1, "%s not responding: %s", client, syserrmsg(errno));
            die(1);
        }
    }

    die(0);
}

/* Revision History
 *
 * $Log: wdreq.c,v $
 * Revision 1.3  2002/11/20 01:16:10  dechavez
 * added dbdir argument to nrts_stawd
 *
 * Revision 1.2  2002/11/19 18:39:18  dechavez
 * pass new maxdur parameter to nrts_stawd
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
