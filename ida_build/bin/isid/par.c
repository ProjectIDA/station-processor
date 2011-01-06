#pragma ident "$Id: par.c,v 1.17 2008/10/10 22:47:24 dechavez Exp $"
/*======================================================================
 *
 *  Get run time parameters. 
 *
 *====================================================================*/
#define INCLUDE_IACP_DEFAULT_ATTR
#include "isid.h"

#define MY_MOD_ID ISID_MOD_LOADINI

#define DELIMITERS " =\t"
#define MAX_TOKEN    32
#define PBLEN       256
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

static VOID PrintHelp(CHAR *myname)
{
#ifndef WIN32_SERVICE
    fprintf(stderr, "%s\n", VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: %s [options]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "db=spec         => set database to `spec'\n");
#ifdef WIN32
    fprintf(stderr, "priority=string => process priority class\n");
#endif /* WIN32 */
    fprintf(stderr, "user=nrts       => ");
    fprintf(stderr, "run as specified user\n");
    fprintf(stderr, "port=value      => ");
    fprintf(stderr, "port number for client connections\n");
    fprintf(stderr, "status=value    => ");
    fprintf(stderr, "port number for status reports\n");
    fprintf(stderr, "bs=value        => ");
    fprintf(stderr, "maximum size of both incoming and outgoing messages\n");
    fprintf(stderr, "ibs=value       => ");
    fprintf(stderr, "maximum size of incoming messages\n");
    fprintf(stderr, "obs=value       => ");
    fprintf(stderr, "maximum size of outgoing messages\n");
    fprintf(stderr, "maxclient=value => ");
    fprintf(stderr, "maximum number of simultaneous clients allowd\n");
    fprintf(stderr, "log=name        => ");
    fprintf(stderr, "log specifier\n");
#ifdef unix
    fprintf(stderr, "-bd             => ");
    fprintf(stderr, "run as a daemon (in the background)\n");
#endif
    fprintf(stderr, "-debug=         => ");
    fprintf(stderr, "turn on debug messages\n");
    fprintf(stderr, "-logtt=         => ");
    fprintf(stderr, "log time tears\n");
    fprintf(stderr, "\n");
#endif /* WIN32_SERVICE */

    exit(1);
}

static VOID Fail(char *fid, FILE *fp, char *path, char *token, UINT32 lineno)
{
    fprintf(stderr, "FATAL ERROR: %s: syntax error at line ", fid);
    fprintf(stderr, "%lu, file `%s', token `%s'\n", lineno, path, token);
    fclose(fp);
    exit(MY_MOD_ID + 1);
}

PARAM *LoadPar(char *prog, int argc, char **argv)
{
int i;
char *path;
char *dbspec = NULL;
static char *bearg[2];
static PARAM par;
static CHAR *default_user = DEFAULT_USER;
static CHAR *fid = "LoadPar";

/* Set default parameters */

    par.daemon      = FALSE;
    par.priority    = DEFAULT_PROCESS_PRIORITY;
    par.log         = NULL;
    par.user        = default_user;
    par.port        = DEFAULT_PORT;
    par.status      = DEFAULT_STATUS;
    par.maxclient   = DEFAULT_MAXCLIENT;
    par.buflen.send = DEFAULT_SENDBUFLEN;
    par.buflen.recv = DEFAULT_RECVBUFLEN;
    par.attr        = IACP_DEFAULT_ATTR;
    par.debug       = FALSE;
    par.logtt       = FALSE;
    par.nrts        = DEFAULT_NRTS_PORT;

/* Scan the command line looking for explict ini file specification */

    path = (char *) NULL;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strcasecmp(argv[i], "-h") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "-help") == 0) {
            PrintHelp(prog);
        } else if (strcasecmp(argv[i], "--help") == 0) {
            PrintHelp(prog);
        }
    }

/*  Load the global initialization */

    if (!isidlSetGlobalParameters(dbspec, prog, &par.glob)) {
        fprintf(stderr, "%s: isiSetGlobalParameters: %s\n", prog, strerror(errno));
        exit(1);
    }

