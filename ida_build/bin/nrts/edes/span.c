#pragma ident "$Id: span.c,v 1.2 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Get time span for data record with the given index.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <unistd.h>
#include "nrts.h"
#include "util.h"
#include "xfer.h"
#include "edes.h"

#define HFD    (edes->inf[i].hfd)
#define DECODE (edes->decode[i])
#define SYS    (edes->sys + i)
#define STA    (SYS->sta + j)
#define CHN    (STA->chn + k)

struct edes_span *get_span(struct edes_params *edes, int i, int j, int k, int index, char *buffer)
{
int fd, dummy;
struct xfer_packet *packet;
off_t off;
static struct edes_span span;
static char *fid = "get_span";

/* Read the header */

    off = CHN->hdr.off + (index * CHN->hdr.len);

    if (lseek(HFD, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(HFD, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, hdr off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EFAULT;
        Xfer_Ack(edes->osd, xfer_errno);
        Xfer_Exit(edes->osd, xfer_errno);
    }

    if (read(HFD, buffer, CHN->hdr.len) != CHN->hdr.len) {
        util_log(1, "%s: read(HFD, buffer, %d): %s",
                 fid, CHN->hdr.len, syserrmsg(errno)
        );
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, hdr off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EFAULT;
        Xfer_Ack(edes->osd, xfer_errno);
        Xfer_Exit(edes->osd, xfer_errno);
    }

/* Decode the header */

    packet = (*DECODE)(edes, i, j, k, 0, buffer, 0, (int *) NULL);
    if (packet == (struct xfer_packet *) NULL) {
        util_log(1, "%s: can't decode header", fid);
        xfer_errno = XFER_EFAULT;
        Xfer_Ack(edes->osd, xfer_errno);
        Xfer_Exit(edes->osd, xfer_errno);
    }

    span.beg = packet->beg;
    span.end = packet->end;
    
    return &span;
}

/* Revision History
 *
 * $Log: span.c,v $
 * Revision 1.2  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
