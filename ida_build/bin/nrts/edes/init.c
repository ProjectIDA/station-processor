#pragma ident "$Id: init.c,v 1.13 2008/01/15 19:45:18 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "logio.h"
#include "nrts.h"
#include "util.h"
#include "ida.h"
#include "xfer.h"
#include "edes.h"
#include "isi/dl.h"
#include "isi/db.h"

#define SYS  (edes->sys + i)
#define SSTA  (SYS->sta + j)
#define SCHN (SSTA->chn + k)

#define CNF  (edes->cnf + i)
#define CSTA  (CNF->sta + j)
#define CCHN (CSTA->chn + k)

#undef  INCLUDE_SILENT_LIST
#ifdef INCLUDE_SILENT_LIST
static BOOL NotOnSilentList(char *peer)
{
int i;
static char *list[] = {
    "EWB.PTWC.NOAA.GOV",
    "vdl1.cr.usgs.gov",
    NULL
};

    for (i = 0; list[i] != NULL; i++) {
        if (strcasecmp(peer, list[i]) == 0) return FALSE;
    }
    return TRUE;
}
#else
#   define NotOnSilentList(peer) TRUE
#endif

#define DEFAULT_DBSPEC "/usr/nrts"

int init(int argc, char **argv, char **home, struct edes_params *edes, char *buffer)
{
size_t edes_len;
FILE *cfp;
int i, j, k, nsys, sint, datlen, rev;
char *log  = NULL, *dbspec = DEFAULT_DBSPEC, *mapname;
int  debug = NRTS_DEFLOG;
int  to    = NRTS_DEFTCPTO;
char *system[MAXSYS];
char fname[MAXPATHLEN+1];
struct nrts_files *file;
struct xfer_packet *packet;
struct rlimit rlimit;
DBIO *db;
int nice, nerror;
ISI_STREAM_NAME name;
ISI_COORDS coords;
ISI_INST inst;
BOOL dump, incomplete;
static char PeerString[MAXPATHLEN], *peer;
static char *fid = "init";

    *home = NULL;
    nice  = 1;
    dump  = FALSE;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            *home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
        } else if (strcasecmp(argv[i], "+dbd") == 0) {
            dump = TRUE;
        } else if (strcasecmp(argv[i], "-dbd") == 0) {
            dump = FALSE;
        } else if (strcmp(argv[i], "config") == 0) {
            fprintf(stderr, "nrts_edes config is no longer required\n");
            exit(0);
        } else if (strcmp(argv[i], "+nice") == 0) {
            nice = 1;
        } else if (strcmp(argv[i], "-nice") == 0) {
            nice = 0;
        } else {
            xfer_errno = XFER_EFAULT;
            return -1;
        }
    }

/* Determine working home  */

    if (*home == NULL) {
        *home = getenv(NRTS_HOME);
    }

    if (*home == NULL) {
        *home = NRTS_DEFHOME;
    }

/*  Start logging facility */

    peer = util_peer(fileno(stdin), PeerString, MAXPATHLEN);
    if (peer != NULL) {
        if (NotOnSilentList(peer)) {
            util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
            util_log(2, VersionIdentString);
            util_log(2, "Connection from %s", PeerString);
        }
    } else {
        util_logopen(log, 1, NRTS_MAXLOG, debug, NULL, argv[0]);
        util_log(2, VersionIdentString);
        util_log(2, "Connection from <can't determine peer string>\n");
    }

    if (chdir(*home) != 0) {
        util_log(1, "%s: can't chdir %s: %s", fid, *home, syserrmsg(errno));
        xfer_errno = XFER_EFAULT;
        return -2;
    }

/* Connect to database */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        util_log(1, "dbioOpen failed");
        return -3;
    }

/* Set niceness */

    set_wait_flag(nice);

