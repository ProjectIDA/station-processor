#pragma ident "$Id: help_menu.c,v 1.2 2000/09/19 23:17:35 nobody Exp $"
#include <curses.h>
#include <signal.h>
#include "nrts.h"
#include "nrts_mon.h"


extern WINDOW *statwin;
extern int indices;
extern int times;
extern int flag_y, flag_x;
extern int winswide;
extern int statwinbeg_y;
extern int statwinlen;

void nomoresys();
void nomorechn();
void drawhelpwin();
void mydelwin();
void redraw();

int help_menu(struct nrts_mmap *map, char *home, struct nrts_files *file, struct nrts_sys *sys, struct nrts_sta *sta)
{
WINDOW *helpwin;
int helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx;
char c;
int ret, w1, h1, bx1, by1, w2, h2, bx2, by2;

	halfdelay(100);
	helpwin_begy = statwinbeg_y + statwinlen/2 - 4;
	helpwin_begx = winswide/2 - 18;
	helpwin_maxy = 12;
	helpwin_maxx = 36;

  w1 = statwin->_maxx;
  h1 = statwin->_maxy;
  bx1 = statwin->_begx;
  by1 = statwin->_begy;

  bx2 = w1/2 - 18;
  by2 = by1 + 3;

  helpwin = subwin(statwin, helpwin_maxy, helpwin_maxx, by2, bx2);
	
/*
	helpwin = newwin(helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx);
*/
  h2 = helpwin->_maxy;
  w2 = helpwin->_maxx;
  bx2 = helpwin->_begx;
  by2 = helpwin->_begy;
/*
  mvcur(0, COLS-1, LINES-1, 0);
  endwin();

 fprintf(stderr, "statwin:h =%d, w = %d, by = %d, bx = %d\n", h1, w1, by1, bx1); 
 fprintf(stderr, "helpwin:h =%d, w = %d, by = %d, bx = %d\n", h2, w2, by2, bx2); 
  exit(0);
*/
	drawhelpwin(helpwin);

	while(1) {
		c = wgetch(helpwin);
		if(c == ' ') {
			mydelwin(helpwin);
			redraw();
			break;
		} else {
			ret = do_cmd(map, home, file, sys, sta, c);
			if(ret == 0) {
				continue;
			} else if(ret < 0) {
				return -1;
			} else if(ret == 2) {
				mydelwin(helpwin);
				nomorechn();
				redraw();
				break;
			} else if(ret == 3) {
				mydelwin(helpwin);
				nomoresys();
				redraw();
				break;
			} else  {
				mydelwin(helpwin);
				redraw();
				break;
			}
		}
	}  

	nocbreak();
	halfdelay(1);
	return 0;

}

void mydelwin(helpwin)
WINDOW *helpwin;
{

	werase(helpwin);
	wrefresh(helpwin);
	delwin(helpwin);

}

void redraw()
{

	wmove(statwin, flag_y+CHAN_HDR_OFFSET, flag_x);
	if(indices) {
		wclrtobot(statwin);
		wmove(statwin, flag_y+CHAN_HDR_OFFSET, flag_x);
		wprintw(statwin, "     chan  beg    end");
		wprintw(statwin, "   lend    status   nrec   count   last read");
	} else if(times) {
		wclrtobot(statwin);
		wmove(statwin, flag_y+CHAN_HDR_OFFSET, flag_x);
		if(winswide  < 80 ) {
	  	wprintw(statwin, "   chan     earliest datum         latest datum      nseg   latency");
		} else {
     	wprintw(statwin, "   chan     earliest datum         latest datum      nseg    status   latency");
		}
	}
	wrefresh(statwin);

}


