#pragma ident "$Id: dumppar.c,v 1.1.1.1 2000/02/08 20:20:11 dec Exp $"
/*======================================================================
 *
 * For debugging only, dump parameter to indicated FILE
 *
 *====================================================================*/
#include "edes.h"
static char buf[1024];

static void dump_cnf_chn(int level, struct edes_cnf_chn *chn)
{
    sprintf(buf, "index = %d, ",   chn->index);
    sprintf(buf+strlen(buf), "wrdsiz=%d, ",    chn->wrdsiz);
    sprintf(buf+strlen(buf), "instype=`%s', ", chn->instype);
    sprintf(buf+strlen(buf), "order=0x%08x, ", chn->order);
    sprintf(buf+strlen(buf), "sint=%.3f, ",    chn->sint);
    sprintf(buf+strlen(buf), "calib=%.3f, ",   chn->calib);
    sprintf(buf+strlen(buf), "calper=%.3f, ",  chn->calper);
    sprintf(buf+strlen(buf), "vang=%.3f, ",    chn->vang);
    sprintf(buf+strlen(buf), "hang=%.3f, ",    chn->hang);
    sprintf(buf+strlen(buf), "paklen=%.3f",    chn->paklen);
    util_log(level, buf);
}

static void dump_cnf_sta(int level, struct edes_cnf_sta *sta, int nchn)
{
int ichn;

    util_log(level, "index = %d, lat=%.3f, lon=%.3f, elev=%.3f, depth=%.3f",
        sta->index, sta->lat, sta->lon, sta->elev, sta->depth
    );
    for (ichn = 0; ichn < nchn; ichn++) {
        util_log(level, "CNF Channel %d of %d", ichn+1, nchn);
        dump_cnf_chn(level, sta->chn + ichn);
    }
}

static void dump_cnf(int level, struct edes_cnf *cnf, struct nrts_sys *sys)
{
int ista;

    util_log(level, "CNF dump");
    for (ista = 0; ista < sys->nsta; ista++) {
        util_log(level, "CNF Station %d of %d", ista+1, sys->nsta);
        dump_cnf_sta(level, cnf->sta + ista, sys->sta[ista].nchn);
    }
}

static void dump_inf_chn(int level, struct edes_inf_chn *chn)
{
    if ((int) chn->beg == (int) XFER_YNGEST) {
        sprintf(buf, "beg=XFER_YNGEST, ");
    } else if ((int) chn->beg == (int) XFER_OLDEST) {
        sprintf(buf, "beg=XFER_OLDEST, ");
    } else {
        sprintf(buf, "beg=%s, ", util_dttostr(chn->beg, 0));
    }
    if ((int) chn->end == (int) XFER_YNGEST) {
        sprintf(buf+strlen(buf), "end=XFER_YNGEST, ");
    } else if ((int) chn->end == (int) XFER_OLDEST) {
        sprintf(buf+strlen(buf), "end=XFER_OLDEST, ");
    } else {
        sprintf(buf+strlen(buf), "end=%s, ", util_dttostr(chn->end, 0));
    }
    sprintf(buf+strlen(buf), "begndx=%ld ", chn->begndx);
    sprintf(buf+strlen(buf), "endndx=%ld ", chn->endndx);
    sprintf(buf+strlen(buf), "nxtndx=%ld ", chn->nxtndx);
    sprintf(buf+strlen(buf), "count=%ld ",  chn->count);
    sprintf(buf+strlen(buf), "status=%d ",  chn->status);
    util_log(level, buf);
}

static void dump_inf_sta(int level, struct edes_inf_sta *sta, int nchn)
{
int ichn;

    for (ichn = 0; ichn < nchn; ichn++) {
        util_log(level, "INF Channel %d of %d", ichn+1, nchn);
        dump_inf_chn(level, sta->chn + ichn);
    }
}

static void dump_inf(int level, struct edes_inf *inf, struct nrts_sys *sys)
{
int ista;

    util_log(level, "INF dump");
    for (ista = 0; ista < sys->nsta; ista++) {
        util_log(level, "INF Station %d of %d", ista+1, sys->nsta);
        dump_inf_sta(level, inf->sta + ista, sys->sta[ista].nchn);
    }
}

void dump_par(int level, struct edes_params *par)
{
int isys;
struct edes_cnf *cnf;

    util_log(level, "PARAMETER DUMP");
    util_log(level, "nsys = %d", par->nsys);
    for (isys = 0; isys < par->nsys; isys++) {
        util_log(level, "----------");
        util_log(level, "System %d of %d:", isys+1, par->nsys);
        dump_cnf(level, par->cnf + isys, par->sys+isys);
        util_log(level, "..........");
        dump_inf(level, par->inf + isys, par->sys+isys);
    }
    util_log(level, "++++++++++");
}

/* Revision History
 *
 * $Log: dumppar.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