/* Now go through the command line looking for explicit overrides */
    
    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            par.port = (UINT16) atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "nrts=", strlen("nrts=")) == 0) {
            par.nrts = (UINT16) atoi(argv[i] + strlen("nrts="));
        } else if (strncasecmp(argv[i], "user=", strlen("user=")) == 0) {
            if ((par.user = strdup(argv[i] + strlen("user="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                return NULL;
            }
        } else if (strncasecmp(argv[i], "status=", strlen("status=")) == 0) {
            par.status = (UINT16) atoi(argv[i] + strlen("status="));

#ifdef unix
        } else if (strcasecmp(argv[i], "-bd") == 0) {
            par.daemon = TRUE;
#endif
        
        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            par.buflen.send = par.buflen.recv = atoi(argv[i] + strlen("bs="));

        } else if (strncmp(argv[i], "ibs=", strlen("ibs=")) == 0) {
            par.buflen.recv = atoi(argv[i] + strlen("ibs="));

        } else if (strncmp(argv[i], "obs=", strlen("obs=")) == 0) {
            par.buflen.send = atoi(argv[i] + strlen("obs="));

        } else if (strncmp(argv[i], "maxclient=", strlen("maxclient=")) == 0) {
            par.maxclient = atoi(argv[i] + strlen("maxclient="));

        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            if ((par.log = strdup(argv[i] + strlen("log="))) == NULL) {
                fprintf(stderr, "%s: strdup failure!\n", prog);
                return NULL;
            }

        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            par.attr.at_dbgpath = argv[i] + strlen("dbgpath=");

        } else if (strcmp(argv[i], "-debug") == 0) {
            par.debug = TRUE;

        } else if (strcmp(argv[i], "-logtt") == 0) {
            par.logtt = TRUE;

        } else if (strcmp(argv[i], "-nolock") == 0) {
            par.glob.flags |= ISI_DL_FLAGS_IGNORE_LOCKS;

        } else if (strncasecmp(argv[i], "priority=", strlen("priority=")) == 0) {
            BOOL ok;
            par.priority = utilStringToProcessPriority(argv[i] + strlen("priority="), &ok);
            if (!ok) {
                fprintf(stderr, "%s: illegal priority '%s'\n", 
                    prog, argv[i] + strlen("priority=")
                );
                PrintHelp(prog);
            }

        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            ;
        } else if (strcasecmp(argv[i], "-h") == 0) {
            ;
        } else if (strcasecmp(argv[i], "-help") == 0) {
            ;
        } else if (strcasecmp(argv[i], "--help") == 0) {
            ;
        } else {
            fprintf(stderr, "\n%s: unrecognized argument '%s'\n\n", prog, argv[i]);
            PrintHelp(prog);
        }
    }

/* If logging not explicity specified, set default depending on status */

    if (par.log == NULL) par.log = par.daemon ? DEFAULT_BACKGROUND_LOG : DEFAULT_FOREGROUND_LOG;

/* All done, return pointer to parameters */

    return &par;
}

/* Log parameters */

VOID LogPar(PARAM *par)
{
    LogMsg(LOG_INFO, "Database    %s",  par->glob.db->dbid);
    LogMsg(LOG_INFO, "Home Dir    %s",  par->glob.root);
    LogMsg(LOG_INFO, "ISI Port    %hu", par->port);
    LogMsg(LOG_INFO, "NRTS Port   %hu", par->nrts);
    LogMsg(LOG_INFO, "StatusPort  %hu", par->status);
    LogMsg(LOG_INFO, "MaxClient   %lu", par->maxclient);
    LogMsg(LOG_INFO, "RecvBuflen  %lu", par->buflen.recv);
    LogMsg(LOG_INFO, "SendBuflen  %lu", par->buflen.send);
#ifdef WIN32
    LogMsg(LOG_INFO, "Priority    %s",  utilProcessPriorityToString(par->priority));
#endif /* WIN32 */
}

/* Revision History
 *
 * $Log: par.c,v $
 * Revision 1.17  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.16  2008/01/25 22:13:23  dechavez
 * removed NRTS channel filtering (now selected by client supplied list)
 *
 * Revision 1.15  2008/01/17 19:38:47  dechavez
 * added -nrts option to help message
 *
 * Revision 1.14  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.13  2007/05/17 22:22:31  dechavez
 * 3.1.0 (beta 1)
 *
 * Revision 1.12  2007/01/25 20:53:38  dechavez
 * added -nolock option
 *
 * Revision 1.11  2007/01/11 22:02:18  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.10  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.9  2006/06/02 21:38:01  dechavez
 * fixed error handling db=spec arguments
 *
 * Revision 1.8  2006/03/13 22:37:14  dechavez
 * switch from isi config file to db lookup for global parameters
 *
 * Revision 1.7  2005/09/10 21:54:49  dechavez
 * set default log spec according to background/foreground state
 *
 * Revision 1.6  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.5  2005/07/06 15:52:11  dechavez
 * added support for new "user" parameter
 *
 * Revision 1.4  2005/01/28 02:02:58  dechavez
 * added dbgpath support
 *
 * Revision 1.3  2004/06/30 20:49:00  dechavez
 * allow setting daemon field TRUE only for unix builds
 *
 * Revision 1.2  2004/06/10 20:42:03  dechavez
 * added support for home directory parameter
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
