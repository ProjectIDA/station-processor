#pragma ident "$Id: mkdl.c,v 1.9 2007/02/08 22:47:59 dechavez Exp $"
/*======================================================================
 *
 *  Create the specified NRTS disk buffer.
 *
 *====================================================================*/
#include <time.h>
#include "nrts.h"
#include "util.h"
#include "ida.h"

#ifdef WIN32
int truncate(char *path, off_t length)
    {
    int fd, ret;

    fd = open (path, O_WRONLY);
    if (fd < 0) return -1;

    ret = ftruncate (fd, length);
    close (fd);
    return ret;
    }
#endif

static void help(char *myname)
{
    fprintf(stderr,"usage: %s [options] sys_name\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"db=spec     => override DBIO_DATABASE specifier\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"home=name   => set NRTS_HOME\n");
    fprintf(stderr,"fill={0|1}  => initialize dl files flag\n");
    exit(1);
}

int main (int argc, char **argv)
{
long count = 0;
int debug = 1;
char *log = (char *) NULL;
FILE *fpcnf, *fp;
int i, j, k;
int rev, flags = 0, fill = 0;
int zero = 0;
off_t off, hoff, doff;
char *home = NULL;
char *sysname  = NULL;
char *buffer;
long len;
long maxlen = 0;
struct stat hstat, dstat;
struct nrts_sys sys, *sysptr;
struct nrts_files *file;
struct nrts_chn *chn;
struct {
    int hdr;
    int dat;
    int sys;
} exist;
struct {
    int hdr;
    int dat;
    int bwd;
} fd;
struct nrts_ida *info;
struct nrts_mmap mmap, *map;
DBIO *db;
char *dbspec = (char *) NULL;
IDA *ida;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "fill=", strlen("fill=")) == 0) {
            fill = atoi(argv[i] + strlen("fill="));
        } else if (strcmp(argv[i], "-fill") == 0) {
            fill = 1;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else {
            if (sysname != NULL) help(argv[0]);
            sysname = argv[i];
        }
    }

/*  Must specify system name  */

    if (sysname == NULL) help(argv[0]);

/* Start logging facility if selected */

    if (debug > 0) util_logopen(log, 1, 9, debug, log, argv[0]);

/*  Get file names  */

    file = nrts_files(&home, sysname);

/*  Open configuration file  */

    if ((fpcnf = fopen(file->cnf, "r")) == NULL) {
        perror(file->cnf);
        exit(1);
    }

    memset(&sys, 0, sizeof(struct nrts_sys));

    rewind(fpcnf);
    if (nrts_rcnf(fpcnf, file->cnf, &sys) != 0) {
        fprintf(stderr, "can't read configuration file\n");
        exit(1);
    }
    fclose(fpcnf);

    if (sys.nsta != 1) {
        fprintf(stderr, "program design supports only one station per disk loop\n");
        exit(1);
    }

/* Load the data base */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        if (dbspec == NULL) {
            fprintf(stderr, "%s: undefined database.  Set DBIO_DATABASE or use db=spec\n", argv[0]);
        } else {
            fprintf(stderr, "%s: can't load database `%s': %s\n", argv[0], dbspec, syserrmsg(errno));
        }
        exit(1);
    }
    printf("Database `%s' loaded\n", db->dbid);
 
    if (sys.type == NRTS_IDA) {
        info = (struct nrts_ida *) &sys.info;
        rev = info->rev;
    } else if (sys.type == NRTS_IDA10) {
        rev = 10;
    }

    if ((ida = idaCreateHandle(sys.sta[0].name, rev, NULL, db, NULL, flags)) == NULL) {
        fprintf(stderr, "%s: idaCreateHandle failed\n", argv[0]);
        exit (1);
    }

    printf("Channel map = '%s'\n", ida->mapname);

