#pragma ident "$Id: idadecode.c,v 1.18 2008/01/16 23:28:06 dechavez Exp $"
/*======================================================================
 *
 *  Decode an IDA format packet
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"
#include "cssio.h"
#include "ida.h"
#include "isi.h"
#include "isi/db.h"

#define ONE_HOUR ((long) 3600)

static struct nrts_sys *sys;
static int             flags;
static int             shift;
static unsigned long   errtol;
static char            msgbuf[1024];
static BOOL CheckTstampLoggingEnabled = TRUE;
static IDA             *ida;

#define write_enabled    (flags & NRTS_TODISK)
#define from_nrts        (flags & NRTS_FROMNRTS)
#define strict_checks    (flags & NRTS_STRICT)
#define station_system  ((flags & NRTS_FROMTTY) || (flags & NRTS_FROMISP))
#define need_seqno      ((station_system) && (flags & NRTS_ADDSEQNO))
#define force_ttst       (flags & NRTS_FORCETTST)

static struct nrts_header hd;

/* Lookup table to give all the stream specific parameters quickly */

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

int nrts_idadecodeinit(
    char *home, char *syscode, struct nrts_sys *sys_ptr, 
    int new_flags, IDA *ida_ptr, struct nrts_ida *info, void *opt,
    int unused
){
int i, nmatch, nchan;
long now;
static int initialized = 0;
ISI_INST inst;
ISI_STREAM_NAME name;
static char *fid = "nrts_idadecodeinit";

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

    ida = ida_ptr;
    sys = sys_ptr;
    if (sys->nsta != 1) {
        util_log(1, "%s: ERROR: nsta = %d", fid, sys->nsta);
        util_log(1, "%s: IDA systems can have only one station", fid);
        return -1;
    }

    shift = info->shift;

/* Fill out the static parts of the packet header */

    hd.wfdisc = wfdisc_null;
    hd.standx = 0;
    sprintf(hd.wfdisc.dir, "%s/%s", home, syscode);
    sprintf(hd.wfdisc.dfile, "%s",  NRTS_DATNAME);
    sprintf(hd.wfdisc.sta, "%s", sys->sta[0].name);
    ida_dhlen(ida->rev.value, &hd.hlen, &hd.dlen);

/* Create and load the per stream look up table */

    nchan = sys->sta[0].nchn;

    channel_table = (struct channel_info *) malloc(nchan * sizeof(struct channel_info));

    if (channel_table == (struct channel_info *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return -3;
    }

    now    = time(NULL);
    nmatch = 0;
    for (i = 0; i < nchan; i++) {
        channel_table[i].count = 0;
        channel_table[i].name  = sys->sta[0].chn[i].name;
        isiStaChnLocToStreamName(hd.wfdisc.sta, channel_table[i].name, NULL, &name);
        isidbLookupInst(ida->db, &name, (UINT32) now, &inst);
        channel_table[i].calib   = inst.calib;
        channel_table[i].calper  = inst.calper;
        channel_table[i].hang    = inst.hang;
        channel_table[i].vang    = inst.vang;
        strlcpy(channel_table[i].instype, inst.type, NRTS_INAMLEN+1);
    }

    if (need_seqno) {
        util_log(1, "input order sequence numbers will be added");
    }

/* Initialization complete */

    return 0;
}

void nrtsToggleCheckTstampLoggingFlag()
{
    CheckTstampLoggingEnabled = !CheckTstampLoggingEnabled;
    util_log(1, "strict time stamp check log messages %s", CheckTstampLoggingEnabled ? "enabled" : "suppressed");
}

/* Check time stamps for known problems */

static int check_tstamp(
    IDA_DHDR *crnt, IDA_DHDR *prev,
    long threshold, char *chname, char *response,
    int loglevel
) {
int prev_locked, prev_patched, crnt_locked;
double prev_off, crnt_off;
long now, diff;
char tmpbuf[80];
static char *fid = "check_tstamp";

    loglevel = CheckTstampLoggingEnabled ? loglevel : 9;

    util_log(3, "%s: strict time checks on %s record", fid, chname);

/* If it has already been patched (upstream NRTS) then accept it */

    if (crnt->beg.code == NRTS_AUTOINC) {
        util_log(loglevel, "%s: patched record accepted as is", fid);
        return 0;
    }

/* If offset is unchanged from the previous packet then don't worry.
 * This assumes that no bad offsets have made it into the disk loop,
 * which is what we are trying very hard to do (and not always
 * succeeding!).
 */

    /* Check the beg time stamp */

    if (prev != (IDA_DHDR *) NULL) {
        diff = 0;
        prev_off = ida_ext1hzoff(&prev->beg);
        crnt_off = ida_ext1hzoff(&crnt->beg);
        diff = (long) ((crnt_off - prev_off) * 1000.0);

    /* Check the end time stamp */

        if (diff == 0) {
            prev_off = ida_ext1hzoff(&prev->end);
            crnt_off = ida_ext1hzoff(&crnt->end);
            diff = (long) ((crnt_off - prev_off) * 1000.0);
        }

    /* If both are OK, then checks are deemed complete */

        if (diff == 0) return 0;
        util_log(2, "%s: %s crnt-prev offset diff=%ld, strict checks to follow", fid, chname, diff);
        sprintf(msgbuf, "%s: %s beg = %s `%c' ", fid, chname, ext_timstr(crnt->beg.ext, tmpbuf), crnt->beg.code);
        sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.hz, crnt->beg.mult, tmpbuf));
        sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.sys, crnt->beg.mult, tmpbuf));
        util_log(2, msgbuf);

        sprintf(msgbuf, "%s: %s end = %s `%c' ", fid, chname, ext_timstr(crnt->end.ext, tmpbuf), crnt->end.code);
        sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.hz, crnt->end.mult, tmpbuf));
        sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.sys, crnt->end.mult, tmpbuf));
        util_log(2, msgbuf);
    }

