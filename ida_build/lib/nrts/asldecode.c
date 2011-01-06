#pragma ident "$Id: asldecode.c,v 1.6 2007/01/07 17:51:59 dechavez Exp $"
/*======================================================================
 *
 *  Decode an ASL miniSEED packet.  Only those channels that are defined
 *  in the database will be accepted.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"
#include "cssio.h"
#include "seed.h"
#include "isi.h"
#include "isi/db.h"
#include "ida.h"

#define ONE_HOUR ((long) 3600)

static struct nrts_sys *sys;
static int             flags;
static int             blksiz;
static char            msgbuf[1024];

#define write_enabled    (flags & NRTS_TODISK)

static struct nrts_header hd;

/* Lookup table to summarize all the channel specific parameters */

struct channel_info {
    int   index;
    long  count;
    char  *name;
    float calib;
    float calper;
    float hang;
    float vang;
    char instype[NRTS_INAMLEN+1];
};

static struct channel_info *channel_table;

/* Initialize all the above stuff */

int nrts_asldecodeinit(
    char *home, char *syscode, struct nrts_sys *sys_ptr, int new_flags,
    IDA *ida, struct nrts_asl *info, void *unused1, int unused2
){
ISI_INST inst;
ISI_STREAM_NAME name;
int i, j, ok, nmatch, nchan;
long now;
static int initialized = 0;
static char *fid = "nrts_asldecodeinit";

/* We can modify the flags many times */

    flags = new_flags;

/* But the rest of the stuff can be done only once */

    if (initialized) return 0;

    sys = sys_ptr;
    if (sys->nsta != 1) {
        util_log(1, "%s: ERROR: nsta = %d", fid, sys->nsta);
        util_log(1, "%s: ASL systems can have only one station", fid);
        return -1;
    }

    blksiz = info->blksiz;

/* Fill out the static parts of the packet header */

    hd.wfdisc = wfdisc_null;
    hd.standx = 0;
    sprintf(hd.wfdisc.dir, "%s/%s", home, syscode);
    sprintf(hd.wfdisc.dfile, "%s",  NRTS_DATNAME);
    sprintf(hd.wfdisc.sta, "%s", sys->sta[0].name);
    hd.hlen = 64;
    hd.dlen = blksiz - hd.hlen;

/* Save all current calib params for this station */

    nchan = sys->sta[0].nchn;

    channel_table = (struct channel_info *)
                    malloc(nchan * sizeof(struct channel_info));
    if (channel_table == (struct channel_info *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        util_log(1, "%s: tried to allocate %d bytes",
            fid, (nchan * sizeof(struct channel_info))
        );
        return -3;
    }

    now    = time(NULL);
    nmatch = 0;
    for (i = 0; i < nchan; i++) {
        channel_table[i].count = 0;
        channel_table[i].name  = sys->sta[0].chn[i].name;
        channel_table[i].index = nrts_chnndx(sys->sta, channel_table[i].name);
        if (channel_table[i].index >= 0) {
            isiStaChnLocToStreamName(hd.wfdisc.sta, channel_table[i].name, NULL, &name);
            isiLookupInst(ida->db, &name, now, &inst);
            channel_table[i].calib = inst.calib;
            channel_table[i].calper = inst.calper;
            channel_table[i].vang = inst.vang;
            channel_table[i].hang = inst.hang;
            memcpy(channel_table[i].instype, inst.type, NRTS_INAMLEN);
            channel_table[i].instype[NRTS_INAMLEN] = 0;
            util_log(1, "ASL channel %s mapped to chan index %d", channel_table[i].name, channel_table[i].index);
            ++nmatch;
        } else {
            channel_table[i].name    = (char *) NULL;
            channel_table[i].calib   = 0.0;
            channel_table[i].calper  = -1.0;
            channel_table[i].hang    = -999.0;
            channel_table[i].vang    = -999.0;
            strlcpy(channel_table[i].instype, "-", NRTS_INAMLEN+1);
        }
    }
    if (nmatch < 1) {
        util_log(1, "no recognized ASL channels match configuration");
        return -5;
    }

/* Note any NRTS configured channels that don't have map entries */

    for (i = 0; i < sys->sta[0].nchn; i++) {
        for (ok = 0, j = 0; ok == 0 && j < nchan; j++) {
            if (channel_table[j].index == i) ok = 1;
        }
        if (ok == 0) {
            util_log(1, "warning: no ASL mapping for %s stream",
                sys->sta[0].chn[i].name
            );
        }
    }

/* Initialization complete */

    return 0;
}

struct nrts_header *nrts_asldecode(char *buffer, int len, int *action)
{
int i, j, status;
long error;
struct nrts_chn *chn;
struct channel_info *channel;
struct seed_minipacket dhead, *prev, *crnt;
static struct seed_minipacket prev_dhead[NRTS_MAXCHN];
static char *fid = "nrts_asldecode";

/* Decode the record header */

    if (buffer[6] != 'D') {
        util_log(2, "unexpected non-data record dropped");
        return (struct nrts_header *) NULL;
    }

/* Must be a data record at this point */

    crnt = &dhead;

/* Decode and determine if desired stream. */

    sprintf(msgbuf, "corrupt data record dropped: ");

    if ((status = seed_minihdr(crnt, buffer)) < 0) {
        sprintf(msgbuf+strlen(msgbuf), "seed_minihdr status %d",
            status
        );
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;

    }
#ifdef DEBUG
    util_log(1, "%s/%s/%s %s %.3lf %4ld",
        crnt->sname, crnt->cname, crnt->nname,
        util_dttostr(crnt->beg, 0), crnt->sint, crnt->nsamp
    );
#endif


/* Find its position in the local lookup table */

    for (j = -1, i = 0; j == -1 && i < sys->sta[0].nchn; i++) {
        if (strcasecmp(crnt->cname, channel_table[i].name) == 0) j = i;
    }

    if (j == -1) {
        util_log(2, "unexpected %s record dropped", crnt->cname);
        return (struct nrts_header *) NULL;
    }

    channel = channel_table + j;

/*  Fill in the output header for this packet */

    sprintf(hd.wfdisc.chan, "%s", crnt->cname);
    switch (crnt->b1000.format) {
      case SEED_INT_32:
        hd.order  = LTL_ENDIAN_ORDER;
        hd.wrdsiz = 4;
        sprintf(hd.wfdisc.datatype, "s4");
        break;
      case SEED_INT_16:
        hd.order  = LTL_ENDIAN_ORDER;
        hd.wrdsiz = 2;
        sprintf(hd.wfdisc.datatype, "s2");
        break;
      default:
        hd.order  = 0;
        hd.wrdsiz = 0;
        hd.wfdisc.datatype[0] = 0;
        break;
    }
    hd.wfdisc.nsamp   = crnt->nsamp;
    hd.wfdisc.time    = crnt->beg;
    hd.wfdisc.endtime = crnt->beg + ((crnt->nsamp - 1) * crnt->sint);
    hd.wfdisc.smprate = (float) (1.0 / crnt->sint);
    hd.wfdisc.calib   = channel->calib;
    hd.wfdisc.calper  = channel->calper;
    strlcpy(hd.wfdisc.instype, channel->instype, NRTS_INAMLEN+1);
    hd.hang  = channel->hang;
    hd.vang  = channel->vang;

    hd.beg.code = hd.end.code = ' ';
    hd.beg.qual = hd.end.qual = 0;

/*  If no further action is specified, then we are done */

    if (*action == NRTS_DECODE) {
        return &hd;
    }

/*  Locate this channel in the NRTS system  */

    if ((hd.chnndx = channel->index) < 0) {
        util_log(2, "unexpected %s record dropped", crnt->cname);
        return (struct nrts_header *) NULL;
    }

    chn = sys->sta[0].chn + hd.chnndx;
#ifdef DEBUG
    util_log(1, "%d: %d %d %s %15.4f %15.4f %8.3f %8.3f %s",
        j, channel->index, hd.chnndx, chn->name, channel->calib,
        channel->calper, channel->vang, channel->hang, channel->instype
    );
#endif

/*  Look for and reject records with duplicate headers or times */

    prev = prev_dhead + hd.chnndx;

    if (write_enabled  && channel->count > 0) {
        if (memcmp(prev, crnt, sizeof(IDA_DHDR)) == 0) {
            util_log(1, "duplicate %s data header, packet dropped", chn->name);
            return (struct nrts_header *) NULL;
        }
    }

/* If filling disk loop, save the sample interval */

    if (write_enabled && channel->count == 0) chn->sint = (float) crnt->sint;

/*  Count time tears  */

    if (channel->count > 0) {
        if ((status = seed_timetear(prev, crnt, &error)) != 0) {
            util_log(1,"%s: seed_timetear failed with status %d: ignored",
                        fid, status
            );
        } else if (error > 1) {
            util_log(1, "%d sample %s time tear", error, chn->name);
        } else if (error < -1) {
            util_log(1, "%d sample %s time tear, packet dropped",
                error, chn->name
            );
            return (struct nrts_header *) NULL;
        }
    }

/*  Save the decoded header for later comparisons  */

    *prev = *crnt;
    ++channel->count;

/*  For ASL we don't attempt wfdisc maintenance */

    *action = NRTS_NOP;

    return &hd;
}

/* Revision History
 *
 * $Log: asldecode.c,v $
 * Revision 1.6  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.5  2006/02/09 19:49:35  dechavez
 * IDA handle and libisidb support (but dropped ASL support elsewhere)
 *
 * Revision 1.4  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2004/06/24 17:32:44  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.2  2002/04/29 17:33:28  dec
 * add new (and unused here) hdr fd to init routine
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
