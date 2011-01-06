#pragma ident "$Id: fixwd.c,v 1.9 2006/06/19 18:06:33 dechavez Exp $"
/*======================================================================
 *
 *  Recreate binary wfdiscs via recursive binary search.
 *
 *====================================================================*/
#include "nrts.h"
#include "cssio.h"
#include "util.h"

#ifndef O_SYNC
#define O_SYNC 0
#endif

static struct gap_list {
    long index;
    struct gap_list *next;
} list_head = {-1, (struct gap_list *) NULL};

#ifdef _WIN32
static HANDLE mfd = NULL;
#else
static int mfd = -1;                    /* map    file descriptor */
#endif
static int wfd = -1;                    /* wfdisc file descriptor */
static int hfd = -1;                    /* header file descriptor */
static char *buf = (char *) NULL;       /* current header         */
static struct nrts_sys *sys = NULL;     /* current system         */
static struct nrts_sta *sta = NULL;     /* current station        */
static struct nrts_chn *chn = NULL;     /* current channel        */
static struct nrts_header *(*decode)(); /* decode function        */

static void free_list()
{
struct gap_list *crnt, *next;
static struct gap_list *null = (struct gap_list *) NULL;

    crnt = list_head.next;

    while (crnt != null) {
        next = crnt->next;
        free(crnt);
        crnt = next;
    }

    list_head.index = -1;
    list_head.next  = (struct gap_list *) NULL;
}

static int clean_return(int status)
{
    if (mfd > 0) util_unlock(mfd, 0, SEEK_SET, 0);
    if (wfd > 0) {
        util_unlock(wfd, 0, SEEK_SET, 0);
        close(wfd);
    }
    if (hfd > 0) {
        util_unlock(hfd, 0, SEEK_SET, 0);
        close(hfd);
    }
    if (buf != (char *) NULL) free(buf);
    free_list();
    return status;
}

static int store_gap(long index)
{
static struct gap_list *null = (struct gap_list *) NULL;
struct gap_list *crnt, *next, *new;
static char *fid = "nrts_fixwd:store_gap";

    util_log(2, "%s: gap found after index %ld", fid, index);

    crnt = &list_head;
    next = crnt->next;

    while (next != null) {
        if (next->index < 0) {
            next->index = index;
            return 0;
        }
        crnt = crnt->next;
        next = next->next;
    }

    new = (struct gap_list *) malloc(sizeof(struct gap_list));
    if (new == null) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return -1;
    }

    new->index = index;
    new->next  = null;

    crnt->next = new;
    
    return 0;
}

static void clear_list()
{
struct gap_list *crnt;
static struct gap_list *null = (struct gap_list *) NULL;

    crnt = list_head.next;

    while (crnt != null) {
        crnt->index = -1;
        crnt = crnt->next;
    }
}

