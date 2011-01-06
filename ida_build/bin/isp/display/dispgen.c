#pragma ident "$Id: dispgen.c,v 1.14 2005/10/19 23:23:17 dechavez Exp $"
/*======================================================================
 *
 * General purpose display routines.
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

static time_t MaxIdleTime = 600;

WINDOW *full_screen = (WINDOW *) NULL;
WINDOW *logtit, *logwin, *sohwin, *cmdwin;

static WINDOW *daslogwin = NULL;
static WINDOW *dassohwin = NULL;
static WINDOW *dascmdwin = NULL;
static WINDOW *daslogtit = NULL;
static WINDOW *sanlogwin = NULL;
static WINDOW *sansohwin = NULL;
static WINDOW *sancmdwin = NULL;
static WINDOW *sanlogtit = NULL;

int cx = 0;
int nx = 0;

extern char *home;
extern char *server;
extern char *Syscode;
extern int   port;

static char *logfile = (char *) NULL;
static FILE *logfp = (FILE *) NULL;

static int confirming = 0;
static int reconfirming = 0;
static int confirmation = -1;

static int nrts_x = 0;
static int nrts_y = 0;
static struct nrts_sys *Nrts = (struct nrts_sys *) NULL;
static int max_nrts_chn;
static int nxt_nrts_chn;
static int beg_nrts_chn;

static int soh_ny, dassoh_ny, sansoh_ny;

#define YES 1
#define  NO 0

#define PROMPT_CONFIRM   "Are you sure? (y/n) "
#define PROMPT_RECONFIRM "Are you REALLY sure? (y/n) "
#define PROMPT_PRIVOPER  "PRIV# "
#define PROMPT_OBSERVER  "observer-> "
#define PROMPT_OPERATOR  "OPERATOR-> "
#define PROMPT_NEEDOPER  "OPERATOR status required, use op to obtain-> "
static char *prompt = PROMPT_OBSERVER;
static int  command_index = 0;
static MUTEX command_mutex = MUTEX_INITIALIZER;
static MUTEX exit_mutex = MUTEX_INITIALIZER;
static int ShutdownStatus;
static BOOL ShutdownRequested = FALSE;

static char host[MINCOLS];

#define MAXSAVE 256
static BOOL OpenLogFile()
{
struct stat finfo;
static char tmp[MAXCOLS+1];
static char *fid = "OpenLogFile";

    if (logfile == NULL) return FALSE;
    if (stat(logfile, &finfo) != 0) return FALSE;
    if ((logfp = fopen(logfile, "r")) == NULL) return FALSE;
    if (finfo.st_size <= MAXSAVE) return TRUE; /* will want to display the whole thing */
    fseek(logfp, finfo.st_size - MAXSAVE, SEEK_SET);
    fgets(tmp, MAXCOLS, logfp); /* get rid of first partial line */
    return TRUE;
}

static void CloseLogFile()
{
static char *fid = "CloseLogFile";

    if (logfp != NULL) fclose(logfp);
    logfp = NULL;
}

static void CheckForExitRequest()
{
    MUTEX_LOCK(&exit_mutex);
        if (ShutdownRequested) {
            if (full_screen != NULL) {
                wclear(full_screen);
                wrefresh(full_screen);
                EndWin();
            }
            sleep(1);
            exit(ShutdownStatus);
        }
    MUTEX_UNLOCK(&exit_mutex);
}

void RequestShutdown(int status)
{
    MUTEX_LOCK(&exit_mutex);
        ShutdownRequested = TRUE;
        ShutdownStatus    = status;
    MUTEX_UNLOCK(&exit_mutex);
}

void SetWindowsFor(int digitizer)
{
    if (digitizer == ISP_DAS) {
        sohwin = dassohwin;
        logwin = daslogwin;
        cmdwin = dascmdwin;
        soh_ny = dassoh_ny;
        logtit = daslogtit;
    } else if (digitizer == ISP_SAN) {
        sohwin = sansohwin;
        logwin = sanlogwin;
        cmdwin = sancmdwin;
        soh_ny = sansoh_ny;
        logtit = sanlogtit;
    }
}

