#pragma ident "$Id: dispsan.c,v 1.16 2006/02/10 02:23:42 dechavez Exp $"
/*======================================================================
 *
 * Interactive display for SAN-based systems.
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

static int CrntNdx = 1;
void IncrementCrntNdx()
{
    CrntNdx += 9;
}

static char *dinstr(struct isp_status *status)
{
int i;
static char dinstr[] = "SAN DIN 00000000 Temp 00.0";
static char badstr[] = "SAN DIN          Temp     ";

    if (!status->san.cmnd.connected) return badstr;
    sprintf(dinstr, "SAN DIN ");
    for (i=7; i >= 0; i--) {
        sprintf(dinstr+strlen(dinstr), "%d",
            (status->san.din & (1 << i)) ? 1 : 0
        );
    }
    sprintf(dinstr+strlen(dinstr), " Temp%5.1f", status->san.temp);
    return dinstr;
}

static void UpdateSanStatus(struct isp_status *status)
{
static int first = 1;
char c, c1, c2, c3;
int i, j, retval, x, y, maxdisp;
float percent;
long uptime, interval;
IDA_CONFIG_REC *cnf;
static char *Tape = "Tape";
static char *Disk = "Disk";
static char *None = "None";
static char *huh  = "????";
static char *Media;
time_t now;
static time_t prev_time = 0;
static char buffer[MAXCOLS+1];
struct isp_params params;
static char *fid = "UpdateSanStatus";
 
    if (GetParams(&params, 0) == 0) {
        strcpy(Syscode, params.sta);
    } else {
        strcpy(Syscode, "???????");
    }
    util_lcase(Syscode);

    uptime  = status->tstamp - status->start;
/********************************************************
    if (Pid() != status->pid) {
        wmove(sohwin, 0, 0);
        util_ucase(Syscode);

        for (i = 0; i < nx; i++) waddch(sohwin, '-'); //wrefresh(sohwin);
        sprintf(buffer, " %s Console <%s> ", Syscode, isp_release);

        wmove(sohwin, 0, (nx/2) - (strlen(buffer)/2));
        waddstr(sohwin, buffer);

        SetPid(status->pid);

        chdir(home);
        chdir(util_lcase(Syscode));
        chdir(ISP_SUBDIR);
    }
********************************************************/
 
        wmove(sohwin, 0, 0);
        util_ucase(Syscode);

        for (i = 0; i < nx; i++) waddch(sohwin, '-');
        sprintf(buffer, " %s Console <%s> ", Syscode, isp_release);

        wmove(sohwin, 0, (nx/2) - (strlen(buffer)/2));
        waddstr(sohwin, buffer);

    if (Pid() != status->pid) {
        SetPid(status->pid);

        chdir(home);
        chdir(util_lcase(Syscode));
        chdir(ISP_SUBDIR);
    }
 
    if (status == (struct isp_status *) NULL) return;

    SetOutputMediaType(status->output.type);

    if (status->output.type == ISP_OUTPUT_DISK) {
        Media = Disk;
    } else if (status->output.type == ISP_OUTPUT_TAPE) {
        Media = Tape;
    } else if (status->output.type == ISP_OUTPUT_NONE) {
        Media = None;
    } else {
        Media = huh;
    }

    cnf     = &status->config;

    mvwaddstr(sohwin, 1, nx - 21, util_lttostr(status->tstamp, 1));
    if (status->extclock.enabled) {
        mvwaddstr(sohwin, 2, nx - 17,
            util_lttostr(status->extclock.datum, 0)
        );
    } else {
#ifdef INCLUDE_MISSING_GPS_MESSAGE
        mvwaddstr(sohwin, 2, nx - 21, "n/a (no GPS at ISP)");
#else
        wmove(sohwin, 2, nx - 34);
        wclrtoeol(sohwin);
#endif
    }

    x = 0;
    y = 2;

    /* SAN */
    x = 1;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_DAS) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_DAS));
    if (status->alarm.code & ISP_ALARM_DAS) wstandend(sohwin);

    /* ADC */
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_ADC) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_ADC));
    if (status->alarm.code & ISP_ALARM_ADC) wstandend(sohwin);

    /* OPR */
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_OPER) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_OPER));
    if (status->alarm.code & ISP_ALARM_OPER) wstandend(sohwin);

    /* IOE */
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_IOERR) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_IOERR));
    if (status->alarm.code & ISP_ALARM_IOERR) wstandend(sohwin);

    /* OFF */
    x += 4;
    wmove(sohwin, y, x);
    if (status->alarm.code & ISP_ALARM_OFFLINE) wstandout(sohwin);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_OFFLINE));
    if (status->alarm.code & ISP_ALARM_OFFLINE) wstandend(sohwin);

    /* OVR */
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

    /* DCL */
    x += 4;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_DASCLK));

    /* ACL */
    x += 4;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_AUXCLK));

    /* NRT */
    if (params.nrts) {
        x += 4;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%c", isp_alrm_code(&status->alarm, ISP_ALARM_NRTS));
    } else {
        x += 3;
        wmove(sohwin, y, x);
        wprintw(sohwin, "n/a");
    }

    x++;

    /* TRG */
    if (status->san.evt) {
        wmove(sohwin, y, x+4);
        wstandout(sohwin);
        wprintw(sohwin, "ON");
        wstandend(sohwin);
    } else {
        wmove(sohwin, y, x+3);
        wprintw(sohwin, "off");
    }
    x += 4;

    /* CAL */
    if (status->san.cal == SAN_CAL_OFF) {
        wmove(sohwin, y, x+3);
        wprintw(sohwin, "off");
    } else if (status->san.cal == SAN_CAL_ON) {
        wmove(sohwin, y, x+4);
        wstandout(sohwin);
        wprintw(sohwin, " ON");
        wstandend(sohwin);
    } else if (status->san.cal == SAN_CAL_PENDING) {
        wmove(sohwin, y, x+3);
        wstandout(sohwin);
        wprintw(sohwin, "PND");
        wstandend(sohwin);
    } else {
        wmove(sohwin, y, x+3);
        wprintw(sohwin, "???");
    }
    x += 4;

    /* DPM */
    if (status->dpm.enabled) {
        wmove(sohwin, y, x+4);
        wprintw(sohwin, "%5.2f", ((float) status->dpm.datum) / params.dpm.multiplier);
    } else {
        wmove(sohwin, y, x+4);
        wprintw(sohwin, " n/a ");
    }

    y += 2;
    x = strlen("ISP Uptime: ");
    wmove(sohwin, y, x);
    wprintw(sohwin, "%s", util_lttostr(uptime, 8));
    x = nx - 27;
    wmove(sohwin, y, x);
    wprintw(sohwin, "%s", dinstr(status));

    ++y;
    x = strlen("SAN Uptime: ");
    wmove(sohwin, y, x);
    if (status->san.cmnd.connected) {
        if (status->san.boottime > 0) {
            interval = status->tstamp - status->san.boottime;
            wprintw(sohwin, "%s ", util_lttostr(interval, 8));
        } else {
            wprintw(sohwin, "  <unknown>  ");
        }
        wprintw(sohwin, "(S/F/P=0x%04x/%04x/%04x)", 
            status->san.gps.status, status->san.gps.phase,
            status->san.gps.pll
        );
    } else {
        wprintw(sohwin, "                                     ");
    }

    if (status->output.type == ISP_OUTPUT_TAPE) {
        x = nx - 27 + strlen("Output Buffer:");
        wmove(sohwin, y, x);
        if (status->buffer.nrec == -12345) {
            wstandout(sohwin);
            wprintw(sohwin, "!!NONE FREE!!");
            wstandend(sohwin);
        } else {
            percent = (float) (status->buffer.nrec * 100) /
                      (float)  status->buffer.capacity;
            wprintw(sohwin, "%3d%% (%6ld)", (int) percent, status->buffer.nrec);
        }
    } else if (status->output.type == ISP_OUTPUT_CDROM) {
        x = nx - 27;
        wmove(sohwin, y, x); wclrtoeol(sohwin);
        if (status->buffer.nrec < 0) {
            wprintw(sohwin, "Pnd:");
        } else {
            percent = (float) (status->buffer.nrec * 100) /
                      (float)  status->buffer.capacity;
            wprintw(sohwin, "Pnd: %dM, %d files (%d%%)",
                status->buffer.nrec, status->output.file, (int) percent
            );
        }
    } else {
        x = nx - 27;
        wmove(sohwin, y, x);
        wprintw(sohwin, "Output buffering suppressed");
    }

    y++;
    wmove(sohwin, y, 0); wclrtoeol(sohwin);
    if (status->output.type == ISP_OUTPUT_CDROM) {
        if (status->last.change > 0) {
            wprintw(sohwin, "Last Archive Dump: ");
            interval = status->tstamp - status->last.change;
            wprintw(sohwin, "%s", util_lttostr(status->last.change, 0));
            wprintw(sohwin, " (%s)", util_lttostr(interval, 8));
        } else {
            wprintw(sohwin, "                   ");
            wprintw(sohwin, "                                ");
        }
        x = nx - 27;
        wmove(sohwin, y, x); wclrtoeol(sohwin);
        if (status->buffer.nrec < 0) {
            wprintw(sohwin, "Rdy:");
        } else {
            wprintw(sohwin, "Rdy: %d ISO %s ready",
                status->output.nrec, status->output.nrec == 1 ? "image" : "images"
            );
        }
    } else if (status->output.type != ISP_OUTPUT_TAPE) {
        ;
    } else if (status->output.state == ISP_OUTPUT_ONLINE) {
        if (status->last.change > 0) {
            wprintw(sohwin, "Last Tape Change: ");
            interval = status->tstamp - status->last.change;
            wprintw(sohwin, "%s", util_lttostr(status->last.change, 0));
            wprintw(sohwin, " (%s)", util_lttostr(interval, 8));
        } else {
            wprintw(sohwin, "                  ");
            wprintw(sohwin, "                                ");
        }
        x = nx - 27;
        wmove(sohwin, y, x);
        wprintw(sohwin, "%s: %7ld Recs", Media, status->output.nrec);
    } else if (status->output.state == ISP_OUTPUT_CHECK) {
        wmove(sohwin, y, 0);
        wprintw(sohwin, "%s: checking %s for data",
            Media, status->output.device
        );
    } else if (status->output.state == ISP_OUTPUT_OPER) {
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
        wprintw(sohwin, "%s: %s skipping to end of data",
            Media, status->output.device
        );
    } else if (status->output.state == ISP_OUTPUT_OFFLINE) {
        wstandout(sohwin);
        wprintw(sohwin, "%s: NO MEDIA PRESENT AT %s",
            Media, status->output.device
        );
        wstandend(sohwin);
    } else {
        wstandout(sohwin);
        wprintw(sohwin, "%s: %s offline", Media, status->output.device);
        wstandend(sohwin);
    }

    y += 3;
    x = strlen("Data ");
    wmove(sohwin, y, x);
    if (status->san.data.connected) {
        interval = status->tstamp - status->san.data.connected;
        wprintw(sohwin, "%s ", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "            ");
        x = strlen("Data      ");
        wmove(sohwin, y, x);
        wstandout(sohwin); wprintw(sohwin, "NO"); wstandend(sohwin);
    }
    x = strlen("Data ddd-hh:mm:ss  ");
    wmove(sohwin, y, x);
    if (status->san.data.lastRead) {
        interval = status->tstamp - status->san.data.lastRead;
        wprintw(sohwin, "%s", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "            ");
    }
    x += strlen("            ");
    wmove(sohwin, y, x);
    wprintw(sohwin, " %3lu  TS: %8lu  LM: %4lu  II: %4lu  +: %4lu",
        status->san.data.count.reconn, status->san.data.count.p1,
        status->san.data.count.p2,status->count.urec,
        status->san.data.count.p3
    );

    y += 1;
    x = strlen("Cmnd ");
    wmove(sohwin, y, x);
    if (status->san.cmnd.connected) {
        interval = status->tstamp - status->san.cmnd.connected;
        wprintw(sohwin, "%s ", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "            ");
        x = strlen("Cmnd      ");
        wmove(sohwin, y, x);
        wstandout(sohwin); wprintw(sohwin, "NO"); wstandend(sohwin);
    }
    x = strlen("Cmnd ddd-hh:mm:ss  ");
    wmove(sohwin, y, x);
    if (status->san.cmnd.lastRead) {
        interval = status->tstamp - status->san.cmnd.lastRead;
        wprintw(sohwin, "%s", util_lttostr(interval, 8));
    } else {
        wprintw(sohwin, "            ");
    }
    x += strlen("            ");
    wmove(sohwin, y, x);
    wprintw(sohwin, " %3lu  SH: %8lu  CF: %4lu            +: %4lu",
        status->san.cmnd.count.reconn, status->san.cmnd.count.p1,
        status->san.cmnd.count.p2,status->san.cmnd.count.p3
    );

    y += 2;
    wmove(sohwin, y, 0);
    if (CrntNdx > status->san.nstream-1) CrntNdx = 1;
    maxdisp = status->san.nstream - CrntNdx;
    if (maxdisp > 9) maxdisp = 9;
    for (i=CrntNdx, j=0; j < 9; i++,j++) {
        status->san.stream[i].name[5] = 0;
        if (i == CrntNdx || (i % 3) == 1) {
            wmove(sohwin, ++y, 0);
        } else if (i > 1) {
            wprintw(sohwin, " ");
        }
        if (i < CrntNdx + maxdisp) {
            wprintw(sohwin, "%5s %9.3e %10.3e",
                status->san.stream[i].name,
                status->san.stream[i].rms,
                status->san.stream[i].ave
            );
        } else {
            wprintw(sohwin, "                          ");
        }
    }

    wmove(cmdwin, 0, cx);

    if (
        (status->alarm.code & ISP_ALARM_DAS)     ||
        (status->alarm.code & ISP_ALARM_ADC)     ||
        (status->alarm.code & ISP_ALARM_OPER)    ||
        (status->alarm.code & ISP_ALARM_IOERR)   ||
        (status->alarm.code & ISP_ALARM_OFFLINE) ||
        (status->alarm.code & ISP_ALARM_OQFULL)  ||
        (status->alarm.code & ISP_ALARM_BQFULL)  ||
        (status->alarm.code & ISP_ALARM_RQFULL)
    ) {
        SoundBeep(BEEP_REQUEST);
    }

    wrefresh(sohwin);
    wrefresh(cmdwin);
}