static int read_record(
long recno,
struct nrts_header **hd
) {
off_t off;
int unused;
struct nrts_header *hdr_ptr;
static char *fid = "nrts_fixwd:read_record";

    off = chn->hdr.off + (recno * chn->hdr.len);
    if (lseek(hfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek: %s", fid, syserrmsg(errno));
        return -1;
    }

    if (read(hfd, buf, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "%s: read: %s", fid, syserrmsg(errno));
        return -2;
    }
    hdr_ptr = (*decode)(buf, chn->hdr.len, &unused);
    if (hdr_ptr != (struct nrts_header *) NULL) {
        hdr_ptr->wfdisc.foff = chn->dat.off + (recno * chn->dat.len);
    } else {
        util_log(1, "%s:%s dl rec %d is unreadable",
            sta->name, chn->name, recno
        );
        util_log(1, "NRTS DISK LOOP IS CORRUPT!");
        return -3;
    }

    *hd = hdr_ptr;

    return 0;
}

static int sanity_check(
struct nrts_header *left,
struct nrts_header *rite
){
static char *fid = "nrts_fixwd:sanity_check";

    if (left->standx != rite->standx) {
        util_log(1, "%s: standx mismatch: %d != %d",
            fid, left->standx, rite->standx
        );
        return -1;
    }

    if (left->chnndx != rite->chnndx) {
        util_log(1, "%s: chnndx mismatch: %d != %d",
            fid, left->chnndx, rite->chnndx
        );
        return -2;
    }

    if (left->wrdsiz != rite->wrdsiz) {
        util_log(1, "%s: wrdsiz mismatch: %d != %d",
            fid, left->wrdsiz, rite->wrdsiz
        );
        return -3;
    }

    if (left->wfdisc.smprate != rite->wfdisc.smprate) {
        util_log(1, "%s: wfdisc.smprate mismatch: %.3f != %.3f",
            fid, left->wfdisc.smprate, rite->wfdisc.smprate
        );
        return -4;
    }

    if (left->wfdisc.foff >= rite->wfdisc.foff) {
        util_log(1, "%s: wfdisc.foff error: %d >= %d",
            fid, left->wfdisc.foff, rite->wfdisc.foff
        );
        return -5;
    }

    return 0;
}

static long gap_test(struct nrts_header *left, struct nrts_header *rite)
{
struct {
    double time;
    long   samp;
    long   byte;
} span;
long gap;

    span.time = rite->wfdisc.time - left->wfdisc.time;
    span.samp = (long) (span.time * left->wfdisc.smprate);
    span.byte = span.samp * left->wrdsiz;

    gap = span.byte - (rite->wfdisc.foff - left->wfdisc.foff);

    if (gap == rite->wrdsiz || gap == -rite->wrdsiz) gap = 0;

    return gap;
}

static int process(long a, long b)
{
long midpoint, gap;
int status;
static int level = 0;
struct nrts_header left, rite, *ptr;
static char *fid = "nrts_fixwd:process";

    if (a == b) return 0;

    ++level;

    util_log(2, "%s(%d): comparing %s:%s from %ld thru %ld",
        fid, level, sta->name, chn->name, a, b
    );

    if (read_record(a, &ptr) != 0) return -1;
    left = *ptr;

    if (read_record(b, &ptr) != 0) return -2;
    rite = *ptr;

    if (sanity_check(&left, &rite) != 0) return -3;

    if (gap = gap_test(&left, &rite)) {
        util_log(3,"%s(%d): gap of %d bytes between %d and %d",
            fid, level, gap, a, b
        );
        if (b == (a + 1)) {
            util_log(3,"%s(%d): gap declared after %d... branch is over",
                fid, level, a
            );
            status = store_gap(a);
        } else {
            midpoint = a + (b - a) / 2;
            if ((status = process(a, midpoint)) == 0) {
                status = process(midpoint, b);
            }
        }
    } else {
        util_log(3,"%s(%d): no gap between %d and %d... branch is over",
            fid, level, a, b
        );
        status = 0;
    }

    util_log(3, "%s(%d): return %d", fid, level, status);
    --level;
    return status;
}

static int update(long a, long b)
{
int done, yr, da, hr, mn, sc, ms;
long begrec, endrec;
struct gap_list *crnt;
struct nrts_header beg, end, *ptr;
static int len = sizeof(struct wfdisc);
static struct gap_list *null = (struct gap_list *) NULL;
static char *fid = "nrts_fixwd:update";

    util_log(3, "%s: begin update(%d, %d)", fid, a, b);
    crnt = list_head.next;

    begrec = a;
    if (crnt != null && crnt->index != -1) {
        endrec = crnt->index;
        crnt   = crnt->next;
    } else {
        endrec = b;
    }

    done = 0;
    do {
        util_log(3, "%s: generate record to cover %d thru %d",
            fid, begrec, endrec
        );
        if (read_record(begrec, &ptr) != 0) return -1;
        beg = *ptr;

        if (read_record(endrec, &ptr) != 0) return -2;
        end = *ptr;

        beg.wfdisc.endtime = end.wfdisc.endtime;
        beg.wfdisc.nsamp   = end.wfdisc.nsamp +
            (end.wfdisc.foff - beg.wfdisc.foff) / beg.wrdsiz;

        util_tsplit(beg.wfdisc.time, &yr, &da, &hr, &mn, &sc, &ms);
        beg.wfdisc.jdate = (yr * 1000) + da;
        if (write(wfd, &beg.wfdisc, len) != len) {
            util_log(1, "%s: write: %s", fid, syserrmsg(errno));
            return -3;
        }

        if (endrec != b) {
            begrec = endrec + 1;
            if (crnt != null && crnt->index != -1) {
                endrec = crnt->index;
                crnt   = crnt->next;
            } else {
                endrec = b;
            }
        } else {
            done = 1;
        }
    } while (!done);

    clear_list();    

    return 0;
}

int nrts_fixwd(struct nrts_files *file, struct nrts_mmap *mmap, char *home, char *syscode, IDA *ida)
{
int i, j, status;
long a, b;
int debug = 0;
size_t buflen;
char *log  = NULL;
int have_data = 0;
static char *fid = "nrts_fixwd";

    sys = (struct nrts_sys *) mmap->ptr;

    if (sys->type == NRTS_ASL) {
        sys->bwd = NRTS_STABLE;
        return clean_return(0);
    }

    mfd = mmap->fd;
    if (util_wlockw(mfd, 0, SEEK_SET, 0) == -1) {
        util_log(1, "%s: util_wlockw: %s", fid, syserrmsg(errno));
        return clean_return(-1);
    }

    sys->bwd = NRTS_BUSY1;

/*  Initialize output file */

    if ((wfd = open(file->bwd, O_RDWR | O_SYNC | O_BINARY)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->bwd, syserrmsg(errno));
        return clean_return(-2);
    }

    if (ftruncate(wfd, 0) != 0) {
        util_log(1, "%s: ftruncate: %s: %s",
            fid, file->bwd, syserrmsg(errno)
        );
        return clean_return(-3);
    }
    sys->bwd = NRTS_CORRUPT;

    if (util_wlockw(wfd, 0, SEEK_SET, 0) == -1) {
        util_log(1, "%s: util_wlockw: %s", fid, syserrmsg(errno));
        return clean_return(-3);
    }

/* Get buffer for header reads */

    buflen = sys->sta[0].chn[0].hdr.len;
    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            if (sys->sta[i].chn[j].count > 0) have_data = 1;
            if (sys->sta[i].chn[j].hdr.len > (int) buflen) {
                buflen = sys->sta[i].chn[j].hdr.len;
            }
        }
    }

    if (!have_data) {
        sys->bwd = NRTS_STABLE;
        return clean_return(0);
    }

    if ((buf = (char *) malloc(buflen)) == NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return clean_return(-4);
    }

