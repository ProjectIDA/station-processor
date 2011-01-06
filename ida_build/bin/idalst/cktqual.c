#pragma ident "$Id: cktqual.c,v 1.5 2008/01/16 23:54:00 dechavez Exp $"
/*=====================================================================
 *
 *  Look for time quality transitions.
 *
 *===================================================================*/
#include "idalst.h"

void cktqual(IDA_DHDR *dhead, struct counters *count, int print)
{
int i;
static long num_locked   = 0;
static long num_unlocked = 0;
static long num_unknown  = 0;
static long num_error    = 0;
static long num_total    = 0;
static long num_nosync   = 0;
static long num_synced   = 0;
static int  tst_stream   = -1;
static int  first  = TRUE;
static long tagno  = TQUAL_TAG;
static long prev_rec;
static IDA_TIME_TAG prev_tag;
IDA_TIME_TAG time;
float percent;

/*  NULL dhead means print last time value and summary  */

    if (dhead == NULL) {
        if (first) return;
        tag(tagno++);
        printf("Final         time quality code is ");
        printf("'%c' ",  prev_tag.code);
        printf("(%hd) ", prev_tag.qual);
        printf("at %s, ", tru_timstr(&prev_tag, NULL));
        printf("record %ld\n", prev_rec);
        tag(tagno++);
        printf("A total of %ld stream %d ", num_total, tst_stream);
        printf("time tags were examined.\n");
        tag(tagno++);
        printf("Clock lost   sync %ld times.\n", num_nosync);
        tag(tagno++);
        printf("Clock gained sync %ld times.\n", num_synced);
        if (num_total > 0) {
            percent = ((float) num_locked / (float) num_total) * (float) 100.0;
            tag(tagno++);
            printf("%5.1f%% ", percent);
            printf("time tags showed the clock locked ");
            printf("(%ld).\n", num_locked);

            percent = ((float) num_unlocked / (float) num_total) * (float) 100.0;
            tag(tagno++);
            printf("%5.1f%% ", percent);
            printf("time tags showed the clock unlocked ");
            printf("(%ld).\n", num_unlocked);

            percent = ((float) num_unknown / (float) num_total) * (float) 100.0;
            tag(tagno++);
            printf("%5.1f%% ", percent);
            printf("time tags had missed time strings or qual codes ");
            printf("(%ld).\n", num_unknown);

            percent = ((float) num_error / (float) num_total) * (float) 100.0;
            tag(tagno++);
            printf("%5.1f%% ", percent);
            printf("time tags had erroneous qual codes ");
            printf("(%ld).\n", num_error);
        }
        first = TRUE;
        return;
    }

/*  Select stream to track for time quality transitions  */

    if (tst_stream < 0) {
        tst_stream = 0;
        tag(tagno++); printf("\n"); tag(tagno++);
        printf("TIME CODE QUALITY SUMMARY (tracking digitizer stream %d)\n", tst_stream);
    }

/*  Loop twice, for first and last datum tags  */

    for (i = 0; i < 2; i++) {
        time = (i == 0) ? dhead->beg : dhead->end;

    /*  For all streams, make sure time quality code is recognized  */

        if (time.qual == IDA_BADTQUAL) {
            tag(tagno++);
            printf("UNRECOGNIZED OMEGA CODE ");
            if (isprint(time.code)) {
                printf("'%c'", time.code);
            } else {
                printf("0x%02x", time.code);
            }
            printf(" at %s, ", tru_timstr(&time, NULL));
            printf("stream %02d, ", dhead->dl_stream);
            printf("rec %ld\n", count->rec);
            ++count->iltq;
        }

    /*  For remainder of routine, work only with specified stream  */

        if (dhead->dl_stream != tst_stream) return;
        ++num_total;

    /*  Count no. of records where clock is locked  */

        if (time.qual == 1) {
            ++num_locked;

    /*  Count no. of records where clock is unlocked  */

        } else if (time.qual > 1 && time.qual <= 6) {
            ++num_unlocked;

    /*  Count no. of records where clock string or quality is missed  */

        } else if (time.qual > 6) {
            ++num_unknown;
            return;

    /*  Count no. of records where clock quality is in error  */

        } else if (time.qual < 0) {
            ++num_error;
            return;
        }

    /*  At this point, we are dealing only with valid locked/unlocked */
    /*  records.  Print initial time quality value                    */

        if (first) {
            tag(tagno++);
            printf("Initial valid time quality code is ");
            printf("'%c' ", time.code);
            printf("(%hd) ", time.qual);
            printf("at %s, ", tru_timstr(&time, NULL));
            printf("record %ld\n", count->rec);
            first = FALSE;

    /*  Print time quality transitions  */

        } else if (prev_tag.qual != time.qual) {
            if (prev_tag.qual == 1 && time.qual != 1) ++num_nosync;
            if (prev_tag.qual != 1 && time.qual == 1) ++num_synced;
            if (print & P_ALLQUAL) {
                tag(tagno++);
                printf("    status changed from ");
                printf("'%c' (%hd) ", prev_tag.code, prev_tag.qual);
                printf("to '%c' (%hd) ", time.code, time.qual);
                printf("at %s, ", tru_timstr(&time, NULL));
                printf("record %ld", count->rec);
                if (time.qual != 1 && (time.qual != prev_tag.qual + 1)) {
                    ++count->iltq;
                    printf(" ?");
                }
                if (time.ext < prev_tag.ext) {
                    printf(" EXTERNAL TIME JUMPED BACKWARDS!");
                }
                printf("\n");
            }
        }

/*  Save this information (see dhead == NULL above)  */

        prev_tag = time;
        prev_rec = count->rec;
    }
}

/* Revision History
 *
 * $Log: cktqual.c,v $
 * Revision 1.5  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
