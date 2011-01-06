#pragma ident "$Id: getrec.c,v 1.2 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Read the specified raw record from the disk loop.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "nrts.h"
#include "edes.h"

#define HFD (edes->inf[i].hfd)
#define DFD (edes->inf[i].dfd)
#define SYS (edes->sys + i)
#define STA (SYS->sta + j)
#define CHN (STA->chn + k)

struct xfer_packet *getrec(struct edes_params *edes, int i, int j, int k, long index, char *buffer, int flag, int swap, int *ws)
{
off_t off;
struct xfer_packet *packet;
static char *fid = "getrec";

    if (index < 0) {
        xfer_errno = XFER_EINVAL;
        return (struct xfer_packet *) NULL;
    }

/* Read the header part */

    off = CHN->hdr.off + (index * CHN->hdr.len);

    if (lseek(HFD, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(HFD, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, hdr off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EIO;
        return (struct xfer_packet *) NULL;
    }

    if (read(HFD, buffer, CHN->hdr.len) != CHN->hdr.len) {
        util_log(1, "%s: read(HFD, buf, %d): %s",
                 fid, CHN->hdr.len, syserrmsg(errno)
        );
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, hdr off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EIO;
        return (struct xfer_packet *) NULL;
    }

/* Read the data part */

    off = CHN->dat.off + (index * CHN->dat.len);

    if (lseek(DFD, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(DFD, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, dat off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EIO;
        return (struct xfer_packet *) NULL;
    }

    if (read(DFD, buffer+CHN->hdr.len, CHN->dat.len) != CHN->dat.len) {
        util_log(1, "%s: read(DFD, buf+CHN->hdr.len, %d): %s",
                 fid, CHN->dat.len, syserrmsg(errno));
        util_log(1, "%s: STA = %s, CHN = %s, index = %ld, dat off =  %ld",
            fid, STA->name, CHN->name, index, off
        );
        xfer_errno = XFER_EIO;
        return (struct xfer_packet *) NULL;
    }

/* Decode into an xfer_packet */

    return (*edes->decode[i])(edes, i, j, k, flag, buffer, swap, ws);
}

/* Revision History
 *
 * $Log: getrec.c,v $
 * Revision 1.2  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
