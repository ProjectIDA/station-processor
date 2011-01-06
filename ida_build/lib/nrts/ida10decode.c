#pragma ident "$Id: ida10decode.c,v 1.16 2008/09/11 16:30:43 dechavez Exp $"
/*======================================================================
 *
 *  Decode an IDA version 10 format packet
 *
 *====================================================================*/
#include "nrts.h"
#include "ida10.h"
#include "util.h"
#include "cssio.h"

static struct nrts_sys *sys;
static int             flags;
static unsigned long   errtol;
static char            msgbuf[1024];
static BOOL *first;
static IDA10_TSHDR *prev_dhead;

#define write_enabled    (flags & NRTS_TODISK)
#define from_nrts        (flags & NRTS_FROMNRTS)
#define strict_checks    (flags & NRTS_STRICT)
#define station_system  ((flags & NRTS_FROMTTY) || (flags & NRTS_FROMISP))

static struct nrts_header hd;

#define MAXHDRLEN 1024

static BOOL IsVirgin(IDA10_TSHDR *dhead, struct nrts_sys *sys, int fdhdr, int chnndx)
{
off_t offset;
struct nrts_chn *chn;
char buf[MAXHDRLEN];
static char *fid = "nrts_ida10decodeinit:IsVirgin";

    chn = &sys->sta[0].chn[chnndx];

    if (chn->count == 0) return TRUE;

    if (chn->hdr.len > MAXHDRLEN) {
        util_log(1, "ABORT: MAXHDRLEN in %s is TOO SMALL!", fid);
        exit(1);
    }
    offset = chn->hdr.off + (chn->hdr.yngest * chn->hdr.len);

    if (lseek(fdhdr, offset, SEEK_SET) != offset) {
        util_log(1, "ABORT: lseek in %s FAILED!", fid);
        exit(1);
    }
    if (read(fdhdr, buf, chn->hdr.len) != chn->hdr.len) {
        util_log(1, "ABORT: read in %s FAILED!", fid);
        exit(1);
    }
    if (ida10Type((UINT8 *)buf) != IDA10_TYPE_TS) {
        util_log(1, "ABORT: %s: WRONG TYPE: expect %d, got %d, chn=%s", fid, IDA10_TYPE_TS, ida10Type((UINT8 *)buf), chn->name);
        exit(1);
    }

    if (ida10UnpackTSHdr((UINT8 *)buf, dhead) == 0) {
        util_log(1, "ABORT: ida10UnpackTSHdr in %s FAILED!", fid);
        exit(1);
    }

    return FALSE;
}
    
/* Initialization */

