#pragma ident "$Id: init.c,v 1.8 2008/09/11 16:41:29 dechavez Exp $"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "nrts_alpha.h"
#ifdef USE_LYNKS_LIBDSS
#include "libdss.h"
extern DSS_HANDLE *DSS_Handle;
#endif /* USE_LYNKS_LIBDSS */
#ifdef USE_LUNA_LIBDSS
#include "libdss.h"
extern DSS_HANDLE *DSS_Handle;
#endif /* USE_LUNA_LIBDSS */

void help(myname)
char *myname;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s if=sys of=buffer_file [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"chan=list   => list of channels to request\n");
    fprintf(stderr,"net=name    => network name\n");
    fprintf(stderr,"sname=name  => override station name with `name'\n");
    fprintf(stderr,"sc=val      => short channel name flag\n");
    fprintf(stderr,"nap=val     => seconds to sleep between attempts\n");
    fprintf(stderr,"verbose=val => verbose flag\n");
    fprintf(stderr,"warn=secs   => no data threshold\n");
    fprintf(stderr,"admin=email => email address of administrator\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"home=name   => set NRTS_HOME\n");
    fprintf(stderr,"+/-cr       => Canadian compression toggle\n");
    fprintf(stderr,"+/-slc      => lower case the station name toggle\n");
    fprintf(stderr,"+/-suc      => upper case the station name toggle\n");
    fprintf(stderr,"+/-clc      => lower case the channel name toggle\n");
    fprintf(stderr,"+/-cuc      => upper case the channel name toggle\n");
#ifdef USE_LYNKS_LIBDSS
    fprintf(stderr,"lynks=val   => PCMCIA socket with Lynks authenticator card\n");
    fprintf(stderr,"pin=string  => Lynks authenticator PIN\n");
#endif /* USE_LYNKS_LIBDSS */
#ifdef USE_LUNA_LIBDSS
    fprintf(stderr,"luna=val    => PCMCIA socket with Chrysalis Luna token\n");
    fprintf(stderr,"pin=string  => Luna token user PIN\n");
    fprintf(stderr,"key=string  => ident string for Luna DSS signature key\n");
#endif /* USE_LUNA_LIBDSS */
    fprintf(stderr,"\n");
    exit(1);
}

