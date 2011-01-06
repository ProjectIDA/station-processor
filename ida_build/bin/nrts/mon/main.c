#pragma ident "$Id: main.c,v 1.4 2006/06/02 21:14:30 dechavez Exp $"
/*======================================================================
 *
 *  Print contents of specified NRTS system buffer.
 *
 *====================================================================*/
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <curses.h>
#ifdef SCO_SV
#include <sys/socket.h>
#endif
#ifdef SOLARIS
#include <netdb.h>
#endif
#include "nrts.h"
#include "util.h"
#include "nrts_mon.h"

#define LOGFILE "nrtslog"

int nsta;
int csys;
int chsys;
int diff;
int beg;
int limchn;
int orig_limchn;
char **Systems;
char Hostname[MAXHOSTNAMELEN];

static int help(char *myname)
{
    fprintf(stderr,"usage: %s [home=path] [logwin=#] [+indices | +times] [sys_name] \n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
int fd;
static FILE *fp;
int i, j, status = 0;
int check = 1;
char logfile[MAXPATHLEN+1];
char *home    = NULL;
struct nrts_files *file;
struct nrts_sys *sys;
struct nrts_sta *sta;
struct nrts_mmap map;
pid_t pid;
pid_t savepid;
int nchn, nlines, linestoskeep;
char input[MAXLINELEN];
char c;
char *site=NULL;
static int first = 1;

		logwinlen = 10;
		logwinarg = 0;
		indices = 0;
		times = 1;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        }else if (strncmp(argv[i], "check=", strlen("check=")) == 0) {
            check = atoi(argv[i] + strlen("check="));
				} else if(strncmp(argv[i], "logwin=", strlen("logwin=")) == 0) {
		         logwinlen = atoi(argv[i] +strlen("logwin="));
						 logwinarg = 1;
				} else if(strncmp(argv[i], "+indices", strlen("+indices")) == 0) {
					indices = 1;
					times = 0;
				} else if(strncmp(argv[i], "+times", strlen("+times")) == 0) {
					indices = 0;
					times = 1;
				} else if(strncmp(argv[i], "-h", strlen("-h")) == 0) {
					help(argv[0]);
					exit(0);
        } else {
            if (site != NULL) help(argv[0]);
            site = argv[i];
        }
    }

	if (home == NULL) home = getenv(NRTS_HOME);
	if (home == NULL) home = NRTS_DEFHOME;
	site = LoadSystems(home, site);

	sprintf(logfile, "%s/log/%s", home, LOGFILE);

		getmaxnchn(home, &nchn);

/*       Signal controlling           */

		signal(SIGINT, die);
		signal(SIGTERM, die);
	
/*  Get file names  */

   file = nrts_files(&home, site);

/*  Get system map  */

    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) != 0) {
        fprintf(stderr,"%s: nrts_mmap: ", argv[0]);
        perror(file->sys);
        exit(1);
    }

    sys = (struct nrts_sys *) map.ptr;
		nsta = 0;
    sta = &sys->sta[nsta];

/*  Creating screen */

		initscr();
		halfdelay(1);
		Cols = COLS; Lines = LINES;
		if(Lines < 17) {
			mvcur(0, Cols-1, Lines-1, 0);
			endwin();
			fprintf(stderr, "too few lines on the window( min - 17)\n");
			exit(1);
		}
			
		mkscreen(home, sys, site, sta->name, nchn, logfile);

	
		chsys = 1;
   	while(1)  {
			if(chsys) {
				if(logwinlen > 0 && first) {
					werase(logtitle);
					wrefresh(logtitle);
					chlog(logfile);
					werase(logwin);
					wrefresh(logwin);
					if(fp != NULL)
						fclose(fp);
					if((fp = fopen(logfile, "r")) == NULL) {
						fprintf(stderr,"can't open logfile %s\n", logfile);
						break;
					} else {
						nlines = 0;		
						while(fgets(input, MAXLINELEN, fp) != NULL) {
							++nlines;
						}
						linestoskeep = nlines - logwinlen;
						rewind(fp);
						for(nlines = 0; nlines <= linestoskeep; nlines++)
							fgets(input, MAXLINELEN, fp);
						curpos = ftell(fp);
					}
				}
				wmove(statwin, flag_y+(CHAN_HDR_OFFSET+1), flag_x);
				wclrtobot(statwin);
    		sys = (struct nrts_sys *) map.ptr;
				nsta = 0;
				sta = &sys->sta[nsta];
				if(!first) {
					chpid(Systems[csys], Hostname, sys->pid);
					chsystem(Systems[csys], home);
				}
				first = 0;
				
				if(statwinlen < (sta->nchn + flag_y + (CHAN_HDR_OFFSET+1)))
					diff = sta->nchn + flag_y + (CHAN_HDR_OFFSET+1) - statwinlen;
				else
					diff = 0;
				beg  = 0;
				limchn = sta->nchn - diff;
				if(diff !=0 && limchn%(CHAN_HDR_OFFSET+1)) {
					limchn--;
					if(limchn%(CHAN_HDR_OFFSET+1)) 
						limchn--;
				}
				orig_limchn = limchn;
			
			}
			if(!chsys && logwinlen > 0) log_win(fp, logfile);
			if(chsys)
				chsys = 0;
			for (i = beg, j = 0; i < limchn; i++, j++)  {
				stat_win(sys, Systems[csys], sta->chn + i, j, sta->nchn);
			}
			c = wgetch(statwin);
			if(c == 'h' && help_menu(&map, home, file, sys, sta) < 0) {
					break;
			} else if(do_cmd(&map, home, file, sys, sta, c) < 0) {
				break;
			}
			wrefresh(statwin);
		}

		mvcur(0, Cols-1, Lines-1, 0);
		endwin();
		exit(0);
        
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.4  2006/06/02 21:14:30  dechavez
 * ignore System entries that don't have NRTS disk loops attached
 *
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