/* Set up message buffer for potential use */

    sprintf(msgbuf, "bogus %s (NRTS) ttag %s: ", chname, response);

/* If the clock quality sys the times are suspect, then believe them */

    if (crnt->beg.error) {
        sprintf(msgbuf+strlen(msgbuf), "beg stamp error flag: 0x%02x",
            crnt->beg.error
        );
        util_log(loglevel, msgbuf);
        return -1;
    }

    if (crnt->end.error) {
        sprintf(msgbuf+strlen(msgbuf), "end stamp error flag: 0x%02x",
            crnt->end.error
        );
        util_log(loglevel, msgbuf);
        return -2;
    }

    if (crnt->beg.qual < 0 || crnt->beg.qual == 6) {
        sprintf(msgbuf+strlen(msgbuf), "bad beg clock qual `%c' (%d)",
            crnt->beg.code, crnt->beg.qual
        );
        util_log(loglevel, msgbuf);
        return -3;
    }

    if (crnt->end.qual < 0 || crnt->beg.qual == 6) {
        sprintf(msgbuf+strlen(msgbuf), "bad end clock qual `%c' (%d)",
            crnt->end.code, crnt->end.qual
        );
        util_log(loglevel, msgbuf);
        return -4;
    }

/* Make sure time increases with time */

    if (crnt->beg.tru >= crnt->end.tru) {
        sprintf(msgbuf+strlen(msgbuf), "beg.tru >= end.tru: ");
        sprintf(msgbuf+strlen(msgbuf), "beg=%s (0x%02x)",
            util_dttostr(crnt->beg.tru, 0), crnt->beg.error
        );
        sprintf(msgbuf+strlen(msgbuf), "end=s (0x%02x)",
            util_dttostr(crnt->end.tru, 0), crnt->end.error
        );
        util_log(loglevel, msgbuf);
        return -5;
    }

#ifdef INCLUDE_THIS_SURE_FIRE_WAY_TO_KEEP_OUT_VH_CHANNELS
/* Make sure that the external and 1-Hz times increment by the same
 * amount between the tofs and tols.
 */

    if ((erroff = ida_offerr(crnt, &offsign, &expoff)) > 1) {
        sprintf(msgbuf+strlen(msgbuf), "external/1-hz discrepancy of ");
        sprintf(msgbuf+strlen(msgbuf), "%c%ld tics",
            offsign > 0 ? '+' : '-', erroff
        );
        util_log(loglevel, msgbuf);
        return -6;
    }
