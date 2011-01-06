#pragma ident "$Id: toida.c,v 1.6 2006/06/26 22:23:10 dechavez Exp $"
/*======================================================================
 *
 *  Convert a miniSEED record into IDA (rev 8) format.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <math.h>
#include "seed.h"
#include "util.h"
#include "ida.h"

#define SEED_TOIDA_MAXSTA 1
#define SEED_TOIDA_MAXCHN 9

struct buf_info {
    char *hdr;            /* pointer to header region          */
    char *dat;            /* pointer to data region            */
    char raw[IDA_BUFSIZ]; /* the whole packet                  */
};

struct chn_info {
    char *sname;          /* station name                       */
    char *cname;          /* channel name                       */
    double expected;      /* expected start time of next packet */
    short filt;           /* IDA filter code                    */
    short chan;           /* IDA channel code                   */
    short strm;           /* IDA stream code                    */
    short maxsamp;        /* number of samples in a full buffer */
    int nsamp;            /* number of samples currently saved  */
    int tolerance;        /* max time stamp error in samples    */
    int bi;               /* index of active output buffer      */
    int nbuf;             /* number of buffers to follow        */
    struct buf_info *buf; /* array of working buffers           */
    char **out;           /* array of pointers to filled buffes */
    int swap;             /* swap/noswap flag                   */
};

struct sta_info {
    char *sname;          /* station name                 */
    int nchn;             /* number of channels to follow */
    struct chn_info chn[SEED_TOIDA_MAXCHN]; /* channel information */
};

struct info {
    int rev;              /* IDA packet format to use     */
    int nsta;             /* number of stations to follow */
    struct sta_info sta[SEED_TOIDA_MAXSTA]; /* station information */
};

/* Given a name and a map, generate the corresponding channel and filter codes */

static int ReverseMap(IDA *ida, char *chnloc, short *pfilt, short *pchan)
{
char test[ISI_CHNLEN+ISI_LOCLEN+1];
IDA_CHNLOCMAP *map;
UINT32 i;

    for (i = 0; i < ida->chnlocmap->count; i++) {
        map = (IDA_CHNLOCMAP *) ida->chnlocmap->array[i];
        sprintf(test, "%s%s", map->chn, map->loc);
        if (strcasecmp(test, chnloc) == 0) {
            *pfilt = (short) map->fcode;
            *pchan = (short) map->ccode;
            return 0;
        }
    }

    return -1;
}

/*======================================================================
 *
 * Parse a chn,chn,... string, and return number of channels added.
 *
 *====================================================================*/

static int parse_cc(struct info *info, int i, char *cc)
{
int j, k, ntoken;
static int maxtoken = SEED_TOIDA_MAXCHN;
char *token[SEED_TOIDA_MAXCHN];
static char *fid = "seed_toida:parse_cc";

/* Parse string into individual channel names */

    ntoken = util_sparse(cc, token, ",", maxtoken);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, cc);
        return -1;
    }

/* Make sure we don't duplicate channel names for this station */

    for (j = 0; j < info->sta[i].nchn; i++) {
        for (k = 0; k < ntoken; k++) {
            if (strcasecmp(
                    (const char *) info->sta[i].chn[j].cname, token[k]
                ) == 0
            ) {
                token[k] = (char *) NULL;
            }
        }
    }

/* Add in the new channel names */

    for (k = 0; k < ntoken; k++) {
        if (token[k] != (char *) NULL) {
            if (info->sta[i].nchn == SEED_TOIDA_MAXCHN) {
                util_log(1, "%s: up SEED_TOIDA_MAXCHN (%d) to at least %d",
                    fid, SEED_TOIDA_MAXCHN, info->sta[i].nchn + 1
                );
                return -300;
            }
            info->sta[i].chn[info->sta[i].nchn].cname = strdup(token[k]);
            ++info->sta[i].nchn;
        }
    }

    return ntoken;
}

/*======================================================================
 *
 * Parse a sta:chn,chn,... string, and return number of channels added
 *
 *====================================================================*/

static int parse_sc(struct info *info, char *sc)
{
int i, ntoken;
static int maxtoken = 2;
char *token[2];
static char *fid = "seed_toida:parse_sc";

    ntoken = util_sparse(sc, token, ":", 2);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, sc);
        return -1;
    } else if (ntoken != 2) {
        util_log(1, "%s: illegal `%s'", fid, sc);
        return -2;
    }

