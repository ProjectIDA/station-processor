#pragma ident "$Id: chsta.c,v 1.3 2003/12/10 06:24:47 dechavez Exp $"
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

extern WINDOW *statwin;
extern int Cols;

char Hostname[MAXHOSTNAMELEN];

void chsta(struct nrts_sys *sys, char *staname)
{
int y, x, i;

	mvwprintw(statwin, 2, 1, " STATIONS:");
	for(i = 0; i < sys->nsta; i++) {
		if(strcmp(staname, sys->sta[i].name) == 0) {
			wprintw(statwin, " ");
			wstandout(statwin);
			wprintw(statwin, "%s", staname);
			wstandend(statwin);
		}	else {
			getyx(statwin, y, x);
			if(x + strlen(sys->sta[i].name) >= Cols - 30)
					wmove(statwin, 3, 10);
			wprintw(statwin, " %s", staname);
		}
	}
}

/* Revision History
 *
 * $Log: chsta.c,v $
 * Revision 1.3  2003/12/10 06:24:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