int ClearStats(char **token, int ntoken)
{
    if (ntoken == 2) {
        if (strncasecmp(token[1], "p", 1) == 0) {
            return RemoteCommand(ISP_CLR_PACKSTAT);
        } else if (strncasecmp(token[1], "c", 1) == 0) {
            return RemoteCommand(ISP_CLR_COMMSTAT);
        } else if (strncasecmp(token[1], "u", 1) == 0) {
            return RemoteCommand(ISP_CLR_COMMSTAT);
        } else if (strncasecmp(token[1], "aux", 3) == 0) {
            return RemoteCommand(ISP_CLR_AUXSTAT);
        } else if (strcasecmp(token[1], "all") == 0) {
            return RemoteCommand(ISP_CLR_EVERYTHING);
        }
    }

    /*printf("syntax: %s {p[ackets] | c[omm] | all}\n", token[0]);*/

    return ISP_OK;
}

static void ProcessCommand(char *cmdbuf)
{
    if (Digitizer() == ISP_DAS) {
        DasProcess(cmdbuf, dassohwin, dascmdwin);
    } else {
        SanProcess(cmdbuf, sansohwin, sancmdwin);
    }
}

void NoteConnecting()
{
    SetPid(-1);
    ShowVersion();
}

void InitSoh()
{
    if (Connected()) {
        switch (Digitizer()) {
          case ISP_DAS:
            InitDasSoh();
            break;
          case ISP_SAN:
            InitSanSoh();
            break;
        }
    } else {
        NoteConnecting();
    }
}

void Prompt()
{
int highlight;

    MUTEX_LOCK(&command_mutex);
        flushinp();
        if (reconfirming) {
            prompt = PROMPT_RECONFIRM;
            highlight = 1;
        } else if (confirming) {
            prompt = PROMPT_CONFIRM;
            highlight = 1;
        } else if (Operator()) {
            prompt = Privileged() ? PROMPT_PRIVOPER : PROMPT_OPERATOR;
            highlight = 1;
        } else if (NeedOperator()) {
            prompt = PROMPT_NEEDOPER;
            highlight = 1;
        } else {
            prompt = PROMPT_OBSERVER;
            highlight = 0;
        }
        wmove(cmdwin, 0, 0);
        wclrtoeol(cmdwin);
        if (highlight) wstandout(cmdwin);
        waddstr(cmdwin, prompt);
        if (highlight) wstandend(cmdwin);
        wmove(cmdwin, 0, (cx = strlen(prompt)));
        wrefresh(cmdwin);
        command_index = 0;
    MUTEX_UNLOCK(&command_mutex);
}

#define MAXREW 256

void UpdateLogWin()
{
static char buffer[MAXCOLS+1];
static char *fid = "UpdateLogWin";
 
    if (logfp == (FILE *) NULL && !OpenLogFile()) return;

/* Display everything from the current postion to the end of the log file */
 
    while(fgets(buffer, MAXCOLS, logfp) != NULL) {
        if (strstr(buffer, "flushing log") != NULL) {
            CloseLogFile();
            return;
        } else if (strstr(buffer, "log flushed") == NULL) {
            waddstr(logwin, buffer);
        }
    }
    wrefresh(logwin);
}

void InitNrts()
{
int y, x;
struct nrts_files *file;
static struct nrts_mmap mmap;
static char *title =
"NRTS Disk Loop Status";
static char *header =
"Chn       Earliest Datum         Latest Datum      Nseg  Status   Latency";

    if (strlen(Syscode) < 1) {
        util_log(1, "InitNrts: no syscode");
        SetWantNrts(0);
        return;
    }

    if (Nrts != (struct nrts_sys *) NULL && Nrts->pid != Pid()) {
        nrts_unmap(&mmap);
        Nrts = (struct nrts_sys *) NULL;
    }

    if (Nrts == (struct nrts_sys *) NULL) {
        file = nrts_files(&home, util_lcase(Syscode));
        if (nrts_mmap(file->sys, "r+", NRTS_SYSTEM, &mmap) != 0) {
            util_log(1, "nrts_mmap: %s", syserrmsg(errno));
            SetWantNrts(0);
            return;
        }
        Nrts = (struct nrts_sys *) mmap.ptr;
    }

    wmove(sohwin, 1, 0);
    wclrtobot(sohwin);    
 
    x = nx/2 - strlen(title)/2;
    mvwaddstr(sohwin, 2, x, title);

    nrts_x = (nx/2 - strlen(header)/2) - 2;
    mvwaddstr(sohwin, 4, nrts_x, header);
    wrefresh(sohwin);

    nrts_y = 5;
    max_nrts_chn = soh_ny - nrts_y - 2;
    nxt_nrts_chn = 0;
    beg_nrts_chn = 0;
}

