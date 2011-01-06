#pragma ident "$Id: sac2css.c,v 1.4 2004/11/01 01:15:58 dechavez Exp $"
/*======================================================================
 *
 *  Generate a 3.0 wfdisc file for all binary SAC files specified on
 *  the command line.  Program assumes that the input files are in the
 *  current directory, and sets the dir entry to '.'.  You can pipe the
 *  output to wdedit and modify if you like.
 *
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "cssio.h"
#include "util.h"
#include "null_sac.h"

#ifdef unix
#include <sys/param.h>
#else
#define MAXPATHLEN 64
#endif

static char *datatype;

#define SAC_HEAD_LEN 632L

static int process(ifp, ofp, fname)
FILE *ifp, *ofp;
char *fname;
{
int ok, yr, da, dum;
double start;
struct wfdisc wfdisc;
struct sac_header sac;

    if (sacio_rbh(ifp, &sac) != 0) {
        perror("sac2css: can't read header");
        return FALSE;
    }

    ok = TRUE;
    if (sac.npts == -12345) {
        fprintf(stderr, "sac2css: npts is undefined!\n");
        ok = FALSE;
    }
    if ((start = sacio_sttodt(&sac)) == (double) -12345) {
        fprintf(stderr, "sac2css: time is undefined!\n");
        ok = FALSE;
    }
    if (sac.b == (float) -12345) {
        fprintf(stderr, "sac2css: begin time (B) is undefined!\n");
        ok = FALSE;
    }
    start += (double) sac.b;
    if (sac.iftype != ITIME || sac.leven != TRUE) {
        fprintf(stderr, "sac2css: input is not an evenly space time series!\n");
        ok = FALSE;
    }
    if (strlen(fname) > 32) {
        fprintf(stderr, "sac2css: file name too long!\n");
        ok = FALSE;
    }
    if (!ok) return FALSE;

    util_tsplit(start, &yr, &da, &dum, &dum, &dum, &dum);

    wfdisc = wfdisc_null;
    wfdisc.jdate = (yr*1000) + da;
    wfdisc.time = start;
    strncpy(wfdisc.sta,  util_ucase(sac.kstnm), 6); wfdisc.sta[6] = 0;
    strncpy(wfdisc.chan, util_lcase(sac.kcmpnm), 8); wfdisc.chan[8] = 0;
    wfdisc.nsamp = sac.npts;
    wfdisc.smprate = (float) ((double) 1.0 / (double) sac.delta);
    wfdisc.endtime = wfdisc.time +
                     (double) (wfdisc.nsamp - 1) / (double) wfdisc.smprate;
    strcpy(wfdisc.datatype, datatype);
    strcpy(wfdisc.dir, ".");
    strcpy(wfdisc.dfile, fname);
    wfdisc.foff = SAC_HEAD_LEN;

    wwfdisc(ofp, &wfdisc);

    return TRUE;
}

int main(int argc, char **argv)
{
FILE *ifp;
int i;
char *fname;

    datatype = (util_order() == BIG_ENDIAN_ORDER) ? "t4" : "f4";

    for (i = 1; i < argc; i++) {
        fname  = argv[i];
        if ((ifp = fopen(fname, "rb")) == NULL) {
            fprintf(stderr,"sac2css: ");
            perror(fname);
            fprintf(stderr,"Ignore and continue.\n");
        } else {
            if (!process(ifp, stdout, fname)) {
                fprintf(stderr,"sac2css: can't process %s\n.", fname);
                fprintf(stderr,"Ignore and continue.\n");
            }
            fclose(ifp);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: sac2css.c,v $
 * Revision 1.4  2004/11/01 01:15:58  dechavez
 * corrected dfile length test to 32
 *
 * Revision 1.3  2003/12/10 06:31:25  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2001/03/16 22:19:48  dec
 * fix return bug in process()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:18  dec
 * import existing IDA/NRTS sources
 *
 */
