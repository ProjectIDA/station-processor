#pragma ident "$Id: read.c,v 1.7 2008/09/11 16:32:54 dechavez Exp $"
#include <stdio.h>
#include <errno.h>
#include "nrts.h"
#include "nrts_alpha.h"

static int short_chans = 0;
static char *buffer;
static int hfd = -1;
static int dfd = -1;
static struct nrts_sys *sys;
static struct nrts_header *(*decode)();
static long nxt_index[NRTS_MAXCHN];
static char *sname = NULL;
static char recase = RECASE_NONE;

int read_init(home, syscode, sysptr, file, scflag, ida, force_sname, rcflag)
char *home;
char *syscode;
struct nrts_sys *sysptr;
struct nrts_files *file;
int scflag;
IDA *ida;
char *force_sname;
int rcflag;
{
int i;
static char *fid = "read_init";

    sys = sysptr;
    if (short_chans = scflag) {
        util_log(1, "will use 2-char channel names");
    } else {
        util_log(1, "will use 3-char channel names");
    }

    recase = rcflag;
    if (recase == RECASE_UPPER) {
        util_log(1, "channel names will be force to all upper case");
    } else if (recase == RECASE_LOWER) {
        util_log(1, "channel names will be force to all lower case");
    }

    if (force_sname != NULL) {
        if (strlen(force_sname) > NRTS_SNAMLEN) {
            util_log(1, "%s: force sname `%s' too long (%d > %d)",
                fid, force_sname, strlen(force_sname), NRTS_SNAMLEN
            );
            die(1);
        }
        if ((sname = strdup(force_sname)) == (char *) NULL) {
            util_log(1, "%s: strdup: %s: %s",
                fid, force_sname, syserrmsg(errno)
            );
            die(1);
        }
    }

    if (sname != NULL) util_log(1, "station name forced to `%s'", sname);

/* Open the disk loop files */

    if ((hfd = open(file->hdr, O_RDONLY)) < 0) {
        util_log(1, "%s: %s: %s", fid, file->hdr, syserrmsg(errno));
        util_log(1, "%s: can't open hdr disk buffer", fid);
        die(1);
    }

    if ((dfd = open(file->dat, O_RDONLY)) < 0) {
        util_log(1, "%s: %s: %s", fid, file->dat, syserrmsg(errno));
        util_log(1, "%s: can't open dat disk buffer", fid);
        die(1);
    }

/*  Initialize the read buffer  */

    if ((buffer = (char *) malloc(IDA_BUFSIZ)) == NULL) {
        util_log(1, "%s: can't malloc read buffer");
        die(1);
    }

/*  Initialize type specific header decoder  */

    decode = (struct nrts_header *(*)())
             nrts_inidecode(home, syscode, sys, 0, ida, (void *) 0, hfd);
    if (decode == NULL) {
        util_log(1, "unable to determine decode function");
        die(1);
    }

/*  Set read inidices so that next read will be youngest record in disk loop */

    for (i = 0; i < sys->sta[0].nchn; i++) {
        nxt_index[i] = sys->sta[0].chn[i].hdr.yngest;
    }

    return 0;
}

