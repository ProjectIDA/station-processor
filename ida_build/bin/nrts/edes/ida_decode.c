#pragma ident "$Id: ida_decode.c,v 1.6 2006/12/06 22:46:55 dechavez Exp $"
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
#include "ida.h"
#include "xfer.h"
#include "cssio.h"
#include "edes.h"

#define SYS  (edes->sys + i)
#define SSTA  (SYS->sta + j)
#define SCHN (SSTA->chn + k)
 
#define CNF  (edes->cnf + i)
#define CSTA  (CNF->sta + j)
#define CCHN (CSTA->chn + k)

#define INF  (edes->inf + i)

static unsigned long host_order = 0;
static BOOL DumpBadData = FALSE;

static struct {
    long count;
    IDA_DHDR dhead;
} prev[MAXSYS][NRTS_MAXSTA][NRTS_MAXCHN];

void ida_decode_init(BOOL DumpBadDataFlag)
{
int i, j, k;

    host_order = util_order();
    DumpBadData = DumpBadDataFlag;

    for (i = 0; i < MAXSYS; i++) {
        for (j = 0; j < NRTS_MAXSTA; j++) {
            for (k = 0; k < NRTS_MAXCHN; k++) {
                prev[i][j][k].count = 0;
            }
        }
    }
}

static void DumpPacket(char *buffer, int len)
{
char path[MAXPATHLEN+1];
FILE *fp;
static char *fid = "DumpPacket";
static char *marker = "****************";

    sprintf(path, "/var/tmp/edes.%d", getpid());
    if ((fp = fopen(path, "a+b")) == NULL) {
        util_log(1, "%s: fopen: %s", fid, strerror(errno));
        return;
    }

    if (fwrite(buffer, 1, len, fp) != len) {
        util_log(1, "%s: fwrite: %s", fid, strerror(errno));
    }
    if (fwrite(marker, 1, strlen(marker), fp) != strlen(marker)) {
        util_log(1, "%s: fwrite: %s", fid, strerror(errno));
    }

    fclose(fp);
    return;
}

struct xfer_packet *ida_decode(struct edes_params *edes, int i, int j, int k, int init_flag, char *buffer, int swap, int *ws)
{
int ii, sign, err;
char *map;
u_long errtic;
short *sdata;
long  *ldata;
UINT64 *lldata;
IDA *ida;
IDA_DHDR dhead, *crnt;
struct xfer_packet *null_packet = (struct xfer_packet *) NULL;
static struct xfer_packet packet;
static char *fid = "ida_decode";

    ida = INF->ida;

    if (ida_rtype(buffer, ida->rev.value) != IDA_DATA) {
        util_log(1, "%s: unexpected non-data packet received/dropped", fid);
        if (DumpBadData) DumpPacket(buffer, 1024);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    crnt = &dhead;
    if (ida_dhead(ida, crnt, buffer) != 0) {
        util_log(1, "%s: unexpected corrupt packet received/dropped", fid);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    if (crnt->nsamp > XFER_MAXDAT) {
        util_log(1, "%s: unexpected large packet received/dropped", fid);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    if (crnt->wrdsiz != 4 && crnt->wrdsiz != 2 && crnt->wrdsiz != 8) {
        util_log(1, "%s: unexpected wrdsiz = %d, packet dropped", fid, crnt->wrdsiz);
        xfer_errno = XFER_EINVAL;
        return null_packet;
    }

    strcpy(packet.sname, SSTA->name);
    strcpy(packet.cname, SCHN->name);
    packet.beg   = crnt->beg.tru;
    packet.end   = crnt->end.tru;
    packet.sint  = crnt->sint;
    packet.nsamp = crnt->nsamp;
    packet.hlen  = SCHN->hdr.len;
    packet.dlen  = SCHN->dat.len;
    packet.tear  = 0; /* maybe */
    if (ws != (int *) NULL) *ws = crnt->wrdsiz;

    if (swap) {
        if (crnt->wrdsiz == 4) {
            ldata = (long *) (buffer + packet.hlen);
            if (crnt->order != host_order) util_lswap(ldata, packet.nsamp);
        } else if (crnt->wrdsiz == 2) {
            sdata = (short *) (buffer + packet.hlen);
            if (crnt->order != host_order) util_sswap(sdata, packet.nsamp);
        } else {
            return null_packet;
        }
    }

    packet.hdr  = buffer;
    packet.dat  = buffer + packet.hlen;
    packet.data = (long *) (buffer + packet.hlen);

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
        err = ida_timerr(crnt, &prev[i][j][k].dhead, &sign, &errtic);
        if (err != 0) {
            util_log(1,"%s: ida_timerr failed with status %d", fid, err);
            return null_packet;
        } else if (errtic != 0) {
            packet.tear = (float)sign*((errtic*crnt->beg.mult)/1000.0);
        }
    }
    prev[i][j][k].dhead = *crnt;
    ++prev[i][j][k].count;

    return &packet;
}

/* Revision History
 *
 * $Log: ida_decode.c,v $
 * Revision 1.6  2006/12/06 22:46:55  dechavez
 * Only support 2 and 4 byte data types (since that is all the protocol can support)
 *
 * Revision 1.5  2006/11/13 19:39:53  dechavez
 * added 64bit data support
 *
 * Revision 1.4  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2004/06/21 20:21:35  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.2  2002/03/11 17:32:06  dec
 * support DumpBadData flag
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
