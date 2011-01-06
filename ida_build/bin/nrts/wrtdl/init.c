#pragma ident "$Id: init.c,v 1.15 2006/02/10 02:07:35 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include "platform.h"
#include "wrtdl.h"

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options] syscode\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"if=name     => take input from named file/server/device\n");
    fprintf(stderr,"db=spec     => override DBIO_DATABASE\n");
    fprintf(stderr,"to=secs     => set read timeout\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"tap=name    => tap tty input to here\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"fwd=secs    => flush wfdisc interval\n");
    fprintf(stderr,"home=name   => set NRTS_HOME\n");
    fprintf(stderr,"+/-strict   => toggle strict time checks (tty input)\n");
    fprintf(stderr,"+/-leapfix  => toggle Magellan leap year fix\n");
    fprintf(stderr,"+/-year     => toggle automatic IDA rev=3 year stamp\n");
    fprintf(stderr,"+/-ctsrts   => toggle hardware tty flow control\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"These additional options are available when requesting ");
    fprintf(stderr,"data from a remote NRTS:\n");
    fprintf(stderr,"port=val    => TCP/IP port number\n");
    fprintf(stderr,"snd=val     => socket transmit buffer length\n");
    fprintf(stderr,"rcv=val     => socket receive  buffer length\n");
    fprintf(stderr,"tcpbuf=val  => set both snd and rcv buffers\n");
    fprintf(stderr,"chan=list   => list of channels to request\n");
    fprintf(stderr,"beg=string  => beg time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"end=string  => end time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"tto=sec     => connect() 'tiny-time-out'\n");
    fprintf(stderr,"+/-retry    => toggle timeout retry behaviour\n");
    fprintf(stderr,"+/-keepup   => toggel continuous/finite connection\n");
    fprintf(stderr,"\n");
    exit(1);
}

