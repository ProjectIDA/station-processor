#pragma ident "$Id: do_cmd.c,v 1.3 2006/06/02 21:14:30 dechavez Exp $"
#include <curses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/param.h>
#include <signal.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif
#include "nrts.h"
#include "nrts_mon.h"

#define INDICES 'i'
#define TIMES 't'
#define INCREASE 'V'
#define RESETLOG 'v'
#define NEXT_SYS 'n'
#define PREV_SYS 'N'
/*
#define NEXT_STA 'n'
*/
#define FLUSH 'f'
#define QUIT 'q'
#define HELP 'h'
#define ENTER '\r'


extern WINDOW *statwin;
extern int statwinlen;
extern int indices;
extern int times;
extern int flag_y, flag_x;
extern int winswide;
extern int nsta;
extern int csys, nsys;
extern int chsys;
extern int beg, limchn, diff;
extern int orig_limchn;
extern char **Systems;
extern char Hostname[MAXHOSTNAMELEN];

 
int do_cmd(struct nrts_mmap *map, char *home, struct nrts_files *file, struct nrts_sys *sys, struct nrts_sta *sta, char c)
{
int fd;
int csys_saved;
int nsta_saved;
struct nrts_mmap *map_saved;
struct nrts_files *file_saved;

	if(c == INDICES)  {
			indices = 1;
			times = 0;
			wmove(statwin, flag_y+2, flag_x);
			wclrtobot(statwin);
			wprintw(statwin, "    chan   beg    end");
			wprintw(statwin, "   lend    status   nrec   count   last read");
			/*
			wprintw(statwin, "    chan   beg    end");
			wprintw(statwin, "    lend   status   nrec   last read");
			*/
			wmove(statwin, flag_y+3, flag_x);
	} else if(c == ENTER) {
		if(diff == 0) 
			return 2;
		wmove(statwin, flag_y+3, flag_x);
		wclrtobot(statwin);
		if(beg == 0) {
			beg += orig_limchn;
			limchn += orig_limchn;
			if(limchn > sta->nchn)
				limchn = sta->nchn;
		} else if(beg != 0 && limchn < sta->nchn){
			beg += orig_limchn;
			limchn += orig_limchn;
			if(limchn > sta->nchn)
				limchn = sta->nchn;
		} else {
			beg = 0;
			limchn = orig_limchn;
		}
	} else if(c == TIMES) {
			times = 1;
			indices = 0;
			wmove(statwin, flag_y+CHAN_HDR_OFFSET, flag_x);
			wclrtobot(statwin);
		   if(winswide  < 80 ) {
	  	   wprintw(statwin, "   chan     earliest datum         latest datum      nseg   latency");
		   } else {
     	   wprintw(statwin, "   chan     earliest datum         latest datum      nseg    status   latency");
		   }
			wmove(statwin, flag_y+CHAN_HDR_OFFSET+1, flag_x);
	} else if(c == INCREASE) {
				kill(sys->pid, SIGUSR1);
	} else if(c == RESETLOG) {
				kill(sys->pid, SIGUSR2);
	} else if(c == NEXT_SYS) {
		if(nsys <= 1)
			return 3;
		csys_saved = csys;
		file_saved = file;
		map_saved = map;
		csys++;
		if(csys > nsys - 1)
			csys = 0;
    file = nrts_files(&home, Systems[csys]);
		if ((fd = open(file->sys, O_RDONLY | O_NDELAY, 0644)) < 0) {
			csys = csys_saved;
    	file = file_saved;;
			return 0;
		} 
		close(fd);
		close(map->fd);
  	if(nrts_mmap(file->sys, "r", NRTS_SYSTEM, map) == 0) {
			chsys = 1;
		} else {
			csys = csys_saved;
    	file = file_saved;;
			map = map_saved;
			return 0;
		}			
	} else if(c == PREV_SYS) {
		if(nsys <= 1)
			return 3;
		csys_saved = csys;
		file_saved = file;
		map_saved = map;
		csys--;
		if(csys < 0)
			csys = nsys - 1;
    file = nrts_files(&home, Systems[csys]);
		if ((fd = open(file->sys, O_RDONLY | O_NDELAY, 0644)) < 0) {
			csys = csys_saved;
    	file = file_saved;;
			return 0;
		} 
		close(fd);
		close(map->fd);
  	if(nrts_mmap(file->sys, "r", NRTS_SYSTEM, map) == 0) {
			chsys = 1;
		} else {
			csys = csys_saved;
    	file = file_saved;;
			map = map_saved;
			return 0;
		}			
	} else if(c == FLUSH && sys->pid > 0) {
		kill(sys->pid, SIGHUP);
	} else if(c == QUIT) {
		return -1;
	} else {
		return 0;
	}
		
	return 1;

}

/* Revision History
 *
 * $Log: do_cmd.c,v $
 * Revision 1.3  2006/06/02 21:14:30  dechavez
 * ignore System entries that don't have NRTS disk loops attached
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
