#pragma ident "$Id: log_win.c,v 1.3 2003/12/10 06:24:47 dechavez Exp $"
#include <curses.h>
#include "nrts.h"
#include "util.h"
#include "nrts_mon.h"

#define MAXLINELEN 1024

extern WINDOW *logwin;
extern long curpos;
extern int logwinlen;

void log_win(FILE *fp, char *logfile)
{
char input[MAXLINELEN];
long newpos;
int linestoskeep, nlines;


	fseek(fp, curpos, SEEK_SET);	

	fseek(fp, 0L, SEEK_END);
	newpos = ftell(fp);
	if(newpos > curpos) {
		fseek(fp, curpos, SEEK_SET);
		nlines = 0;
		while(fgets(input, MAXLINELEN, fp) != NULL) {
			++nlines;
		}
		linestoskeep = nlines - logwinlen;
		if(linestoskeep > 0) {
			for(nlines = 0; nlines <= linestoskeep; nlines++)
				fgets(input, MAXLINELEN, fp);
			curpos = ftell(fp);
		}
	}

	fseek(fp, curpos, SEEK_SET);	
	
	while(fgets(input, MAXLINELEN, fp) != NULL) {
		if (strstr(input, "flushing log") != NULL) {
			fclose(fp);
			sleep(1);
			while ((fp = fopen(logfile, "r")) == (FILE *) NULL);
		} else if (strstr(input, "log flushed") == NULL) {
			waddstr(logwin, input);
			wrefresh(logwin);
		}
		curpos = ftell(fp);
	}

}

/* Revision History
 *
 * $Log: log_win.c,v $
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
