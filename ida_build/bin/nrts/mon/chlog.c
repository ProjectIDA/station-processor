#pragma ident "$Id: chlog.c,v 1.2 2000/09/19 23:17:35 nobody Exp $"
#include <curses.h>
#include "nrts_mon.h"

extern WINDOW *logtitle;
extern int winswide;

int chlog(char *log) 
{
char string[128];
int i;

	sprintf(string, " %s ", log);
 	for(i = 0; i < ((winswide)/2 - (strlen(string)/2)) + 1 ; i++) {
		waddch(logtitle, '-');
		wrefresh(logtitle);
	}
	waddstr(logtitle, string);
 	for(i = 0; i < ((winswide)/2 - (strlen(string)/2)) ; i++) {
		waddch(logtitle, '-');
		wrefresh(logtitle);
	}
}

/* Revision History
 *
 * $Log: chlog.c,v $
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