void UpdateNrts()
{
int x, y, i, j, n, left;
long latency;
static char *more = "press <enter> for more channels or <esc> if done";

    x = nrts_x;

    if (Nrts == (struct nrts_sys *) NULL) return;

    left = Nrts->sta[0].nchn - beg_nrts_chn;
    n = (max_nrts_chn < left) ? max_nrts_chn : left;
    for (i = 0, j = beg_nrts_chn; i < n; i++, j++) {
        y = nrts_y + i;
        wmove(sohwin, y, nrts_x);
        wprintw(sohwin, "%-5s ",  Nrts->sta[0].chn[j].name);
        if (Nrts->sta[0].chn[j].beg > 0) {
            wprintw(sohwin, "%s ",util_dttostr(Nrts->sta[0].chn[j].beg, 0));
        } else {
            wprintw(sohwin, "                      ");
        }
        if (Nrts->sta[0].chn[j].end > 0) {
            wprintw(sohwin, "%s ",util_dttostr(Nrts->sta[0].chn[j].end, 0));
        } else {
            wprintw(sohwin, "                      ");
        }
        wprintw(sohwin, "%5d ", Nrts->sta[0].chn[j].nseg);
        switch (Nrts->sta[0].chn[j].status) {
            case NRTS_IDLE:
                wprintw(sohwin, " idle   ");
                break;
            case NRTS_BUSY1:
                wprintw(sohwin, "busy(1) ");
                break;
            case NRTS_BUSY2:
                wprintw(sohwin, "busy(2) ");
                break;
            case NRTS_BUSY3:
                wprintw(sohwin, "busy(3) ");
                break;
            default:
                wprintw(sohwin, "CORRUPT ");
                break;
        }
        if (Nrts->sta[0].chn[j].beg > 0) {
            latency = time(NULL) - Nrts->sta[0].chn[j].tread;
            wprintw(sohwin, "%s", util_lttostr(latency, 8));
        } else {
            wprintw(sohwin, "            ");
        }
    }

    if (Nrts->sta[0].nchn > n) {
        nxt_nrts_chn = beg_nrts_chn + n;
        if (nxt_nrts_chn >= Nrts->sta[0].nchn) nxt_nrts_chn = 0;
        y++;
        wmove(sohwin, y, 0);
        wclrtobot(sohwin);
        x = nx/2 - strlen(more)/2;
        mvwaddstr(sohwin, y, x, more);
    }

    wmove(cmdwin, 0, cx);
    wrefresh(sohwin);
    wrefresh(cmdwin);
}

void InitDisplay()
{
int i, x, y;
char buffer[MAXCOLS+1];
static char *fid = "InitDisplay";

    SetWantNrts(0);
    Syscode[0] = 0;

    if (full_screen != NULL) wclear(full_screen);
    if (logwin != (WINDOW *) NULL || logfile != (char *) NULL) {
        touchwin(logtit);
        wmove(logtit, 0, 0);
        for (i = 0; i < nx; i++) waddch(logtit, '-');
        sprintf(buffer, " %s ", logfile);
        x = nx/2 - strlen(buffer)/2;
        mvwaddstr(logtit, 0, x, buffer);
        wrefresh(logtit);
        touchwin(logwin);
        CloseLogFile();
        OpenLogFile();
        UpdateLogWin();
    }

    if (sohwin != (WINDOW *) NULL) {
        touchwin(sohwin);
        for (i = 0; i < nx; i++) waddch(sohwin, '-');
        InitSoh();
    }

    if (cmdwin != (WINDOW *) NULL) {
        touchwin(cmdwin);
        Prompt();
    }
}