int init(int argc, char **argv, struct param *param)
{
unsigned long tictol = 0;
void *opt;
int i, status, flags, rev;
char *home     = NULL;
char *iname    = NULL;
#ifdef WIN32
char *log      = NULL;
#else
char *log      = "syslogd";
#endif
char *syscode  = NULL;
char *SYSCODE  = NULL;
int debug      = NRTS_DEFLOG;
int background = 1;
int stamp_year = 1;
int ctsrts     = 0;
int leapfix    = 0;
int strict     = 1;
int addseqno   = 1;
int forcettst  = 0;
float minseg   = 0.0;
time_t now;
struct tm *gmt;
struct nrts_files  *file;
char *dbspec = (char *) NULL;
DBIO *db = NULL;
char *mapname = NULL;
LOGIO *lp;
static struct nrts_sys *sys;
static struct nrts_ida *info;
static struct nrts_mmap mmap, *map;
static char *fid = "init";

    XferLibInit();

    param->sys     = NULL;
    param->beg     = NRTS_OLDEST;
    param->end     = NRTS_YNGEST;
    param->keeparg = NULL;
    param->timeout = -1;
    param->tto     = 0;
    param->retry   = 1;
    param->port    = -1;
    param->tapfile = (char *) NULL;
    param->ida     = (IDA *) NULL;
    param->fwd     = 60;
    param->dbgpath = NULL;
    param->IsiStatPath = NULL;

    param->blksiz  = 512; /* ASL network block size */
    param->tolerance = 1; /* largest tolerated time tear in samples */

    param->req.protocol = 0x01;
    param->req.type     = XFER_WAVREQ;
    param->req.timeout  = NRTS_DEFTCPTO;
    param->req.sndbuf   = 0;
    param->req.rcvbuf   = 0;
    param->req.preamble.ver01.client_id = getpid();
    param->req.preamble.ver01.format    = XFER_CNFNRTS;
    param->wavreq = &param->req.request.wav.ver01;
    param->wavreq->keepup = 0;
    param->wavreq->comp   = XFER_CMPIGPP;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "fwd=", strlen("fwd=")) == 0) {
            param->fwd = atoi(argv[i]+strlen("fwd="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "dbgpath=", strlen("dbgpath=")) == 0) {
            param->dbgpath = argv[i] + strlen("dbgpath=");
        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            param->timeout = atoi(argv[i] + strlen("to="));
            param->req.timeout = param->timeout;
        } else if (strncmp(argv[i], "tto=", strlen("tto=")) == 0) {
            param->tto = atoi(argv[i] + strlen("tto="));
        } else if (strncmp(argv[i], "minseg=", strlen("minseg=")) == 0) {
            minseg = atof(argv[i] + strlen("minseg="));
        } else if (strncmp(argv[i], "bg=", strlen("bg=")) == 0) {
            background = atoi(argv[i] + strlen("bg="));
        } else if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
            param->port = atoi(argv[i] + strlen("port="));
        } else if (strncmp(argv[i], "tcpbuf=", strlen("tcpbuf=")) == 0){
            param->req.sndbuf = atoi(argv[i] + strlen("tcpbuf="));
            param->req.rcvbuf = param->req.sndbuf;
        } else if (strncmp(argv[i], "snd=", strlen("snd=")) == 0) {
            param->req.sndbuf = atoi(argv[i] + strlen("snd="));
        } else if (strncmp(argv[i], "rcv=", strlen("rcv=")) == 0) {
            param->req.rcvbuf = atoi(argv[i] + strlen("rcv="));
        } else if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            param->blksiz = atoi(argv[i] + strlen("bs="));
        } else if (strncmp(argv[i], "comp=", strlen("comp=")) == 0) {
            param->wavreq->comp  = atoi(argv[i] + strlen("comp="));
        } else if (strncmp(argv[i], "tolerance=", strlen("tolerance=")) == 0) {
            param->tolerance = atoi(argv[i] + strlen("tolerance="));
        } else if (strcmp(argv[i], "+retry") == 0) {
            param->retry = 1;
        } else if (strcmp(argv[i], "-retry") == 0) {
            param->retry = 0;
        } else if (strcmp(argv[i], "++retry") == 0) {
            param->retry = -1;
        } else if (strcmp(argv[i], "+keepup") == 0) {
            param->wavreq->keepup = 1;
        } else if (strcmp(argv[i], "-keepup") == 0) {
            param->wavreq->keepup = 0;
        } else if (strncmp(argv[i], "tap=", strlen("tap=")) == 0) {
            param->tapfile = strdup(argv[i] + strlen("tap="));
        } else if (strncmp(argv[i], "chan=", strlen("chan=")) == 0) {
            param->keeparg = strdup(argv[i] + strlen("chan="));
        } else if (strcmp(argv[i], "+ctsrts") == 0) {
            ctsrts = 1;
        } else if (strcmp(argv[i], "-ctsrts") == 0) {
            ctsrts = 0;
        } else if (strcmp(argv[i], "+strict") == 0) {
            strict = 1;
        } else if (strcmp(argv[i], "-strict") == 0) {
            strict = 0;
        } else if (strcmp(argv[i], "+forcettst") == 0) {
            forcettst = 1;
        } else if (strcmp(argv[i], "-forcettst") == 0) {
            forcettst = 0;
        } else if (strcmp(argv[i], "+seqno") == 0) {
            addseqno = 1;
        } else if (strcmp(argv[i], "-seqno") == 0) {
            addseqno = 0;
        } else if (strcmp(argv[i], "+leapfix") == 0) {
            leapfix = 1;
        } else if (strcmp(argv[i], "-leapfix") == 0) {
            leapfix = 0;
        } else if (strcmp(argv[i], "+year") == 0) {
            stamp_year = 1;
        } else if (strcmp(argv[i], "-year") == 0) {
            stamp_year = 0;
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            iname = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            param->beg = util_attodt(argv[i] + strlen("beg="));
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            param->end = util_attodt(argv[i] + strlen("end="));
            param->wavreq->keepup = 0;
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "tictol=", strlen("tictol=")) == 0) {
            tictol = (unsigned long) atol(argv[i] + strlen("tictol="));
        } else if (strncmp(argv[i], "isilog=", strlen("isilog=")) == 0) {
            param->IsiStatPath = strdup(argv[i] + strlen("isilog="));
        } else {
            if (syscode != NULL) {
                help(argv[0]);
            } else {
                syscode = argv[i];
            }
        }
    }

    flags = NRTS_TODISK;
    if (strict)  flags  |= NRTS_STRICT;
    if (addseqno) flags |= NRTS_ADDSEQNO;
    if (forcettst) flags |= NRTS_FORCETTST;

    if (syscode == NULL) {
        fprintf(stderr,"%s: missing system code\n", argv[0]);
        help(argv[0]);
    }

