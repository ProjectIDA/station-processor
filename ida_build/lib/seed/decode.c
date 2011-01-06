#pragma ident "$Id: decode.c,v 1.4 2005/05/25 22:40:22 dechavez Exp $"
/*======================================================================
 *
 *  Decode a mini seed record header.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <math.h>
#include "seed.h"
#include "util.h"

#define MAX_TRY 5

static int logReturn(char *fid, int retval)
{
    util_log(3, "%s: return %d", fid, retval);
    return retval;
}

int seed_minihdr(
struct seed_minipacket *output,
char *buffer
) {
char *ptr;
int type, status;
long next, count;
static char *fid = "seed_decodemini";
static char tmpbuf[2048];

/* Decode it, ignoring all blockettes except for 1000 */

    util_log(3, "%s: calling seed_fsdh", fid);
    if ((status = seed_fsdh(&output->fsdh, buffer)) != 0) {
        util_log(1, "%s: can't decode FSDH: status %d", fid, status);
        return logReturn(fid, -1);
    }
    util_log(3, "%s: seed_fsdh returns %d", fid, status);

    if (strcasecmp(output->fsdh.chnid, "LOG") == 0) {
        util_log(2, "%s: ignore LOG record", fid);
        return logReturn(fid, -2);
    }

    util_log(3, "%s: looking for Blockette 1000", fid);
    next = output->fsdh.first;
    count = 0;
    do {
        ptr  = buffer + next;
        type = seed_type(ptr, &next, output->fsdh.swap);
    } while (++count < MAX_TRY && next != 0 && type != 1000);

    if (count == MAX_TRY) {
        util_log(3, "%s: abandon Blockette 1000 search (MAX_TRY=%d)",
            fid, MAX_TRY
        );
#ifdef DUMP_IGNORED_RECORD
        util_hexlog(1, buffer, 256, 0, 'x', tmpbuf);
        util_log(3, "%s:  seqno = %ld", fid, output->fsdh.seqno);
        util_log(3, "%s:  staid = %s",  fid, output->fsdh.staid);
        util_log(3, "%s:  locid = %s",  fid, output->fsdh.locid);
        util_log(3, "%s:  chnid = %s",  fid, output->fsdh.chnid);
        util_log(3, "%s:  start = %s",  fid, util_dttostr(output->fsdh.start, 0));
        util_log(3, "%s:  nsamp = %hd", fid, output->fsdh.nsamp);
        util_log(3, "%s: srfact = %hd", fid, output->fsdh.srfact);
        util_log(3, "%s: srmult = %hd", fid, output->fsdh.srmult);
        util_log(3, "%s:   more = %d",  fid, (int) output->fsdh.more);
        util_log(3, "%s:    bod = %hd", fid, output->fsdh.bod);
        util_log(3, "%s:  first = %hd", fid, output->fsdh.first);
#endif /* DUMP_IGNORED_RECORD */
        return logReturn(fid, -3);
    }

    if (type != 1000) {
        util_log(2, "%s: no Blockette 1000!", fid);
        return logReturn(fid, -4);
    }

    util_log(3, "%s: calling seed_b1000", fid);
    if ((status = seed_b1000(&output->b1000, ptr)) != 0) {
        util_log(1, "%s: can't decode Blockette 1000: status %d",
            fid, status
        );
        return logReturn(fid, -5);
    }
    util_log(3, "%s: seed_b1000 returns %d", fid, status);

/* Fill in the derived fields */

    output->srclen = (long)
             (pow(2.0, (double) output->b1000.length)) - output->fsdh.bod;
    output->order = output->b1000.order ? BIG_ENDIAN_ORDER : LTL_ENDIAN_ORDER;
    output->nsamp = output->fsdh.nsamp;
    output->beg   = output->fsdh.start;
    output->sname = output->fsdh.staid;
    output->cname = output->fsdh.chnid;
    output->nname = output->fsdh.netid;
    output->sint  = seed_makesint(output->fsdh.srfact,output->fsdh.srmult);

    return logReturn(fid, 0);
}

/* Revision History
 *
 * $Log: decode.c,v $
 * Revision 1.4  2005/05/25 22:40:22  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2000/10/06 19:52:10  dec
 * fix runaway condition searching for Blockette 1000's, include debugging
 * code
 *
 * Revision 1.2  2000/09/21 22:06:09  nobody
 * no Blockette 1000 errors logged at level 2
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
