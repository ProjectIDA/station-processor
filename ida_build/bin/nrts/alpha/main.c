#pragma ident "$Id: main.c,v 1.5 2008/09/11 16:40:59 dechavez Exp $"
/*======================================================================
 *
 *  NRTS interface to GSETT-3 Alpha Protocol (buffered mode).
 *
 *  Assumes single station per NRTS system.
 *
 *====================================================================*/
#include "nrts_alpha.h"

#ifdef USE_LIBDSS
    DSS_HANDLE *DSS_Handle;
#else
#   define DSS_Handle NULL
#endif

main(argc, argv)
int  argc;
char *argv[];
{
int i, nwait, nrec, nchan, count;
AlphaConn  *conn;
AlphaChan  *chan;
AlphaFrame *frame;
int nap = DEF_NAP;
int index[NRTS_MAXCHN];
int status[NRTS_MAXCHN];
int auth;
float increment;
struct static_info static_info;
struct nrts_packet *packet;
double default_start;
double next_time;
double prev_time = -1.0;
double start     = -1.0;
double last_beg  = -1.0;
char *admin;
time_t lastwrite, delay;
struct notify notify;
static char buffer[512];

    conn = init(argc, argv,
        &nchan, index, &static_info, &nap, &default_start, &notify
    );

    if (conn == NULL) exit(1);

#ifdef USE_LIBDSS
    auth = (DSS_Handle == (DSS_HANDLE *) NULL) ? AUTH_OFF : AUTH_ON;
#else
    auth = AUTH_OFF;
#endif

    lastwrite = time(NULL);
    notify.flag = 0;
    count = 0;

    while(1) {

        util_log(3, "start of main loop, count=%d", count);

    /* Get start time of next frame to send */

        next_time = AlphaGetNextTime(conn);

        if (next_time < 0.0) {
            ++count;
            util_log(3, "AlphaGetNextTime has no opinion, count = %d", count);
            if (last_beg > 0.0) {
                next_time = last_beg + (static_info.increment * (double) count);
                util_log(1, "request forced to %s",
                    util_dttostr(next_time, 0)
                );
                last_beg = next_time;
            } else {
                next_time = default_start;
                util_log(1, "request time set to end of native disk loop (%s)",
                    util_dttostr(next_time, 0)
                );
            }
            prev_time = next_time;
        } else if (next_time == prev_time) {
            ++count;
            util_log(3, "AlphaGetNextTime repeats request for %s, count = %d",
                util_dttostr(next_time, 0), count
            );
            increment = static_info.increment * (double) count;
            if (last_beg > 0.0) {
                next_time = last_beg + increment;
                util_log(3, "request time incremented by %.3lf sec from last_beg to %s",
                    increment, util_dttostr(next_time, 0)
                );
                if (next_time < prev_time) {
                    util_log(1, "Huh? next packet time must be wrong");
                    next_time = prev_time + static_info.increment;
                    util_log(1, "request time reset to next packet (%s)",
                        util_dttostr(next_time, 0)
                    );
                }
                last_beg = next_time;
            } else {
                next_time += increment;
                util_log(3, "request time incremented by %.3lf sec from next_time to %s",
                    increment, util_dttostr(next_time, 0)
                );
            }
        } else {
            util_log(3, "AlphaGetNextTime requests %s", util_dttostr(next_time, 0));
            prev_time = next_time;
        }

        frame = AlphaAllocFrame(conn, next_time);
        util_log(5, "AlphaAllocFrame(.., %s)", util_dttostr(next_time, 0));

    /* Read required data from native disk loop and fill frame */

        for (i = 0; i < nchan; i++) status[index[i]] = UNKNOWN;

        start = (double) -1;
        nwait = nchan;
        nrec  = 0;

        while (nwait > 0) {

            delay = (time(NULL) - lastwrite);
            util_log(3, "nwait=%d, nrec=%d, delay=%d", nwait, nrec, delay);
            if (notify.flag == 0) {
                if (delay > notify.time) {
                    util_log(1, "no data written for over %d seconds",
                        notify.time
                    );
                    util_log(1, "sending email to %s", notify.address);
                    util_email(notify.address, notify.subject, notify.fname);
                    notify.flag = 1;
                } else {
                    util_log(4, "delay %d is less than %d... no email sent",
                        delay, notify.time
                    );
                }
            } else {
                util_log(3, "delay = %d, notification already sent", delay);
            }

            for (i = 0; i < nchan; i++) {
                if (status[index[i]] == UNKNOWN) {
                    status[index[i]] = getrec(index[i], next_time, &packet);
                    if (status[index[i]] == PRESENT) {
                        sprintf(buffer, "%s:%s %.3f %d %d beg=%s",
                            packet->sname, packet->cname,
                            packet->sint, packet->nsamp, packet->wrdsiz,
                            util_dttostr(packet->beg.time, 0)
                        );
                        sprintf(buffer+strlen(buffer), " end=%s",
                            util_dttostr(packet->end.time, 0)
                        );
                        util_log(2, "%s", buffer);
                        if (changed(packet, &static_info)) {
                            util_log(1, "abort: channel constants changed");
                            util_email(
                                notify.address, 
                                "nrts_alpha aborted",
                                NULL
                            );
                            die(1);
                        }
                        if (start < (double) 0) start = packet->beg.time;
                        if (packet->beg.time != start) {
                            sprintf(buffer,"wrong %s:%s start time, ",
                                packet->sname, packet->cname
                            );
                            sprintf(buffer+strlen(buffer), "expected %s ",
                                util_dttostr(start, 0)
                            );
                            sprintf(buffer+strlen(buffer), "but got %s", 
                                util_dttostr(packet->beg.time, 0)
                            );
                            util_log(1, "%s", buffer);
                            util_log(1, "packet dropped");
                        } else {
                            chan = AlphaGetAuthChannel(
                                conn,
                                static_info.site_name,
                                packet->cname,
                                static_info.samprate,
                                auth,
                                static_info.itype,
                                static_info.otype
                            );
                            util_log(5, "AlphaGetAuthChannel(...,%s,%s,%.2f,%d,%s,%s)",
                                static_info.site_name,
                                packet->cname,
                                static_info.samprate,
                                auth,
                                static_info.itype,
                                static_info.otype
                            );
                            AlphaSetFrameAuthChannel(
                                frame, chan, DSS_Handle, packet->beg.time, packet->data,
                                static_info.nsamp, static_info.len, 0
                            );
                            util_log(5, "AlphaSetFrameAuthChannel(..., %s, ...)",
                                util_dttostr(packet->beg.time, 0)
                            );
                            last_beg = packet->beg.time;
                            ++nrec;
                        }
                        --nwait;
                    } else if (status[index[i]] == ABSENT) {
                        --nwait;
                    }
                }
            }
            if (nwait > 0 && nap > 0) {
                util_log(3, "sleep(%d)", nap);
                sleep(nap);
            }
        }

    /* Send the frame */

        if (nrec > 0) {
            util_log(3, "%d records available", nrec);
            if (AlphaSendFrame(frame) < 0) {
                util_log(1, "abort: AlphaSendFrame: %s", syserrmsg(errno));
                util_email(notify.address, "nrts_alpha aborted", NULL);
                die(1);
            } else {
                util_log(5, "AlphaSendFrame OK");
                lastwrite = time(NULL);
                notify.flag = 0;
            }
            if (nrec != nchan) {
                util_log(2, "partial frame sent (%d/%d channels)", nrec, nchan);
            }
            count = 0;
        } else {
            util_log(3, "no data available for requested start time, count=%d", count);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.5  2008/09/11 16:40:59  dechavez
 * improved debug messages, fixed infinite loop bug in main loop
 *
 * Revision 1.4  2001/04/23 15:29:36  dec
 * use static increment instead of new calc of same when forcing next time
 *
 * Revision 1.3  2001/01/31 22:53:08  dec
 * fixed bug with status index
 *
 * Revision 1.2  2000/03/09 19:58:53  dec
 * Added support for Luna2, Makefile left for unauthenticated builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
