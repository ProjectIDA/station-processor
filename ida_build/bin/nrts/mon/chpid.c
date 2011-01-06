#pragma ident "$Id: chpid.c,v 1.2 2000/09/19 23:17:35 nobody Exp $"
#include <stdio.h>
#include <curses.h>
#include <sys/param.h>
#include <signal.h>
#include "nrts_mon.h"

extern WINDOW *stattitle;
extern int winswide;


void chpid(char *sysname, char *hostname, int pid)
{
char string[1024];
int i;

	wmove(stattitle, 0, 0);
	sprintf(string, " %s@%s NRTS status (pid %d) ", sysname, hostname, pid);
  for(i = 0; i < ((winswide)/2 - (strlen(string)/2)) ; i++) {
		waddch(stattitle, '-');
		wrefresh(stattitle);
	}
	waddstr(stattitle, string);
  for(i = 0; i < ((winswide)/2 - (strlen(string)/2)) ; i++) {
		waddch(stattitle, '-');
		wrefresh(stattitle);
	}
}

/* Revision History
 *
 * $Log: chpid.c,v $
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