static void DisplaySanPar(struct isp_params *params)
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
    wprintw(sohwin, "SAN data: %s:%d",
        params->san.addr, params->san.port.data
    );
    y += 1; wmove(sohwin, y, x);
    wprintw(sohwin, "SAN cmnd: %s:%d",
        params->san.addr, params->san.port.cmnd
    );

    y += 2; wmove(sohwin, y, x);
    wprintw(sohwin, "IDA data Rev: %d", params->pktrev);

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
    if (!params->baro.enabled && !params->clock.enabled) {
        wprintw(sohwin, "No auxiliary ISP inputs configured");
    } else {
        wprintw(sohwin, "                 Auxiliary ISP inputs");
        y += 1; wmove(sohwin, y, x);
        wprintw(sohwin,
            " Device      Port        Ibaud  Obaud  Flow  Timeout"
        );
        if (params->baro.enabled) {
            y += 1; wmove(sohwin, y, x);
            wprintw(sohwin, "%-10s  %-11s %6d %6d  %s  default",
                "Barometer",
                params->baro.port, params->baro.baud, params->baro.baud,
                flow(params->baro.flow)
            );
        }
        if (params->clock.enabled) {
            y += 1; wmove(sohwin, y, x);
            wprintw(sohwin, "%-10s  %-11s %6d    n/a  %s  default",
                "GPS Clock",
                params->clock.port, params->clock.baud,
                flow(params->clock.flow)
            );
        }
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
"Strm Chan Filt Mode Name    Strm Chan Filt Mode Name   Detector       ";
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
        wprintw(sohwin, "%4d %s %s %s %s", i, chan, filt, mode, name);
    }

    x += strlen("Strm Chan Filt Mode Name    ");
    for (i = 12, y = save; i < 25; i++, y++) {
        wmove(sohwin, y, x);
        isp_dascnf_text(cnf, i, &chan, &filt, &name, &mode);
        wprintw(sohwin, "%4d %s %s %s %s", i, chan, filt, mode, name);
    }

    y  = save;
    x += strlen("Strm Chan Filt Mode Name   ");
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

