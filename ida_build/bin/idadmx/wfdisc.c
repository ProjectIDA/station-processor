#pragma ident "$Id: wfdisc.c,v 1.9 2007/01/11 22:02:14 dechavez Exp $"
/*======================================================================
 *
 *  Wfdisc related routines.
 *
 *----------------------------------------------------------------------
 *
 *  get_wfdisc()
 *
 *  Get the most recent wfdisc list entry which corresponds to the
 *  given output file.  If there is not already a wfdisc record in 
 *  the list, one is created and added to it.  Returns a pointer to 
 *  the list element if successful, otherwise aborts.
 *
 *  If a time stamp increment error was detected then we log it and
 *  create a new wfdisc record, regardless of the size of the error.
 *
 *----------------------------------------------------------------------
 *
 *  mkwfdisc()
 *
 *  Create a new wfdisc record, and add it to the list.
 *
 *----------------------------------------------------------------------
 *
 *  fill_wfdisc()
 *
 *  Fill wfdisc field of wflist element.
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;
extern IDA *ida;

static struct wflist listhead;

static long prev_recno[IDA_MAXSTREAMS];
static IDA_DHDR prev_head[IDA_MAXSTREAMS];

static struct counter *count;

static int cssformat;
static unsigned long byteorder;

#define INDEX    (dhead->dl_stream)

static void fill_wfdisc(struct wflist *new, BufferedStream *fp, IDA_DHDR *dhead)
{
ISI_STREAM_NAME name;
static ISI_INST inst = ISI_UNDEFINED_INST;
int yr, da, hr, mn, sc, ms;

    idaBuildStreamName(ida, dhead, &name);
    isidbLookupInst(ida->db, &name, (UINT32) dhead->beg.tru, &inst);

/*  Initialize the wfdisc  */

    new->wfdisc = wfdisc28_null;

    util_tsplit(dhead->beg.tru, &yr, &da, &hr, &mn, &sc, &ms);
    new->wfdisc.date   = (1000L * (long) yr) + (long) da;
    new->wfdisc.time   = dhead->beg.tru;
    new->wfdisc.nsamp  = 0;
    new->wfdisc.smprat = 1.0 / dhead->sint;
    if (byteorder == 0) {
        if (dhead->rev > 1) {
            strcpy(new->wfdisc.dattyp, dhead->wrdsiz == 4 ? "s4" : "s2");
        } else {
            strcpy(new->wfdisc.dattyp, dhead->wrdsiz == 4 ? "i4" : "i2");
        }
    } else if (byteorder == BIG_ENDIAN_ORDER) {
        strcpy(new->wfdisc.dattyp, dhead->wrdsiz == 4 ? "s4" : "s2");
    } else {
        strcpy(new->wfdisc.dattyp, dhead->wrdsiz == 4 ? "i4" : "i2");
    }
    strcpy(new->wfdisc.dir,  crnt_dname());
    strcpy(new->wfdisc.file, crnt_fname());
    strcpy(new->wfdisc.sta, ida->site);
    sprintf(new->wfdisc.chan, "%s", name.chnloc);
    new->wfdisc.segtyp = 'o';
    new->wfdisc.foff = fp->nTotalBytes;
    new->wfdisc.calib = inst.calib;
    new->wfdisc.calper = inst.calper;
    strcpy(new->wfdisc.instyp, inst.type);
}

void wrt_wfdisc(void)
{
FILE *fp;
char fname[MAXPATHLEN], *prefix;
static char *default_prefix = "css";
struct wflist *crnt;
struct wfdisc wfdisc30;

    if ((prefix = vol_id()) == NULL) prefix = default_prefix;
    sprintf(fname,"%s%c%s.wfdisc", basedir(), PATH_DELIMITER, prefix);

    if ((fp = fopen(fname, "w")) == NULL) {
        sprintf(Buffer,"wrt_wfdisc: %s: %s", fname, syserrmsg(errno));
        logmsg(Buffer);
        return;
    }
#ifdef WIN32
/***********************************************************************/
/*  Do not remove!                                                     */
/*  WinIdadmx uses this output to get the wfdisc file name from idadmx */
/***********************************************************************/
    printf("\nOutput file name:%s\n", fname);
#endif
    crnt = listhead.next;
    while (crnt != NULL) {
        if (crnt->wfdisc.nsamp != 0) {
            if (cssformat == CSS_28) {
                wwfdisc28(fp, &crnt->wfdisc);
            } else {
                wf28to30(&wfdisc30, &crnt->wfdisc);
                wwfdisc(fp, &wfdisc30);
            }
        }
        crnt = crnt->next;
    }
    fclose(fp);

    cssioWriteDescriptor(basedir(), prefix);
}