int OpenDisplay(char *server, int port, char *logfile_name)
{
int daslog_xo, daslog_yo, daslog_ny, dascmd_ny;
int dassoh_xo, dassoh_yo, dascmd_xo, dascmd_yo;
int sanlog_xo, sanlog_yo, sanlog_ny, sancmd_ny;
int sansoh_xo, sansoh_yo, sancmd_xo, sancmd_yo;

    gethostname(host, MINCOLS-1);

    if (logfile_name != (char *) NULL) {
        logfile = strdup(logfile_name);
        OpenLogFile();
    }

    if ((full_screen = initscr()) == (WINDOW *) NULL) {
        EndWin();
        perror("initscr");
        return -1;
    } else if (COLS < MINCOLS || LINES < MINLINES) {
        EndWin();
        printf("Window too small (%d x %d minimum)\n", MINCOLS, MINLINES);
        return -3;
    } else if (cbreak() == ERR) {
        EndWin();
        perror("cbreak");
        return -3;
    }

    raw();
    noecho();
    nx = COLS < MAXCOLS ? COLS : MAXCOLS;

    if (logfp != (FILE *) NULL) {
        daslog_yo = 1;
        daslog_ny = LINES - DAS_SOH_NLINES - 2;
        daslogwin = newwin(daslog_ny, nx, daslog_yo,      0);
        daslogtit = newwin(daslog_ny, nx,      0,      0);
        scrollok(daslogwin, TRUE);
        sanlog_yo = 1;
        sanlog_ny = LINES - SAN_SOH_NLINES - 2;
        sanlogwin = newwin(sanlog_ny, nx, sanlog_yo,      0);
        sanlogtit = newwin(sanlog_ny, nx,      0,      0);
        scrollok(sanlogwin, TRUE);
    } else {
        daslog_ny = sanlog_ny = 0;
        logwin = (WINDOW *) NULL;
    }


    dassoh_xo = 0;
    dassoh_yo = daslog_yo + daslog_ny;
    dassoh_ny = DAS_SOH_NLINES;
    dassohwin = newwin(dassoh_ny, nx, dassoh_yo, dassoh_xo);
    sansoh_xo = 0;
    sansoh_yo = sanlog_yo + sanlog_ny;
    sansoh_ny = SAN_SOH_NLINES;
    sansohwin = newwin(sansoh_ny, nx, sansoh_yo, sansoh_xo);

    dascmd_xo = 0;
    dascmd_yo = dassoh_yo + dassoh_ny;
    dascmd_ny = CMD_NLINES;
    dascmdwin = newwin(dascmd_ny, nx, dascmd_yo, dascmd_xo);

    if (dascmdwin == NULL) {
        fprintf(stderr, "newwin(%d, %d, %d, %d)", dascmd_ny, nx, dascmd_yo, dascmd_xo);
        perror("");
        return -1;
    }
    wtimeout(dascmdwin, 250);
    sancmd_xo = 0;
    sancmd_yo = sansoh_yo + sansoh_ny;
    sancmd_ny = CMD_NLINES;
    sancmdwin = newwin(sancmd_ny, nx, sancmd_yo, sancmd_xo);
    wtimeout(sancmdwin, 250);
    if (sancmdwin == NULL) {
        perror("newwin");
        return -1;
    }

    SetWindowsFor(ISP_SAN);

    InitDisplay();

    return 0;
}

int Confirm()
{
    confirming = 1;
    Prompt();
    while (TakeInput(cmdwin) == 0);
    confirming = 0;
    Prompt();
    return confirmation == YES ? 1 : 0;
}

int Reconfirm()
{
    reconfirming = confirming = 1;
    Prompt();
    while (TakeInput(cmdwin) == 0);
    reconfirming = confirming = 0;
    Prompt();
    return confirmation == YES ? 1 : 0;
}

void WaitToClear()
{
    Prompt();
    while (TakeInput(cmdwin) == 0) UpdateLogWin();
    Prompt();
}

void SetMaxIdle(time_t value)
{
    MaxIdleTime = value;
}