/* See if we already have an entry for this station */

    for (i = 0; i < info->nsta; i++) {
        if (strcasecmp((const char *) info->sta[i].sname, token[0]) == 0) {
            return parse_cc(info, i, token[1]);
        }
    }

/* Create an entry for this station */

    if (info->nsta == SEED_TOIDA_MAXSTA) {
        util_log(1, "%s: increase SEED_TOIDA_MAXSTA (%d) to at least %d",
            fid, SEED_TOIDA_MAXSTA, info->nsta + 1
        );
        return -3;
    }

    info->sta[info->nsta].sname = strdup(token[0]);
    info->sta[info->nsta].nchn = 0;
    ++info->nsta;

    return parse_cc(info, info->nsta - 1, token[1]);
}

/*======================================================================
 *
 * Parse a sta:chn,chn,...+sta:chn,chn,... string and return number of
 * stations added.
 *
 *====================================================================*/

static int parse_scsc(info, scsc)
struct info *info;
char *scsc;
{
int i, ntoken, retval;
static int maxtoken = SEED_TOIDA_MAXSTA;
char *token[SEED_TOIDA_MAXSTA];
static char *fid = "seed_toida:parse_scsc";

    ntoken = util_sparse(scsc, token, "+", maxtoken);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, scsc);
        return -1;
    }

    for (i = 0; i < ntoken; i++) {
        if ((retval = parse_sc(info, token[i])) < 0) {
            return retval;
        }
    }

    return ntoken;
}

/* Initialization routine */

void *seed_toidainit(char *scsc, IDA *ida, int nbuf, int tolerance)
{
int i, j, k, hlen, dlen, errors;
u_long host_order;
struct info *info;
struct sta_info *sta;
struct chn_info *chn;
static char *fid = "seed_toidainit";

    if (ida->rev.value != 8) {
        util_log(1, "%s: rev %d is not supported", fid, ida->rev.value);
        return (void *) NULL;
    }

    info = (struct info *) malloc(sizeof(struct info));
    if (info == (struct info *) NULL) {
        util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
        return (void *) NULL;
    }

    info->nsta = 0;
    info->rev  = ida->rev.value;
    if ((info->nsta = parse_scsc(info, scsc)) < 0) {
        util_log(1, "%s: can't parse `%s'", fid, scsc);
        return (void *) NULL;
    }

    if (info->nsta == 0) {
        util_log(1, "%s: no stations in `%s'!", fid, scsc);
        return (void *) NULL;
    }

    errors = 0;
    host_order = util_order();
    ida_dhlen(ida->rev.value, &hlen, &dlen);
    for (i = 0; i < info->nsta; i++) {
        sta = info->sta + i;
        for (j = 0; j < sta->nchn; j++) {
            chn = sta->chn + j;
            chn->sname = sta->sname;
            chn->strm = j;
            if (ReverseMap(ida,chn->cname,&chn->filt,&chn->chan) != 0) {
                util_log(1, "%s: no entry for channel %s in '%s' map", fid, chn->cname, ida->mapname);
                ++errors;
            }
            chn->buf = (struct buf_info *) malloc(sizeof(struct buf_info)*nbuf);
            if (chn->buf == (struct buf_info *) NULL) {
                util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
                return (void *) NULL;
            }
            chn->out = (char **) malloc(sizeof(char *)*nbuf);
            if (chn->out == (char **) NULL) {
                util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
                return (void *) NULL;
            }
            chn->maxsamp   = dlen / sizeof(long);
            chn->nsamp     = 0;
            chn->tolerance = tolerance;
            chn->nbuf      = nbuf;
            chn->bi        = 0;
            chn->swap      = (host_order != BIG_ENDIAN_ORDER);
            for (k = 0; k < chn->nbuf; k++) {
                chn->buf[k].hdr = chn->buf[k].raw;
                chn->buf[k].dat = chn->buf[k].raw + hlen;
            }
        }
    }

#ifdef DEBUG_TEST

    fprintf(stderr, "info dump\n");
    fprintf(stderr, "info->rev  = %d\n", info->rev);
    fprintf(stderr, "info->nsta = %d\n", info->nsta);
    fprintf(stderr, "  sta dump\n");
    for (i = 0; i < info->nsta; i++) {
        fprintf(stderr, "  info->sta[%d].sname = %s\n",
            i, info->sta[i].sname
        );
        fprintf(stderr, "  info->sta[%d].nchn  = %d\n",
            i, info->sta[i].nchn
        );
        fprintf(stderr, "    chn dump\n");
        for (j = 0; j < info->sta[i].nchn; j++) {
            fprintf(stderr, "    info->sta[%d].chn[%d].sname     = %s\n",
                i, j, info->sta[i].chn[j].sname
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].cname     = %s\n",
                i, j, info->sta[i].chn[j].cname
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].expected  = %lf\n",
                i, j, info->sta[i].chn[j].expected
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].filt      = %hd\n",
                i, j, info->sta[i].chn[j].filt
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].chan      = %hd\n",
                i, j, info->sta[i].chn[j].chan
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].strm      = %hd\n",
                i, j, info->sta[i].chn[j].strm
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].maxsamp   = %hd\n",
                i, j, info->sta[i].chn[j].maxsamp
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].nsamp     = %d\n",
                i, j, info->sta[i].chn[j].nsamp
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].tolerance = %d\n",
                i, j, info->sta[i].chn[j].tolerance
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].bi        = %d\n",
                i, j, info->sta[i].chn[j].bi
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].nbuf      = %d\n",
                i, j, info->sta[i].chn[j].nbuf
            );
            fprintf(stderr, "    info->sta[%d].chn[%d].swap      = %d\n",
                i, j, info->sta[i].chn[j].swap
            );
        }
    }

