#pragma ident "$Id: chsystem.c,v 1.6 2006/11/13 19:38:54 dechavez Exp $"
#include <curses.h>
#include <sys/param.h>
#include <signal.h>
#include <time.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif
#include "nrts.h"
#include "util.h"
#include "nrts_mon.h"

#define GETLINE util_getline(fp, input, MAXTEXT, '#', &lineno)

extern WINDOW *statwin;
extern int Cols;
extern int Lines;
extern int nsys;
extern int csys;
extern char **Systems;

char Hostname[MAXHOSTNAMELEN];

int chsystem(char *site, char *home)
{
static FILE *fp;
int i, s, c, y, x, j;
char string[80];
char input[1024];
char *token[MAXTOKENS];
int ntokens;
int lineno  = 0;

	mvwprintw(statwin, 0, 1, " SYSTEMS: ");
	for( i = 0, j = 1; i < nsys; i++) {
		if(strcmp(Systems[i], site) == 0) {
			wstandout(statwin);
			getyx(statwin, y, x);
			if (x + 5 >= Cols - 27) wmove(statwin, j++, 11);
			wprintw(statwin, "%-6.6s ", site);
			wstandend(statwin);
		} else {
			getyx(statwin, y, x);
			if (x + 5 >= Cols - 27) wmove(statwin, j++, 11);
			wprintw(statwin, "%-6.6s ", Systems[i]);
		}
	}
}

/* Revision History
 *
 * $Log: chsystem.c,v $
 * Revision 1.6  2006/11/13 19:38:54  dechavez
 * support 6 character station names
 *
 * Revision 1.5  2006/06/02 21:14:30  dechavez
 * ignore System entries that don't have NRTS disk loops attached
 *
 * Revision 1.4  2004/01/29 18:54:04  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.3  2001/09/12 22:40:43  dec
 * increase sysname length to 5
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
