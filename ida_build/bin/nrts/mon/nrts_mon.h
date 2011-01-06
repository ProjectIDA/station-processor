#pragma ident "$Id: nrts_mon.h,v 1.5 2006/06/02 21:14:30 dechavez Exp $"
#ifndef nrst_stat_h_included
#define nrts_stat_h_included

#include <curses.h>
#include "util.h"
#include "nrts.h"

#define MAXTOKENS 128
#define MAXLINELEN 1024
#define MAXTEXT   128

#ifdef PTWC_SPECIFIC_BUILD
#define CHAN_HDR_OFFSET 5
#else
#define CHAN_HDR_OFFSET 4
#endif

int getmaxnchn(char *home, int *nchn);
char *LoadSystems(char *home, char *defsys);
int help_menu(struct nrts_mmap *map, char *home, struct nrts_files *file, struct nrts_sys *sys, struct nrts_sta *sta);
int chlog(char *log);
int chsystem(char *sysname, char *home);
int mkscreen();
void chpid();
void log_win();
int do_cmd(struct nrts_mmap *map, char *home, struct nrts_files *file, struct nrts_sys *sys, struct nrts_sta *sta, char c);
int help_win();
void stat_win();
void die();
int Cols, Lines;
int logwinlen;
int winswide;
int logwinarg;
int statwinlen;
int statwinbeg_y;
int statwinlines;
int flag_y;
int flag_x;
long curpos;
int indices;
int times;
int nsys;
WINDOW *logwin;
WINDOW *statwin;
WINDOW *logtitle;
WINDOW *stattitle;

#endif

/* Revision History
 *
 * $Log: nrts_mon.h,v $
 * Revision 1.5  2006/06/02 21:14:30  dechavez
 * ignore System entries that don't have NRTS disk loops attached
 *
 * Revision 1.4  2003/12/10 06:24:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2000/10/11 20:37:29  dec
 * change default (non-PTWC) chan offset to 4
 *
 * Revision 1.2  2000/09/19 23:17:36  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
