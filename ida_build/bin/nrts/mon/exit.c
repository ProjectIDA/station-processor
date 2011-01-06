#pragma ident "$Id: exit.c,v 1.2 2000/09/19 23:17:35 nobody Exp $"
#include "nrts_mon.h"
	mvcur(0, Cols-1, Lines-1, 0);
	endwin();
	exit(0);

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