void drawhelpwin(helpwin)
WINDOW *helpwin;
{

	mvwaddstr(helpwin,  0, 0, "+----------------------------------+");
	mvwaddstr(helpwin,  1, 0, "|  n - next system                 |");
	mvwaddstr(helpwin,  2, 0, "|  i - display dl indicies         |");
	mvwaddstr(helpwin,  3, 0, "|  f - flush wfdiscs               |");
	mvwaddstr(helpwin,  4, 0, "|  h - print this message          |");
	mvwaddstr(helpwin,  5, 0, "|  t - display dl time stamps      |");
	mvwaddstr(helpwin,  6, 0, "|  q - quit this program           |");
	mvwaddstr(helpwin,  7, 0, "|  V - increase log level          |");
	mvwaddstr(helpwin,  8, 0, "|  v - reset log level             |");
	mvwaddstr(helpwin,  9, 0, "|  ENTER - next batch of channels  |");
	mvwaddstr(helpwin, 10, 0, "|  press SPACE to exit help        |");
	mvwaddstr(helpwin, 11, 0, "+----------------------------------+");
	wrefresh(helpwin);


}

void nomorechn()
{
WINDOW *helpwin;
int helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx;
char c;
int w1, h1, bx1, by1, w2, h2, bx2, by2;

	helpwin_begy = statwinbeg_y + statwinlen/2 - 4;
	helpwin_begx = winswide/2 - 22;
	helpwin_maxy = 12;
	helpwin_maxx = 45;

  w1 = statwin->_maxx;
  h1 = statwin->_maxy;
  bx1 = statwin->_begx;
  by1 = statwin->_begy;

  bx2 = w1/2 - 18;
  by2 = by1 + 3;

  helpwin = subwin(statwin, helpwin_maxy, helpwin_maxx, by2, bx2);
	
/*
	helpwin = newwin(helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx);
*/

	mvwaddstr(helpwin,  0, 0, "+-------------------------------------------+");
	mvwaddstr(helpwin,  1, 0, "|                                           |");
	mvwaddstr(helpwin,  2, 0, "|                                           |");
	mvwaddstr(helpwin,  3, 0, "|                                           |");
	mvwaddstr(helpwin,  4, 0, "| All configured channels already displayed |");
	mvwaddstr(helpwin,  5, 0, "|                                           |");
	mvwaddstr(helpwin,  6, 0, "|                                           |");
	mvwaddstr(helpwin,  7, 0, "|        Press any key to continue          |");
	mvwaddstr(helpwin,  8, 0, "|                                           |");
	mvwaddstr(helpwin,  9, 0, "|                                           |");
	mvwaddstr(helpwin, 10, 0, "|                                           |");
	mvwaddstr(helpwin, 11, 0, "+-------------------------------------------+");
	wrefresh(helpwin);

	c = wgetch(helpwin);
	mydelwin(helpwin);
		
}

void nomoresys()
{
WINDOW *helpwin;
int helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx;
char c;
int w1, h1, bx1, by1, w2, h2, bx2, by2;

	helpwin_begy = statwinbeg_y + statwinlen/2 - 4;
	helpwin_begx = winswide/2 - 22;
	helpwin_maxy = 12;
	helpwin_maxx = 37;
	
  w1 = statwin->_maxx;
  h1 = statwin->_maxy;
  bx1 = statwin->_begx;
  by1 = statwin->_begy;

  bx2 = w1/2 - 18;
  by2 = by1 + 3;

  helpwin = subwin(statwin, helpwin_maxy, helpwin_maxx, by2, bx2);
/*
	helpwin = newwin(helpwin_maxy, helpwin_maxx, helpwin_begy, helpwin_begx);
*/

	mvwaddstr(helpwin,  0, 0, "+-----------------------------------+");
	mvwaddstr(helpwin,  1, 0, "|                                   |");
	mvwaddstr(helpwin,  2, 0, "|                                   |");
	mvwaddstr(helpwin,  3, 0, "|                                   |");
	mvwaddstr(helpwin,  4, 0, "|  No additional systems available  |");
	mvwaddstr(helpwin,  5, 0, "|                                   |");
	mvwaddstr(helpwin,  6, 0, "|                                   |");
	mvwaddstr(helpwin,  7, 0, "|     Press any key to continue     |");
	mvwaddstr(helpwin,  8, 0, "|                                   |");
	mvwaddstr(helpwin,  9, 0, "|                                   |");
	mvwaddstr(helpwin, 10, 0, "|                                   |");
	mvwaddstr(helpwin, 11, 0, "+-----------------------------------+");
	wrefresh(helpwin);

	c = wgetch(helpwin);
	mydelwin(helpwin);
		
}

/* Revision History
 *
 * $Log: help_menu.c,v $
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
