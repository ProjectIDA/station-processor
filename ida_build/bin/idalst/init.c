#pragma ident "$Id: init.c,v 1.5 2006/03/24 18:38:44 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include <stdlib.h>
#include "idalst.h"

void init( int  argc, char **argv, struct counters *count, int *print, int *check, int *sort, int *group, int *verbose, char *buffer)
{
int i, rev, ok, data_flag, loglevel = IDA_LOGDEBUG, flags = 0;
long bs;
char *input = NULL, *map = NULL, *sta =  NULL;
DBIO *db;
struct defaults def;
char *dbspec = (char *) NULL;
 
/*  Initialize counters and set default options  */

    memset(count, 0, sizeof(struct counters));
    for (i = 0; i < IDA_MAXSTREAMS; i++) count->stream[i] = count->totpts[i] = 0;

    strcpy(Global->isplog, "./isplog");

    def.print    = 0xffffffff & ~P_TAG & ~P_DATA & ~P_TTRIP;
    def.print   &= ~P_ALLQUAL & ~P_DUNMAP;
    def.check    = 0xffffffff & ~C_LASTW & ~C_TTC;
    def.sort     = BY_STREAM;
    def.group    = BY_SIZE;
    def.input    = "stdin"; /*(char *) NULL;*/
    def.verbose  = 0;

    rev      = -1;
    *print   = def.print;
    *check   = def.check;
    *sort    = def.sort;
    *group   = def.group;
    *verbose = def.verbose;
    loglevel = def.loglevel = 5;
    bs       = def.bs = 1024;
    input    = def.input;
    data_flag= 1;

/*  Scan command line  */
/*  Broken into many if's because MSC compiler couldn't handle it  */

    if (argc == 1) help(&def, stdout);

    for (i = 1; i < argc; i++) {

        ok = 0;
        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i] + strlen("rev=")); ++ok;

        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            input = argv[i] + strlen("if="); ++ok;

        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            bs = util_atolk(argv[i] + strlen("if=")); ++ok;

        } else if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sta = argv[i] + strlen("sta="); ++ok;

        } else if (strncmp(argv[i], "map=", strlen("map=")) == 0) {
            map = argv[i] + strlen("map="); ++ok;

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db="); ++ok;
        }

        if (strcmp(argv[i], "+pall") == 0) {
            if (*print & P_TAG) {
                *print = 0xffffffff, ++ok;
            } else {
                *print = 0xffffffff & ~P_TAG; ++ok;
            }
        } else if (strcmp(argv[i], "+pmap") == 0) {
            *print |= P_STRMAP; ++ok;
        } else if (strcmp(argv[i], "+pcr") == 0) {
            *print |= P_CORRUPT; ++ok;
        } else if (strcmp(argv[i], "+pbt") == 0) {
            *print |= P_BADTAG; ++ok;
        } else if (strcmp(argv[i], "+ptt") == 0) {
            *print |= P_TTRIP; ++ok;
        } else if (strcmp(argv[i], "+pdd") == 0) {
            *print |= P_DUPDAT; ++ok;
        } else if (strcmp(argv[i], "+pum") == 0) {
            *print |= P_DUNMAP; ++ok;
        } else if (strcmp(argv[i], "+ptq") == 0) {
            *print |= P_ALLQUAL; ++ok;
        } else if (strcmp(argv[i], "+pl") == 0) {
            *print |= P_LOG; ++ok;
            init_plog(loglevel);
        } else if (strncmp(argv[i], "+pl=", strlen("+pl=")) == 0) {
            *print |= P_LOG; ++ok;
            init_plog(loglevel = atoi(argv[i] + strlen("+pl=")));
        } else if (strcmp(argv[i], "+pi") == 0) {
            *print |= P_IDENT; ++ok;
        } else if (strcmp(argv[i], "+pd") == 0) {
            *print |= P_DATA; ++ok;
        } else if (strcmp(argv[i], "+psn") == 0) {
            *print |= P_SEQNO; ++ok;
        } else if (strcmp(argv[i], "+ps") == 0) {
            *print |= P_SREC; ++ok;
        } else if (strcmp(argv[i], "+pc") == 0) {
            *print |= P_CALIB; ++ok;
        } else if (strcmp(argv[i], "+pe") == 0) {
            *print |= P_EVENT; ++ok;
        } else if (strcmp(argv[i], "+pk") == 0) {
            *print |= P_CONFIG; ++ok;
        } else if (strcmp(argv[i], "+pdcc") == 0) {
            *print |= P_DCCREC; ++ok;
        } else if (strcmp(argv[i], "+pdet") == 0) {
            *print |= P_DETECT; ++ok;
        } else if (strcmp(argv[i], "+v") == 0) {
            *verbose = TRUE; ++ok;
        } else if (strcmp(argv[i], "+data") == 0) {
            data_flag = TRUE; ++ok;
        }

        if (strcmp(argv[i], "+call") == 0) {
            *check = 0xffffffff; ++ok;
        } else if (strcmp(argv[i], "+ctag") == 0) {
            *check |= C_BADTAG; ++ok;
        } else if (strcmp(argv[i], "+ci") == 0) {
            *check |= C_TINC; ++ok;
        } else if (strcmp(argv[i], "+coff") == 0) {
            *check |= C_TOFF; ++ok;
        } else if (strcmp(argv[i], "+clw") == 0) {
            *check |= C_LASTW; ++ok;
        } else if (strcmp(argv[i], "+cdr") == 0) {
            *check |= C_SAME; ++ok;
        } else if (strcmp(argv[i], "+cns") == 0) {
            *check |= C_NSAMP; ++ok;
        } else if (strcmp(argv[i], "+ctq") == 0) {
            *check |= C_TQUAL; ++ok;
        } else if (strcmp(argv[i], "+ctg") == 0) {
            *check |= C_TTC; ++ok;
        }

        if (strcmp(argv[i], "-pall") == 0) {
            *print = (*print & P_TAG); ++ok;
        } else if (strcmp(argv[i], "-pmap") == 0) {
            *print &= ~P_STRMAP; ++ok;
        } else if (strcmp(argv[i], "-pcr") == 0) {
            *print |= ~P_CORRUPT; ++ok;
        } else if (strcmp(argv[i], "-pbt") == 0) {
            *print |= ~P_BADTAG; ++ok;
        } else if (strcmp(argv[i], "-ptt") == 0) {
            *print &= ~P_TTRIP; ++ok;
        } else if (strcmp(argv[i], "-pdd") == 0) {
            *print &= ~P_DUPDAT; ++ok;
        } else if (strcmp(argv[i], "-pum") == 0) {
            *print &= ~P_DUNMAP; ++ok;
        } else if (strcmp(argv[i], "-ptq") == 0) {
            *print &= ~P_ALLQUAL; ++ok;
        } else if (strcmp(argv[i], "-pl") == 0) {
            *print &= ~P_LOG; ++ok;
        } else if (strcmp(argv[i], "-pi") == 0) {
            *print &= ~P_IDENT; ++ok;
        } else if (strcmp(argv[i], "-pd") == 0) {
            *print &= ~P_DATA; ++ok;
        } else if (strcmp(argv[i], "-psn") == 0) {
            *print &= ~P_SEQNO; ++ok;
        } else if (strcmp(argv[i], "-ps") == 0) {
            *print &= ~P_SREC; ++ok;
        } else if (strcmp(argv[i], "-pc") == 0) {
            *print &= ~P_CALIB; ++ok;
        } else if (strcmp(argv[i], "-pe") == 0) {
            *print &= ~P_EVENT; ++ok;
        } else if (strcmp(argv[i], "-pk") == 0) {
            *print &= ~P_CONFIG; ++ok;
        } else if (strcmp(argv[i], "-pdcc") == 0) {
            *print &= ~P_DCCREC; ++ok;
        } else if (strcmp(argv[i], "-pdet") == 0) {
            *print &= ~P_DETECT; ++ok;
        } else if (strcmp(argv[i], "-v") == 0) {
            *verbose = FALSE; ++ok;
        } else if (strcmp(argv[i], "-data") == 0) {
            data_flag = FALSE; ++ok;
        }

        if (strcmp(argv[i], "-call") == 0) {
            *check = 0x0000; ++ok;
        } else if (strcmp(argv[i], "-ctag") == 0) {
            *check &= ~C_BADTAG; ++ok;
        } else if (strcmp(argv[i], "-ci") == 0) {
            *check &= ~C_TINC; ++ok;
        } else if (strcmp(argv[i], "-coff") == 0) {
            *check &= ~C_TOFF; ++ok;
        } else if (strcmp(argv[i], "-clw") == 0) {
            *check &= ~C_LASTW; ++ok;
        } else if (strcmp(argv[i], "-cdr") == 0) {
            *check &= ~C_SAME; ++ok;
        } else if (strcmp(argv[i], "-cns") == 0) {
            *check &= ~C_NSAMP; ++ok;
        } else if (strcmp(argv[i], "-ctq") == 0) {
            *check &= ~C_TQUAL; ++ok;
        } else if (strcmp(argv[i], "-ctg") == 0) {
            *check &= ~C_TTC; ++ok;
        }

        if (strcmp(argv[i], "+tag") == 0) {
            *print |= P_TAG; ++ok;
        } else if (strcmp(argv[i], "-tag") == 0) {
            *print &= ~P_TAG; ++ok;
        }

        if (strcmp(argv[i], "sort=rec") == 0) {
            *sort = BY_RECORD; ++ok;
        } else if (strcmp(argv[i], "sort=stream") == 0) {
            *sort = BY_STREAM; ++ok;
        }

        if (strcmp(argv[i], "grp=sign") == 0) {
            *group = BY_SIGN; ++ok;
        } else if (strcmp(argv[i], "grp=size") == 0) {
            *group = BY_SIZE; ++ok;
        } else if (strcmp(argv[i], "grp=none") == 0) {
            *group = FALSE; ++ok;
        }

        if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help(&def, stdout);
        }

        if (strncasecmp(argv[i], "isplog=", strlen("isplog=")) == 0) {
            strcpy(Global->isplog, argv[i] + strlen("isplog=")); ++ok;
        }

        if (!ok) {
            fprintf(stderr,"idalst: ");
            fprintf(stderr,"unrecognized argument '%s'\n",argv[i]);
            help(&def, stderr);
        }
    }

/*  Initialize database */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        fprintf(stderr, "idalst: dbioOpen: %s\n", strerror(errno));
        exit(1);
    }

    if ((ida = idaCreateHandle(sta, rev, map, db, NULL, flags)) == NULL) {
        fprintf(stderr, "idalst: idaCreateHandle: %s\n", strerror(errno));
        exit(1);
    }

/*  Open input  */

    if (inigetdat(bs, input, data_flag) != 0) {
        fprintf(stderr,"idalst: can't initialize input\n");
        exit(1);
    }

/*  Print run parameters  */

    pinput( input, bs, data_flag, map, count, *print, *check, *sort, *group, loglevel);

/*  Set up exit handler(s)  */

    exitcode();

}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.5  2006/03/24 18:38:44  dechavez
 * true time tag test support added (akimov)
 *
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/09/30 22:28:19  dechavez
 * seqno support
 *
 * Revision 1.2  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