int TakeInput(WINDOW *win)
{
static char cmdbuf[MINCOLS+1];
int input, full, done;
static time_t LastAction = 0;

#define BS   0010
#define TAB  0011
#define DEL  0177
#define ESC  0033
#define CR   0012
#define CLR  0025

    if (LastAction == 0) LastAction = time(NULL);
    if (MaxIdleTime > 0  && (time(NULL) - LastAction > MaxIdleTime)) Quit(0);

    CheckForExitRequest();

    MUTEX_LOCK(&command_mutex);
        full = (command_index == MINCOLS);
    MUTEX_UNLOCK(&command_mutex);
    if (full) return 0;

    MUTEX_LOCK(&command_mutex);
        input = wgetch(win);
    MUTEX_UNLOCK(&command_mutex);

    if (ChangedState()) Prompt();
    if (input == ERR) return 0;
    LastAction = time(NULL);

    switch(input) {
        case ESC:
            InitDisplay();
            if (confirming) confirmation = NO;
            done = 1;
            break;

        case DEL:
            input = BS;
        case BS:
            MUTEX_LOCK(&command_mutex);
                if (command_index) {
                    --command_index;
                    --cx;
                    wechochar(cmdwin, (char) input);
                    wechochar(cmdwin, ' ');
                    wechochar(cmdwin, (char) input);
                }
            MUTEX_UNLOCK(&command_mutex);
            done = 0;
            break;

        case CLR:
            Prompt();
            done = 0;
            break;

        case CR:
            MUTEX_LOCK(&command_mutex);
                cmdbuf[command_index] = 0;
            MUTEX_UNLOCK(&command_mutex);
            done = 1;
            if (confirming) {
                if (cmdbuf[0] == 'y' || cmdbuf[0] == 'Y') {
                    confirmation = YES;
                } else if (cmdbuf[0] == 'n' || cmdbuf[0] == 'N') {
                    confirmation = NO;
                } else {
                    done = 0;
                }
            } else if (WantNrts() && command_index == 0) {
                beg_nrts_chn = nxt_nrts_chn;
            } else {
                ProcessCommand(cmdbuf);
            }
            Prompt();
            break;

        default:
            MUTEX_LOCK(&command_mutex);
                cmdbuf[command_index] = (char) input;
                wechochar(cmdwin, cmdbuf[command_index]);
                ++cx; ++command_index;
            MUTEX_UNLOCK(&command_mutex);
            done = 0;
            break;
    }

    return done;
}

void ShowVersion()
{
int x, y;
static char *copyright[] = {
    "Copyright (c) 1997-2005, Regents of the University of California.",
    "All rights reserved.",
    (char *) NULL
};
static char *credits[] = {
    "Software designed and developed by ESSW",
    "San Diego, California, USA",
    "dechavez@essw.com",
    (char *) NULL
};
BOOL connected;
char **text;
int i, nlines;
static char buffer[MAXCOLS+1];

    connected = Connected();

    nlines = 1; /* release */
    for (text = copyright; *text != (char *) NULL; text++) ++nlines;
    ++nlines;   /* skip a line */
    for (text = credits; *text != (char *) NULL; text++) ++nlines;
    if (connected) nlines += 3;

    wmove(sohwin, 1, 0);
    wclrtobot(sohwin);
    y = ((soh_ny - 1) / 2) - (nlines/2);
    x = nx/2 - strlen(isp_release)/2;

    mvwaddstr(sohwin, y++, x, isp_release);
    for (text = copyright; *text != (char *) NULL; text++) {
        x = nx/2 - strlen(*text)/2;
        mvwaddstr(sohwin, y++, x, *text);
    }

    y++;

    for (text = credits; *text != (char *) NULL; text++) {
        x = nx/2 - strlen(*text)/2;
        mvwaddstr(sohwin, y++, x, *text);
    }

    if (!connected) {
        y+=2;
        sprintf(buffer, "CONNECTING TO %s PORT %d", server, port);
        x = nx/2 - strlen(buffer)/2;
        wstandout(sohwin);
        mvwaddstr(sohwin, y, x, buffer);
        wstandend(sohwin);
        wrefresh(sohwin);
    } else {
        wrefresh(sohwin);
        WaitToClear();
    }
}

static int AlarmDetail(
struct isp_alarm *alarm,
time_t tstamp,
int y, int x,
int mask,
char *code,
char *desc
) {
time_t onset, duration;

    if (!(alarm->code & mask)) return 0;

    switch (mask) {
      case ISP_ALARM_DAS:
        onset = alarm->das;
        break;
      case ISP_ALARM_OPER:
        onset = alarm->oper;
        break;
      case ISP_ALARM_IOERR:
        onset = alarm->ioerr;
        break;
      case ISP_ALARM_OFFLINE:
        onset = alarm->offline;
        break;
      case ISP_ALARM_RQFULL:
        onset = alarm->rqfull;
        break;
      case ISP_ALARM_OQFULL:
        onset = alarm->oqfull;
        break;
      case ISP_ALARM_BQFULL:
        onset = alarm->bqfull;
        break;
      case ISP_ALARM_AUX:
        onset = alarm->aux;
        break;
      case ISP_ALARM_DASDBG:
        onset = alarm->dasdbg;
        break;
      case ISP_ALARM_DASCLK:
        onset = alarm->dasclk;
        break;
      case ISP_ALARM_AUXCLK:
        onset = alarm->auxclk;
        break;
      case ISP_ALARM_NRTS:
        onset = alarm->nrts;
        break;
      default:
        return 0;
    }

    duration = tstamp - onset;
    wmove(sohwin, y, x);
    wprintw(sohwin, " %s  %s ", code, util_lttostr(onset, 0));
    wprintw(sohwin, " %s  %s", util_lttostr(duration, 8), desc);

    return 1;
}