/*  Get file names  */

    file = nrts_files(&home, syscode);

/*  Switch from stderr to log file  */

    if (log == NULL) log = file->log;
    SYSCODE = (strcmp(log, "syslogd")==0) ? util_ucase(strdup(syscode)) : NULL;
    util_logopen(log, 1, NRTS_MAXLOG, debug, SYSCODE, argv[0]);
    util_log(1, VersionIdentString);
    lp = InitLogging(argv[0], log, debug == NRTS_DEFLOG ? FALSE : TRUE);

/*  Get system map  */

    map = &mmap;
    if (nrts_mmap(file->sys, "r+", NRTS_SYSTEM, map) != 0) {
        util_log(1, "%s: nrts_mmap failed", fid);
        return -1;
    }
    param->sys = sys = (struct nrts_sys *) map->ptr;

/*  Load the database */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        if (dbspec == NULL) {
            fprintf(stderr, "%s: undefined database.  Set DBIO_DATABASE or use db=spec\n", argv[0]);
        } else {
            fprintf(stderr, "%s: can't load database `%s': %s\n", argv[0], dbspec, syserrmsg(errno));
        }
        return -1;
    }
    util_log(1, "%s database loaded OK\n", db->dbid);
    
/* Lock system file and check for consistency */

    util_log(2, "locking system");

/*    This is not applicable for WIN32                    */
#ifndef WIN32

    if (sys->pid != 0 && kill(sys->pid, SIGHUP) == 0) {
        util_log(1, "system is already active (pid %d)", sys->pid);
        return -1;
    }
#endif
    if (nrts_wlock(map) == -1) {
        if (errno == EACCES) {
            util_log(1,"exit: system is locked (pid=%d)\n", argv[0], sys->pid);
            exit(1);
        } else {
            util_log(1,"%s: nrts_wlock: %s", fid, syserrmsg(errno));
            return -1;
        }
    }

    util_log(2, "checking disk loop integrity");

    if ((status = nrts_chksys(sys)) != 0) {
        util_log(1, "correcting disk loop (status %d)", status);
        if ((status = nrts_fixsys(sys)) != 0) {
            util_log(1,"FATAL: unable to repair disk loop (status %d)", status);
            return -1;
        }
    }
    util_log(2, "disk loop is intact");

/* Store our process id  */

    sys->pid = getpid();

/*  Load system specific information  */

    if (sys->type == NRTS_IDA) {
        info = (struct nrts_ida *) sys->info;
        param->wavreq->format = XFER_WAVIDA;
        rev = info->rev;
        mapname = info->map;
    } else if (sys->type == NRTS_IDA10) {
        param->wavreq->format = XFER_WAVIDA;
        rev = 10;
        mapname = NULL;
    } else {
        util_log(1, "%s: unsupported system type = %d", fid, sys->type);
        return -1;
    }

    if ((param->ida = idaCreateHandle(sys->sta[0].name, rev, mapname, db, NULL, flags)) == NULL) {
        util_log(1, "idaCreateHandle failed");
        return -1;
    }

/* Insure the binary wfdiscs are current */

    if ((status = nrts_fixwd(file, map, home, syscode, param->ida)) != 0) {
        util_log(1, "nrts_fixwd: status %d", status);
        return -1;
    }