AlphaConn *init(
    int argc, char **argv,
    int *nchan, int *index, struct static_info *static_info, int *nap,
    double *youngest, struct notify *notify
){
int cr          = 0;
char *home      = NULL;
char *log       = NULL;
int debug       = NRTS_DEFLOG;
char *net       = ALPHA_NET;
char *sname     = NULL;
int verbose     = 0;
char *input     = NULL;
char *output    = NULL;
char *chan_list = NULL;
int short_chans = 0;
char *tmp;
int i, j, rev, status, arg2, dur, ntoken;
int background  = 1;
struct {
    int sta;
    int chn;
} recase = { RECASE_NONE, RECASE_NONE };
char *token[NRTS_MAXCHN];
struct nrts_packet *packet;
struct nrts_sys    *sys;
struct nrts_files  *file;
struct nrts_asl    *asl;
static struct nrts_mmap map;
AlphaConn *conn;
char buffer[1024];
DBIO *db;
IDA *ida;
char *dbspec = (char *) NULL;
char *mapname;

#ifdef USE_LYNKS_LIBDSS
DSS_PAR dss_par;
int use_dss, pin_set, dss_error;
#endif /* USE_LYNKS_LIBDSS */

#ifdef USE_LUNA_LIBDSS
DSS_PAR dss_par;
int use_dss, pin_set, dss_error;
#endif /* USE_LUNA_LIBDSS */

static char *fid = "init";

/*  Get command line arguments  */

    notify->address = DEF_ADDRESS;
    notify->time    = DEF_WARN;

#ifdef USE_LYNKS_LIBDSS
    use_dss        = 0;
    dss_par.socket = -1;
    pin_set        = 0;
#endif /* USE_LYNKS_LIBDSS */

#ifdef USE_LUNA_LIBDSS
    use_dss        = 0;
    dss_par.slotID = -1;
    dss_par.label  = NULL_PTR;
    dss_par.pin    = NULL_PTR;
    pin_set        = 0;
#endif /* USE_LUNA_LIBDSS */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "chan=", strlen("chan=")) == 0) {
            chan_list = strdup(argv[i] + strlen("chan="));
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            input = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "of=", strlen("of=")) == 0) {
            output = argv[i] + strlen("of=");
        } else if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "net=", strlen("net=")) == 0) {
            net = argv[i] + strlen("net=");
        } else if (strncmp(argv[i], "sname=", strlen("sname=")) == 0) {
            sname = argv[i] + strlen("sname=");
        } else if (strncmp(argv[i], "bg=", strlen("bg=")) == 0) {
            background = atoi(argv[i] + strlen("bg="));
        } else if (strncmp(argv[i], "sc=", strlen("sc=")) == 0) {
            short_chans = atoi(argv[i] + strlen("sc="));
        } else if (strncmp(argv[i], "verbose=", strlen("verbose=")) == 0) {
            verbose = atoi(argv[i] + strlen("verbose="));
        } else if (strncmp(argv[i], "nap=", strlen("nap=")) == 0) {
            *nap = atoi(argv[i] + strlen("nap="));
        } else if (strncmp(argv[i], "warn=", strlen("warn=")) == 0) {
            notify->time = (time_t) atoi(argv[i] + strlen("warn="));
        } else if (strncmp(argv[i], "admin=", strlen("admin=")) == 0) {
            notify->address = argv[i] + strlen("admin=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strcmp(argv[i], "+cr") == 0) {
            cr = 1;
        } else if (strcmp(argv[i], "-cr") == 0) {
            cr = 0;

        } else if (strcmp(argv[i], "+slc") == 0) {
            recase.sta = RECASE_LOWER;
        } else if (strcmp(argv[i], "-slc") == 0) {
            if (recase.sta == RECASE_LOWER) recase.sta = 0;
        } else if (strcmp(argv[i], "+suc") == 0) {
            recase.sta = RECASE_UPPER;
        } else if (strcmp(argv[i], "-suc") == 0) {
            if (recase.sta == RECASE_UPPER) recase.sta = 0;

        } else if (strcmp(argv[i], "+clc") == 0) {
            recase.chn = RECASE_LOWER;
        } else if (strcmp(argv[i], "-clc") == 0) {
            if (recase.chn == RECASE_LOWER) recase.chn = 0;
        } else if (strcmp(argv[i], "+cuc") == 0) {
            recase.chn = RECASE_UPPER;
        } else if (strcmp(argv[i], "-cuc") == 0) {
            if (recase.chn == RECASE_UPPER) recase.chn = 0;

#ifdef USE_LYNKS_LIBDSS
        } else if (strncmp(argv[i], "lynks=", strlen("lynks=")) == 0) {
            dss_par.socket = atoi(argv[i] + strlen("lynks="));
            use_dss = 1;
        } else if (strncmp(argv[i], "pin=", strlen("pin=")) == 0) {
            memcpy(dss_par.pin, argv[i] + strlen("pin="), CI_PIN_SIZE);
            pin_set = use_dss = 1;
#endif /* USE_LYNKS_LIBDSS */

#ifdef USE_LUNA_LIBDSS
        } else if (strncmp(argv[i], "luna=", strlen("luna=")) == 0) {
            dss_par.slotID = (CK_SLOT_ID) atoi(argv[i] + strlen("luna="));
            use_dss = 1;
        } else if (strncmp(argv[i], "pin=", strlen("pin=")) == 0) {
            dss_par.pin = (CK_CHAR_PTR) strdup(argv[i] + strlen("pin="));
            if (dss_par.pin == NULL) {
                perror("malloc");
                exit(1);
            }
            pin_set = use_dss = 1;
        } else if (strncmp(argv[i], "key=", strlen("key=")) == 0) {
            dss_par.label = (CK_CHAR_PTR) strdup(argv[i] + strlen("key="));
            if (dss_par.label == NULL) {
                perror("malloc");
                exit(1);
            }
            use_dss = 1;
#endif /* USE_LUNA_LIBDSS */

        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n",
                argv[0], argv[i]
            );
            help(argv[0]);
        }
    }

/* Must specify input system and output buffer file */

    if (input == NULL || output == NULL) help(argv[0]);

    if (chan_list == NULL && (chan_list = strdup(ALPHA_DEFCHAN)) == NULL) {
        perror("strdup");
        return NULL;
    }

/* If Lynks authentication is desired, must give both socket and pin */

#ifdef USE_LYNKS_LIBDSS
    if (use_dss && (dss_par.socket == -1 || !pin_set)) {
        fprintf(stderr, 
            "%s: must specify both lynks socket and pin for DSS\n",
            argv[0]
        );
        help(argv[0]);
    }
