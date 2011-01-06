#pragma ident "$Id: etoh.c,v 1.6 2004/12/10 17:51:13 dechavez Exp $"
/*======================================================================
 *
 *  Convert from epoch to human time using specified format.
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"

void help(myname)
char *myname;
{
    fprintf(stderr, "usage: [code=code] [-s] [time ...]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "code  Output string\n");
    fprintf(stderr, " 0    yy:ddd-hh:mm:ss\n");
    fprintf(stderr, " 1    Mon dd, year hh:mm:ss\n");
    fprintf(stderr, " 2    yy:ddd-hh:mm:ss, when time is an interval\n");
    fprintf(stderr, " 3    yydddhhmmss\n");
    fprintf(stderr, " 4    yyyyddd\n");
    fprintf(stderr, " 5    Day Mon dd, year\n");
    fprintf(stderr, " 6    yyyymmddhhmmss\n");
	fprintf(stderr, " 7    yyyy mm dd hh mm ss\n");
	fprintf(stderr, " 8    ddd-hh:mm:ss.msc, where time is an interval\n");
	fprintf(stderr, " 9    yydddhh\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "default code=0\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, j;
int base, code = 0, factor = 0;
int status;
long ltime;
char *string;
char tmpbuf[64];

    if (argc == 1) help(argv[0]);
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "code=", strlen("code=")) == 0) {
            code = atoi(argv[i]+strlen("code="));
            if (code < 0 || code > 9) help(argv[0]);
        } else if (strcmp(argv[i], "-s") == 0) {
            factor = 1000;
        } else {
			if (strstr(argv[i], "x") != NULL) {
				base = 16;
			} else if (strstr(argv[i], "X") != NULL) {
				base = 16;
			} else if (argv[i][0] == '0') {
				base = 8;
			} else {
				base = 10;
			}
			ltime = strtol(argv[i], NULL, base);
            printf("%s\n", utilLttostr(ltime, code+factor, tmpbuf));
            status = 0;
        }
    }

    exit(status);
}

/* Revision History
 *
 * $Log: etoh.c,v $
 * Revision 1.6  2004/12/10 17:51:13  dechavez
 * added support for SAN epoch time stamps
 *
 * Revision 1.5  2004/09/28 23:35:11  dechavez
 * added -s option to support decoding SAN epoch times
 *
 * Revision 1.4  2003/12/17 20:09:11  dechavez
 * switch to utilLttostr
 *
 * Revision 1.3  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2001/10/24 23:35:16  dec
 * added support for format codes 7, 8, 9
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
