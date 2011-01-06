#pragma ident "$Id: prt.c,v 1.5 2005/07/26 00:05:02 dechavez Exp $"
/*======================================================================
 *
 *  Print contents of various nrts structures.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

static char buffer[1024];

#define bufset(buffer) buffer[0] = 0; \
                       if (log != NULL) sprintf(buffer, "%s: ",log)
#define bufprt(buffer) if (log != NULL) util_log(4, "%s", buffer); \
                       if (fp  != NULL) fprintf(fp, "%s\n", buffer)

void nrts_prtsta(fp, sta, log, what)
FILE *fp;
struct nrts_sta *sta;
char *log;
int what;
{
int i;
time_t lastwrite;
struct nrts_chn *chn;
static char *TimesHeader =
"   name     earliest datum        latest datum      nseg    nrec   sint   last update";
static char *fid = "nrts_prtsta";

    if (what & NRTS_CONFIG) {
        bufset(buffer);
        sprintf(buffer+strlen(buffer), "%s configuration ", sta->name);
        sprintf(buffer+strlen(buffer), "(%d channels)", sta->nchn);
        bufprt(buffer);

        bufset(buffer);
        sprintf(buffer+strlen(buffer), "   name      nrec      hide       len  ");
        sprintf(buffer+strlen(buffer) ,"hlen  dlen      hoff      doff");
        bufprt(buffer);

        for (i = 0; i < sta->nchn; i++) {
            chn = sta->chn + i;
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "%7s",  chn->name);
            sprintf(buffer+strlen(buffer), " %9d", chn->hdr.nrec);
            sprintf(buffer+strlen(buffer), " %9d", chn->hdr.hide);
            sprintf(buffer+strlen(buffer), " %9d", chn->hdr.nrec - chn->hdr.hide);
            sprintf(buffer+strlen(buffer), " %5d", chn->hdr.len);
            sprintf(buffer+strlen(buffer), " %5d", chn->dat.len);
            sprintf(buffer+strlen(buffer), " %9d", chn->hdr.off);
            sprintf(buffer+strlen(buffer), " %9d", chn->dat.off);
            bufprt(buffer);
        }
    }

    
    if (what & NRTS_INDICES) {
        bufset(buffer);
        sprintf(buffer+strlen(buffer), "%s buffer status", sta->name);
        bufprt(buffer); bufset(buffer);
        sprintf(buffer+strlen(buffer), "   name    beg    end   lend   status");
        sprintf(buffer+strlen(buffer), "    nrec    sint latency");
        bufprt(buffer);

        for (i = 0; i < sta->nchn; i++) {
            chn = sta->chn + i;
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "%7s",    chn->name);
            sprintf(buffer+strlen(buffer), " %6d",   chn->hdr.oldest);
            sprintf(buffer+strlen(buffer), " %6d",   chn->hdr.yngest);
            sprintf(buffer+strlen(buffer), " %6d",   chn->hdr.lend);
            sprintf(buffer+strlen(buffer), " %8s",   nrts_status(chn->status));
            sprintf(buffer+strlen(buffer), " %7d",   chn->nread);
            sprintf(buffer+strlen(buffer), " %7.3f", chn->sint);
            lastwrite = time(NULL) - chn->tread;
            sprintf(buffer+strlen(buffer), " %s",  nrts_latency(lastwrite));
            bufprt(buffer);
        }
    }
    
    if (what & NRTS_TIMES) {
        bufset(buffer);
        sprintf(buffer+strlen(buffer), "%s buffer times", sta->name);
        bufprt(buffer); bufset(buffer);
        sprintf(buffer+strlen(buffer), "%s", TimesHeader);
        bufprt(buffer);

        for (i = 0; i < sta->nchn; i++) {
            chn = sta->chn + i;
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "%7s",  chn->name);
            if (chn->beg > (double) 0) {
                sprintf(buffer+strlen(buffer), " %s",  util_dttostr(chn->beg, 0));
            } else {
                sprintf(buffer+strlen(buffer), "                      ");
            }
            if (chn->end > (double) 0) {
                sprintf(buffer+strlen(buffer), " %s",  util_dttostr(chn->end, 0));
            } else {
                sprintf(buffer+strlen(buffer), "                      ");
            }
            sprintf(buffer+strlen(buffer), " %4d",   chn->nseg);
            sprintf(buffer+strlen(buffer), " %7d",   chn->nread);
            sprintf(buffer+strlen(buffer), " %7.3f", chn->sint);
            lastwrite = time(NULL) - chn->tread;
            sprintf(buffer+strlen(buffer), " %s",  nrts_latency(lastwrite));
            bufprt(buffer);
        }
    }

    return;
}

void nrts_prtsys(fp, sys, log, what)
FILE *fp;
struct nrts_sys *sys;
char *log;
int what;
{
time_t now, uptime;
int i;
struct nrts_ida *ida;
struct nrts_asl *asl;
static char *fid = "nrts_prtsys";

    if (fp == NULL && log == NULL) return;

    if (what & NRTS_SYSINFO) {
        bufset(buffer);
        sprintf(buffer+strlen(buffer), "host order = 0x%08x ", sys->order);
        if (sys->order == LTL_ENDIAN_ORDER) {
            sprintf(buffer+strlen(buffer), "(little endian)");
        } else if (sys->order == BIG_ENDIAN_ORDER) {
            sprintf(buffer+strlen(buffer), "(big endian)");
        } else {
            sprintf(buffer+strlen(buffer), "(UNKNOWN!)");
        }
        bufprt(buffer); bufset(buffer);

        bufset(buffer);
        sprintf(buffer+strlen(buffer), "type       = %d", sys->type);

        if (sys->type == NRTS_IDA) {
            ida = (struct nrts_ida *) sys->info;
            sprintf(buffer+strlen(buffer), " (IDA)");
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "rev        = %d", ida->rev);
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "map        = %s", ida->map);
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "shift      = %d", ida->shift);
        } else if (sys->type == NRTS_ASL) {
            asl = (struct nrts_asl *) sys->info;
            sprintf(buffer+strlen(buffer), " (ASL)");
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "blksiz     = %d", asl->blksiz);
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "reformat   = %d", asl->reformat);
            bufprt(buffer); bufset(buffer);
            sprintf(buffer+strlen(buffer), "map        = %s", asl->map);
        } else if (sys->type == NRTS_IDA10) {
            sprintf(buffer+strlen(buffer), " (IDA10)");
            bufprt(buffer); bufset(buffer);
        } else {
            sprintf(buffer+strlen(buffer), " (UNKNOWN TYPE)");
        }
        bufprt(buffer); bufset(buffer);

        sprintf(buffer+strlen(buffer), "pid        = %d", sys->pid);
        bufprt(buffer); bufset(buffer);

        if (sys->pid > 0) {
            now    = time(NULL);
            uptime = now - sys->start;
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "now        = %s",util_lttostr(now, 0));
            bufprt(buffer); bufset(buffer);
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "start      = %s",util_lttostr(sys->start,0));
            bufprt(buffer); bufset(buffer);
            bufset(buffer);
            sprintf(buffer+strlen(buffer), "uptime     = %s",nrts_latency(uptime));
            bufprt(buffer); bufset(buffer);
        }

        bufset(buffer);
        sprintf(buffer+strlen(buffer), "status     = %s", nrts_status(sys->status));
        bufprt(buffer); bufset(buffer);

        bufset(buffer);
        sprintf(buffer+strlen(buffer), "bwd        = %s", nrts_status(sys->bwd));
        bufprt(buffer); bufset(buffer);

        bufset(buffer);
        sprintf(buffer+strlen(buffer), "nsta       = %d", sys->nsta);
        bufprt(buffer);
        for (i = 0; i < sys->nsta; i++) {
            nrts_prtsta(fp, sys->sta + i, log, what);
        }
    }

    return;
}

#define SC_PER_MN  ((time_t)    60)
#define SC_PER_HR  ((time_t)  3600)
#define SC_PER_DA  ((time_t) 86400)

char *nrts_latency(secs)
time_t secs;
{
static char illegal[] = "            ";
static char buffer[]  = "            ";
time_t da, hr, mn, sc;

    da = secs / SC_PER_DA; secs -= da * SC_PER_DA;
    hr = secs / SC_PER_HR; secs -= hr * SC_PER_HR;
    mn = secs / SC_PER_MN; secs -= mn * SC_PER_MN;
    sc = secs;

/*  Should not need this, but just make sure we don't clobber buffer */

    if (da > (time_t) 999) return illegal;
    if (da < (time_t) -99) return illegal;
    if (hr > (time_t)  99) return illegal;
    if (hr < (time_t)  -9) return illegal;
    if (mn > (time_t)  99) return illegal;
    if (mn < (time_t)  -9) return illegal;
    if (sc > (time_t)  99) return illegal;
    if (sc < (time_t)  -9) return illegal;

/*  Make the string  */

    sprintf(buffer,"%3.3ld-%2.2ld:%2.2ld:%2.2ld", da, hr, mn, sc);

    return buffer;
}

char *nrts_status(status)
int status;
{
    if (status == NRTS_IDLE)    return "idle";
    if (status == NRTS_STABLE)  return "stable";
    if (status == NRTS_BUSY1)   return "busy (1)";
    if (status == NRTS_BUSY2)   return "busy (2)";
    if (status == NRTS_BUSY3)   return "busy (3)";
    if (status == NRTS_CORRUPT) return "corrupt!";

    return "notdef";
}

/* Revision History
 *
 * $Log: prt.c,v $
 * Revision 1.5  2005/07/26 00:05:02  dechavez
 * changed "latency" column to "last update"
 *
 * Revision 1.4  2005/05/06 22:17:45  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.3  2004/06/24 17:37:07  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.2  2003/10/16 17:01:37  dechavez
 * removed nrts_prtreq()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