void DisplayAlarms(struct isp_alarm *alarm, time_t tstamp)
{
int y, x, save;
char *title;
static char *active   = "ACTIVE ALARMS";
static char *inactive = "NO ACTIVE ALARMS";
static char *header    = 
"Code        Onset          Duration    Condition                     ";

    title = alarm->code ? active : inactive;

    x = 0;
    y = 4;
    wmove(sohwin, y, 0);
    wclrtobot(sohwin);    

    if (alarm->code == 0) y = soh_ny / 2;
    x = nx/2 - strlen(title)/2;
    mvwaddstr(sohwin, y, x, title);
    if (alarm->code == 0) {
        wrefresh(sohwin);
        return;
    }

    y += 2;
    save = x = nx/2 - strlen(header)/2;
    mvwaddstr(sohwin, y, x, header);

    y ++;
    if (Digitizer() == ISP_DAS) {
        y += AlarmDetail(alarm, tstamp, y, save,
            ISP_ALARM_DAS, "DAS", "No data or bad data from DAS  "
        );
    } else {
        y += AlarmDetail(alarm, tstamp, y, save,
            ISP_ALARM_DAS, "SAN", "No data or bad data from SAN  "
        );
    }
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_ADC,     "ADC", "ADC timeouts suspected!       "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_OPER,    "OPR", "Operator input required       "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_IOERR,   "IOE", "Mass store I/O error          "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_OFFLINE, "OFF", "Mass store device off-line    "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_OQFULL,  "OVR", "Output queue overflow         "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_RQFULL,  "OVR", "Input queue overflow          "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_BQFULL,  "OVR", "Barometer input queue overflow"
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_AUX,     "AUX", "Auxiliary data error          "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_DASDBG,  "DBG", "DAS is in debug mode          "
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_DASCLK,  "DCL", "digitizer clock unlocked or error"
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_AUXCLK,  "ACL", "ISP GPS clock unlocked or error"
    );
    y += AlarmDetail(alarm, tstamp, y, save,
        ISP_ALARM_NRTS,    "NRT", "NRTS acquisition stopped"
    );

    wrefresh(sohwin);
}

void FirstDisplay()
{
    Prompt();
    while (Digitizer() == ISP_UNKNOWN) {
        TakeInput(cmdwin);
        UpdateLogWin();
    }
}

/* Revision History
 *
 * $Log: dispgen.c,v $
 * Revision 1.14  2005/10/19 23:23:17  dechavez
 * updated copyright
 *
 * Revision 1.13  2004/09/29 19:09:40  dechavez
 * restore title bars after daslog/isplog screen clear
 *
 * Revision 1.12  2004/09/29 18:15:05  dechavez
 * sped up InitDisplay by removing individual subwindow clear/refreshs
 *
 * Revision 1.11  2003/12/04 23:41:08  dechavez
 * removed // comments causing old Solaris compiler to complain
 *
 * Revision 1.10  2003/10/02 20:24:51  dechavez
 * 2.3.1 mods (fixed console hang on exit, improved dumb terminal refresh
 * a bit, added idle timeout)
 *
 * Revision 1.9  2003/06/11 20:55:53  dechavez
 * use new form of utilQuery and utilPause
 *
 * Revision 1.8  2002/05/21 17:01:31  dec
 * added RequestShutdown() and CheckForExitRequest() for (hopefully) more
 * gracefull terminations
 *
 * Revision 1.7  2002/03/15 22:54:47  dec
 * added TAB, maybe will eventually use when multi-sys support added
 *
 * Revision 1.6  2002/02/22 23:49:56  dec
 * fixed auxiliary typo
 *
 * Revision 1.5  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.4  2001/02/23 17:32:58  dec
 * update copyright date range
 *
 * Revision 1.3  2000/11/15 20:01:47  dec
 * explicitly initialize window global pointers to NULL
 *
 * Revision 1.2  2000/11/02 20:30:43  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 */