/*  Allocate read buffers  */

    util_log(2, "allocating read buffers");
    param->len = sys->reclen;
    if ((param->prev = (char *) calloc(param->len, sizeof(char))) == NULL) {
        util_log(1, "malloc: %s", syserrmsg(errno));
        return -1;
    }
    if ((param->crnt = (char *) calloc(param->len, sizeof(char))) == NULL) {
        util_log(1, "malloc: %s", syserrmsg(errno));
        return -1;
    }

/*  Initialize data input routine  */

    util_log(2, "initializing data input routines");
    if (init_getrec(home,syscode,iname,param,ctsrts,leapfix,&flags) != 0) {
        util_log(1, "%s: init_getrec failed", fid);
        return -1;
    }

/*  Initialize the disk writer  */

    util_log(2, "initializing disk i/o routines");
    opt = (void *) &tictol;
    if (nrts_iniwrtdl(home, syscode, sys, flags, param->ida, opt) != 0) {
        util_log(1, "%s: nrts_iniwrtdl failed", fid);
        return -1;
    }

/*  For IDA rev 3 (aka MK4) data, determine initial year value  */
/*  This assumes that the system clock is more or less correct  */

    if (sys->type == NRTS_IDA && param->ida->rev.value == 3) {
        if (stamp_year) {
            now = time(NULL);
            gmt = gmtime(&now);
            info->mk4_year = gmt->tm_year + 1900;
            util_log(1, "IDA MK4 system... will use %hd for year", info->mk4_year);
        } else {
            info->mk4_year = -1;
            util_log(1, "IDA MK4 system... will believe datalogger year stamp");
        }
    }

/*  Finally (sigh) announce if we are using Fels' shifts or not  */

    if (sys->type == NRTS_IDA && info->shift)  util_log(1, "fels' shifts will be applied (sigh)");

/*  Go into the background and update process id  */

    util_log(1, "initialization complete");

    util_logclose();

#ifndef WIN32
    if (background && util_bground(0, 0) < 0) {
        util_logopen(log, 1, NRTS_MAXLOG, debug, SYSCODE, argv[0]);
        util_log(1, "util_bground: %s", syserrmsg(errno));
        return -1;
    }
#endif
    util_logopen(log, 1, NRTS_MAXLOG, debug, SYSCODE, argv[0]);
    util_log(2, "updating pid");
    param->req.preamble.ver01.client_id = sys->pid = getpid();

/* start signal handler and wfdisc flush threads */

    SetSys(sys);
    StartSignalHandler();
    StartFlushThread(param);

/*  begin data acquistion  */

    util_log(1, "starting data acquisition");
    sys->start = time(NULL);

    return 0;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.15  2006/02/10 02:07:35  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.14  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.13  2005/05/06 22:17:41  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.12  2005/05/06 01:09:29  dechavez
 * added optional logging of ISI:IACP recv stats
 *
 * Revision 1.11  2005/01/28 02:09:45  dechavez
 * Added dbgpath debug support for ISI feeds
 *
 * Revision 1.10  2004/04/26 21:01:56  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.9  2004/02/05 21:08:57  dechavez
 * added support for tee files
 *
 * Revision 1.8  2004/01/29 18:52:36  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.7  2003/12/23 21:14:32  dechavez
 * XferLibInit() since now we are using the re-entrant version of libxfer
 *
 * Revision 1.6  2003/12/22 22:13:57  dechavez
 * added fwd command line option, removed exit_handler and added calls to
 * start signal handler and flush wfdisc threads
 *
 * Revision 1.5  2003/11/25 20:41:14  dechavez
 * ANSI function declarations, made local use only functions static
 *
 * Revision 1.4  2002/04/25 20:21:20  dec
 * print extern version ident string instead of macro
 *
 * Revision 1.3  2000/02/18 06:39:25  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 00:17:19  dec
 * added forcetst command line option, print VERSION instead of SCCS version
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
