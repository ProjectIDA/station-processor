#pragma ident "$Id: dodata.c,v 1.7 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Current buffer is a data record...
 *
 *====================================================================*/
#include <assert.h>
#include "idalst.h"

#define PRVR     (prev_recno[head->dl_stream])
#define PRVH     (last[head->dl_stream])
#define HAVEPREV (count->stream[head->dl_stream] > 0)

static void prt_badd_hdr(int print, long *tagptr)
{
long tagno;

    tagno = *tagptr;
    if (tagno == BADD_TAG) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("BAD 'D' RECORD SUMMARY\n");
        tag(tagno++); printf("Record #   Problem\n");
    }
    *tagptr = tagno;
}

static void savehead(IDA_DHDR *prvh, IDA_DHDR *crnt, long *prvr, struct counters *count)
{
    *prvh = *crnt;
    *prvr = count->rec;
    ++count->stream[crnt->dl_stream];
}


void dodata(char *buffer, IDA_DHDR *first, IDA_DHDR *last, struct counters *count, int print, int check, int sort, int group)
{
int status, swab;
IDA_DREC  drec;
IDA_DHDR *head;
static long prev_recno[IDA_MAXSTREAMS];
long jump;
int nsamp_ok, time_ok, trig, reboot;
static long tagno = BADD_TAG;
int bad = FALSE;

/*  Fill the structure  */

    swab = (check & C_LASTW) ? 1 : 0;

    if ((status = ida_drec(ida, &drec, buffer, 0, swab)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_DATA, status, print);
        ++count->badd;
        ++count->corrupt;
        return;
    }

    head = &drec.head;
    if (head->dl_stream < 0 || head->dl_stream >= IDA_MAXSTREAMS) {
        prt_badd_hdr(print, &tagno); tag(tagno++);
        printf("%8ld   ", count->rec);
        printf("illegal data logger stream code = ");
        printf("%hd\n", head->dl_stream);
        ++count->badd;
        return;
    }

/*  Increment relevant ARS/DAS counters  */

    if (head->atod == IDA_ARS) ++count->ars;
    if (head->atod == IDA_DAS) ++count->das;

/*  If this is the first instance of this stream, save the header  */

    if (!HAVEPREV) first[head->dl_stream] = *head;

/*  Print the header contents  */

    if (print & P_DATA) pdata(&drec, count, print);
        
/*  Check for duplicate headers, early return if so  */

    if (
        (check & C_SAME) &&
        ckdupdat(&PRVH, PRVR, head, count->rec, print, count) != 0
    ) return;

/*  Print time triples  */

    if (print & P_TTRIP) pttrip(head, count, print);

/*  Count the number of triggered records  */

    if (head->mode != 0) {
        ++count->trgrec;
        trig = TRUE;
    } else {
        trig = FALSE;
    }

/*  Insure number of samples is reasonable and add to stream total  */

    if ((check & C_NSAMP) && !(nsamp_ok = ida_sampok(head))) {
        prt_badd_hdr(print, &tagno); tag(tagno++);
        printf("%8ld   ", count->rec);
        printf("illegal nsamp =%4hd for ", head->nsamp);
        printf("format = ");
        switch (head->form) {
          case S_UNCOMP:  printf("S_UNCOMP, ");  break;
          case L_UNCOMP:  printf("L_UNCOMP, ");  break;
          case IGPP_COMP: printf("IGPPCOMP, "); break;
          default:        printf("UNKNOWN!, ");  break;
        }
        printf("wrdsiz = %2d, ", head->wrdsiz);
        printf("nbytes = %3d\n", head->nbytes);
        if (!bad) {
            bad = TRUE;
            ++count->badd;
        }
    } else {
        nsamp_ok = TRUE;
    }
    if (nsamp_ok) count->totpts[head->dl_stream] += head->nsamp;

/*  Report on internal time tag structure errors  */

    if ((check & C_BADTAG) && head->beg.error) {
        ++count->badtag;
        if (print & P_BADTAG) pbadtag(&head->beg,count,IDA_DATA,print);
        time_ok = FALSE;
    } else {
        time_ok = TRUE;
    }

    if ((check & C_BADTAG) && head->end.error) {
        ++count->badtag;
        if (print & P_BADTAG) pbadtag(&head->end,count,IDA_DATA,print);
        time_ok = FALSE;
    } else {
        time_ok = TRUE;
    }

/*  Look for time quality transitions  */

    if (check & C_TQUAL) cktqual(head, count, print);

/*  Look for external time jumps  */

    if ((check & C_TINC) && time_ok) ckextjmp(head,count,print);

/*  Check for intra-record time jumps (valid only after rev 5) */

    if ((check & C_TINC) && ida->rev.value >= 5 && time_ok && nsamp_ok) {
        if (head->beg.sys >= head->end.sys) {
            prt_badd_hdr(print, &tagno); tag(tagno++);
            printf("%8ld   ", count->rec);
            printf("beg sys_time ");
            printf("(%s) ",  sys_timstr(head->beg.sys, head->beg.mult, NULL));
            printf(">= end sys_time ");
            printf("(%s)\n", sys_timstr(head->end.sys, head->end.mult, NULL));
            if (!bad) {
                bad = TRUE;
                ++count->badd;
            }
        } else {
            jump = cktinc(head,NULL,count->rec,-1L,sort,group,print);
            if (jump != 0) {
                ++count->ierr;
                if (jump <= -BIG_TIMERR) ++count->nerr;
                if (jump >=  BIG_TIMERR) ++count->perr;
            }
        }
    }

/* check for true time jumps */

    if (check & C_TTC) chktrut(head);

/*  Look for DAS reboots */

    if (HAVEPREV && head->beg.sys < PRVH.beg.sys) {
        reboot = 1;
        ++count->reboots;
    } else {
        reboot = 0;
    }

/*  Check for inter-record time jumps  */

    if (
        (check & C_TINC) && 
        time_ok && nsamp_ok && HAVEPREV && 
        !trig && !bad && !reboot
    ) {
        jump = cktinc(head,&PRVH,count->rec,PRVR,sort,group,print);
        if (jump != 0) {
            ++count->ierr;
            if (jump <= -BIG_TIMERR) ++count->nerr;
            if (jump >=  BIG_TIMERR) ++count->perr;
        }
    }

/*  Check for time offset errors  */

    if ((check & C_TOFF) && ida->rev.value >= 5 && time_ok && nsamp_ok) {
        if (cktoff(head, count->rec, print) != 0) ++count->oerr;
    }

/*  Look for compression/decompression errors  */

    if ((check & C_LASTW) && head->form == IGPP_COMP && nsamp_ok) {
        cklastw(&drec, count, print);
    }

/*  Save this record header and number  */

    savehead(&PRVH, head, &PRVR, count);
}

/* Revision History
 *
 * $Log: dodata.c,v $
 * Revision 1.7  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.6  2006/03/24 18:38:44  dechavez
 * true time tag test support added (akimov)
 *
 * Revision 1.5  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.4  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.3  2003/12/10 06:31:20  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
