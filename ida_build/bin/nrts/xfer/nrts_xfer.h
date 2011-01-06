#pragma ident "$Id: nrts_xfer.h,v 1.3 2006/02/10 02:04:00 dechavez Exp $"
#ifndef nrts_xfer_h_included
#define nrts_xfer_h_included

#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include "nrts.h"
#include "util.h"

#define MAX_BUFSIZ IDA_BUFSIZ

#define HARDCODED_MAXDUR_PARAMETER 0 /* disable maxdur */

struct span {
    double beg;  /* time of first sample in record */
    double end;  /* time of last  sample in record */
};

/* Function prototypes */

#endif /* xfer_h_included */

/* Revision History
 *
 * $Log: nrts_xfer.h,v $
 * Revision 1.3  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.2  2002/11/19 18:39:03  dechavez
 * added HARDCODED_MAXDUR
 *
 * Revision 1.1.1.1  2000/02/08 20:20:17  dec
 * import existing IDA/NRTS sources
 *
 */