/*  See if we already have a disk buffer  */

    if (stat(file->hdr, &hstat) != 0) {
        if (errno == ENOENT) {
            exist.hdr = 0;
        } else {
            fprintf(stderr, "can't stat ", file->hdr);
            perror(file->hdr);
            exit(1);
        }
    } else {
        exist.hdr = 1;
    }

    if (stat(file->dat, &dstat) != 0) {
        if (errno == ENOENT) {
            exist.dat = 0;
        } else {
            fprintf(stderr, "can't stat ", file->dat);
            perror(file->dat);
            exit(1);
        }
    } else {
        exist.dat = 1;
    }

    if (stat(file->sys, &dstat) != 0) {
        if (errno == ENOENT) {
            exist.sys = 0;
        } else {
            fprintf(stderr, "can't stat ", file->sys);
            perror(file->sys);
            exit(1);
        }
    } else {
        exist.sys = 1;
    }

    if (exist.hdr || exist.dat || exist.sys) {
        fprintf(stderr, "Disk buffer files for this system already exist.\n");
        if (utilQuery("OK to delete? ")) {
            if (exist.hdr) unlink(file->hdr);
            if (exist.dat) unlink(file->dat);
            if (exist.sys) unlink(file->sys);
        } else {
            exit(1);
        }
    }

/*  Initialize system information  */

    sys.order = util_order();
    sys.pid   = 0;

    sys.status     = NRTS_IDLE;
    sys.bwd        = NRTS_STABLE;
    sys.nread      = 0;
    sys.count      = 0;
    sys.reclen     = IDA_BUFSIZ;

    maxlen = sys.reclen;

/* Initialize channel information  */

    hoff = doff = 0;
    for (i = 0; i < sys.nsta; i++) {
        for (j = 0; j < sys.sta[i].nchn; j++) {

            chn = sys.sta[i].chn + j;

            if (chn->dat.nrec <= chn->dat.hide) {
                fprintf(stderr, "*** cnf error *** ");
                fprintf(stderr, "one or more streams are misconfigured (nrec < hide)\n");
                exit(1);
            }

        /* initialize status fields */

            chn->status = NRTS_IDLE;
            chn->nread  = 0;
            chn->count  = 0;
            chn->nseg   = 0;
            chn->tread  = NRTS_UNDEFINED_TIMESTAMP;
            chn->beg    = (double) NRTS_UNDEFINED_TIMESTAMP;
            chn->end    = (double) NRTS_UNDEFINED_TIMESTAMP;
            chn->sint   = (float) 0;

        /* initialize indices */

            chn->hdr.oldest = 0;
            chn->hdr.yngest = -1;
            chn->hdr.siz    = (chn->hdr.len * chn->hdr.nrec);
            chn->hdr.off    = hoff; hoff += chn->hdr.siz;
            chn->hdr.lend   = chn->hdr.nrec - chn->hdr.hide;

            chn->dat.oldest = 0;
            chn->dat.yngest = -1;
            chn->dat.siz    = (chn->dat.len * chn->dat.nrec);
            chn->dat.off    = doff; doff += chn->dat.siz;
            chn->dat.lend   = chn->dat.nrec - chn->dat.hide;

            if (chn->hdr.len > maxlen) maxlen = chn->hdr.len;
            if (chn->dat.len > maxlen) maxlen = chn->dat.len;

        }
    }

    nrts_prtsys(stdout, &sys, NULL, NRTS_SYSINFO | NRTS_CONFIG);

/* Create the binary wfdiscs file */

    if ((fd.bwd = open(file->bwd, O_CREAT | O_RDWR | O_TRUNC, 0644)) < 0) {
        perror(file->bwd);
        exit(1);
    }
    close(fd.bwd);