#endif /* DEBUG_TEST */

    if (errors) return (void *) NULL;

    return (void *) info;
}

static struct chn_info *get_strm(
    char *sname, char *cname, struct info *info
){
struct sta_info *sta;
struct chn_info *chn;
int i, j;

    for (i = 0; i < info->nsta; i++) {
        sta = info[i].sta + i;
        if (strcasecmp(sta->sname, sname) == 0) {
            for (j = 0; j < sta->nchn; j++) {
                chn = sta[i].chn + j;
                if (strcasecmp(chn->cname, cname) == 0) return chn;
            }
        }
    }
    return (struct chn_info *) NULL;
}

/* Fill in a rev 8 IDA packet header */

static void idarev8(
    char *packet, struct chn_info *strm, double tofs, double sint
) {
double tols;
u_short sval, mult = 1;
u_long  lval, hz, sys, ext, tmp;
int yr, da, hr, mn, sc, ms;
static u_long base = 0;

    memset(packet, 0, IDA_BUFLEN);

    util_tsplit(tofs, &yr, &da, &hr, &mn, &sc, &ms);
#ifdef DEBUG_TEST
    fprintf(stderr, "begining of packet, set tofs = %s\n", util_dttostr(tofs,0));
    fprintf(stderr, "yr=%d da=%d hr=%d mn=%d sc=%d ms=%d\n", yr,da,hr,mn,sc,ms);
#endif

/* Record type */

    packet[0] = 'D';
    packet[1] = ~packet[0];

/* Year */

    sval = htons((short) yr);
    memcpy(packet + 6, &sval, 2);

/* System time multiplier */

    sval = htons(mult);
    memcpy(packet + 8, &sval, 2);

/* Data start times */

    ext = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    if (base == 0) {
        base = ext - 86400;
#ifdef DEBUG_TEST
        fprintf(stderr, "set Hz time base to %ul, %s\n", base, ext_timstr(base));
#endif
    }

    hz  = ((ext - base) * 1000/mult);
    sys = hz + (ms/mult);

    lval = htonl(sys);
    memcpy(packet + 10, &lval, 4);

    lval = htonl(ext);
    memcpy(packet + 14, &lval, 4);

    lval = htonl(hz);
    memcpy(packet + 18, &lval, 4);

/* Data start time quality code */

    packet[22] = '?';

/* Unit ID (2 => MK7 DAS) */

    sval = htons(2);
    memcpy(packet + 24, &sval, 2);

/* Station name */

    memcpy(packet + 26, strm->sname, 8);

/* Data format */

    sval = 0; /* uncompressed longs */
    memcpy(packet + 34, &sval, 2);

/* Channel and filter */

    sval = htons(strm->chan);
    memcpy(packet + 36, &sval, 2);

    sval = htons(strm->filt);
    memcpy(packet + 38, &sval, 2);

/* Nsamp */

    sval = htons(strm->maxsamp);
    memcpy(packet + 40, &sval, 2);

/* Data end times (keep ext and hz the same to avoid offset "errors") */

    tols = tofs + ((strm->maxsamp-1) * sint);
    util_tsplit(tols, &yr, &da, &hr, &mn, &sc, &ms);
    tmp = (da * 86400) + (hr * 3600) + (mn * 60) + sc;
    sys = ((tmp - base) * 1000/mult) + ms/mult;

    lval = htonl(sys);
    memcpy(packet + 42, &lval, 4);

    lval = htonl(ext);
    memcpy(packet + 46, &lval, 4);

    lval = htonl(hz);
    memcpy(packet + 50, &lval, 4);

/* Data end time quality code */

    packet[54] = '?';

/* Stream ID */

    sval = htons(strm->strm);
    memcpy(packet + 56, &sval, 2);
}

