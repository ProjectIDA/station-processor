#pragma ident "$Id: main.c,v 1.2 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Provide selected NRTS data to a remote client.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "nrts_xfer.h"
#include "nrts.h"
#include "util.h"

void main(int argc, char **argv)
{
char *client, *home = NULL;
u_short sval;
int ifd, ofd, to;
int action;
char dbdir[MAXPATHLEN+1];
static char *fid = "main";

    if ((client = init(argc, argv, &home, &ifd, &ofd, &to, dbdir)) == NULL) {
        fprintf(stderr, "%s: failed to init\n", argv[0]);
        util_log(1, "abort: failed to init");
        exit(1);
    }

    while (1) {
        if (util_read(ifd, (char *) &sval, sizeof(sval), to) == sizeof(sval)) {
            action = (int) ntohs(sval);
            if (action == NRTS_DATREQ) {
                do_datreq(client, home, ifd, ofd, to, dbdir);
            } else if (action == NRTS_WDREQ) {
                do_wdreq(client, home, ifd, ofd, to);
            } else if (action == NRTS_CNFREQ) {
                do_cnfreq(client, home, ifd, ofd, to);
            } else {
                util_log(1, "unknown request code 0x%04x", action);
                die(1);
            }
        } else {
            util_log(1, "client@%s not responding: %s",client,syserrmsg(errno));
            die(1);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
