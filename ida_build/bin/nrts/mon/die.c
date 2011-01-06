#pragma ident "$Id: die.c,v 1.2 2000/09/19 23:17:35 nobody Exp $"
#include <curses.h>
#include <signal.h>
#include <sys/types.h>
#include "nrts_mon.h"

extern int Cols, Lines;

void die()
{

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	mvcur(0, Cols-1, Lines-1, 0);
	endwin();
	exit(0);

}

/* Revision History
 *
 * $Log: die.c,v $
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
