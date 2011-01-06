#pragma ident "$Id: init.c,v 1.8 2007/09/25 21:03:30 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "i10dmx.h"
#include "q330.h"

extern int saveTtags;

void init(int  argc, char **argv, struct counter *count, int *verbose, FILE **fp)
{
int i;
int cssformat = CSS_30;
char *input = NULL;
char *sname = NULL;
char *dbspec = NULL;
char *outdir;
char *cfgpath = NULL;
Q330_CFG *cfg = NULL;
static Q330_DETECTOR detector;

/*  Set initial values  */

    memset(count, 0, sizeof(struct counter));

    outdir     = "dmxdata";
    *verbose   = 0;

/*  Scan command line  */

    for (i = 1; i < argc; i++) {

        if (strncmp(argv[i], "sta=", strlen("sta=")) == 0) {
            sname = argv[i] + strlen("sta=");

        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            input = argv[i] + strlen("if=");

        } else if (strncmp(argv[i], "outdir=", strlen("outdir=")) == 0) {
            outdir = argv[i] + strlen("outdir=");

        } else if (strncmp(argv[i], "datdir=", strlen("datdir=")) == 0) {
            outdir = argv[i] + strlen("datdir=");

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");;

        } else if (strncmp(argv[i], "detect=", strlen("detect=")) == 0) {
            cfgpath = argv[i] + strlen("detect=");;

        } else if (strncmp(argv[i], "+ttag", strlen("+ttag")) == 0) {
            saveTtags = 1;

        } else if (strncmp(argv[i], "-ttag", strlen("-ttag")) == 0) {
            saveTtags = 0;

        } else if (strncmp(argv[i], "+v", strlen("+v")) == 0) {
            *verbose = 1;
            
        } else if (strncmp(argv[i], "-v", strlen("-v")) == 0) {
            *verbose = 0;

        } else if (strncmp(argv[i], "-h", strlen("-h")) == 0) {
            help();

        } else {
            fprintf(stderr,"Unrecognized argument '%s'\n",argv[i]);
            help();
        }
    }

/*  Open input  */

    if (input == NULL) {
        *fp = stdin;
    } else if ((*fp = fopen(input, "rb")) == NULL) {
        fprintf(stderr,"i10dmx: fopen: ");
        perror(input);
        exit(1);
    }

/* detect option precludes use of default station name */

    if (cfgpath != NULL) {
        if ((cfg = q330ReadCfg(cfgpath)) == NULL) {
            fprintf(stderr, "q330ReadCfg: %s: %s\n", cfgpath, strerror(errno));
            exit(1);
        }
        if (!q330LookupDetector(sname, cfg, &detector)) {
            fprintf(stderr, "q330LookupDetector: %s\n", strerror(errno));
            exit(1);
        }
        if (!detector.enabled) {
            fprintf(stderr, "No detector found in %s\n", cfgpath);
            exit(1);
        } else {
            InitDetector(cfgpath, &detector);
        }
        sname = NULL;
    }

/*  Initialize output directory and name generator  */

    nameinit(outdir, sname);

/*  Initialize the error logger  */

    init_msglog(input, count);

/*  Initialize the ttag logger */

    if (saveTtags) init_ttag();

/*  Initialize wfdisc routines  */

    init_wfdisc(cssformat, count, dbspec);

/* Initialized buffered I/O */

    utilInitOutputStreamBuffers();

/*  Set up exit handlers  */

    exitcode();

}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.8  2007/09/25 21:03:30  dechavez
 * added detector support
 *
 * Revision 1.7  2006/02/09 20:12:38  dechavez
 * libisidb database support
 *
 * Revision 1.6  2005/10/11 22:50:54  dechavez
 * allow for datdir option as alias for outdir
 *
 * Revision 1.5  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.4  2002/03/15 23:03:11  dec
 * FILE input required instead of MIO
 *
 * Revision 1.3  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.2  2000/11/06 23:17:43  dec
 * Release 1.1.1
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
