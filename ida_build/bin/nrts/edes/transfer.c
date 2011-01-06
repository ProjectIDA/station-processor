#pragma ident "$Id: transfer.c,v 1.4 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Send over the next record for the indicated channel
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "edes.h"

#define STA  (edes->sys[i].sta + j)
#define CHN  (edes->sys[i].sta[j].chn + k)
#define ICHN (edes->inf[i].sta[j].chn + k)
#define SWAP (format != XFER_WAVRAW && format != XFER_WAVIDA)

extern char *Peer;

void transfer(struct xfer_cnf *cnf, int format, int comp, struct edes_params *edes, int i, int j, int k, char *buffer, char *peer)
{
int status, ws;
static u_long count = 0;
static struct xfer_packet *packet;
static struct xfer_wav wav;
static char *fid = "transfer";

/* Get an xfer_packet from the disk loop */

    util_log(3, "read %s:%s nrts packet", STA->name, CHN->name);
    packet = getrec(edes, i, j, k, ICHN->nxtndx, buffer, 0, SWAP, &ws);
    if (packet != (struct xfer_packet *) NULL) {

    /* Load the xfer waveform packet */

        util_log(3, "reformat %s:%s packet", STA->name, CHN->name);
        if ((status = reformat(&wav,format,comp,edes,i,j,k,packet)) != 0) {
            util_log(1, "%s: reformat failed: status %d", fid, status);
            Xfer_Ack(edes->osd, XFER_EFAULT);
            Xfer_Exit(edes->osd, XFER_EFAULT);
        }

    /* Send the waveform packet */

        if (++count == 1) util_log(1, "begin waveform transfer to %s", peer);
        util_log(2, "sending %s:%s wavform packet", STA->name, CHN->name);
        if ((status = Xfer_SendWav(edes->osd, cnf, &wav)) != XFER_OK) {
            util_log(1, "abandon %s: %s", Peer, Xfer_ErrStr());
            Xfer_Ack(edes->osd, XFER_EFAULT);
            Xfer_Exit(edes->osd, XFER_EFAULT);
        }
    }

/* Increment index for next time through */

    if (ICHN->nxtndx == ICHN->endndx) {
        util_log(2, "last record for this channel sent");
        ICHN->status = edes->keepup ? EDES_WAITING : EDES_DONE;
    } else {
        util_log(3, "increment %s:%s indices", STA->name, CHN->name);
        ++ICHN->nxtndx;
        if (
            ICHN->endndx < ICHN->begndx &&
            ICHN->nxtndx == CHN->hdr.nrec
        ) ICHN->nxtndx = 0;
    }

    if (ICHN->status == EDES_DONE) {
        util_log(2, "done with %s:%s", STA->name, CHN->name);
    } else if (ICHN->status == EDES_WAITING) {
        util_log(2, "waiting for more %s:%s data", STA->name, CHN->name);
    }
}

/* Revision History
 *
 * $Log: transfer.c,v $
 * Revision 1.4  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2004/04/26 21:11:14  dechavez
 * misc debugging statements added, plus MySQL support (via dbspec instead of dbdir)
 *
 * Revision 1.2  2002/04/25 21:58:33  dec
 * print "abandon" message when xfer_SendWave fails, and just exit w/o ack
 * for timeout, pipe and i/o errors
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