void InitSanSoh()
{
int y, x;
static char *fid = "InitSanSoh";

    x = 0;
    y = 1;

    CrntNdx = 1;
    wmove(sohwin, y, 0);
    wclrtobot(sohwin);    
 
    mvwaddstr(sohwin, y, 0,
        "SAN ADC OPR IOE OFF OVR DCL ACL NRT  TRG CAL   DPM"
    );

    ++y;    
    mvwaddstr(sohwin, y, nx - 24, "  GPS:");
 
    y += 2;
    mvwaddstr(sohwin, y, 0, "ISP Uptime: ");
    mvwaddstr(sohwin, y, nx - 27, "SAN DIN:          Temp");

    ++y;
    mvwaddstr(sohwin, y, 0, "SAN Uptime: ");
    mvwaddstr(sohwin, y, nx-27, "Output Buffer:   % (      )");

    ++y;
    /* Tape line rewritten every time, leave blank here */
    wmove(sohwin, y, 0);
    wclrtoeol(sohwin);

    y += 2;
    mvwaddstr(sohwin, y, 0, 
        "      Connected        Idle     Rct  "
        "------------- Packet Counts -------------"
    );
    mvwaddstr(sohwin, ++y, 0, "Data ");
    mvwaddstr(sohwin, ++y, 0, "Cmnd ");
 
    y += 2;
    mvwaddstr(sohwin, y, 0,
        " Chan    RMS        Mean    "
         "Chan    RMS        Mean    "
         "Chan    RMS        Mean"
    );

    wrefresh(sohwin);
}

