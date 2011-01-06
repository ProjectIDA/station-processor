#pragma ident "$Id: cnfreq.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Service a request for system configuration information.
 *
 *====================================================================*/
#include <errno.h>
#include <memory.h>
#include "nrts.h"
#include "nrts_xfer.h"
#include "util.h"

int do_cnfreq(char *client, char *home, int isd, int osd, int to)
{
u_short sval;
char *syscode;
size_t len, slen;
struct nrts_files *file;
struct nrts_sys *sys;
struct nrts_mmap map;
static char *fid = "do_cnfreq";

    util_log(1, "configuration request from %s", client);

    slen = sizeof(sval);
    if (util_read(isd, (char *) &sval, slen, to) != slen) {
        util_log(1, "%s: 1: read: %s", fid, syserrmsg(errno));
        sval = htons(-1);
        util_write(osd, (char *) &sval, len, to);
        die(1);
    }
    len = ntohs(sval);

    if ((syscode = (char *) malloc(len)) == NULL) {
        util_log(1, "%s: 2: malloc: %s", fid, syserrmsg(errno));
        sval = htons(-errno);
        util_write(osd, (char *) &sval, slen, to);
        die(1);
    }

    if (util_read(isd, syscode, len, to) != len) {
        util_log(1, "%s: 3: read: %s", fid, syserrmsg(errno));
        sval = htons(-errno);
        util_write(osd, (char *) &sval, slen, to);
        die(1);
    }

    file = nrts_files(&home, syscode);
    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) != 0) {
        util_log(1, "can't get `%s' system map %s: %s",
            syscode, file->sys, syserrmsg(errno)
        );
        sval = htons(-errno);
        util_write(osd, (char *) &sval, slen, to);
        die(1);
    }
    sys = (struct nrts_sys *) map.ptr;

    util_log(1, "returning static `%s' system information", syscode);

    if (nrts_sendsys(osd, sys, to) != NRTS_OK) {
        util_log(1, "%s: 4: nrts_sendsys failed: %s", fid, syserrmsg(errno));
        die(1);
    }

    die(0);
}

/* Revision History
 *
 * $Log: cnfreq.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
