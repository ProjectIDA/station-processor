#pragma ident "$Id: dispdas.c,v 1.11 2006/02/10 02:23:42 dechavez Exp $"
/*======================================================================
 *
 * Interactive display
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <curses.h>
#include <errno.h>
#include "isp.h"
#include "isp_console.h"

extern WINDOW *full_screen, *logtit, *logwin, *sohwin, *cmdwin;
extern int cx, nx;

extern char *home;
extern char *Syscode;

struct {
    int rel;
    int ver;
    int sub;
    int dsp_rev;
    int atod_id;
    int jumpers;
    int valid;
} dasinfo;

static void UpdateStatus(struct isp_status *status)
{
static int first = 1;
char c, c1, c2, c3;
int i, retval, x, y;
float percent;
long uptime, interval;
IDA_CONFIG_REC *cnf;
static char *tape = "tape";
static char *Tape = "Tape";
static char *disk = "disk";
static char *Disk = "Disk";
static char *huh  = "????";
char *media, *Media;
time_t now;
static time_t prev_time = 0;
static char buffer[MAXCOLS+1];
struct isp_params params;
 
    if (GetParams(&params, 0) == 0) {
        strcpy(Syscode, params.sta);
    } else {
        strcpy(Syscode, "???????");
    }
    util_lcase(Syscode);

    SetOutputMediaType(status->output.type);

    uptime  = status->tstamp - status->start;
    if (Pid() != status->pid) {
        wmove(sohwin, 0, 0);
        util_ucase(Syscode);
        sprintf(buffer, " %s Console <%s> ", Syscode, isp_release);

        for (i = 0; i < nx; i++) waddch(sohwin, '-'); wrefresh(sohwin);

        wmove(sohwin, 0, (nx/2) - (strlen(buffer)/2));
        waddstr(sohwin, buffer);

        SetPid(status->pid);

        chdir(home);
        chdir(util_lcase(Syscode));
        chdir(ISP_SUBDIR);
    }
 
    if (status == (struct isp_status *) NULL) return;
    if (status->output.type == ISP_OUTPUT_DISK) {
        media = disk;
        Media = Disk;
    } else if (status->output.type == ISP_OUTPUT_TAPE) {
        media = tape;
        Media = Tape;
    } else {
        media = huh;
        Media = huh;
    }

    cnf     = &status->config;

    mvwaddstr(sohwin, 1, nx - 21, util_lttostr(status->tstamp, 1));
    if (status->extclock.enabled) {
        mvwaddstr(sohwin, 2, nx - 17,
            util_lttostr(status->extclock.datum, 0)
        );
    } else {
        mvwaddstr(sohwin, 2, nx - 21, "DISABLED             ");
    }

    x = 0;
    y = 2;
    wmove(sohwin, y, x);
    wprintw(sohwin, "0x%04x", status->alarm.code);
    x += 8;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_DAS) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_DAS));
    if (status->alarm.code & ISP_ALARM_DAS) wstandend(sohwin);
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_OPER) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_OPER));
    if (status->alarm.code & ISP_ALARM_OPER) wstandend(sohwin);
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_IOERR) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_IOERR));
    if (status->alarm.code & ISP_ALARM_IOERR) wstandend(sohwin);
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_OFFLINE) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_OFFLINE));
    if (status->alarm.code & ISP_ALARM_OFFLINE) wstandend(sohwin);
    x += 4;
    c1 = isp_alrm_code(&status->alarm, ISP_ALARM_OQFULL);
    c2 = isp_alrm_code(&status->alarm, ISP_ALARM_BQFULL);
    c3 = isp_alrm_code(&status->alarm, ISP_ALARM_RQFULL);
    c = c1;
    c = (c1 > c2) ? c1 : c2;
    if (c3 > c) c = c3;
    wmove(sohwin, y, x);
    if (c != '0') wstandout(sohwin);
    wprintw(sohwin, "%c", c);
    if (c != '0') wstandend(sohwin);
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_AUX) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_AUX));
    if (status->alarm.code & ISP_ALARM_AUX) wstandend(sohwin);
    x += 4;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_DASDBG));
    x += 4;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_DASCLK));
    x += 4;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_AUXCLK));
    if (params.nrts) {
        x += 4;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_NRTS));
    } else {
        x += 3;
        wmove(sohwin, y, x);
        wprintw(sohwin, "n/a");
    }

    y += 2;
    x = strlen("Uptime: ");
    wmove(sohwin, y, x);
    wprintw(sohwin, "%s", util_lttostr(uptime, 8));

    if (status->last.change > 0) {
        interval = status->tstamp - status->last.change;
        if (status->output.type == ISP_OUTPUT_TAPE) {
            wprintw(sohwin, "  Last tape change: %s",
                util_lttostr(status->last.change, 0)
            );
            wprintw(sohwin, " (%s)", util_lttostr(interval, 8));
        } else if (status->output.type == ISP_OUTPUT_CDROM) {
            wprintw(sohwin, "  Last Archive Dump: %s",
                util_lttostr(status->last.change, 0)
            );
            wprintw(sohwin, " (%s)", util_lttostr(interval, 8));
        }
    } else {
        wclrtoeol(sohwin);
    }

    y++;
    wmove(sohwin, y, x=0);
    if (status->output.type == ISP_OUTPUT_TAPE) {
        wprintw(sohwin, "Output Buffer: ");
        if (status->buffer.nrec == -12345) {
            wstandout(sohwin);
            wprintw(sohwin, "NONE AVAILABLE!");
            wstandend(sohwin);
        } else {
            percent = (float) (status->buffer.nrec * 100) /
                      (float)  status->buffer.capacity;
            wprintw(sohwin, "%3d%% (%7ld) ",
                (int) percent, status->buffer.nrec
            );
        }
    
        x += 31;
        wmove(sohwin, y, x);
        if (status->output.state == ISP_OUTPUT_ONLINE) {
            if (media == disk) {
                wprintw(sohwin, "%s: %7ld recs",
                    Media, status->output.nrec
                );
                percent = (float) (status->output.nrec * 100) /
                          (float) status->output.capacity;
                wprintw(sohwin, "  Capacity: %3d%%", (int) percent);
            } else if (media == tape) {
                if (status->output.file == 1) {
                    wprintw(sohwin, "%s: %7ld recs",
                        Media, status->output.nrec
                    );
                } else {
                    wprintw(sohwin, "%s (file %d): %7ld recs",
                        Media, status->output.file, status->output.nrec
                    );
                }
                if (WantIntervals() && status->last.write > 0) {
                    interval = status->tstamp - status->last.write;
                    wprintw(sohwin, "  Last write: %s",
                        util_lttostr(interval, 8)
                    );
                } else {
                    wclrtoeol(sohwin);
                }
            }
        } else if (status->output.state == ISP_OUTPUT_CHECK) {
            wmove(sohwin, y, x);
            wprintw(sohwin, "%s: checking %s for data",
                Media, status->output.device
            );
        } else if (status->output.state == ISP_OUTPUT_OPER) {
            wmove(sohwin, y, x);
            wstandout(sohwin);
            if (Operator()) {
                wprintw(sohwin, "%s contains data(!): eject | append | ERASE ?",
                    Media
                );
            } else {
                wprintw(sohwin, "%s: WAITING FOR OPERATOR INPUT",
                    Media
                );
            }
            wstandend(sohwin);
        } else if (status->output.state == ISP_OUTPUT_SKIP) {
            wmove(sohwin, y, x);
            wprintw(sohwin, "%s: %s skipping to end of data",
                Media, status->output.device
            );
        } else if (status->output.state == ISP_OUTPUT_OFFLINE) {
            wmove(sohwin, y, x);
            wstandout(sohwin);
            wprintw(sohwin, "%s: NO MEDIA PRESENT AT %s",
                Media, status->output.device
            );
            wstandend(sohwin);
        } else {
            wmove(sohwin, y, x);
            wstandout(sohwin);
            wprintw(sohwin, "%s: %s offline",         
                Media, status->output.device
            );
            wstandend(sohwin);
        }
    } else if (status->output.type == ISP_OUTPUT_CDROM) {
        wprintw(sohwin, "CD-R: ");
        if (status->buffer.nrec > -1) {
            percent = (float) (status->buffer.nrec * 100) /
                      (float)  status->buffer.capacity;
            wprintw(sohwin, 
                "%d Mbytes (%d%%) staged in %d files "
                "[ %d ISO %s ready to burn ]",
                status->buffer.nrec,
                (int) percent,
                status->output.file,
                status->output.nrec,
                status->output.nrec == 1 ? "image" : "images"
            );
        }
    } else {
        wprintw(sohwin, "Output buffering suppressed");
    }
    wclrtoeol(sohwin);

    y += 2;
    x = 17;
    wmove(sohwin, y, x);
    if (WantQueue()) {
        wprintw(sohwin, " | Heap: B=%d/%d C=%d/%d O=%d/%d D=%d/%d",
            status->heap.barometer.nfree, status->heap.barometer.lowat,
            status->heap.commands.nfree,  status->heap.commands.lowat,
            status->heap.obuf.nfree,      status->heap.obuf.lowat,
            status->heap.packets.nfree,   status->heap.packets.lowat
        );
    } else if (WantIntervals()) {
       if (status->cleared.comm > 0) {
           interval = status->tstamp - status->cleared.comm;
           wprintw(sohwin, " (%s)", util_lttostr(interval, 8));
       }
    }
    wclrtoeol(sohwin);

    y++;
    x = 65;
    wmove(sohwin, y, x);
    if (status->last.commerr > 0) {
        wprintw(sohwin, "Last Error");
    } else {
        wprintw(sohwin, "          ");
    }

    y++;
    x = 0;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.badr);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.sync);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.shrt);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.naks);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.dups);
    x += 7;
    wmove(sohwin, y, x);
    if (status->last.read > 0) {
        interval = status->tstamp - status->last.read;
        wprintw(sohwin, " %s ", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "              ");
    }
    x += 14;
    wmove(sohwin, y, x);
    if (status->last.pkt > 0) {
        interval = status->tstamp - status->last.pkt;
        wprintw(sohwin, " %s", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "              ");
    }
    x += 14;
    wmove(sohwin, y, x);
    if (status->last.commerr > 0) {
        interval = status->tstamp - status->last.commerr;
        wprintw(sohwin, " %s", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "              ");

    }

    y += 2;
    if (!WantQueue()) {
        if (WantIntervals()) {
            x = 18;
            wmove(sohwin, y, x);
            if (status->cleared.pkt > 0) {
                interval = status->tstamp - status->cleared.pkt;
                wprintw(sohwin, "(%s)", util_lttostr(interval, 8));
            } else {
                wclrtoeol(sohwin);
            }
        } else {
            wclrtoeol(sohwin);
        }
        x = 34;
        wmove(sohwin, y, x);
        wprintw(sohwin, "Trg: ");
        if (status->have_config && cnf->detect.chan == 0) {
            wprintw(sohwin, "n/a      ");
        } else if (status->flag.event) {
            wstandout(sohwin);
            wprintw(sohwin, "ON");
            wstandend(sohwin);
            wprintw(sohwin, "       ");
        } else {
            wprintw(sohwin, "off      ");
        }

        x = 48;
        wmove(sohwin, y, x);
        wprintw(sohwin, "Cal: ");
        if (status->flag.calib == ISP_CALIB_OFF) {
            wprintw(sohwin, "off      ");
        } else if (status->flag.calib == ISP_CALIB_PENDING) {
            wstandout(sohwin);
            wprintw(sohwin, "pending");
            wstandend(sohwin);
            wprintw(sohwin, "  ");
        } else if (status->flag.calib == ISP_CALIB_PENDING_AT_ISP) {
            wstandout(sohwin);
            wprintw(sohwin, "pending*");
            wstandend(sohwin);
            wprintw(sohwin, " ");
        } else if (status->flag.calib == ISP_CALIB_ON) {
            wstandout(sohwin);
            wprintw(sohwin, "ON");
            wstandend(sohwin);
            wprintw(sohwin, "       ");
        } else {
            wprintw(sohwin, "%d?       ", status->flag.calib);
        }
 
        x = 62;
        wmove(sohwin, y, x);
        wprintw(sohwin, "Cnf: ");
        if (status->flag.config == ISP_CONFIG_UNKNOWN) {
            wstandout(sohwin);
            wprintw(sohwin, "unknown");
            wstandend(sohwin);
        } else if (status->flag.config == ISP_CONFIG_INPROGRESS) {
            wstandout(sohwin);
            wprintw(sohwin, "in progress");
            wstandend(sohwin);
        } else if (status->flag.config == ISP_CONFIG_VERIFIED) {
            wprintw(sohwin, "verified");
        } else {
            wstandout(sohwin);
            wprintw(sohwin, "%d?", status->flag.config);
            wstandend(sohwin);
        }
        wclrtoeol(sohwin);
    } else {
        x = 17;
        wmove(sohwin, y, x);
        wprintw(sohwin, " |  Q's: B=%d/%d C=%d/%d O=%d/%d Dr=%d/%d Dp=%d/%d Do=%d/%d Dn=%d/%d",
            status->queue.baro.npend,        status->queue.baro.hiwat,
            status->queue.das_write.npend,   status->queue.das_write.hiwat,
            status->queue.obuf.npend,        status->queue.obuf.hiwat,
            status->queue.das_read.npend,    status->queue.das_read.hiwat,
            status->queue.das_process.npend, status->queue.das_process.hiwat,
            status->queue.massio.npend,      status->queue.massio.hiwat,
            status->queue.nrts.npend,        status->queue.nrts.hiwat
        );
    }
    wclrtoeol(sohwin);

    y += 2;
    x = 0;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%7ld ", status->count.drec);
    x += 8;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.irec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.krec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.crec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.lrec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.erec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.orec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.urec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%6ld ", status->count.arec);
    x += 7;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%7ld ", status->count.srec);
    x += 8;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%7ld", status->count.nrec);

    y += 2;
    if (WantDasStat()) {
        x = strlen("Last DAS tstamp: ");
        wmove(sohwin, y, x);
        if (status->clock > 0) {
            wprintw(sohwin, "%s", util_lttostr(status->clock, 0));
            x += 29;
            wmove(sohwin, y, x);
            if (status->clock_qual == 1) {
                wprintw(sohwin, "Locked  ");
            } else if (status->clock_qual > 6) {
                wprintw(sohwin, "Unknown ");
            } else if (status->clock_qual < 0) {
                wprintw(sohwin, "ERROR   ");
            } else {
                wprintw(sohwin, "Unlocked");
            }
        } else {
            wprintw(sohwin, "                          ");
            x += 29;
            wmove(sohwin, y, x);
            wprintw(sohwin, "        ");
        }
        y += 2;
        x = 0;
        wmove(sohwin, y, x);
        if (status->das.start > 0) {
            interval = status->tstamp - status->das.start;
            wprintw(sohwin, "%s ", util_lttostr(interval, 8));
        } else {
            wprintw(sohwin, "             ");
        }
        x += 13;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%5ld ", status->das.reboots);
        x += 6;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%8ld ", status->das.acks);
        x += 9;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%6ld ", status->das.dropped);
        x += 7;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%6ld ", status->das.naks);
        x += 7;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%6ld ", status->das.timeouts);
        x += 7;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%6ld ", status->das.triggers);
        x += 7;
        wmove(sohwin, y, x);
        if (status->last.dasstat > 0) {
            interval = status->tstamp - status->last.dasstat;
            wprintw(sohwin, " %s", util_lttostr(interval, 8));
        } else {
            wprintw(sohwin, "             ");
        }
    } else {
        if (status->extclock.enabled) {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%s", util_lttostr(status->extclock.datum, 0));
            x = 40;
            wmove(sohwin, y, x);
            if (status->extclock.qual == 1) {
                wprintw(sohwin, "Locked  ");
            } else if (status->extclock.qual > 6) {
                wprintw(sohwin, "Unknown ");
            } else if (status->extclock.qual < 0) {
                wprintw(sohwin, "ERROR   ");
            } else {
                wprintw(sohwin, "Unlocked");
            }
            x = 58;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%ld", status->extclock.err);
        } else {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "DISABLED");
            wclrtoeol(sohwin);
        }
        y++;
        if (status->baro.enabled) {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%8.3f", (float) status->baro.datum / 1000.0);
            x = 25;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->baro.nrec);
            x = 37;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->baro.rerr);
            x = 49;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->baro.miss);
            x = 63;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->baro.drop);
        } else {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "DISABLED");
            wclrtoeol(sohwin);
        }
        y++;
        if (status->dpm.enabled) {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%8.2f", (float) status->dpm.datum / params.dpm.multiplier);
            x = 25;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->dpm.nrec);
            x = 37;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->dpm.rerr);
            x = 49;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->dpm.miss);
            x = 63;
            wmove(sohwin, y, x);
            wprintw(sohwin, "%6ld", status->dpm.drop);
        } else {
            x = 11;
            wmove(sohwin, y, x);
            wprintw(sohwin, "DISABLED");
            wclrtoeol(sohwin);
        }
    }

    wmove(cmdwin, 0, cx);

    if (
        (status->alarm.code & ISP_ALARM_DAS)     ||
        (status->alarm.code & ISP_ALARM_OPER)    ||
        (status->alarm.code & ISP_ALARM_IOERR)   ||
        (status->alarm.code & ISP_ALARM_OFFLINE) ||
        (status->alarm.code & ISP_ALARM_OQFULL)  ||
        (status->alarm.code & ISP_ALARM_BQFULL)  ||
        (status->alarm.code & ISP_ALARM_RQFULL)  ||
        (status->alarm.code & ISP_ALARM_AUX)
    ) {
        SoundBeep(BEEP_REQUEST);
    }

    wrefresh(sohwin);
    wrefresh(cmdwin);
}

static void DisplayDasPar(struct isp_params *params)
{
int y, x;
static char *title = "Current ISPD run-time parameters";
static char *flowstr[4] = {
    "????", "hard", "soft", "none"
};
#define flow(flag) ((flag >= 1 && flag <= 3) ? flowstr[flag] : flowstr[0])

    x = 0;
    y = 1;

    wmove(sohwin, y, 0);
    wclrtobot(sohwin);    
    y += 1;
    x = nx/2 - strlen(title)/2;
    mvwaddstr(sohwin, y, x, title);
    x = 0;

    y += 2; wmove(sohwin, y, x);
    wprintw(sohwin, "IDA data Rev: %d", params->pktrev);

    y += 2; wmove(sohwin, y, x);
    wprintw(sohwin, " Device      Port        Ibaud  Obaud  Flow  Timeout");
    
    y += 1; wmove(sohwin, y, x);
    wprintw(sohwin, "%-10s  %-11s %6d %6d  %s  %d/%d/%d", "DAS uplink",
        params->port, params->ibaud, params->obaud, flow(TTYIO_FNONE),
        params->ttyto, params->nodatato, params->iddatato
    );

    if (params->baro.enabled) {
        y += 1; wmove(sohwin, y, x);
        wprintw(sohwin, "%-10s  %-11s %6d %6d  %s  default", "Barometer",
            params->baro.port, params->baro.baud, params->baro.baud,
            flow(params->baro.flow)
        );
    }

    if (params->clock.enabled) {
        y += 1; wmove(sohwin, y, x);
        wprintw(sohwin, "%-10s  %-11s %6d    n/a  %s  default", "GPS Clock",
            params->clock.port, params->clock.baud, flow(params->clock.flow)
        );
    }

    if (params->baro.enabled) {
        y += 2; wmove(sohwin, y, x);
        wprintw(sohwin, "Barometer stream=%d filt=%d chan=%d sint=%d",
            params->baro.strm, params->baro.filt, params->baro.chan,
            params->baro.sint
        );
    }

    y += 2; wmove(sohwin, y, x);
    wprintw(sohwin, "Output device: %s", params->odev);

    if (OutputMediaType() == ISP_OUTPUT_TAPE) {
        y += 1; wmove(sohwin, y, x);
        wprintw(sohwin, "%d buffers of %d records each (bfact = %d, obuf = %d)",
            params->numobuf, params->bfact * params->obuf,
            params->bfact, params->obuf
        );
    } else if (OutputMediaType() == ISP_OUTPUT_CDROM) {
        wprintw(sohwin, " (%d MB capacity)", params->bfact);
    }

    y += 2; wmove(sohwin, y, x);
    if (dasinfo.valid) {
        if (dasinfo.sub < 0) {
            wprintw(sohwin, "DAS: Rev V%d.%d/%d, CPUID %d, jumpers 0x%0x",
                dasinfo.rel, dasinfo.ver, dasinfo.dsp_rev, dasinfo.atod_id,
                dasinfo.jumpers
            );
        } else {
            wprintw(sohwin, "DAS: Rev V%d.%d.%d/%d, CPUID %d, jumpers 0x%0x",
                dasinfo.rel, dasinfo.ver, dasinfo.sub, dasinfo.dsp_rev,
                dasinfo.atod_id, dasinfo.jumpers
            );
        }
    } else {
        wprintw(sohwin, "DAS: data unavailable");
    }

    y += 2; wmove(sohwin, y, x);
    wprintw(sohwin, "NRTS: %s", params->nrts ? "enabled" : "DISABLED");

    wrefresh(sohwin);
}

static void DisplayDasCnf(struct isp_dascnf *cnf)
{
int i, x, y, tst, save;
char *chan, *filt, *name, *mode;
static char *title  = "Current DAS Configuration";
static char *header = 
"St Chan Filt Mode Name    St Chan Filt Mode Name       Detector       ";
static char *fid = "DisplayDasCnf";
 
    wmove(sohwin, 1, 0);
    wclrtobot(sohwin);

    y = 2;
    x = nx/2 - strlen(title)/2;
    mvwaddstr(sohwin, y, x, title);

    save = (y += 2) + 1;
    x = nx/2 - strlen(header)/2;
    mvwaddstr(sohwin, y, x, header);
    for (i = 0, y = save; i < 12; i++, y++) {
        wmove(sohwin, y, x);
        isp_dascnf_text(cnf, i, &chan, &filt, &name, &mode);
        wprintw(sohwin, "%2d %s %s %s %-5s", i, chan, filt, mode, name);
    }

    x += strlen("St Chan Filt Mode Name    ");
    for (i = 12, y = save; i < 25; i++, y++) {
        wmove(sohwin, y, x);
        isp_dascnf_text(cnf, i, &chan, &filt, &name, &mode);
        wprintw(sohwin, "%2d %s %s %s %-5s", i, chan, filt, mode, name);
    }

    y  = save;
    x += strlen("St Chan Filt Mode Name   ");
    wmove(sohwin, y, x);
    if (cnf->detect.bitmap != 0) {
        wprintw(sohwin, "STA     = %d", cnf->detect.sta);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "LTA     = %d", cnf->detect.lta);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "Thresh  = %.2f",
            10000.0 / (float) cnf->detect.thresh
        );
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "Voters  = %d", cnf->detect.voters);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "MaxTrig = %d", cnf->detect.maxtrig);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "PreBuf  = %d", cnf->detect.preevent);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "Chan    = 0x%0x", cnf->detect.bitmap);
        y++; wmove(sohwin, y, x);
        wprintw(sohwin, "KeyChan = %d", cnf->detect.key);
    } else {
        wprintw(sohwin, "Disabled");
    }

    wrefresh(sohwin);
}

void InitDasSoh()
{
int y, x;

    x = 0;
    y = 1;

    wmove(sohwin, y, 0);
    wclrtobot(sohwin);    
 
    mvwaddstr(sohwin, y, 0,
        "Alarms DAS OPR IOE OFF OVR AUX DBG DCL ACL NRT"
    );
    mvwaddstr(sohwin, y, nx - 34, "  ISP Clock: ");

    ++y;    
    mvwaddstr(sohwin, y, nx - 34, "  GPS Clock: ");
 
    y += 2;
    mvwaddstr(sohwin, y, 0, "Uptime: ");
    wclrtoeol(sohwin);

    ++y;
    /* mvwaddstr(sohwin, y, 0, "Output Buffer:    %  (       )"); */
    wclrtoeol(sohwin);

    y += 2;
    wmove(sohwin, y, 0);
    wprintw(sohwin, "UPLINK STATISTICS ");
    ++y;
    mvwaddstr(sohwin, y, 0,
        "BadPkt NoSync  Short    NAK DupPkt   Last Read    Last Packet"
    );
    y++;

    y += 2;
    mvwaddstr(sohwin, y,  0, "PACKETS RECEIVED");
    wclrtoeol(sohwin);
    ++y;
    mvwaddstr(sohwin, y, 0,
        "   Data  Ident Config  Calib DASLog  Event  Other ISPLog    Aux  Status   Total"
    );
 
    y += 3;
    if (WantDasStat()) {
        mvwaddstr(sohwin, y, 0,
            "Last DAS tstamp:                    Quality:"
        );
        wclrtoeol(sohwin);
        ++y;
        mvwaddstr(sohwin, y, 0,
            " Last Reboot Count     Sent   Drop   Errs NoAcks  Trigs   Last Report"
        );
        wclrtoeol(sohwin);
        ++y;
        wmove(sohwin, y, 0);
        wclrtoeol(sohwin);
    } else {
        mvwaddstr(sohwin, y, 0,
            "GPS Clock:                    Quality:            Errors: "
        );
        wclrtoeol(sohwin);
        ++y;
        mvwaddstr(sohwin, y, 0,
            "Barometer:          mbar        recs        errs        missed        dropped"
        );
        wclrtoeol(sohwin);
        ++y;
        mvwaddstr(sohwin, y, 0,
            "DPM:                volts       recs        errs        missed        dropped"
        );
        wclrtoeol(sohwin);
    }
 
    wrefresh(sohwin);
}

