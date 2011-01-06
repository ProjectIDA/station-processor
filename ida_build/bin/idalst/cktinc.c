#pragma ident "$Id: cktinc.c,v 1.5 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Check for (system) time stamp increment errors.
 *
 *====================================================================*/
#include "idalst.h"

#define HDR1 "St Prev #/Q Crnt #/Q   Expected Stamp   "
#define HDR2 "Actual stamp        Error         Nsamp"
#define PRTHDR printf("%s%s\n", HDR1, HDR2)

long cktinc(IDA_DHDR *crnt_hdr, IDA_DHDR *prev_hdr, long crnt_rec, long prev_rec, int sort, int group, int print)
{
IDA_TIME_TAG beg, end;
unsigned long errtic, exptag;
float errsec, errsmp;
long lerrsmp;
int sign, err;
long tagno;
static long ptagno = PINCERR_TAG;
static long ntagno = NINCERR_TAG;
static long stagno = SINCERR_TAG;
static long ltagno = LINCERR_TAG;
static long etagno = EINCERR_TAG;
static long first_err   = TRUE;
static long first_perr  = TRUE;
static long first_nerr  = TRUE;
static long first_large = TRUE;
static long first_small = TRUE;
static long first_ext   = TRUE;

    err = ida_timerr(crnt_hdr, prev_hdr, &sign, &errtic);
    if (err != 0) {
        tag(LAST_TAG); printf("ida_timerr failure, status '%d'\n", err);
        return 0L;
    }
    if (errtic == 0) return 0L;

    if (prev_hdr == NULL) {   /* Found intra-record jump */
        beg = crnt_hdr->beg;
        end = crnt_hdr->end;
        prev_rec = crnt_rec;
    } else {                  /* Found inter-record jump */
        beg = prev_hdr->end;
        end = crnt_hdr->beg;
    }

    exptag  = (sign > 0) ? end.sys - errtic : end.sys + errtic;
    errsec  = (float) ((errtic * crnt_hdr->beg.mult) / 1000.0);
    errsmp  = errsec / crnt_hdr->sint;
    lerrsmp = (long) errsmp;
    if (errsmp != (float) lerrsmp) ++lerrsmp;

    if (group == BY_SIGN && sign > 0) {
        if (first_perr) {
            ptagno = PINCERR_TAG;
            tag(ptagno++); printf("\n"); tag(ptagno++);
            printf("POSITIVE TIME STAMP INCREMENT ERRORS ");
            printf("(missing data)\n");
            tag(ptagno++); PRTHDR;
            first_perr = FALSE;
        }
        if (sort == BY_RECORD) ++ptagno;
        tagno = ptagno;
    } else if (group == BY_SIGN && sign < 0) {
        if (first_nerr) {
            ntagno = NINCERR_TAG;
            tag(ntagno++); printf("\n"); tag(ntagno++);
            printf("NEGATIVE TIME STAMP INCREMENT ERRORS ");
            printf("(extra data)\n");
            tag(ntagno++); PRTHDR;
            first_nerr = FALSE;
        }
        if (sort == BY_RECORD) ++ntagno;
        tagno = ntagno;
    } else if (group == BY_SIZE && lerrsmp < BIG_TIMERR) {
        if (first_small) {
            stagno = SINCERR_TAG;
            tag(stagno++); printf("\n"); tag(stagno++);
            printf("SMALL TIME STAMP INCREMENT ERRORS ");
            printf("(+ => missing data, - => extra data)\n");
            tag(stagno++); PRTHDR;
            first_small = FALSE;
        }
        if (sort == BY_RECORD) ++stagno;
        tagno = stagno;
    } else if (group == BY_SIZE && lerrsmp > BIG_TIMERR) {
        if (first_large) {
            ltagno = LINCERR_TAG;
            tag(ltagno++); printf("\n"); tag(ltagno++);
            printf("LARGE TIME STAMP INCREMENT ERRORS ");
            printf("(+ => missing data, - => extra data)\n");
            tag(ltagno++); PRTHDR;
            first_large = FALSE;
        }
        if (sort == BY_RECORD) ++ltagno;
        tagno = ltagno;
    } else {
        if (first_err) {
            tagno = PINCERR_TAG;
            tag(tagno++); printf("\n"); tag(tagno++);
            printf("TIME STAMP INCREMENT ERRORS ");
            printf("(+ => missing data, - => extra data)\n");
            tag(tagno++); PRTHDR;
            first_err = FALSE;
        }
        if (sort == BY_RECORD) ++tagno;
    }

    tag(tagno);
    printf("%02d",  crnt_hdr->dl_stream);
    if (prev_rec != crnt_rec) {
        printf("%7ld",  prev_rec);
        printf("%2d",   beg.qual);
    } else {
        printf("         ");
    }
    printf("%7ld",  crnt_rec);
    printf("%2d",   end.qual);
    printf(" %s",   sys_timstr(exptag,  beg.mult, NULL));
    printf(" %s",   sys_timstr(end.sys, beg.mult, NULL));
    printf(" %c",   sign >= 0 ? '+' : '-');
    printf("%s ",   sys_timstr(errtic, beg.mult, NULL));
    if (errsmp <= 9999.9) {
        printf("%6.1f", errsmp);
    } else {
        printf("  LOTS");
    }
    if (lerrsmp >= BIG_TIMERR) printf("*");
    printf("\n");

    if (prev_hdr != NULL && crnt_hdr->beg.ext < prev_hdr->beg.ext) {
        if (first_ext) {
            etagno = EINCERR_TAG;
            tag(etagno++); printf("\n"); tag(etagno++);
            printf("INCONSISTENT SYSTEM VS EXTERNAL TIME INCREMENTS\n");
            tag(etagno++);
            printf("Str Record     Ext. time     Q     1-Hz time    ");
            printf("Datum (sys) time\n");
            first_ext = FALSE;
        }
        tag(etagno++);
        printf(" %02d", prev_hdr->dl_stream);
        printf("%7ld",  prev_rec);
        if (prev_hdr->beg.year == 9999) {
            printf(" %s ",  util_lttostr(prev_hdr->beg.ext, 0));
        } else {
            printf(" yyyy:%s ",  ext_timstr(prev_hdr->beg.ext, NULL));
        }
        printf("%d",    prev_hdr->beg.qual);
        printf(" %s",   sys_timstr(prev_hdr->beg.hz,  prev_hdr->beg.mult, NULL));
        printf(" %s",   sys_timstr(prev_hdr->beg.sys, prev_hdr->beg.mult, NULL));
        printf("\n");
        tag(etagno++);
        printf("   %7ld", prev_rec);
        if (prev_hdr->end.year == 9999) {
            printf(" %s ",  util_lttostr(prev_hdr->end.ext, 0));
        } else {
            printf(" yyyy:%s ",  ext_timstr(prev_hdr->end.ext, NULL));
        }
        printf("%d",    prev_hdr->end.qual);
        printf(" %s",   sys_timstr(prev_hdr->end.hz,  prev_hdr->end.mult, NULL));
        printf(" %s",   sys_timstr(prev_hdr->end.sys, prev_hdr->end.mult, NULL));
        printf("\n");
        tag(etagno++);
        printf("   %7ld", crnt_rec);
        if (crnt_hdr->beg.year == 9999) {
            printf(" %s ",  util_lttostr(crnt_hdr->beg.ext, 0));
        } else {
            printf(" yyyy:%s ",  ext_timstr(crnt_hdr->beg.ext, NULL));
        }
        printf("%d",    crnt_hdr->beg.qual);
        printf(" %s",   sys_timstr(crnt_hdr->beg.hz,  crnt_hdr->beg.mult, NULL));
        printf(" %s",   sys_timstr(crnt_hdr->beg.sys, crnt_hdr->beg.mult, NULL));
        printf("\n");
        tag(etagno++);
        printf("   %7ld", crnt_rec);
        if (crnt_hdr->end.year == 9999) {
            printf(" %s ",  util_lttostr(crnt_hdr->end.ext, 0));
        } else {
            printf(" yyyy:%s ",  ext_timstr(crnt_hdr->end.ext, NULL));
        }
        printf("%d",    crnt_hdr->end.qual);
        printf(" %s",   sys_timstr(crnt_hdr->end.hz,  crnt_hdr->end.mult, NULL));
        printf(" %s",   sys_timstr(crnt_hdr->end.sys, crnt_hdr->end.mult, NULL));
        printf("\n");
    }
    

    return sign * lerrsmp;
}

/* Revision History
 *
 * $Log: cktinc.c,v $
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:18  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
