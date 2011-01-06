#pragma ident "$Id: init.c,v 1.6 2006/06/27 00:03:59 akimov Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "idadmx.h"
#include "dbio.h"

extern IDA *ida;

void init(int argc, char **argv, struct counter *count, int *verbose, int *echo, MIO **fp, int *ffu_shift, FILE **hp, unsigned long *swab, int *fixyear_flag)
{
int i, dump_head, ok = 1, rev;
int cssformat = CSS_30;
int beg_yr = -1;
int end_yr = -1;
int beg_da = -1;
int end_da = -1;
int flags = 0;
char *dis_name;
char *map = NULL;
char *input = NULL;
char *sname = NULL;
char *outdir, *ident;
char *spec = NULL;
DBIO *db = NULL;

/*  Set initial values  */

    memset(count, 0, sizeof(struct counter));

    outdir     = "dmxdata";
    rev        = -1;
    ident      = NULL;
    dis_name   = NULL;
    *verbose   = 0;
    *echo      = 0;
    dump_head  = 0;
    *ffu_shift = 1; /* apply Fels' fuck up shifts by default (sigh) */
    *swab      = 1; /* byte swap waveform data into host order by default */
    *fixyear_flag = 1; /* try to make sense of year transition */

/*  Scan command line  */

    if (argc == 1) help();

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i] + strlen("rev="));

        } else if (strncmp(argv[i], "map=", strlen("map=")) == 0) {
            map = argv[i] + strlen("map="); ++ok;

        } else if (strcmp(argv[i], "3.0") == 0) {
            cssformat = CSS_30;

        } else if (strcmp(argv[i], "2.8") == 0) {
            cssformat = CSS_28;

        } else if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sname = argv[i] + strlen("sta=");

        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            input = argv[i] + strlen("if=");

        } else if (strncmp(argv[i], "id=", strlen("id=")) == 0) {
            ident = argv[i] + strlen("id=");

        } else if (strncmp(argv[i], "outdir=", strlen("outdir=")) == 0) {
            outdir = argv[i] + strlen("outdir=");

        } else if (strncmp(argv[i], "datdir=", strlen("datdir=")) == 0) {
            outdir = argv[i] + strlen("outdir=");

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            spec = argv[i] + strlen("db=");

        } else if (strncmp(argv[i], "yrs=", strlen("yrs=")) == 0) {
            if (strlen(argv[i]) != strlen("yrs=yy/yy")) {
                fprintf(stderr,"idadmx: ERROR - syntax error '%s'\n",argv[i]);
                help();
            }
            sscanf(argv[i] + strlen("yrs="), "%2d/%2d", &beg_yr, &end_yr);
            if(beg_yr<70) beg_yr+= 2000; else beg_yr+= 1900; 
            if(end_yr<70) end_yr+= 2000; else end_yr+= 1900; 
        } else if (strncmp(argv[i], "days=", strlen("days=")) == 0) {
            if (strlen(argv[i]) != strlen("days=ddd/ddd")) {
                fprintf(stderr,"idadmx: ERROR - syntax error '%s'\n",argv[i]);
                help();
            }
            sscanf(argv[i] + strlen("days="), "%3d/%3d", &beg_da, &end_da);
        
        } else if (strncmp(argv[i], "+echo", strlen("+echo")) == 0) {
            *echo = 1;
            
        } else if (strncmp(argv[i], "-echo", strlen("-echo")) == 0) {
            *echo = 0;

        } else if (strncmp(argv[i], "+header", strlen("+header")) == 0) {
            dump_head = 1;

        } else if (strncmp(argv[i], "-header", strlen("-header")) == 0) {
            dump_head = 0;
            
        } else if (strncmp(argv[i], "+ffu_shift", strlen("+ffu_shift")) == 0) {
            *ffu_shift = 1;
            
        } else if (strncmp(argv[i], "-ffu_shift", strlen("-ffu_shift")) == 0) {
            *ffu_shift = 0;
            
        } else if (strncmp(argv[i], "+swab", strlen("+swab")) == 0) {
            *swab = 1;
            
        } else if (strncmp(argv[i], "-swab", strlen("-swab")) == 0) {
            *swab = 0;

        } else if (strncmp(argv[i], "+v", strlen("+v")) == 0) {
            *verbose = 1;
            
        } else if (strncmp(argv[i], "-v", strlen("-v")) == 0) {
            *verbose = 0;
            
        } else if (strncmp(argv[i], "-fixyear", strlen("-fixyear")) == 0) {
            *fixyear_flag = 0;

        } else if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help();

        } else {
            fprintf(stderr,"Unrecognized argument '%s'\n",argv[i]);
            help();
        }
    }

    if ((db = dbioOpen(spec, NULL)) == NULL) {
       fprintf(stderr, "idadmx: dbioOpen: %s: %s\n", spec ? spec : "$DBIO_DATABASE", strerror(errno));
        exit(1);
    }

    ok = 1;

/*  Station code and packet rev are required  */

    if (sname == NULL) {
        fprintf(stderr,"idadmx: no station code given\n");
        ok = 0;
    } else {
        for (i = 0; i < strlen(sname); i++) {
            if (!isprint(sname[i])) {
                fprintf(stderr, "idadmx: unprintable station code");
                ok = 0;
            }
        }
    }

    if (!ok) exit(1);

/*  Initialize global handle */

    if ((ida = idaCreateHandle(sname, rev, map, db, NULL, flags)) == NULL) {
        fprintf(stderr, "idadmx: idaCreateHandle: %s\n", strerror(errno));
        exit(1);
    }

/*  Initialize year checker  */

    init_fixyear(beg_yr, end_yr, beg_da, end_da);

/*  Open input  */

    if (input == NULL) input = "stdin";
    if ((*fp = mioopen(input, "rb")) == NULL) {
        fprintf(stderr,"idadmx: mioopen: ");
        perror(input);
        exit(1);
    }

/*  Initialize output directory and name generator  */

    nameinit(outdir, ident, dump_head, hp);

/*  Initialize the error logger  */

    init_msglog(input, count);

/*  Initialize the data reader  */

    *swab = init_rdrec(*verbose, *echo, *ffu_shift, *swab, *fixyear_flag);

/*  Initialize wfdisc routines  */

    init_wfdisc(cssformat, count, *swab);

/*  Set up exit handlers  */

    exitcode();

}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.6  2006/06/27 00:03:59  akimov
 * removed unreferenced local variables
 *
 * Revision 1.5  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.4  2005/10/11 22:49:33  dechavez
 * allow datdir option as an alias for outdir
 *
 * Revision 1.3  2005/02/16 18:29:45  dechavez
 * fixed Y2K problem decoding yrs= parameters (aap)
 *
 * Revision 1.2  2002/03/11 17:17:33  dec
 * clarified channel map error messages
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