#endif /* INCLUDE_THIS_SURE_FIRE_WAY_TO_KEEP_OUT_VH_CHANNELS */

/* Make sure that the time stamp is "close" to the NRTS host's notion
 * of time (only makes sense if this is the station system, and requires
 * that the system clock be more or less OK).
 */

    if (station_system) {
        now = (long) time(NULL);
        diff = now > (long) crnt->end.tru ? now - (long) crnt->end.tru :
                                             (long) crnt->end.tru - now;
        if (diff > threshold) {
 
            sprintf(msgbuf, "suspect %s (NRTS) ttag %s: ", chname, response);
 
            sprintf(msgbuf+strlen(msgbuf), "sample time is far from host time");
            util_log(loglevel, msgbuf); msgbuf[0] = 0;

            sprintf(msgbuf+strlen(msgbuf), "end.tru = %s, ",
                util_dttostr(crnt->end.tru, 0)
            );
            sprintf(msgbuf+strlen(msgbuf), "clock = %s, ",
                util_lttostr(now, 0)
            );
            sprintf(msgbuf+strlen(msgbuf), "diff = %ld secs", diff);
            util_log(loglevel, msgbuf);
            return -7;

        }
    }

/* Make sure that the external time did not jump backwards */

    if (prev != NULL && crnt->beg.ext < prev->beg.ext) {
        sprintf(msgbuf, "suspect timestamp %s: ", response);
        sprintf(msgbuf+strlen(msgbuf) ,"time jumped backwards");
        util_log(loglevel, msgbuf);
        return -8;
    }

/* If the clock is unlocked for this record, and locked on the previous,
 * force the previous record's timestamps.  This is to try to catch things
 * like the Magellan day 271 error.
 */

    if (prev != NULL) {
        prev_locked = (prev->beg.qual == 1 || prev->end.qual == 1);
        crnt_locked = (crnt->beg.qual == 1 && crnt->end.qual == 1);
        prev_patched = (
            prev->beg.code == NRTS_AUTOINC || prev->end.code == NRTS_AUTOINC
        );
        if (!crnt_locked && (prev_locked || prev_patched)) {
            sprintf(msgbuf, "suspect timestamp %s: ", response);
            sprintf(msgbuf+strlen(msgbuf) ,"clock unlocked"); 
            util_log(loglevel, msgbuf);
            return -9;
        }
    }

    util_log(2, "%s: %s record passed strict time checks", fid, chname);

    return 0;
}

/* Correct known problems with external time stamps */

static void patch_tstamp( IDA_DHDR *crnt, IDA_DHDR *prev, char *buffer)
{
int status;
unsigned long ltemp;
short stemp;
char tmpbuf[80];
static char *fid = "patch_tstamp";

    if (ida->rev.value < 5 || ida->rev.value > 9) {
        util_log(1, "UPDATE REV SUPPORT IN patch_tstamp IN IDADECODE.C!");
        nrts_die(1);
    }

#define YEAR_OFF     6
#define BEGEXT_OFF  14
#define BEGHZ_OFF   18
#define BEGQUAL_OFF 22
#define ENDSYS_OFF  42
#define ENDEXT_OFF  46
#define ENDHZ_OFF   50
#define ENDQUAL_OFF 54

    sprintf(msgbuf, "orig beg = %s `%c' ", ext_timstr(crnt->beg.ext, tmpbuf), crnt->beg.code);
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.hz, crnt->beg.mult, tmpbuf));
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.sys, crnt->beg.mult, tmpbuf));
    util_log(2, msgbuf);

    sprintf(msgbuf, "orig end = %s `%c' ", ext_timstr(crnt->end.ext, tmpbuf), crnt->end.code);
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.hz, crnt->end.mult, tmpbuf));
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.sys, crnt->end.mult, tmpbuf));
    util_log(2, msgbuf);