/*  Recreate the wfdisc list by reading from disk  */

    if ((hfd = open(file->hdr, O_RDONLY | O_BINARY)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->bwd, syserrmsg(errno));
        return clean_return(-5);
    }

    util_log(2, "re-creating wfdisc list from raw data");
    decode = (struct nrts_header *(*)())
             nrts_inidecode(home, syscode, sys, 0, ida, (void *) 0, hfd);
    if (decode == NULL) {
        util_log(1, "%s: nrts_inidecode failed", fid);
        return clean_return(-6);
    }
    for (i = 0; i < sys->nsta; i++) {
        sta = sys->sta + i;
        for (j = 0; j < sys->sta[0].nchn; j++) {
            chn = sta->chn +j;
            if (chn->hdr.yngest >= 0) {
                if (chn->hdr.yngest < chn->hdr.oldest) {
                    a = chn->hdr.oldest;
                    b = chn->hdr.nrec - 1;
                    status = process(a, b);
                    if (status != 0) {
                        util_log(1, "%s: process: status %d", fid, status);
                        return clean_return(-7);
                    }
                    status = update(a, b);
                    if (status != 0) {
                        util_log(1, "%s: update: status %d", fid, status);
                        return clean_return(-8);
                    }
                    a = 0;
                    b = chn->hdr.yngest;
                } else {
                    a = chn->hdr.oldest;;
                    b = chn->hdr.yngest;
                }
                status = process(a, b);
                if (status != 0) {
                    util_log(1, "%s: process: status %d", fid, status);
                    return clean_return(-9);
                }
                status = update(a, b);
                if (status != 0) {
                    util_log(1, "%s: update: status %d", fid, status);
                    return clean_return(-10);
                }
            }
        }
    }

    sys->bwd = NRTS_STABLE;
 
    return clean_return(0);
}

/* Revision History
 *
 * $Log: fixwd.c,v $
 * Revision 1.9  2006/06/19 18:06:33  dechavez
 * fix win32 initialization error (aap)
 *
 * Revision 1.8  2006/02/09 19:46:04  dechavez
 * IDA handle support
 *
 * Revision 1.7  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.6  2004/07/26 22:53:33  dechavez
 * open files with O_BINARY attribute
 *
 * Revision 1.5  2004/06/24 17:39:01  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.4  2003/12/10 06:02:38  dechavez
 * improved function declarations to calm solaris cc
 *
 * Revision 1.3  2002/04/29 17:32:31  dec
 * pass header fd to nrts_inidecode
 *
 * Revision 1.2  2000/06/24 23:42:24  dec
 * fixed pointer problem with linked list
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