/*  Create and zero fill the disk buffer files  */

    if (fill) {
        if ((buffer = (char *) malloc(maxlen)) == NULL) {
            perror("malloc");
            exit(1);
        } else {
            memset(buffer, 0, maxlen);
        }
    }

    if ((fd.hdr = open(file->hdr, O_CREAT | O_RDWR, 0644)) < 0) {
        perror(file->hdr);
        exit(1);
    }

    if ((fd.dat = open(file->dat, O_CREAT | O_RDWR, 0644)) < 0) {
        perror(file->dat);
        exit(1);
    }

    hoff = 0;
    doff = 0;
    for (i = 0; i < sys.nsta; i++) {
        printf("Creating %s buffers:\n", sys.sta[i].name);
        for (j = 0; j < sys.sta[i].nchn; j++) {
            chn = sys.sta[i].chn + j;
            printf("%ld records of %s...", chn->hdr.nrec, chn->name);
            fflush(stdout);
            if (fill) {
                for (k = 0; k < chn->hdr.nrec; k++) {
                    if (write(fd.hdr, buffer, chn->hdr.len) != chn->hdr.len) {
                        perror(file->hdr);
                        exit(1);
                    }
                    if (write(fd.dat, buffer, chn->dat.len) != chn->dat.len) {
                        perror(file->dat);
                        exit(1);
                    }
                }
            } else {
                len = chn->hdr.len * chn->hdr.nrec;
                off = hoff + len - 1; hoff += len;
                if (lseek(fd.hdr, off, SEEK_SET) != off) {
                    perror(file->hdr);
                    exit(1);
                }
                if (write(fd.hdr, &zero, 1) != 1) {
                    perror(file->hdr);
                    exit(1);
                }
                len = chn->dat.len * chn->dat.nrec;
                off = doff + len - 1; doff += len;
                if (lseek(fd.dat, off, SEEK_SET) != off) {
                    perror(file->dat);
                    exit(1);
                }
                if (write(fd.dat, &zero, 1) != 1) {
                    perror(file->dat);
                    exit(1);
                }
            }
            printf("done\n");
        }
    }
    printf("\n");
    close(fd.hdr);
    close(fd.dat);

/* Print a description of the new disk buffer */

    printf("\n");
    for (i = 0; i < sys.nsta; i++) {
        nrts_prtsta(stdout, sys.sta + i, NULL, NRTS_PRTALL);
    }

/*  Map the system structure into shared memory space  */

    if ((fp = fopen(file->sys, "w")) == (FILE *) NULL) {
        fprintf(stderr, "truncatefopen ");
        perror(file->sys);
        exit(1);
    }
    fclose(fp);
    if (truncate(file->sys, sizeof(struct nrts_sys)) != 0) {
        fprintf(stderr, "truncate: ");
        perror(file->sys);
        exit(1);
    }
    map = &mmap;
    if (nrts_mmap(file->sys, "w+", NRTS_SYSTEM, map) != 0) {
        perror("nrts_mmap");
        exit(1);
    }

    sysptr = (struct nrts_sys *) map->ptr;

    if (nrts_wlockw(map) == -1) {
        perror("nrts_wlockw");
        exit(1);
    }

    *sysptr = sys;
    nrts_prtsys(stdout, sysptr, NULL, NRTS_SYSINFO | NRTS_CONFIG);

    if (nrts_unlock(map) == -1) {
        perror("nrts_unlock");
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: mkdl.c,v $
 * Revision 1.9  2007/02/08 22:47:59  dechavez
 * check for configuration inconsistencies, make no fill the default
 *
 * Revision 1.8  2006/02/10 02:06:33  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.7  2005/05/23 20:52:16  dechavez
 * WIN32 mods (aap)
 *
 * Revision 1.6  2005/05/06 22:17:39  dechavez
 * checkpoint build following removal of old raw nrts constructs
 *
 * Revision 1.5  2004/04/26 21:12:35  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.4  2003/12/10 06:24:52  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2003/11/04 00:39:31  dechavez
 * use NRTS_UNDEFINED_TIMESTAMP instead of hardcoded value
 *
 * Revision 1.2  2003/06/11 21:02:19  dechavez
 * use utilQuery
 *
 * Revision 1.1.1.1  2000/02/08 20:20:13  dec
 * import existing IDA/NRTS sources
 *
 */