/* Force beg & end ext & hz times to match the previous record */

    stemp = prev->beg.year;
    SSWAB(&stemp, 1);
    memcpy(buffer + YEAR_OFF, &stemp, 2);

    ltemp = prev->beg.ext;
    LSWAB(&ltemp, 1);
    memcpy(buffer + BEGEXT_OFF, &ltemp, 4);

    ltemp = prev->beg.hz;
    LSWAB(&ltemp, 1);
    memcpy(buffer + BEGHZ_OFF, &ltemp, 4);

    buffer[BEGQUAL_OFF] = NRTS_AUTOINC;

    ltemp = prev->end.ext;
    LSWAB(&ltemp, 1);
    memcpy(buffer + ENDEXT_OFF, &ltemp, 4);

    ltemp = prev->end.hz;
    LSWAB(&ltemp, 1);
    memcpy(buffer + ENDHZ_OFF, &ltemp, 4);

    buffer[ENDQUAL_OFF] = NRTS_AUTOINC;

/* Reload header to reflect modified data buffer contents */

    if ((status = ida_dhead(ida, crnt, buffer)) < 0) {
        sprintf(msgbuf, "%s: PROGRAM BUG corrupted data header: ", fid);
        sprintf(msgbuf+strlen(msgbuf), "dhead status %d", status);
        util_log(1, msgbuf);
        nrts_die(1);
    }

    sprintf(msgbuf, "corr beg = %s `%c' ", ext_timstr(crnt->beg.ext, tmpbuf), crnt->beg.code);
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.hz, crnt->beg.mult, tmpbuf));
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->beg.sys, crnt->beg.mult, tmpbuf));
    util_log(2, msgbuf);

    sprintf(msgbuf, "corr end = %s `%c' ", ext_timstr(crnt->end.ext, tmpbuf), crnt->end.code);
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.hz, crnt->end.mult, tmpbuf));
    sprintf(msgbuf+strlen(msgbuf), "%s ", sys_timstr(crnt->end.sys, crnt->end.mult, tmpbuf));
    util_log(2, msgbuf);
}

/* Force external end time stamp to match expected */

static void fix_extendtime(IDA_DHDR *crnt)
{
double expected;
unsigned long uldiff;

    expected = crnt->beg.tru +
               (double) ((crnt->nsamp - 1) * crnt->sint);

    /*  Truncate to to nearest millisecond  */

    expected = nrts_dtime(nrts_time(expected));
    crnt->end.tru = nrts_dtime(nrts_time(crnt->end.tru));

    if (expected > crnt->end.tru) {
        uldiff = (unsigned long) ((expected > crnt->end.tru) * 1000.0);
    } else {
        uldiff = (unsigned long) ((crnt->end.tru - expected) * 1000.0);
    }

    if (uldiff > crnt->end.mult) {
        util_log(3, "expected endtime %s", util_dttostr(expected, 0));
        util_log(3, "does not match   %s", util_dttostr(crnt->end.tru, 0));
        util_log(3, "force datum end time to agree with expected time");
        crnt->end.tru = expected;
    }
}

static void add_seqno(char *buffer)
{
static unsigned short stmp, seqno = 0;

    if (ida->rev.value < 5 || ida->rev.value > 9) return;

    stmp = ++seqno;
    SSWAB(&stmp, 1);
    memcpy(buffer + 1022, &stmp, 2);

}

struct nrts_header *nrts_idadecode(char *buffer, int len, int *action)
{
float errsec, errsmp;
struct nrts_chn *chn;
unsigned long errtic;
long *ldata;
int i, status, sign, err, index;
int patch_enabled = 0;
int need_check;
IDA_DHDR dhead, *prev, *crnt;
ISI_STREAM_NAME name;
static IDA_DHDR prev_dhead[NRTS_MAXCHN];
static char *fid = "nrts_idadecode";

    patch_enabled = force_ttst ||
                    (write_enabled && station_system && strict_checks);

/* Decode the record header */

    if (ida_rtype(buffer, ida->rev.value) != IDA_DATA) {
        util_log(3, "unexpected non-data record dropped");
        return (struct nrts_header *) NULL;
    }

/* Must be a data record at this point */

    crnt = &dhead;
    memset(crnt, 0, sizeof(IDA_DHDR));

/* Decode and determine if desired stream. */

