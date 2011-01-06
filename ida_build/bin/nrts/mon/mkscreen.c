#pragma ident "$Id: mkscreen.c,v 1.4 2006/06/02 21:14:30 dechavez Exp $"
#include <curses.h>
#include <sys/param.h>
#include <signal.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif
#include <time.h>
#include "nrts.h"
#include "util.h"
#include "nrts_mon.h"

#define GETLINE util_getline(fp, input, MAXTEXT, '#', &lineno)

extern WINDOW *logtitle;
extern WINDOW *logwin;
extern WINDOW *stattitle;
extern WINDOW *statwin;
extern int Cols, Lines;
extern int logwinlen;
extern int winswide;
extern int statwinlen;
extern int flag_y, flag_x;
extern int logwinarg;
extern int indices, times;
extern int statwinbeg_y;
extern int nsys;
extern int csys;
extern char **Systems;

extern char Hostname[MAXHOSTNAMELEN];

int mkscreen(char *home, struct nrts_sys *sys, char *site, char *staname, int nchn, char *log)
{
FILE *fp;
int i, s, c, y, x;
struct tm *tm_gmt, *tm_loc;
char string[80];
char input[1024];
char *token[MAXTOKENS];
int ntokens;
int lineno  = 0;
struct nrts_mmap *map;
struct nrts_sys *system;
struct nrts_files *file;

	gethostname(Hostname, MAXHOSTNAMELEN);
	winswide = Cols - 1;
	flag_y = 1; flag_x = 1;
	if(logwinlen > 0) {
		statwinlen = Lines - logwinlen - 2;
		if((nchn + flag_y + 5) >= statwinlen) {
			logwinlen -= (nchn + flag_y + 3) - statwinlen;
		} else if(!logwinarg) {
			logwinlen += statwinlen - (nchn + flag_y + 3);
		}
		if(!logwinarg && logwinlen < 3)
			logwinlen = 3;
		if(logwinlen > 0) {
			statwinlen = Lines - logwinlen - 2;
			if(statwinlen < 16) {
				statwinlen = 16;
				logwinlen = Lines - statwinlen - 2;
			}
			if(logwinlen > 0) {
				statwinbeg_y = logwinlen + 2;
				logwin = newwin(logwinlen,winswide,1,0);
				logtitle = newwin(1,winswide,0,0);
				stattitle = newwin(1,winswide,logwinlen + 1,0);
				scrollok(logwin, TRUE);
			} else {
				stattitle = newwin(1,winswide,0,0);
				logwinlen = 0;
			}

		} else if(logwinlen <= 0) {
				logwinlen = 0;
				statwinlen = Lines -  1;
				statwinbeg_y = 1;
				stattitle = newwin(1,winswide,0,0);
		}
	} else {
		statwinlen = Lines -  1;
		statwinbeg_y = 1;
		stattitle = newwin(1,winswide,0,0);
	}

	statwin = newwin(statwinlen, winswide,statwinbeg_y,0);
	wtimeout(statwin, 1);
	leaveok(statwin, FALSE); 
	scrollok(statwin, FALSE);
	noecho();
	nonl();

	if(logwinlen > 0) {	
		chlog(log);
	}

	chpid(site, Hostname, sys->pid);
	chsystem(site, home);

	wmove(statwin, flag_y+CHAN_HDR_OFFSET, flag_x);
	if(indices) {
		wprintw(statwin, "     chan  beg    end");
		wprintw(statwin, "   lend    status   nrec   count   last read");
	} else if(times) {

		if(winswide  < 80 ) {
	  	wprintw(statwin, "   chan     earliest datum         latest datum      nseg   latency");
		} else {
     	wprintw(statwin, "   chan     earliest datum         latest datum      nseg    status   latency");
		}
	}
	/* 
	if(indices) {
		wprintw(statwin, "     chan  beg    end");
		wprintw(statwin, "    lend   status   nrec   last read");
	} else if(times) {
		sprintf(string,  "    chan   earliest datum     latest datum      nseg    status    nrec    latency");
		if(winswide  < 80 ) {
	  	wprintw(statwin, "    chan   earliest datum     latest datum       nseg   nrec    latency");
		} else {
  		wprintw(statwin, "    chan   earliest datum     latest ");
    	wprintw(statwin, "datum       nseg    status   nrec    latency");
		}
	}
	*/
	mvwaddstr(statwin, 0, Cols - 26, "   UTC: ");
	if(sys->pid > 0)
		mvwaddstr(statwin, 1, Cols - 26, "UPTIME: ");
	wrefresh(statwin);
			
	return (nchn + flag_y + (CHAN_HDR_OFFSET+1) - statwinlen);

}

/* Revision History
 *
 * $Log: mkscreen.c,v $
 * Revision 1.4  2006/06/02 21:14:30  dechavez
 * ignore System entries that don't have NRTS disk loops attached
 *
 * Revision 1.3  2004/01/29 18:54:04  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