void *SanDisplay(void *dummy)
{
struct isp_status status;
struct isp_params params;
struct isp_alarm  alarms;
time_t tstamp;

    InitSanSoh();
    Prompt();

    while (1) {
        if (WantNrts()) {
            UpdateNrts();
        } else if (Connected()) {
            if (GetStatus(&status, 1) == 0) UpdateSanStatus(&status);
            if (WantAlarms() && GetAlarms(&alarms, &tstamp, 0) == 0) {
                DisplayAlarms(&alarms, tstamp);
                NeedAlarms(0); 
                WaitToClear();
            }
            if (WantParams() && GetParams(&params, 0) == 0) {
                DisplaySanPar(&params);
                NeedParams(0); 
                WaitToClear();
            }
        }
        TakeInput(cmdwin);
        UpdateLogWin();
    }
}

/* Revision History
 *
 * $Log: dispsan.c,v $
 * Revision 1.16  2006/02/10 02:23:42  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.15  2003/12/10 06:25:41  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.14  2003/12/04 23:41:08  dechavez
 * removed // comments causing old Solaris compiler to complain
 *
 * Revision 1.13  2003/10/16 18:17:48  dechavez
 * shifted the second +: over by one
 *
 * Revision 1.12  2003/10/02 20:24:51  dechavez
 * 2.3.1 mods (fixed console hang on exit, improved dumb terminal refresh
 * a bit, added idle timeout)
 *
 * Revision 1.11  2002/09/09 21:52:29  dec
 * added DPM display support
 *
 * Revision 1.10  2002/03/15 22:55:53  dec
 * removed spurious "Tape output..." line when no auto media support used
 *
 * Revision 1.9  2002/02/22 23:49:32  dec
 * supress printing of undefined CD-R stats, fixed auxiliary typo
 *
 * Revision 1.8  2001/10/24 23:20:54  dec
 * added CDROM support (only partial support for DAS systems)
 *
 * Revision 1.7  2001/10/08 18:14:11  dec
 * SAN display to support configuration w/o tape output
 *
 * Revision 1.6  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.5  2001/04/19 22:44:50  dec
 * audible ADC alarm
 *
 * Revision 1.4  2000/11/08 17:42:38  dec
 * fixed y offset in GPS time print
 *
 * Revision 1.3  2000/11/06 23:12:17  dec
 * clean up display some more
 *
 * Revision 1.2  2000/11/02 20:30:43  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 */