/*  Initialize Xfer routines */

    if (Xfer_ServInit(*home, to) != XFER_OK) {
        util_log(1, "%s: Xfer_ServInit: %s", fid, Xfer_ErrStr());
        xfer_errno = XFER_EFAULT;
        return -4;
    }

/* Check implementation limits */

    if (XFER_SNAMLEN < NRTS_SNAMLEN) {
        util_log(1, "%s: increase XFER_SNAMLEN", fid);
        xfer_errno = XFER_ELIMIT;
        return -5;
    }

    if (XFER_CNAMLEN < NRTS_CNAMLEN) {
        util_log(1, "%s: increase XFER_CNAMLEN", fid);
        xfer_errno = XFER_ELIMIT;
        return -6;
    }

    if (XFER_INAMLEN < NRTS_INAMLEN) {
        util_log(1, "%s: increase XFER_INAMLEN", fid);
        xfer_errno = XFER_ELIMIT;
        return -7;
    }

    if (XFER_MAXSTA < NRTS_MAXSTA) {
        util_log(1, "%s: increase XFER_MAXSTA", fid);
        xfer_errno = XFER_ELIMIT;
        return -8;
    }

    if (XFER_MAXCHN < NRTS_MAXCHN) {
        util_log(1, "%s: increase XFER_MAXCHN", fid);
        xfer_errno = XFER_ELIMIT;
        return -9;
    }

/*  Get info about all supported systems  */

    if ((nsys = nrts_systems(*home, system, MAXSYS)) < 0) {
        util_log(1, "%s: nrts_systems: status %d", fid, nsys);
        xfer_errno = XFER_EFAULT;
        return -10;
    }

    edes->tstamp = time(NULL);
    edes->nsys = nsys;
    edes->isd  = fileno(stdin);
    edes->osd  = fileno(stdout);
    utilSetNonBlockingSocket(edes->isd);
    utilSetNonBlockingSocket(edes->osd);

/* Set the resource limits for number of open files to the max */

    if (getrlimit(RLIMIT_NOFILE, &rlimit) != 0) {
        util_log(1, "can't get resource limits: %s", syserrmsg(errno));
        xfer_errno = XFER_EFAULT;
        return -11;
    }
    rlimit.rlim_cur = rlimit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rlimit) != 0) {
        util_log(1, "can't set resource limits: %s", syserrmsg(errno));
        xfer_errno = XFER_EFAULT;
        return -12;
    }