#endif /* USE_LYNKS_LIBDSS */

/* If Luna authentication is desired, must give at least socket */

#ifdef USE_LUNA_LIBDSS
    if (use_dss && dss_par.slotID == -1) {
        fprintf(stderr, 
            "%s: must specify Luna socket for DSS\n",
            argv[0]
        );
        help(argv[0]);
    }
#endif /* USE_LUNA_LIBDSS */

/*  Start logging */

    if (log != NULL) {
        util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
    }
#if defined USE_LUNA_LIBDSS
    util_log(1, "%s (with Luna2 authentication)", VersionIdentString);
#elif defined USE_LYNKS_LIBDSS
    util_log(1, "%s (with Lynks authentication)", VersionIdentString);
#else
    util_log(1, "%s (no authentication)", VersionIdentString);
#endif

/*  mmap the system  */

    file = nrts_files(&home, input);
    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) != 0) {
        util_log(1, "abort: can't attach to system `%d'", input);
        return NULL;
    }
    sys = (struct nrts_sys *) map.ptr;

/*  Load the database */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        util_log(1, "%s: can't load database `%s': %s", fid, dbspec, syserrmsg(errno));
        return NULL;
    }

    util_log(1, "`%s' database loaded successfully", dbspec);

/*  Make sure that our assumptions are OK  */

    if (sys->nsta != 1) {
        util_log(1, "abort: program design assumes one station per system");
        return NULL;
    }

    if (sys->sta[0].nchn > NRTS_MAXCHN) {
        util_log(1, "abort: too many channels... increase NRTS_MAXCHN");
        return NULL;
    }

/* Create the IDA handle */

    switch (sys->type) {
      case NRTS_IDA:
        rev = ((struct nrts_ida *) sys->info)->rev;
        mapname = ((struct nrts_ida *) sys->info)->map;
        break;
      case NRTS_IDA10:
        rev = 10;
        mapname = NULL;
        break;
      default:
        util_log(1, "%s: unsupported system `%d'", fid, sys->type);
        return NULL;
    }
    if ((ida = idaCreateHandle(sys->sta[0].name, rev, mapname, db, NULL, 0)) == NULL) {
        util_log(1, "idaCreateHandle failed");
        return NULL;
    }

/*  Get list of channels to service  */

    if (chan_list == NULL) {
        *nchan = sys->sta[0].nchn;
        for (i = 0; i < *nchan; i++) index[i] = i;
    } else {
        if ((tmp = strdup(chan_list)) == NULL) {
            perror("strdup");
            return NULL;
        }
        util_lcase(tmp);
        ntoken = util_sparse(tmp, token, ",:/", NRTS_MAXCHN);
        if (ntoken < 1) {
            util_log(1, "abort: bad chan_list `%s'", chan_list);
            return NULL;
        }
        *nchan = 0;
        for (i = 0; i < ntoken; i++) {
            for (j = 0; j < sys->sta[0].nchn; j++) {
                if (strcasecmp(token[i], sys->sta[0].chn[j].name) == 0){
                    index[*nchan] = j;
                    ++*nchan;
                }
            }
        }
        if (*nchan == 0) {
            util_log(1, "abort: no valid channels found in `%s'", chan_list);
            return NULL;
        }
    }

    for (i = 0; i < *nchan; i++) {
        for (j = i+1; j < *nchan; j++) {
            if (index[i] == index[j]) {
                util_log(1, "abort: duplicate channel names specified");
                return NULL;
            }
        }
    }

    if (*nchan != ntoken) {
        util_log(1, "abort: unsupported channels requested");
        return NULL;
    }

/*  Initialize the disk loop reader  */

    if (read_init(home, input, sys, file, short_chans, ida, sname, recase.chn) != 0) {
        util_log(1, "abort: can't initilize disk loop reader");
        return NULL;
    }

