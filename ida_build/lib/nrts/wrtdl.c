#pragma ident "$Id: wrtdl.c,v 1.8 2006/02/09 19:40:16 dechavez Exp $"
/*======================================================================
 *
 *  Write a single packet to the disk buffer and update the wfdiscs
 *  (if applicable).
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

#ifndef O_SYNC
#define O_SYNC 0
#endif

static int terminate  = 0;
static int exitstatus = 0;
static int locked     = 0;

static struct {
    int hdr;
    int dat;
} fd = {-1, -1};

static struct nrts_sys *sys;
static struct nrts_header *(*decode)();
static char *mybuf;

int nrts_iniwrtdl(
    char *home, char *syscode, struct nrts_sys *sysptr, int flags,
    IDA *ida, void *opt
) {
off_t off;
struct nrts_chn *chn;
struct nrts_files *file;
struct nrts_header *hd;
int i, j, nop = 0;
static char *fid = "nrts_iniwrtdl";

    sys = sysptr;

/*  Open the disk buffers  */

    if ((file = nrts_files(&home, syscode)) == NULL) {
        util_log(1, "%s: nrts_files failed", fid);
        return -1;
    }

    if ((fd.hdr = open(file->hdr, O_RDWR  | O_BINARY | O_SYNC)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->hdr, syserrmsg(errno));
        return -3;
    }

    if ((fd.dat = open(file->dat, O_RDWR  | O_BINARY | O_SYNC)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->dat, syserrmsg(errno));
        return -4;
    }

/* Initialize system specific decoder  */

    util_log(2, "%s: flagword = 0x%02x", fid, flags);
    decode = (struct nrts_header *(*)())
             nrts_inidecode(home, syscode, sys, flags, ida, opt, fd.hdr);

    if (decode == NULL) {
        util_log(2, "%s: nrts_inidecode failed", fid);
        return -5;
    }

/* Initialize the wfdiscs  */

    if (nrts_inibwd(file, sys, syscode, home) != 0) {
        util_log(2, "%s: nrts_inibwd failed", fid);
        return -6;
    }
        
/* Initialize the record counters  */

    sys->nread = 0;
    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) sys->sta[i].chn[j].nread = 0;
    }

/* Allocate space for internal buffer, if required */

    if (sys->type == NRTS_ASL) {
        if ((mybuf = (char *) malloc(sys->reclen)) == (char *) NULL) {
            util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
            return -7;
        }

    /* Read yngest record in the disk loop so we can track gaps */

        for (i = 0; i < sys->nsta; i++) {
            for (j = 0; j < sys->sta[i].nchn; j++) {
                chn = sys->sta[i].chn + j;
                if (chn->count > 0) {
                    off = chn->hdr.off + (chn->hdr.yngest * chn->hdr.len);
                    if (lseek(fd.hdr, off, SEEK_SET) != off) {
                        util_log(1, "%s: lseek(fd.hdr, %ld, SEEK_SET): %s",
                                 fid, off, syserrmsg(errno)
                        );
                    } else if (read(fd.hdr, mybuf, sys->reclen) == sys->reclen) {
                        hd = (*decode)(mybuf, sys->reclen, &nop);
                        if (hd != (struct nrts_header *) NULL) {
                            util_log(1, "%s: last %s packet: %s",
                                fid, chn->name,
                                util_dttostr(hd->wfdisc.time, 0)
                            );
                        } else {
                            util_log(1, "%s: can't decode %s header!",
                                fid, chn->name
                            );
                        }
                    } else {
                        util_log(1, "%s: %s read: %s",
                            fid, chn->name, syserrmsg(errno)
                        );
                    }
                }
            }
        }
    }

    return 0;
}

int nrts_wrtdl(buf, hdptr)
char *buf;
struct nrts_header **hdptr;
{
int status;
off_t off = 0, hoff, doff;
long oldest, yngest, lend;
struct nrts_sta *sta;
struct nrts_chn *chn;
struct nrts_header *hd;
int action = 0;
static int decodeonly = NRTS_DECODE;
static char *fid = "nrts_wrtdl";

    locked = 1;

    *hdptr = NULL;

/*  Decode this record to determine where it goes  */

    if ((hd = (*decode)(buf, sys->reclen, &action)) == NULL) {
        if (terminate) nrts_die(exitstatus); else return 0;
    }
    sta = &sys->sta[hd->standx];
    chn = &sta->chn[hd->chnndx];
    if (chn->end >= hd->wfdisc.time) {
        util_log(2, "%s: %s:%s overlap, packet dropped", fid, sta->name, chn->name);
        return 0;
    }

/*  Demux the record to disk  */

