#pragma ident "$Id: ckextjmp.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*=====================================================================
 *
 *  Look for external time jumps.
 *
 *===================================================================*/
#include "idalst.h"

#define TST_STREAM 0 

void ckextjmp(IDA_DHDR *dhead, struct counters *count, int print)
{
int i;
static int  first  = TRUE;
static long tagno  = EXTJMP_TAG;
static long prev_rec;
static IDA_TIME_TAG prev_tag;
IDA_TIME_TAG crnt_tag;

/*  Work only with specific stream  */

    if (dhead->dl_stream != TST_STREAM) return;

/*  Loop twice, for first and last datum tags  */

    for (i = 0; i < 2; i++) {
        crnt_tag = (i == 0) ? dhead->beg : dhead->end;

    /*  Look for external time going backwards  */
    
        if (first) {
            prev_tag = crnt_tag;
            prev_rec = count->rec;
            first = FALSE;
        } else {
            if (crnt_tag.ext < prev_tag.ext) {
                ++count->exterr;
                if (tagno == EXTJMP_TAG) {
                    tag(tagno++); printf("\n");
                    tag(tagno++);
                    printf("BACKWARD EXTERNAL TIME JUMPS (digitizer stream %d)\n", TST_STREAM);
                    tag(tagno++);
                    printf("------- Previous -------  ");
                    printf("------- Current --------\n");
                    tag(tagno++);
                    printf(" Record    External Time    Q  Record  ");
                    printf("  External Time     Q   Current - Previous\n");
                }
                tag(tagno++);
                printf("%7ld",  prev_rec);
                if (prev_tag.year == 9999) {
                    printf("  %s ", util_lttostr(prev_tag.ext, 0));
                } else {
                    printf("  yyyy:%s ", ext_timstr(prev_tag.ext, NULL));
                }
                printf("%3d ",  prev_tag.qual);
                printf("%7ld",  count->rec);
                if (crnt_tag.year == 9999) {
                    printf("  %s ", util_lttostr(crnt_tag.ext, 0));
                } else {
                    printf("  yyyy:%s ", ext_timstr(crnt_tag.ext, NULL));
                }
                printf("%3d ",  crnt_tag.qual);
                printf("     ");
                printf("-%s\n", ext_timstr(prev_tag.ext-crnt_tag.ext, NULL));
            }
            prev_tag = crnt_tag;
            prev_rec = count->rec;
        }
    }
}

/* Revision History
 *
 * $Log: ckextjmp.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:18  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
