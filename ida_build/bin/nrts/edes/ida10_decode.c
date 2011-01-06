#pragma ident "$Id: ida10_decode.c,v 1.7 2007/03/06 18:38:06 dechavez Exp $"
/*======================================================================
 *
 *  Reformat a raw IDA data record into a xfer_packet.  It is assumed
 *  that the NRTS has taken care of *ALL* possible data problems, so if
 *  any unexpected problems arise in decoding the packet we throw up
 *  our hands and return an error status.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "util.h"
#include "nrts.h"
#include "ida10.h"
#include "xfer.h"
#include "cssio.h"
#include "edes.h"

#define SYS  (edes->sys + i)
#define SSTA  (SYS->sta + j)
#define SCHN (SSTA->chn + k)
 
#define CNF  (edes->cnf + i)
#define CSTA  (CNF->sta + j)
#define CCHN (CSTA->chn + k)

static struct {
    long count;
    IDA10_TSHDR tshdr;
} prev[MAXSYS][NRTS_MAXSTA][NRTS_MAXCHN];

void ida10_decode_init(BOOL DumpBadDataFlag)
{
int i, j, k;

    for (i = 0; i < MAXSYS; i++) {
        for (j = 0; j < NRTS_MAXSTA; j++) {
            for (k = 0; k < NRTS_MAXCHN; k++) {
                prev[i][j][k].count = 0;
            }
        }
    }
}

struct xfer_packet *ida10_decode(struct edes_params *edes, int i, int j, int k, int init_flag, char *buffer, int swap, int *ws)
{
int sign, status;
UINT64 err;
char *map;
IDA10_TS ts;
REAL64 errsec, errsmp;
struct xfer_packet *null_packet = (struct xfer_packet *) NULL;
static struct xfer_packet packet;
static char *fid = "ida10_decode";

    if (!ida10UnpackTS((UINT8 *) buffer, &ts)) {
        util_log(1, "%s: unexpected corrupt packet received/dropped", fid);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    if (ts.hdr.nsamp > XFER_MAXDAT) {
        util_log(1, "%s: unexpected large packet received/dropped", fid);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    strcpy(packet.sname, SSTA->name);
    strcpy(packet.cname, SCHN->name);
    packet.sint  = (float) ts.hdr.sint;
    packet.nsamp = ts.hdr.nsamp;
    packet.hlen  = SCHN->hdr.len;
    packet.dlen  = SCHN->dat.len;
    packet.tear  = 0; /* maybe */
    if (ws != (int *) NULL) {
        switch (ts.hdr.datatype) {
          case IDA10_DATA_INT8: *ws = 1; break;
          case IDA10_DATA_INT16: *ws = 2; break;
          case IDA10_DATA_INT32: *ws = 4; break;
          case IDA10_DATA_REAL32: *ws = 4; break;
          case IDA10_DATA_REAL64: *ws = 8; break;
          default: *ws = -1;
        }
    }

    packet.hdr  = buffer;
    packet.dat  = buffer + packet.hlen;
    memcpy(packet.dbuf, ts.data.int8, ts.hdr.nbytes);
    packet.data = (long *) (packet.dbuf);

/* If this is a call from init, then we are done here */

    if (init_flag) return &packet;

/* Otherwise take it from stored config and then check for time tears */

    packet.lat   = CSTA->lat;
    packet.lon   = CSTA->lon;
    packet.elev  = CSTA->elev;
    packet.depth = CSTA->depth;
    packet.vang  = CCHN->vang;
    packet.hang  = CCHN->hang;
    strcpy(packet.instype, CCHN->instype);

    packet.calib  = CCHN->calib;
    packet.calper = CCHN->calper;

    if (prev[i][j][k].count) {
        status = ida10TtagIncrErr(&prev[i][j][k].tshdr, &ts.hdr, &sign, &err);
        if (status != 0) {
            util_log(1,"%s: ida10TtagIncrErr failed with status %d", fid, status);
            return null_packet;
        } else {
            errsec = (REAL64) err / (REAL64) NANOSEC_PER_SEC;
            errsmp = errsec / (REAL64) ts.hdr.sint;
            packet.tear = (int) errsmp;
        }
        if (packet.tear == 0) {
            ts.hdr.tofs = prev[i][j][k].tshdr.tols + ts.hdr.sint;
            ts.hdr.tols = ts.hdr.tofs + ((ts.hdr.nsamp - 1) * ts.hdr.sint);
        }
    }
    packet.beg   = ts.hdr.tofs;
    packet.end   = ts.hdr.tols;
    prev[i][j][k].tshdr = ts.hdr;
    ++prev[i][j][k].count;

    return &packet;
}

/* Revision History
 *
 * $Log: ida10_decode.c,v $
 * Revision 1.7  2007/03/06 18:38:06  dechavez
 * Force time stamps for all contiguous packets to increment by one sample interval
 *
 * Revision 1.6  2007/02/14 23:47:11  dechavez
 * Patch time stamps in generic packets if system time increments OK
 *
 * Revision 1.5  2006/11/13 19:55:54  dechavez
 * don't check for ttag.valid anymore (doesn't exist), set packet.tear
 * to be error in samples (ie, 0 if error is less than one sample),
 * support all possible data types in determining ws value (even though
 * use of ws is really deprecrated elsewhere)
 *
 * Revision 1.4  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2003/10/16 18:19:00  dechavez
 * use new int8 data field as data source
 *
 * Revision 1.2  2002/03/11 17:28:56  dec
 * added new DumpBadDataFlag to init routine, but ignored here
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
