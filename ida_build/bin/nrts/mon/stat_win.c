#pragma ident "$Id: stat_win.c,v 1.3 2003/12/10 06:24:47 dechavez Exp $"
#include <curses.h>
#include <time.h>
#include <sys/param.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif
#include "nrts.h"
#include "util.h"
#include "nrts_mon.h"


extern WINDOW *statwin;
extern int Cols, Lines;
extern int logwinlen;
extern int winswide;
extern int statwinlen;
extern int flag_y, flag_x;
extern int indices, times;
extern char Hostname[MAXHOSTNAMELEN];

void stat_win(struct nrts_sys *sys, char *sysname, struct nrts_chn *chn, int index, int nchn)
{
time_t loc_time, gm_time, cur_time, latency, uptime;
struct tm *tm_gmt, *tm_loc;
int diffs;
int y, x;
static int savedpid;

	latency = time(&cur_time) - chn->tread;
	uptime = time(NULL) - sys->start;
	
	wmove(statwin, flag_y +  (CHAN_HDR_OFFSET+1) + index, flag_x);
	if(indices) {
		wprintw(statwin, "%7s", chn->name);
		wprintw(statwin, " %6ld", chn->hdr.oldest);
		wprintw(statwin, " %6ld", chn->hdr.yngest);
		wprintw(statwin, " %6ld", chn->hdr.lend);
		wprintw(statwin, " %8s", nrts_status(chn->status));
		wprintw(statwin, " %7ld", chn->nread);
		wprintw(statwin, " %7u", chn->count);
		wprintw(statwin, "  %8s",  nrts_latency(latency));
	} else if(times) {
		wprintw(statwin, "%7s", chn->name);
		if(chn->beg > (double) 0) {
			wprintw(statwin, " %s",  util_dttostr(chn->beg, 0));
		} else {
			wprintw(statwin, "                      ");
		}
		if(chn->end > (double) 0) {
			wprintw(statwin, " %s",  util_dttostr(chn->end, 0));
		} else {
			wprintw(statwin, "                      ");
		}
		wprintw(statwin, " %5d", chn->nseg);
		getyx(statwin, y, x);
		if(winswide  > 80) {
			wprintw(statwin, "%9s", nrts_status(chn->status));
		}
		/*
		wprintw(statwin, "%7ld", chn->nread);
		*/
		wprintw(statwin, "%14s", nrts_latency(latency));
	}
	mvwaddstr(statwin, 0, Cols - 18, util_lttostr(time(NULL), 0));
	if(savedpid != sys->pid) {
		chpid(sysname, Hostname, sys->pid);
	}
	if(sys->pid > 0) {
		wmove(statwin, 1, Cols - 26);
		wprintw(statwin, "UPTIME: %17s", nrts_latency(uptime));
	} else if(sys->pid == 0 && savedpid != 0) {
		wmove(statwin, 1, Cols - 22);
		wprintw(statwin, "                        ");
	}
	savedpid = sys->pid;

	return;

}

/* Revision History
 *
 * $Log: stat_win.c,v $
 * Revision 1.3  2003/12/10 06:24:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2000/09/19 23:17:36  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