    sprintf(msgbuf, "corrupt data record dropped: ");

    if ((status = ida_dhead(ida, crnt, buffer)) < 0) {
        sprintf(msgbuf+strlen(msgbuf), "dhead status %d", status);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;

    }

/* Find its position in the NRTS chn and local lookup tables */

    idaBuildStreamName(ida, crnt, &name);
    index = nrts_chnndx(&sys->sta[0], name.chnloc);

    if ((hd.chnndx = index) < 0) {
        util_log(1, "unsupported %s:%s record ignored by NRTS", name.sta, name.chnloc);
        return (struct nrts_header *) NULL;
    }

    chn = sys->sta[0].chn + hd.chnndx;

/* Sanity checks on header contents */

    if ( ida_sampok(crnt) == 0) {
        sprintf(msgbuf+strlen(msgbuf), "%s illegal number of samples = %d", chn->name, crnt->nsamp);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    } else if ( (crnt->wrdsiz != 2) && (crnt->wrdsiz != 4)) {
        sprintf(msgbuf+strlen(msgbuf), "%s unexpected wordsize = %d", chn->name, crnt->wrdsiz);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    } else if ( (crnt->beg.error & TTAG_BAD_OFFSET) || (crnt->end.error & TTAG_BAD_OFFSET)) {
        sprintf(msgbuf+strlen(msgbuf), "%s bad offset (hz time > sys time)", chn->name);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    } else if ( (crnt->beg.error & TTAG_BAD_SYSTIM) || (crnt->end.error & TTAG_BAD_SYSTIM)) {
        sprintf(msgbuf+strlen(msgbuf), "%s bad systim (beg >= time)", chn->name);
        util_log(1, msgbuf);
        return (struct nrts_header *) NULL;
    } else if (crnt->sint <= 0.0) {
        sprintf(msgbuf+strlen(msgbuf), "%s bad sample interval %.3f", chn->name, crnt->sint);
        util_log(1, msgbuf);
        util_hexlog(1, (unsigned char *) buffer, 64, 0, 'x', msgbuf);
        return (struct nrts_header *) NULL;
    }

/* If this is the start of a data logger aquisition session, 
 * then absolutely refuse to accept any data with problematic external
 * time stamps.
 */

    need_check = (
        channel_table[index].count == 0 && (
            (write_enabled && station_system && strict_checks) ||
            force_ttst
        )
    );
    if (
        need_check &&
        check_tstamp(crnt, NULL, ONE_HOUR, channel_table[index].name, "dropped", 1) != 0
    ) return (struct nrts_header *) NULL;

/*  Sometimes end external time is screwed up... if tapping, make it fit  */

    if (!write_enabled  ) fix_extendtime(crnt);

/*  Look for and reject records with duplicate headers or times */

    prev = prev_dhead + hd.chnndx;

    if (write_enabled && channel_table[index].count > 0) {
        if (memcmp(prev, crnt, sizeof(IDA_DHDR)) == 0) {
            util_log(1, "duplicate %s data header, packet dropped", chn->name);
            return (struct nrts_header *) NULL;
        }
        if (prev->beg.sys == crnt->beg.sys && prev->end.sys == crnt->end.sys) {
            util_log(1, "duplicate %s start and end times, packet dropped", chn->name);
            return (struct nrts_header *) NULL;
        }
    }

/* If filling disk loop, save the sample interval */

    if (write_enabled && channel_table[index].count == 0) chn->sint = crnt->sint;

/*  Look for time tears  */