int seed_minitoida(
    struct seed_minipacket *seed, void *info_ptr, char ***out
) {
int error, nout, rem, need, ncopy, nbytes;
char *hdr, *dat, *raw, *ptr, *src;
double tons; /* time of next sample */
struct info *info;
struct chn_info *strm;
static char *fid = "seed_minitoida";

/* Reject this packet if it does not belong to a supported stream */

    info = (struct info *) info_ptr;
    strm = get_strm(seed->sname, seed->cname, info);
    if (strm == (struct chn_info *) NULL) return 0;

/* Check for time increment errors */

    if (strm->nsamp > 0) {
        error = (int) labs((long)((seed->beg-strm->expected)/seed->sint));

        if (error > strm->tolerance) {
            util_log(2, "%s: %d sample time tear, %s:%s packet reset",
                fid, error, strm->sname, strm->cname
            );
            strm->nsamp = strm->bi = 0;
        }
    }
    strm->expected = seed->beg + (seed->sint * (double) seed->nsamp);

/* Loop over all samples in the input packet */

    nout   = 0;
    hdr    = strm->buf[strm->bi].hdr;
    dat    = strm->buf[strm->bi].dat;
    raw    = strm->buf[strm->bi].raw;
    rem    = seed->nsamp;
    tons   = seed->beg;
    src    = (char *) seed->data;

    while (rem) {
        if (strm->nsamp == 0) idarev8(hdr, strm, tons, seed->sint);
        ptr = (char *) dat + (strm->nsamp * sizeof(long));
        need = strm->maxsamp - strm->nsamp;
        ncopy = need < rem ? need : rem;
        nbytes = ncopy * sizeof(long);

#ifdef DEBUG_TEST
        fprintf(stderr, "before: nsamp=%d, rem=%d, need=%d, ncopy=%d\n",
            strm->nsamp, rem, need, ncopy
        );
#endif /* DEBUG_TEST */

        memcpy(ptr, src, nbytes);
        if (strm->swap) util_lswap((long *) ptr, ncopy);

        src += nbytes;
        rem -= ncopy;
        strm->nsamp += ncopy;

#ifdef DEBUG_TEST
        fprintf(stderr, "after:  nsamp=%d, rem=%d\n", strm->nsamp, rem);
#endif /* DEBUG_TEST */

        if (strm->nsamp == strm->maxsamp) {
            strm->out[nout++] = raw;
            if (nout == strm->nbuf) {
                util_log(1, "%s: out of buffers: nout=%d nbuf=%d",
                    fid, nout, strm->nbuf
                );
                util_log(1, "%s/%s/%s %s %.3lf %4ld %8ld %8ld\n",
                    seed->sname, seed->cname, seed->nname,
                    util_dttostr(seed->beg, 0), seed->sint, seed->nsamp,
                    seed->data[0], seed->data[seed->nsamp-1]
                );
                return -2;
            }
            tons += seed->sint * ((double) ncopy);
#ifdef DEBUG_TEST
            fprintf(stderr, "save full IDA packet, set tons = %s (ncopy=%d, sint=%.3f)\n",
                util_dttostr(tons, 0), ncopy, seed->sint
            );
#endif /* DEBUG_TEST */
            strm->nsamp = 0;
            strm->bi++;
            strm->bi %= strm->nbuf;
            hdr = strm->buf[strm->bi].hdr;
            dat = strm->buf[strm->bi].dat;
            raw = strm->buf[strm->bi].raw;
        }
    }

    *out = strm->out;
    return nout;
}

