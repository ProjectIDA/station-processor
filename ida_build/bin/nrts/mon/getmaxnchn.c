#pragma ident "$Id: getmaxnchn.c,v 1.4 2004/01/29 18:54:04 dechavez Exp $"
#include <sys/param.h>
#include <signal.h>
#include <time.h>
#include "nrts_mon.h"

#define GETLINE util_getline(fp, input, MAXTEXT, '#', &lineno)


int getmaxnchn(char *home, int *nchn)
{
static FILE *fp;
char string[80];
char input[1024];
char *token[MAXTOKENS];
int ntokens;
int lineno  = 0;
static int max_nchn;
struct nrts_mmap map;
struct nrts_sys *sys;
struct nrts_sta *sta;
struct nrts_files *file;
char *ptr_nrts_home;
char nrts_home[256];
char *system;


	if(home == NULL) {
		ptr_nrts_home = getenv(NRTS_HOME);
	} else {
		ptr_nrts_home = home;
	}


	sprintf(string, "%s/etc/Systems", ptr_nrts_home);

	if((fp = fopen(string, "r")) == NULL) {
		fprintf(stderr, "can't open file %s\n", string);
		exit(0);
	} else {
		while(GETLINE == 0) {
			ntokens = util_sparse(input, token, " ", MAXTOKENS);
			if(ntokens > 0) {
				system = token[0];
        file = nrts_files(&home, system);
        if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) != 0) {
            perror(file->sys);
            continue;
        }
        sys = (struct nrts_sys *) map.ptr;
        sta = &sys->sta[0];
		    if(max_nchn < sta->nchn)
			    max_nchn = sta->nchn;
      }
    }
		*nchn = max_nchn;
		fclose(fp);
	}
}

/* Revision History
 *
 * $Log: getmaxnchn.c,v $
 * Revision 1.4  2004/01/29 18:54:04  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.3  2001/05/20 18:43:52  dec
 * remove uneeded includes
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
