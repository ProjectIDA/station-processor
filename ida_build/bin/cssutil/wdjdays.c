#pragma ident "$Id: wdjdays.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  List julian days of given wfdisc file.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"

static long beg, end;
static int first = 1;

static int wdjdays(fp)
FILE *fp;
{
long beg_time, end_time;
struct wfdisc wfdisc;

    while (rwfdrec(fp, &wfdisc) == 0) {
        beg_time = (long) wfdisc.time;
        end_time = (long) wfdisc.endtime;
        if (first) {
            beg = beg_time;
            end = end_time;
            first = 0;
        } else {
            beg = (beg_time < beg) ? beg_time : beg;
            end = (end_time > end) ? end_time : end;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
char *begtime, *endtime, *begyear, *endyear;
int i, j, begday, endday, beg_day, end_day, beg_year, end_year;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "r")) == NULL) {
                fprintf(stderr, "wdjdays: fopen: ");
                perror(fname);
                exit(1);
            } else {
                wdjdays(fp);
                fclose(fp);
            }
        }
    } else {
        wdjdays(stdin);
    }

		/* Allocating memory for date string */
		begtime = (char *) malloc(strlen("yyyyddd") + 1);
		endtime = (char *) malloc(strlen("yyyyddd") + 1);
		/* Allocating memory for year string only*/
		begyear = (char *) malloc(strlen("yyyy") + 1);
		endyear = (char *) malloc(strlen("yyyy") + 1);
		/* Getting beg and end date strings */
		strcpy(begtime, (char *)util_lttostr(beg, 4));
		strcpy(endtime, (char *)util_lttostr(end, 4));
		/* Extracting year string from entire date */
		strncpy(begyear, begtime, 4);
		strncpy(endyear, endtime, 4);
		/* Converting strings of beg date to the type 'int' */
		begday = atoi(begtime);
		beg_year = atoi(begyear);
		beg_day = atoi(begtime + 4);
		/* Converting strings of end date to the type 'int' */
		endday = atoi(endtime);
		endday = atoi(endtime);
		end_year = atoi(endyear);
		end_day = atoi(endtime + 4);
		/* If we have end_day < end_day, it is the case around New Year */
		/* I do not check leap year and just use 366 days - it is not big deal */
		if(beg_day > end_day) {
			/* Before New Year */
			for(i = beg_day; i <= 366; i++) {
				sprintf(begtime, "%4d%3d", beg_year, i);
				for(j = 0; j <= strlen(begtime); j++) 
					if(begtime[j] == ' ')
						begtime[j] = '0';
				printf("%s\n", begtime);
			}
			/* New Year - from first day to end_day */
			/* Not only one year, because I saw a problem with year, which */
			/* was on BORG data( I saw 1997 year). It was error, but if */
			/* I used only end_year, 1996 was missed*/
			for(++beg_year; beg_year <= end_year; beg_year++) {
				for(i = 1; i <= end_day; i++) {
					sprintf(begtime, "%4d%3d", beg_year, i);
					for(j = 0; j <= strlen(begtime); j++) 
						if(begtime[j] == ' ')
							begtime[j] = '0';
					printf("%s\n", begtime);
				}
			}
		} else {
			/* The case of one year */
			for(i = begday; i <= endday; i++)
    		printf("%d\n", i);
		}

		free(begtime); free(endtime);
		free(begyear); free(endyear);
    exit(0);
}

/* Revision History
 *
 * $Log: wdjdays.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