int nrts_ida10decodeinit(
    char *home, char *syscode, struct nrts_sys *sys_ptr, 
    int new_flags, IDA *unused1, struct nrts_ida *unused2, void *opt,
    int fdhdr
){
int i;
static int initialized = FALSE;
static char *fid = "nrts_ida10decodeinit";

/* We can modify the flag and tic tolerance many times */

    flags = new_flags;

    if (opt != (void *) 0) {
        errtol = *((unsigned long *) opt);
        util_log(1, "time tears of %ld tics or less are tolerated",
            (long) errtol
        );
    } else {
        errtol = 0;
    }

/* But the rest of the stuff can be done only once */

    if (initialized) return 0;

    sys = sys_ptr;
    if (sys->nsta != 1) {
        util_log(1, "%s: ERROR: nsta = %d", fid, sys->nsta);
        util_log(1, "%s: IDA systems can have only one station", fid);
        return -1;
    }

    first = (BOOL *) malloc(sys->sta[0].nchn * sizeof(BOOL));
    if (first == (BOOL *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return -1;
    }

    prev_dhead = (IDA10_TSHDR *) malloc(sys->sta[0].nchn * sizeof(IDA10_TSHDR));
    if (prev_dhead == (IDA10_TSHDR *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return -1;
    }

    for (i = 0; i < sys->sta[0].nchn; i++)
        first[i] = IsVirgin(&prev_dhead[i], sys, fdhdr, i);

/* Fill out the static parts of the packet header */

    hd.wfdisc = wfdisc_null;
    hd.standx = 0;
    sprintf(hd.wfdisc.dir, "%s/%s", home, syscode);
    sprintf(hd.wfdisc.dfile, "%s",  NRTS_DATNAME);
    sprintf(hd.wfdisc.sta, "%s", sys->sta[0].name);
    sprintf(hd.wfdisc.datatype, "s4");
    hd.hlen = IDA10_TSHEADLEN;
    hd.dlen = sys->reclen - hd.hlen;

/* Initialization complete */

    initialized = TRUE;

    return 0;
}

static BOOL UnreliableTimeTag(IDA10_CLOCK_STATUS *status)
{
    if (!status->init) return TRUE;
    //if (status->suspect) return TRUE;
    return FALSE;
}

struct nrts_header *nrts_ida10decode(char *buffer, int len, int *action)
{
struct nrts_chn *chn;
REAL64 errsec, errsmp;
int i, status, sign;
UINT64 err;
IDA10_TSHDR dhead, *prev, *crnt;
static char tqual[2] = {'?', 'M'};
static short tcode[2] = {5, 1};
static char *fid = "nrts_ida10decode";

    if (ida10Type((UINT8 *)buffer) != IDA10_TYPE_TS) return NULL;

/* Decode the record header */

    memset(&dhead, 0, sizeof(IDA10_TSHDR));
    crnt = &dhead;
    if (ida10UnpackTSHdr((UINT8 *)buffer, crnt) == 0) {
        util_log(2, "corrupt data record dropped");
        return (struct nrts_header *) NULL;
    }

/* Determine if desired stream. */

    for (hd.chnndx = -1, i = 0; hd.chnndx < 0 && i < sys->sta[0].nchn; i++) {
        if (strcasecmp(sys->sta[0].chn[i].name, crnt->cname) == 0) hd.chnndx = i;
    }
    if (hd.chnndx == -1) {
        util_log(2, "unconfigured channel %s, packet dropped", crnt->cname);
        return (struct nrts_header *) NULL;
    }

    chn = sys->sta[0].chn + hd.chnndx;

/* Sanity checks on header contents */

    sprintf(msgbuf, "corrupt data record dropped: ");

    if (crnt->unused > 0) {
        util_log(1, "short %s packet dropped (%d unused bytes)", chn->name, crnt->unused);
        return (struct nrts_header *) NULL;
    } else if (crnt->unused < 0) {
        sprintf(msgbuf+strlen(msgbuf), "%s claims %d extra bytes?", chn->name, crnt->unused);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    }

    if (UnreliableTimeTag(&crnt->cmn.ttag.beg.status) || UnreliableTimeTag(&crnt->cmn.ttag.end.status)) {
        sprintf(msgbuf+strlen(msgbuf), "%s unreliable time stamp", chn->name);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    }

/*  Look for and reject records with duplicate or overlapping headers or times */

    prev = &prev_dhead[hd.chnndx];

    if (write_enabled && !first[hd.chnndx]) {
        if (memcmp(prev, crnt, sizeof(IDA10_TSHDR)) == 0) {
            util_log(1, "duplicate %s data header, packet dropped", chn->name);
            return (struct nrts_header *) NULL;
        }
        if (
            memcmp(&prev->cmn.ttag.beg, &crnt->cmn.ttag.beg, sizeof(IDA10_TTAG)) == 0 &&
            memcmp(&prev->cmn.ttag.end, &crnt->cmn.ttag.end, sizeof(IDA10_TTAG)) == 0
        ) {
            util_log(1, "duplicate %s start and end times, packet dropped",
                chn->name
            );
            return (struct nrts_header *) NULL;
        }
        if (crnt->tofs <= prev->tols) {
            sprintf(msgbuf, "%s <= ", util_dttostr(crnt->tofs, 0));
            sprintf(msgbuf+strlen(msgbuf), "%s", util_dttostr(prev->tols, 0));
            util_log(1, "dropped overlapping %s packet (%s)", chn->name, msgbuf);
            return (struct nrts_header *) NULL;
        }
    }

/*  Fill in the output header for this packet */

    sprintf(hd.wfdisc.chan, "%s", chn->name);
    hd.wrdsiz         = 4;
    hd.wfdisc.nsamp   = crnt->nsamp;
    hd.wfdisc.time    = crnt->tofs;
    hd.wfdisc.endtime = crnt->tols;
    hd.wfdisc.smprate = (float) crnt->srate;
    hd.order = BIG_ENDIAN_ORDER;

    hd.beg.code = (char) tcode[crnt->cmn.ttag.beg.status.locked ? 1 : 0];
    hd.beg.qual = tqual[crnt->cmn.ttag.beg.status.locked ? 1 : 0];

    hd.end.code = (char) tcode[crnt->cmn.ttag.end.status.locked ? 1 : 0];
    hd.end.qual = tqual[crnt->cmn.ttag.end.status.locked ? 1 : 0];

/* If filling disk loop, save the sample interval */

    if (write_enabled && first[hd.chnndx]) chn->sint = (float) crnt->sint;

/*  Look for time tears  */

    if (!first[hd.chnndx]) {

        status = ida10TtagIncrErr(prev, crnt, &sign, &err);
        if (status != 0) {
            util_log(1,"%s: ida10TtagIncrErr: error %d", fid, status);
            return (struct nrts_header *) NULL;
        } else {
            errsec = (REAL64) err / (REAL64) NANOSEC_PER_SEC;
            errsmp = errsec / (REAL64) crnt->sint;
        }
        if ((unsigned long) errsmp > errtol) {
            if (write_enabled) {
                util_log(1, "%c%.5f sec %s time tear (%.2lf samp)",
                    sign > 0 ? '+' : '-',errsec, chn->name,  errsmp
                );
            }
            if (sign < 0) errsec *= -1.0;
            hd.tear = errsec;
            *action = NRTS_CREATE;

        } else {

    /* No time tear detected */

            hd.tear = 0.0;
            *action = NRTS_EXTEND;
        }
    } else {
        hd.tear = 0.0;
        *action = NRTS_TESTWD;
    }

/*  Save the decoded header for later comparisons  */

    *prev = *crnt;
    first[hd.chnndx] = FALSE;

    return &hd;
}

/* Revision History
 *
 * $Log: ida10decode.c,v $
 * Revision 1.16  2008/09/11 16:30:43  dechavez
 * Ingore suspect bit in time status in UnreliableTimeTag() test.
 * This is to prevent apps that "dip" into the NRTS disk loop from
 * thinking there is a corrupt packet (specifically nrts_alpha).
 * I'm only doing this patently foolish thing to get nrts_alpha
 * for KDAK running again.
 *
 * Revision 1.15  2006/11/10 06:38:57  dechavez
 * generic time stamp/status
 *
 * Revision 1.14  2006/02/09 19:42:45  dechavez
 * IDA handle support
 *
 * Revision 1.13  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.12  2005/05/06 22:17:45  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.11  2004/09/28 22:41:15  dechavez
 * More detailed error message in IsVirgin()
 *
 * Revision 1.10  2004/06/24 17:39:01  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.9  2002/11/06 00:52:55  dechavez
 * clarified short packet message
 *
 * Revision 1.8  2002/11/05 17:48:03  dechavez
 * look for and drop packets where the "unused" header field is non-zero,
 * print time stamps in log message when dropping overlapping packets
 *
 * Revision 1.7  2002/09/09 21:48:26  dec
 * silently drop non-data packets
 *
 * Revision 1.6  2002/04/29 17:36:55  dec
 * read disk loop as part of initialization
 *
 * Revision 1.5  2002/03/15 22:46:07  dec
 * changed IDA10_HEADLEN to more specific IDA10_TSHEADLEN
 *
 * Revision 1.4  2001/10/24 23:31:23  dec
 * add initialization code to quiet debugger
 *
 * Revision 1.3  2001/09/09 01:11:08  dec
 * allow for arbitrary length data field
 *
 * Revision 1.2  2000/02/17 01:40:40  dec
 * remove newline in log message
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