/*  Get NRTS system information for everything on this host */

    for (nerror = 0, i = 0; i < edes->nsys; i++) {

    /* Determine dl file names for this system */

        file = nrts_files(home, system[i]);

    /* Get mmapd NRTS system descriptor */

        if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, edes->map + i) != 0) {
            util_log(1, "%s: nrts_mmap: %s: %s",
                fid, system[i], syserrmsg(errno)
            );
            xfer_errno = XFER_EFAULT;
            return -13;
        }

        *(SYS) = *((struct nrts_sys *) edes->map[i].ptr);
        if (SYS->nsta != 1) {
            util_log(1, "%s: %s has %d stations, not allowed", fid, system[i], SYS->nsta);
            return -14;
        }

    /* Open dl header file */

        if ((edes->inf[i].hfd = open(file->hdr, O_RDONLY)) < 0) {
            util_log(1, "%s: can't open dl hdr file `%s': %s", 
                fid, file->hdr, syserrmsg(errno)
            );
            xfer_errno = XFER_EFAULT;
            return -15;
        }

    /* Open dl data file */

        if ((edes->inf[i].dfd = open(file->dat, O_RDONLY)) < 0) {
            util_log(1, "%s: can't open dl dat file `%s': %s", 
                fid, file->dat, syserrmsg(errno)
            );
            xfer_errno = XFER_EFAULT;
            return -16;
        }

    /* Determine raw packet decoders */

        switch (SYS->type) {
          case NRTS_IDA:
            edes->decode[i] = ida_decode;
            rev = ((struct nrts_ida *) SYS->info)->rev;
            mapname = ((struct nrts_ida *) SYS->info)->map;
            ida_decode_init(dump);
            break;
          case NRTS_IDA10:
            edes->decode[i] = ida10_decode;
            rev = 10;
            mapname = NULL;
            ida10_decode_init(dump);
            break;
          default:
            util_log(1, "%s: unsupported system `%d'", fid, SYS->type);
            xfer_errno = XFER_EFAULT;
            return -17;
        }

        if ((edes->inf[i].ida = idaCreateHandle(SYS->sta[0].name, rev, mapname, db, NULL, 0)) == NULL) {
            util_log(1, "%s: idaCreateHandle failed for site %s, rev %d", fid, SYS->sta[0].name, rev);
            return -18;
        }

    /* Load static parameters from the database */

        for (j = 0; j < SYS->nsta; j++) {
            isidbLookupCoords(db, SSTA->name, (UINT32) edes->tstamp, &coords);
            CSTA->lat = coords.lat;
            CSTA->lon = coords.lon;
            CSTA->elev = coords.elev;
            CSTA->depth = coords.depth;
            for (k = 0; k < SSTA->nchn; k++) {
                incomplete = FALSE;
                isiStaChnLocToStreamName(SSTA->name, SCHN->name, NULL, &name);
                isidbLookupInst(db, &name, (UINT32) edes->tstamp, &inst);
                CCHN->calib = inst.calib;
                CCHN->calper = inst.calper;
                CCHN->hang = inst.hang;
                CCHN->vang = inst.vang;
                memcpy(CCHN->instype, inst.type, NRTS_INAMLEN);
                CCHN->instype[NRTS_INAMLEN] = 0;
                if (!isidbLookupSint(db, &name, &sint)) {
                    incomplete = TRUE;
                } else if (!isidbLookupDatlen(db, &name, &datlen)) {
                    incomplete = TRUE;
                } else if (!isidbLookupWrdsiz(db, &name, &CCHN->wrdsiz)) {
                    incomplete = TRUE;
                }
                if (incomplete) {
                    util_log(1, "WARNING: incomplete sint map for %s:%s", SSTA->name, SCHN->name);
                    ++nerror;
                } else {
                    CCHN->sint   = (float) sint / 1000.0;
                    CCHN->order  = util_order();
                    CCHN->paklen = CCHN->sint * (datlen / CCHN->wrdsiz);
                }
            }
        }
    }

    if (nerror != 0) {
        util_log(1, "ERROR - incomplete sint database");
        xfer_errno = XFER_EFAULT;
        return -19;
    }

/* Load server specific signal handlers */

    if (handlers(fileno(stdout)) != 0) {
        util_log(1, "%s: load signal handler(s): %s", fid, *home, syserrmsg(errno));
        xfer_errno = XFER_EFAULT;
        return -20;
    }

    return 0;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.13  2008/01/15 19:45:18  dechavez
 * Fail with server fault when sint data are incomplete
 *
 * Revision 1.12  2007/01/11 22:02:33  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.11  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.10  2004/09/28 23:31:43  dechavez
 * conditional log supression added
 *
 * Revision 1.9  2004/04/26 21:11:13  dechavez
 * misc debugging statements added, plus MySQL support (via dbspec instead of dbdir)
 *
 * Revision 1.8  2002/09/09 22:07:42  dec
 * explicitly set sockets to non-blocking
 *
 * Revision 1.7  2002/04/25 21:13:02  dec
 * print extern ident string instead of VERSION macro
 *
 * Revision 1.6  2002/04/02 01:27:03  nobody
 * log peer right away at level 2 for debugging aid
 *
 * Revision 1.5  2002/03/11 17:31:50  dec
 * added +/-dbd parameter to set DumpBadData flag for decoder init routines
 *
 * Revision 1.4  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.3  2000/02/18 06:39:22  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 01:11:06  dec
 * Added ReleaseNotes, christened 2.10.1
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
