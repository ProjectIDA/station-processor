#pragma ident "$Id: summary.c,v 1.4 2006/03/24 18:38:44 dechavez Exp $"
/*======================================================================
 *
 *  End of run summary.
 *
 *====================================================================*/
#include "idalst.h"

void summary(int read_status, struct counters *count, IDA_DHDR *first, IDA_DHDR *last, int print, int check)
{
int i, good_data;
long total, tagno = SUM_TAG;
ISI_STREAM_NAME name;

/* Flush log record listing */

    plog((IDA_LOG_REC *) NULL, count, print);

/*  Print count of each record type  */

    total = count->ident + count->log + count->calib + count->config
          + count->event + count->posthdr + count->oldhdr + count->resvd
          + count->data  + count->dcc;

    tagno = SUM_TAG;
    tag(tagno++); printf("\n");
    if (total) {
        tag(tagno++); printf("RECOGNIZED RECORD COUNT\n");
        if (count->dcc) {
            tag(tagno++);
            printf("no. DCC applied records%6ld\n", count->dcc);
        }
        if (count->ident) {
            tag(tagno++);
            printf("no. ident records      %6ld\n", count->ident);
        }
        if (count->log) {
            tag(tagno++);
            printf("no. log records        %6ld\n", count->log);
        }
        if (count->calib) {
            tag(tagno++);
            printf("no. calib records      %6ld\n", count->calib);
        }
        if (count->config) {
            tag(tagno++);
            printf("no. config records     %6ld\n", count->config);
        }
        if (count->event) {
            tag(tagno++);
            printf("no. event records      %6ld\n", count->event);
        }
        if (count->posthdr) {
            tag(tagno++);
            printf("no. post-header records%6ld\n", count->posthdr);
        }
        if (count->oldhdr) {
            tag(tagno++);
            printf("no. header records     %6ld\n", count->oldhdr);
        }
        if (count->resvd) {
            tag(tagno++);
            printf("no. reserved records   %6ld\n", count->resvd);
        }
        if (count->srec) {
            tag(tagno++);
            printf("no. DAS status records %6ld\n", count->srec);
        }
        if (count->isplog) {
            tag(tagno++);
            printf("no. ISP log records    %6ld\n", count->isplog);
        } else {
            unlink(Global->isplog);
        }
        if (count->data) {
            tag(tagno++);
            printf("no. data records       %6ld ",  count->data);
            printf("(%ld ARS + %ld DAS)", count->ars, count->das);
            if (count->data != (count->ars + count->das)) {
                printf(" (UNACCOUNTED FOR DATA RECORDS!)");
            }
            if (count->dunmap) {
                printf(" (%ld w/o internal stream map)", count->dunmap);
            }
            printf("\n");
        }
        tag(tagno++); printf("                       ------\n",  total);
        tag(tagno++); printf("Total                  %6ld\n", total);
    } else {
        tag(tagno++); printf("*** NO RECOGNIZED RECORDS FOUND ***\n");
    }

/*  Exit status  */

    tag(tagno++); printf("\n"); tag(tagno++);
    if (read_status == READ_EOF) {
        printf("End-of-data encountered after reading ");
        printf("%ld records.\n", count->rec);
    } else if (read_status == READ_ERR) {
        printf("Read error at record %ld.\n", count->rec);
    } else if (read_status == READ_DONE) {
        printf("%ld records processed.\n", count->rec);
    } else if (read_status == USER_ABORT) {
        printf("Terminated by user after reading about ");
        printf("%ld records.\n", count->rec);
    } else {
        printf("Read routine exited for UNKNOWN reason at ");
        printf("record %d.\n", count->rec);
    }

/*  Total error count  */

    total = count->ierr   + count->exterr + count->notdef +
            count->dupadj + count->dupdat + count->badtag +
            count->iltq   + count->lastw  + count->badd   +
            count->dunmap + count->corrupt+ count->oerr;

    tag(tagno++); printf("\n");
    if (total == 0) { 
        tag(tagno++);
        printf("NO ERRORS DETECTED.\n");
    } else {
        tag(tagno++); 
        printf("Detected %d errors:\n",total);
    }

/*  Corrupt records  */

    if (count->corrupt) {
        tag(tagno++);
        printf("no. corrupt records (all types)   = ");
        printf("%ld\n", count->corrupt);
    }

    if (count->badd) {
        tag(tagno++);
        printf("no. corrupt data records          = ");
        printf("%ld\n", count->badd);
    }

/*  Corrupt time tags  */

    if (count->badtag) {
        tag(tagno++);
        printf("no. corrupt time tags             = ");
        printf("%ld\n", count->badtag);
    }

/*  Time stamp increment errors  */

    if (count->ierr) {
        tag(tagno++);
        printf("no. time stamp increment errors   = %ld", count->ierr);
        if (count->perr || count->nerr) {
            printf(" (%ld > +%d samples",  count->perr, BIG_TIMERR);
            printf(", %ld > -%d samples)", count->nerr, BIG_TIMERR);
        }
        printf("\n");
    }

    if (count->oerr) {
        tag(tagno++);
        printf("no. time offset errors            = ");
        printf("%ld\n", count->oerr);
    }

    if (count->exterr) {
        tag(tagno++);
        printf("no. external time stamp jumps     = ");
        printf("%ld\n", count->exterr);
    }

/*  Unrecognized errors  */

    if (count->notdef) {
        tag(tagno++);
        printf("no. unrecognized records          = ");
        printf("%ld\n", count->notdef);
    }

    if (count->dunmap) {
        tag(tagno++);
        printf("no. unmapped data records         = ");
        printf("%ld\n", count->dunmap);
    }

/*  Duplicate adjacent records  */

    if (count->dupadj) {
        tag(tagno++);
        printf("no. adjacent duplicate records    = ");
        printf("%ld\n", count->dupadj);
    }

/*  Duplicate data records (maybe or maybe not adjacent) */

    if (count->dupdat) {
        tag(tagno++);
        printf("no. duplicate data records        = ");
        printf("%ld\n", count->dupdat);
    }

/*  Illegal clock quality transitions  */

    cktqual(NULL, count, print);
    if (count->iltq) {
        tag(tagno++);
        printf("no. questionable clock qual jumps = ");
        printf("%ld\n", count->iltq);
    }

/*  Compression errors  */

    if (count->lastw) {
        tag(tagno++);
        printf("no. (de)compression errors        = ");
        printf("%ld\n", count->lastw);
    }

/*  List of encountered streams and their description  */

    for (good_data = 0, i = 0; i < IDA_MAXSTREAMS; i++) {
        good_data += count->stream[i];
    }

    if (!good_data) {
        tag(tagno++); printf("\n"); tag(tagno++);
        printf("*** NO RECOGNIZED DATA STREAMS FOUND ***\n");
        print &= ~P_STRMAP;
    }

    if (print & P_STRMAP) {
        tag(tagno++); printf("\n"); tag(tagno++);
        printf("ENCOUNTERED STREAM SUMMARY (map=`%s')\n", ida->mapname);
        tag(tagno++);
        printf("St  Ch Fl Tf  Count  Mapped name\n");
        for (i = 0; i < IDA_MAXSTREAMS; i++) {
            if (count->stream[i] > 0) {
                tag(tagno++);
                printf("%02d  %02d %02d %02d", i, last[i].dl_channel, last[i].dl_filter, last[i].mode);
                printf("%7ld  ", count->stream[i]);
                idaBuildStreamName(ida, &last[i], &name);
                printf("%s", name.chnloc);
                printf("\n");
            }
        }
    }

    if (check & C_TTC) printttglist();
}

/* Revision History
 *
 * $Log: summary.c,v $
 * Revision 1.4  2006/03/24 18:38:44  dechavez
 * true time tag test support added (akimov)
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:22  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