struct nrts_packet *index_read(chan_id, index)
int chan_id;
long index;
{
int dummy;
off_t off;
struct nrts_chn *chn;
struct nrts_header *hd;
static struct nrts_packet packet;
static char *fid = "index_read";

    if (index < 0) index = nxt_index[chan_id];

    chn = sys->sta[0].chn + chan_id;
    util_log(6, "%s: reading channel `%s' index %d", fid, chn->name, index);

    if (index > chn->hdr.nrec) {
        util_log(1, "%s: illegal index `%ld' given for channel id `%d'", 
            fid, index, chan_id
        );
        die(1);
    }

/* Read the header part */

    off = chn->hdr.off + (index * chn->hdr.len);

    if (lseek(hfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(hfd, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        die(1);
    }

    if (read(hfd, buffer, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "%s: read(hfd, buf, %d): %s",
                 fid, chn->hdr.len, syserrmsg(errno)
        );
        util_log(1, "%s: chn = %s, index = %ld, hdr off =  %ld",
            fid, chn->name, index, off
        );
        die(1);
    }

/* Read the data part */

    off = chn->dat.off + (index * chn->dat.len);

    if (lseek(dfd, off, SEEK_SET) != off) {
        util_log(1, "%s: lseek(dfd, %ld, SEEK_SET): %s",
                 fid, off, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, dat off =  %ld",
            fid, chn->name, index, off
        );
        die(1);
    }

    if (read(dfd, buffer+chn->hdr.len, chn->dat.len) != chn->dat.len) {
        util_log(1, "%s: read(dfd, buf+chn->hdr.len, %d): %s",
                 fid, chn->dat.len, syserrmsg(errno));
        util_log(1, "%s: chn = %s, index = %ld, dat off =  %ld",
            fid, chn->name, index, off
        );
        die(1);
    }

/* Increment index for next read */

    if (index != chn->hdr.yngest) {
        nxt_index[chan_id] = index + 1;
        if (nxt_index[chan_id] == chn->hdr.nrec) nxt_index[chan_id] = 0;
    }

/* Decode the header */
 
    if ((hd = (*decode)(buffer, chn->hdr.len, &dummy)) == NULL) {
        util_log(1, "abort: record at index %ld is corrupt", index);
        die(1);
    }

    if (sname == NULL) {
        strcpy(packet.sname, hd->wfdisc.sta);
    } else {
        strcpy(packet.sname, sname);
    }

    if (short_chans && strlen(hd->wfdisc.chan) >= 3) {
        sprintf(packet.cname, "%c%c", hd->wfdisc.chan[0], hd->wfdisc.chan[2]);
    } else {
        strncpy(packet.cname, hd->wfdisc.chan, 3);
        packet.cname[3] = 0;
    }
    if (recase == RECASE_UPPER) util_ucase(packet.cname);
    if (recase == RECASE_LOWER) util_lcase(packet.cname);

    packet.beg.time = hd->wfdisc.time;
    packet.beg.code = hd->beg.code;
    packet.beg.qual = hd->beg.qual;
    packet.end.time = hd->wfdisc.endtime;
    packet.end.code = hd->end.code;
    packet.end.qual = hd->end.qual;
    packet.tear     = hd->tear;
    packet.sint     = 1.0/hd->wfdisc.smprate;
    packet.nsamp    = hd->wfdisc.nsamp;
    packet.wrdsiz   = hd->wrdsiz;
    packet.order    = hd->order;
    packet.type     = sys->type;
    packet.hlen     = hd->hlen;
    packet.dlen     = hd->dlen;
    packet.header   = buffer;
    packet.data     = buffer + packet.hlen;

    return &packet;
}

int getrec(chan_id, time, output)
int chan_id;
double time;
struct nrts_packet **output;
{
double earliest, latest;
struct nrts_chn *chn;
long oldest, yngest;
struct nrts_packet *packet;
static char *fid = "getrec";

/*  First see if the next record is the one we want  */

    packet = index_read(chan_id, nxt_index[chan_id]);

    if (time >= packet->beg.time && time <= packet->end.time) {
        util_log(4, "%s:%s record has expected time %s, no search required", 
            packet->sname, packet->cname, util_dttostr(time, 0)
        );
        *output = packet;
        return PRESENT;
    }

/*  Search the disk buffer */

    util_log(4, "searching native disk loop for %s:%s %s record",
        packet->sname, packet->cname, util_dttostr(time, 0)
    );
    chn = sys->sta[0].chn + chan_id;

    oldest = chn->hdr.oldest;
    yngest = chn->hdr.yngest;

/* First, insure the desired time falls inside the disk buffer */

    packet = index_read(chan_id, oldest);
    if (time > packet->beg.time - packet->sint && time <= packet->end.time) {
        *output = packet;
        return PRESENT;
    }

    earliest = packet->beg.time - packet->sint;
    if (time < earliest) {
        util_log(1, "%s:%s %s data are no longer present",
            packet->sname, packet->cname, util_dttostr(time, 0)
        );
        util_log(4, "%s: earliest = %s", fid, util_dttostr(earliest, 0));
        *output = NULL;
        return ABSENT;
    }

    packet = index_read(chan_id, yngest);
    if (time > packet->beg.time - packet->sint && time <= packet->end.time) {
        *output = packet;
        return PRESENT;
    }

    latest = packet->end.time;

    if (time > latest) {
        util_log(4, "%s:%s %s data have yet to be acquired",
            packet->sname, packet->cname, util_dttostr(time, 0)
        );
        util_log(4, "%s: latest = %s", fid, util_dttostr(latest, 0));
        *output = NULL;
        return UNKNOWN;
    }

/* Desired time inside disk loop, search for it */
/* First case, no wraparound at end of disk buffer */

    if (oldest < yngest) {
        return do_search(chan_id, time, oldest, yngest, output);
    }

/* Second case, data wrap around end of disk buffer */

    packet = index_read(chan_id, (long) 0);
    if (time > packet->beg.time - packet->sint && time <= packet->end.time) {
        *output = packet;
        return PRESENT;
    }

    if (time > packet->beg.time - packet->sint && time <= latest) {
        return do_search(chan_id, time, 0, yngest, output);
    } else {
        packet = index_read(chan_id, (long) chn->hdr.nrec - 1);
        if (time > packet->beg.time - packet->sint && time <= packet->end.time) {
            *output = packet;
            return PRESENT;
        }
        if (time >= earliest && time <= packet->end.time) {
            return do_search(chan_id, time, oldest, chn->hdr.nrec-1, output);
        }
    }

    util_log(1, "%s: FATAL PROGRAM LOGIC ERROR!", fid);
    util_log(1, "%s: time   = %s", fid, util_dttostr(time,   0));
    util_log(1, "%s: earliest = %s", fid, util_dttostr(earliest, 0));
    util_log(1, "%s: latest   = %s", fid, util_dttostr(latest,   0));
    die(1);
}

int do_search(chan_id, target, left, rite, output)
int chan_id;
double target;
long left;
long rite;
struct nrts_packet **output;
{
long test;
struct nrts_packet *packet;
static char *fid = "do_search";
char tmpbuf[1024];

    while (rite - left > 1) {
        test = left + ((rite - left)/2);
        packet = index_read(chan_id, test);
        sprintf(tmpbuf, "do_search: %d: beg=%s ", test, util_dttostr(packet->beg.time, 0));
        sprintf(tmpbuf+strlen(tmpbuf), "target=%s ", util_dttostr(target, 0));
        sprintf(tmpbuf+strlen(tmpbuf), "end=%s", util_dttostr(packet->end.time, 0));
        util_log(5, tmpbuf);
        if (target > packet->beg.time - packet->sint && target <= packet->end.time) {
            *output = packet;
            util_log(4, "data found at index %ld", test);
            return PRESENT;
        }

        if (target > packet->end.time) {
            left = test;
        } else {
            rite = test;
        }
    }

    util_log(3, "%s:%s %s data are missing from disk buffer",
        packet->sname, packet->cname, util_dttostr(target, 0)
    );
    *output = NULL;
    return ABSENT;
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.7  2008/09/11 16:32:54  dechavez
 * added debug messages to do_search
 *
 * Revision 1.6  2006/02/10 02:04:50  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.5  2005/09/13 00:44:47  dechavez
 * Use IDA_BUFSIZ to allocate read buffer instead of (no longer existant) sys raw len
 *
 * Revision 1.4  2003/03/13 23:19:20  dechavez
 * 2.11.0 (5 char mapped to 3 char)
 *
 * Revision 1.3  2002/02/11 17:34:02  dec
 * fixed error in mapping to 2-char names from large (>3 char) names
 *
 * Revision 1.2  2001/01/11 17:39:52  dec
 * Add sname= option to allow the user to force the output station name.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