void *DasDisplay(void *dummy)
{
struct isp_status status;
struct isp_params params;
struct isp_dascnf dascnf;
struct isp_alarm  alarms;
IDA_IDENT_REC  *idrec;
IDA_CONFIG_REC *cnf;
time_t tstamp;

    InitDasSoh();
    Prompt();

    while (1) {
        if (WantNrts()) {
            UpdateNrts();
        } else if (Connected()) {
            if (GetStatus(&status, 1) == 0) {
                UpdateStatus(&status);
                if (status.have_iddata) {
                    idrec = &status.iddata;
                    cnf   = &status.config;
                    dasinfo.jumpers = cnf->jumpers;
                    dasinfo.atod_id = idrec->atod_id;
                    dasinfo.dsp_rev = idrec->dsp_rev;
                    dasinfo.rel     = idrec->atod_rev / 100;
                    if (dasinfo.rel < 8) {
                        dasinfo.rel = idrec->atod_rev / 10;
                        dasinfo.ver = idrec->atod_rev - (dasinfo.rel * 10);
                        dasinfo.sub = -1;
                    } else {
                        dasinfo.ver =
                            (idrec->atod_rev - (dasinfo.rel * 100)) / 10;
                        dasinfo.sub =
                            idrec->atod_rev - 
                            (dasinfo.rel * 100) - (dasinfo.ver * 10);
                    }
                    dasinfo.valid = 1;
                }
            }
            if (WantAlarms() && GetAlarms(&alarms, &tstamp, 0) == 0) {
                DisplayAlarms(&alarms, tstamp);
                NeedAlarms(0); 
                WaitToClear();
            }
            if (WantParams() && GetParams(&params, 0) == 0) {
                DisplayDasPar(&params);
                NeedParams(0); 
                WaitToClear();
            }
            if (WantDasCnf() && GetDasCnf(&dascnf, 0) == 0) {
                DisplayDasCnf(&dascnf);
                NeedDasCnf(0); 
                WaitToClear();
            }
        }
        TakeInput(cmdwin);
        UpdateLogWin();
    }
}

/* Revision History
 *
 * $Log: dispdas.c,v $
 * Revision 1.11  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.10  2003/12/10 06:25:41  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.9  2002/09/10 17:53:14  dec
 * added DPM support
 *
 * Revision 1.8  2002/05/15 22:20:42  dec
 * fixed "missing" output buffer line problem
 *
 * Revision 1.7  2002/05/14 20:46:20  dec
 * support 5-char channel names
 *
 * Revision 1.6  2002/02/27 00:18:41  dec
 * changed wording of CD-R status line
 *
 * Revision 1.5  2002/02/22 23:50:31  dec
 * added CD-R support
 *
 * Revision 1.4  2001/10/24 23:20:54  dec
 * added CDROM support (only partial support for DAS systems)
 *
 * Revision 1.3  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/11/02 20:30:43  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 */