    if (channel_table[index].count > 0) {

        err = ida_timerr(crnt, prev, &sign, &errtic);
        if (err != 0) {
            util_log(1,"%s: ida_timerr failed with status %d: ignored", fid, err);
        } else if (errtic > errtol) {
            errsec  = (float) ((errtic * crnt->beg.mult) / 1000.0);
            errsmp  = errsec / crnt->sint;
            if (write_enabled) {
                util_log(2, "%c%.2f sec %s time tear (%.2f samp, %lu tics)",
                    sign > 0 ? '+' : '-',errsec, chn->name,  errsmp, errtic
                );
                need_check = (station_system && strict_checks) || force_ttst;
                if ( need_check && check_tstamp( crnt, prev, ONE_HOUR, chn->name, "dropped", 1) != 0) return (struct nrts_header *) NULL;
            }
            if (sign < 0) errsec *= 1.0;
            hd.tear = errsec;
            *action = NRTS_CREATE;

        } else {

    /* No time tear detected.  Patch station ext & 1-Hz time if not OK */

            if (
                patch_enabled &&
                check_tstamp(
                    crnt, prev, ONE_HOUR, chn->name, "patched", 2
                ) != 0
            ) patch_tstamp(crnt, prev, buffer);
            hd.tear = 0.0;
            *action = NRTS_EXTEND;
        }
    } else {
        hd.tear = 0.0;
        *action = NRTS_TESTWD;
    }

/*  Fill in the output header for this packet */

    sprintf(hd.wfdisc.chan, "%s", chn->name);
    hd.wrdsiz         = crnt->wrdsiz;
    hd.wfdisc.nsamp   = crnt->nsamp;
    hd.wfdisc.time    = crnt->beg.tru;
    hd.wfdisc.endtime = crnt->end.tru;
    hd.wfdisc.smprate = (float) (1.0 / crnt->sint);
    hd.wfdisc.calib   = channel_table[index].calib;
    hd.wfdisc.calper  = channel_table[index].calper;
    sprintf(hd.wfdisc.datatype, "%s", crnt->wrdsiz == 2 ? "s2" : "s4");
    strlcpy(hd.wfdisc.instype, channel_table[index].instype, CSS_INSTYPE_LEN+1);
    hd.hang  = channel_table[index].hang;
    hd.vang  = channel_table[index].vang;
    hd.order = crnt->order;

    hd.beg.code = crnt->beg.code;
    hd.beg.qual = crnt->beg.qual;

    hd.end.code = crnt->end.code;
    hd.end.qual = crnt->end.qual;

/*  Save the decoded header for later comparisons  */

    *prev = *crnt;
    ++channel_table[index].count;

/*  Add sequence number if desired */

    if (need_seqno) add_seqno(buffer);

/*  Finally, apply Fels' shifts if neccesary  */

    if (
        (write_enabled      ) &&
        (shift              ) &&
        (crnt->ffu_shift > 0) &&
        (len == sys->reclen)
    ) {
        ldata = (long *) (buffer + chn->hdr.len);
        for (i = 0; i < crnt->nsamp; i++) ldata[i] >>= crnt->ffu_shift;
    }

    return &hd;
}

/* Revision History
 *
 * $Log: idadecode.c,v $
 * Revision 1.18  2008/01/16 23:28:06  dechavez
 * added new destination buffers to timstr function calls
 *
 * Revision 1.17  2007/01/11 21:59:29  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.16  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.15  2006/06/26 22:24:34  dechavez
 * removed unreference local variables
 *
 * Revision 1.14  2006/02/09 19:42:26  dechavez
 * IDA handle support
 *
 * Revision 1.13  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.12  2005/05/06 22:17:45  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.11  2005/02/09 16:59:05  dechavez
 * removed tabs
 *
 * Revision 1.10  2005/02/07 19:15:24  dechavez
 * added nrtsToggleCheckTstampLoggingFlag()
 *
 * Revision 1.9  2005/01/25 18:43:30  dechavez
 * removed external-1Hz comparison between tofs and tols
 *
 * Revision 1.8  2004/06/24 17:39:01  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.7  2004/06/21 19:53:30  dechavez
 * changed time_tag "true" field to "tru
 *
 * Revision 1.6  2004/01/29 18:29:50  dechavez
 * initialize input buffer in nrts_idadecode()
 *
 * Revision 1.5  2002/04/29 17:33:28  dec
 * add new (and unused here) hdr fd to init routine
 *
 * Revision 1.4  2001/05/07 22:30:09  dec
 * cast util_hexlog() buffer to proper type
 *
 * Revision 1.3  2000/09/19 23:14:16  nobody
 * time tear log entries to level 2
 *
 * Revision 1.2  2000/02/18 00:28:48  dec
 * improved bogus time tag handling
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