static struct wflist *mkwfdisc(char *path, BufferedStream *fp, IDA_DHDR *dhead)
{
struct wflist *new, *crnt, *next;

/*  Create and initialize new element  */

    if ((new = (struct wflist *) malloc(sizeof(struct wflist))) == NULL) {
        sprintf(Buffer,"mkwfdisc: malloc: %s", syserrmsg(errno));
        logmsg(Buffer);
        die(ABORT);
    }

    strcpy(new->name, path);
    fill_wfdisc(new, fp, dhead);

/*  Add it to the list  */

    crnt = &listhead;
    next = crnt->next;
    while (next != NULL) {
        crnt = crnt->next;
        next = next->next;
    }    

    crnt->next = new;
    new->next  = NULL;

/*  Done, return pointer to new element  */

    return new;
}    

void init_wfdisc(int format, struct counter *countptr, unsigned long swab)
{
    cssformat = format;
    byteorder = swab;
    listhead.next = NULL;
    count = countptr;
    memset(prev_recno, 0, IDA_MAXSTREAMS * sizeof(long));
}

struct wflist *get_wfdisc(char *path, BufferedStream *fp, IDA_DHDR *dhead)
{
struct wflist *crnt, *match;
int newrec, sign, offsign, err;
unsigned long errtic, erroff;
float errsec, errsmp;

/*  Search list for the latest wfdisc record for this file  */

    match = NULL;
    crnt  = listhead.next;
    while (crnt != NULL) {
        if (strcmp(crnt->name, path) == 0) match = crnt;
        crnt = crnt->next;
    }

/*  If there was no match, then we create an entry  */

    if (match == NULL) {
        prev_recno[INDEX] = count->rec;
        prev_head[INDEX] = *dhead;
        return mkwfdisc(path, fp, dhead);
    }

/*  Otherwise, make sure time incremented properly  */

    assert(prev_recno[INDEX] != 0);
    err = ida_timerr(dhead, &prev_head[INDEX], &sign, &errtic);

/*  If there is a time stamp increment error, make a new record  */

    if (err < 0) {
        sprintf(Buffer, "ida_timerr failure, status '%d'", err);
        logmsg(Buffer);
        return NULL;
    } else if (errtic == 0) {
        newrec = 0;
    } else {
        if (dhead->beg.error || dhead->end.error) {
            sprintf(Buffer,"get_wfdisc: ");
            sprintf(Buffer+strlen(Buffer),"Illegal time tag, ");
            sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
            logmsg(Buffer);
            return NULL;
        }
        if (ida_offerr(dhead, &offsign, &erroff) > 1) {
            sprintf(Buffer,"get_wfdisc: external/1-hz discrepancy of ");
            sprintf(Buffer+strlen(Buffer), "%c%ld tics",
                offsign > 0 ? '+' : '-', erroff
            );
            logmsg(Buffer);
            sprintf(Buffer,"record %d ignored.\n",count->rec);
            logmsg(Buffer);
            return NULL;
        }
        if (dhead->beg.ext == prev_head[INDEX].beg.ext) {
            sprintf(Buffer,"get_wfdisc: ");
            sprintf(Buffer+strlen(Buffer),"System time jumped but external ");
            sprintf(Buffer+strlen(Buffer),"time did not, ");
            sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
            logmsg(Buffer);
            return NULL;
        }
        newrec = 1;
        errsec = (errtic * dhead->beg.mult) / 1000.0;
        errsmp = errsec / dhead->sint;
        if (dhead->mode == CONT) {
            sprintf(Buffer, "Time jump of %c", sign > 0 ? '+' : '-');
            sprintf(Buffer+strlen(Buffer), "%.1f samples, ", errsmp);
            sprintf(Buffer+strlen(Buffer), "stream %.2hd, ", dhead->dl_stream);
            sprintf(Buffer+strlen(Buffer), "between ");
            sprintf(Buffer+strlen(Buffer), "recs %ld ", prev_recno[INDEX]);
            sprintf(Buffer+strlen(Buffer), "and %ld", count->rec);
            logmsg(Buffer);
        }
    }

    prev_recno[INDEX] = count->rec;
    prev_head[INDEX]  = *dhead;

    return newrec ? mkwfdisc(path, fp, dhead) : match;
}

/* Revision History
 *
 * $Log: wfdisc.c,v $
 * Revision 1.9  2007/01/11 22:02:14  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.8  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.7  2005/11/10 00:12:58  dechavez
 * respect case of sname as specified by user on command line
 *
 * Revision 1.6  2005/10/11 22:48:39  dechavez
 * generate descriptor file
 *
 * Revision 1.5  2005/09/30 16:48:08  dechavez
 * 09-30-2005 aap win32 mods
 *
 * Revision 1.4  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.3  2004/06/21 20:11:29  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.2  2002/03/11 17:17:27  dec
 * removed tabs and dead code
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
