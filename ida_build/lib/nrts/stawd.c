#pragma ident "$Id: stawd.c,v 1.5 2004/06/24 17:37:07 dechavez Exp $"
/*======================================================================
 *
 *  Return all the wfdiscs for a given NRTS station.
 *
 *====================================================================*/
#include "nrts.h"
#include "cssio.h"
#include "util.h"

int nrts_stawd(char *home, char *sta, struct wfdisc **output, int flush, int maxdur, char *dbspec)
{
long count = 0;
long i, nrec;
char *syscode;
struct wfdisc *wfdisc;
static char *fid = "nrts_stawd";

    if (sta == NULL) {
        errno = EINVAL;
        return -1;
    }

/*  Get wfdiscs for the system which contains this station  */

    if ((syscode = nrts_syscode(home, util_lcase(sta))) == NULL) {
        util_log(1, "no system found for station %s", sta);
        nrec = -2;
    } else {
        util_log(3, "extracting wfdiscs, using sys `%s' for sta `%s'",
            syscode, sta
        );
        if ((nrec = nrts_syswd(home, syscode, &wfdisc, flush, maxdur, dbspec)) < 0) {
            util_log(1, "nrts_syswd error %d", nrec);
            return -3;
        }
        util_log(3, "system has a total of %ld wfdisc records", nrec);
    }

/*  Save only those which are for this station  */

    for (i = 0; i < nrec; i++) {
        if (strcmp(util_lcase(wfdisc[i].sta), sta) == 0) {
            wfdisc[count++] = wfdisc[i];
        }
    }
    if (nrec) util_log(3, "%ld wfdisc records are for station %s", count, sta);

    *output = wfdisc;

    return count;
}

/* Revision History
 *
 * $Log: stawd.c,v $
 * Revision 1.5  2004/06/24 17:37:07  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.4  2004/04/23 00:46:04  dechavez
 * changed dbdir to dbspec
 *
 * Revision 1.3  2002/11/20 01:03:54  dechavez
 * pass thru dbdir to syswd
 *
 * Revision 1.2  2002/11/19 18:34:58  dechavez
 * pass thru maxdur to syswd
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