    if ((yngest = chn->hdr.yngest + 1) == chn->hdr.nrec) {
        yngest = 0;
        if (action != NRTS_NOP) action = NRTS_CREATE;
    }
    if (yngest == chn->hdr.lend) {
        if ((oldest = chn->hdr.oldest + 1) == chn->hdr.nrec) oldest = 0;
        if ((lend   = chn->hdr.lend   + 1) == chn->hdr.nrec) lend   = 0;
        if ((status = nrts_bwd(hd, NRTS_REDUCE)) != 0) {
            util_log(1, "nrts_bwd failed: status = %d", status);
            if (terminate) nrts_die(exitstatus); else return -3;
        }
    } else {
        oldest = chn->hdr.oldest;
        lend   = chn->hdr.lend;
    }

    chn->hdr.backup.oldest = oldest;
    chn->hdr.backup.yngest = yngest;
    chn->hdr.backup.lend   = lend;

    hoff = chn->hdr.off + (yngest * chn->hdr.len);
    doff = chn->dat.off + (yngest * chn->dat.len);
    hd->wfdisc.foff = doff;

    if (lseek(fd.hdr, hoff, SEEK_SET) != hoff) {
        util_log(1, "%s: lseek(fd.hdr, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno)
        );
        if (terminate) nrts_die(exitstatus); else return -4;
    }

    if (lseek(fd.dat, doff, SEEK_SET) != doff) {
        util_log(1, "%s: lseek(fd.dat, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno)
        );
        if (terminate) nrts_die(exitstatus); else return -5;
    }

    chn->status = NRTS_BUSY1;

    if (write(fd.hdr, buf, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "%s: write(fd.hdr, buf, %d): %s",
                 fid, chn->hdr.len, syserrmsg(errno)
        );
        if (terminate) nrts_die(exitstatus); else return -6;
    }

    if (write(fd.dat, buf + chn->hdr.len, chn->dat.len) != chn->dat.len) {
        util_log(1, "%s: write(fd.dat, buf, %d): %s",
                 fid, chn->dat.len, syserrmsg(errno)
        );
        if (terminate) nrts_die(exitstatus); else return -7;
    }
    
/*  Update the pointers  */

    chn->status = NRTS_BUSY2;

    chn->hdr.oldest = oldest;
    chn->hdr.yngest = yngest;
    chn->hdr.lend   = lend;

    chn->status = NRTS_IDLE;

/*  Update the counters  */

    ++chn->nread;
    ++chn->count;
    chn->tread = time(NULL);

    ++sys->nread;
    ++sys->count;

/*  Update the wfdiscs  */

    if ((status = nrts_bwd(hd, action)) != 0) {
        util_log(1, "nrts_bwd failed: status = %d", status);
    }

    locked = 0;

    if (terminate) nrts_die(exitstatus);

    if (hdptr != NULL) *hdptr = hd;

/* Since ASL data are variable length in time, we have no easy way to
 * determine the oldest data around.  This will do it.
 */

    if (sys->type == NRTS_ASL) {
        chn->end = hd->wfdisc.endtime;
        hoff = chn->hdr.off + oldest;
        if (lseek(fd.hdr, hoff, SEEK_SET) != hoff) {
            util_log(1, "%s: lseek(fd.hdr, %ld, SEEK_SET): %s",
                     fid, off, syserrmsg(errno)
            );
        } else if (read(fd.hdr, mybuf, sys->reclen) == sys->reclen) {
            if ((hd = (*decode)(mybuf, sys->reclen, &decodeonly)) != NULL) {
                chn->beg = hd->wfdisc.time;
            }
        }
    }

    return 0;
}

int nrts_term(status)
int status;
{
    if (locked) {
        util_log(1, "disk loop is locked, setting terminate flag");
        terminate = 1;
        exitstatus = status;
    } else {
        nrts_die(exitstatus);
    }
    return status;
}

void nrts_closedl()
{
    util_log(1, "closing disk loop files");
    if (fd.hdr != -1) close(fd.hdr);
    if (fd.dat != -1) close(fd.dat);
    nrts_closebwd();
}

/* Revision History
 *
 * $Log: wrtdl.c,v $
 * Revision 1.8  2006/02/09 19:40:16  dechavez
 * IDA handle support
 *
 * Revision 1.7  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.6  2005/05/06 22:17:46  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.5  2004/07/26 22:46:40  dechavez
 * open files with O_BINARY option, remove unecessary includes
 *
 * Revision 1.4  2004/01/30 01:11:50  dechavez
 * refuse to write overlapping packets
 *
 * Revision 1.3  2002/04/29 17:31:17  dec
 * pass header fd to nrts_inidecode
 *
 * Revision 1.2  2001/05/20 16:14:19  dec
 * changed all instances of die() to nrts_die()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:35  dec
 * import existing IDA/NRTS sources
 *
 */