#ifdef DEBUG_TEST

#define BHZ_ONLY

#ifdef BHZ_ONLY
#define keep_block(seed) (strcasecmp(seed.cname, "BHZ") == 0)
#else
#define keep_block(seed) 1
#endif

#include <stdio.h>

main(int argc, char **argv)
{
int i, fd, count, nrec, nout, to, rev=8;
char *buffer, *string, **rec;
size_t blksiz;
void *info;
struct seed_minipacket seed;
IDADB *db;
char *dbspec;
static char *defspec = "/usr/nrts/etc";

    util_logopen(NULL, 1, 9, 3, NULL, argv[0]);

    if ((dbspec = getenv("IDA_DBDIR")) == (char *) NULL) dbspec = defspec;

    if ((db = idadb_init(dbspec, 0)) == (IDADB *) NULL) {
        fprintf(stderr, "%s: unable to load database: ", argv[0]);
        perror(dbspec);
        exit(1);
    }

    if (idadb_defmap(db, rev, 0) != 0) {
        fprintf(stderr, "%s: unable set rev %d channel map",
            argv[0], rev
        );
        perror(dbspec);
        exit(1);
    }

#ifndef SOCKET_TEST
    fd     = fileno(stdin);
    blksiz = 512;
    to     = 0;
#else
    blksiz = 512;
    to     = 30;
    fd = util_connect("kip.iu.liss.org", NULL, 4000, "tcp", 5, 30);
    if (fd < 0) {
        perror("util_connect");
        exit(1);
    }
#endif

    fprintf(stderr, "using blocksize = %d\n", blksiz);
    buffer = malloc(blksiz);
    seed.datlen = 2 * blksiz;
    seed.data = (long *) malloc(seed.datlen * sizeof(long));
    if (buffer == (char *) NULL || seed.data == (long *) NULL) {
        perror("malloc");
        exit(1);
    }

    info = seed_toidainit("KIP:BHZ,BHN,BHE", rev,20,1,db);
    if (info == (void *) NULL) {
        fprintf(stderr, "can't init!\n");
        exit(1);
    }

    nout = count = 0;
    while (seed_readmini(fd, &seed, buffer, blksiz, to) == 0) {
        if (keep_block(seed)) {
            fprintf(stderr, "Block %3d: ", ++count);
            fprintf(stderr, "%s/%s/%s %s %.3lf %4ld %8ld %8ld\n",
                seed.sname, seed.cname, seed.nname,
                util_dttostr(seed.beg, 0), seed.sint, seed.nsamp,
                seed.data[0], seed.data[seed.nsamp-1]
            );
            if ((nrec = seed_minitoida(&seed, info, &rec)) < 0) {
                fprintf(stderr, "seed_minitoida failed: status %d\n", nrec);
                exit(1);
            }
            for (i = 0; i < nrec; i++) {
                if (fwrite(rec[i], IDA_BUFLEN, 1, stdout) != 1) {
                    perror("fwrite");
                    exit(1);
                }
                fflush(stdout);
                fprintf(stderr, "IDA packet %d flushed\n", ++nout);
            }
        }
    }
    exit(0);
}
#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: toida.c,v $
 * Revision 1.6  2006/06/26 22:23:10  dechavez
 * removed unreferenced local variables
 *
 * Revision 1.5  2006/02/09 19:57:34  dechavez
 * IDA handle support
 *
 * Revision 1.4  2005/05/25 22:40:22  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2004/04/23 00:45:27  dechavez
 * changed dbdir to dbspec
 *
 * Revision 1.2  2000/09/21 22:04:43  nobody
 * reduce time tears to level 2
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