/*  Make sure that static information is constant across requested channels  */

    packet = index_read(index[0], -1);
    
    strcpy(static_info->site_name, packet->sname);
    if (recase.sta == RECASE_UPPER) {
        util_log(1, "site name will be force to all upper case");
        util_ucase(static_info->site_name);
    } else if (recase.sta == RECASE_LOWER) {
        util_log(1, "site name will be force to all lower case");
        util_lcase(static_info->site_name);
    }
    static_info->sint      = packet->sint;
    static_info->samprate  = 1.0 / static_info->sint;
    static_info->nsamp     = packet->nsamp;
    static_info->wrdsiz    = packet->wrdsiz;
    static_info->len       = packet->nsamp * packet->wrdsiz;
    static_info->increment = (float) static_info->nsamp * static_info->sint;
    util_log(1, "static increment set to %.3f seconds", static_info->increment);

    if (packet->order == BIG_ENDIAN_ORDER) {
        sprintf(static_info->itype, "s%d", packet->wrdsiz);
    } else if (packet->order == LTL_ENDIAN_ORDER) {
        sprintf(static_info->itype, "i%d", packet->wrdsiz);
    } else {
        util_log(1, "abort: unrecognized byte order 0x%08x", packet->order);
        return NULL;
    }

    if (cr) {
        util_log(1, "data will be sent using Canadian compression");
        strcpy(static_info->otype, "cr");
    } else {
        util_log(1, "data will be sent uncompressed");
        strcpy(static_info->otype, static_info->itype);
    }

    util_log(1, "input  data type is `%s'", static_info->itype);
    util_log(1, "output data type is `%s'", static_info->otype);
    *youngest = packet->beg.time;

    for (i = 1; i < *nchan; i++) {
        packet = index_read(index[i], -1);
        if (changed(packet, static_info)) {
            util_log(1, "abort: required constants vary across channels");
            return NULL;
        }
        if (packet->beg.time < *youngest) *youngest = packet->beg.time;
    }
    
/* Open connection with AlphaSend server */

    dur  = (int) (static_info->sint * 1000.0) * static_info->nsamp;
    arg2 = TWENTY_MIN / dur;

    conn = AlphaConnect(ALPHA_BUFFER, output, arg2, net, dur, verbose);
    if (conn == NULL) {
        perror("AlphaConnect");
        exit(1);
    }

    util_log(1, "connected to LIFO buffer `%s'", output);

/* Signal/exit handlers  */

    util_log(2, "setting up signal/exit handlers");
    if (exitcode() != 0) {
        util_log(1, "abort: exitcode() failed: %s", syserrmsg(errno));
        return NULL;
    }

/* Notification set up  */

    sprintf(buffer, SUBJECT, input);
    if ((notify->subject = strdup(buffer)) == NULL) {
        util_log(1, "abort: strdup: %s", syserrmsg(errno));
        return NULL;
    }

    sprintf(buffer, "%s/etc/%s", home, MESSAGE);
    if ((notify->fname = strdup(buffer)) == NULL) {
        util_log(1, "abort: strdup: %s", syserrmsg(errno));
        return NULL;
    }

    util_log(1, "email notification to be sent to %s", notify->address);
    util_log(1, "after %d seconds of no data", notify->time);
    util_log(2, "canned message file is %s", notify->fname);

/* Go into the background */

    util_logclose();
    if (background && util_bground(0, 0) < 0) {
        util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
        util_log(1, "util_bground: %s", syserrmsg(errno));
        return NULL;
    }
    util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);

#if defined USE_LUNA_LIBDSS
    util_log(1, "%s (with Luna2 authentication)", VersionIdentString);
#elif defined USE_LYNKS_LIBDSS
    util_log(1, "%s (with Lynks authentication)", VersionIdentString);
#else
    util_log(1, "%s (no authentication)", VersionIdentString);
#endif

/* Initialize the DSS library (must be _AFTER_ fork()) */

#ifdef USE_LIBDSS
    if (use_dss) {
        DSS_Handle = dssInit(&dss_par, &dss_error);
        if (DSS_Handle == (DSS_HANDLE *) NULL) {
            util_log(1, "FATAL error: dssInit error %d", dss_error);
            return NULL;
        }
        util_log(1, "authenticating frames using libdss");
    } else {
        util_log(1, "frames will be UNAUTHENTICATED");
    }
#endif /* USE_LIBDSS */

    util_log(1, "initialization complete");

    return conn;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.8  2008/09/11 16:41:29  dechavez
 * improved debug messages
 *
 * Revision 1.7  2006/02/10 02:04:50  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.6  2004/04/26 21:03:25  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.5  2003/03/13 23:19:20  dechavez
 * 2.11.0 (5 char mapped to 3 char)
 *
 * Revision 1.4  2001/04/23 15:27:20  dec
 * cosmetic
 *
 * Revision 1.3  2001/01/11 17:39:52  dec
 * Add sname= option to allow the user to force the output station name.
 *
 * Revision 1.2  2000/03/09 19:58:53  dec
 * Added support for Luna2, Makefile left for unauthenticated builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
