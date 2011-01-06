#pragma ident "$Id: names.c,v 1.2 2003/12/10 06:30:31 dechavez Exp $"
/*======================================================================
 *
 *  Routines to manipulate IDADRM file names.  All meta knowledge about
 *  the tree structure and file name syntax is hidden in here.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "util.h"
#include "drm.h"

static int all_set = 0;
static struct drm_names drm_name;

int drm_init(home, buffer, reqname)
char *home;
char *buffer;
char *reqname;
{
FILE *fp;
int i, j;
char *string;
struct drm_finfo *finfo;
struct drm_cnf *cnf;
static char *fid = "drm_init";

    if (all_set) {
        util_email(DRM_ADMIN, "drm_init error 1", NULL);
        util_log(1, "%s: program error 1", fid);
        drm_exit(DRM_BAD);
    }

    if (home == NULL) home = getenv(DRM_HOME);
    if (home == NULL) home = DRM_DEFHOME;
    if (home == NULL) home = NRTS_DEFHOME;

    if (reqname != NULL) {
        if ((finfo = drm_fparse(reqname, buffer)) == NULL) {
            util_log(1, "%s: fatal error: can't parse `%s'", fid, reqname);
            return -1;
        }
        drm_ack_init(finfo->type);
    } else {
        finfo = NULL;
    }

/* Directories */

    sprintf(buffer, "%s", home);
    if ((drm_name.dir.home = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/etc", drm_name.dir.home);
    if ((drm_name.dir.etc = strdup(buffer)) == NULL) return -1;

    if (finfo != NULL && finfo->type == DRM_AUTODRM) {
        sprintf(buffer, "%s/msgs/gse", drm_name.dir.home);
    } else {
        sprintf(buffer, "%s/msgs", drm_name.dir.home);
    }
    if ((drm_name.dir.msgs = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/flags", drm_name.dir.home);
    if ((drm_name.dir.flags = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/tmp", drm_name.dir.home);
    if ((drm_name.dir.tmp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/hold", drm_name.dir.home);
    if ((drm_name.dir.hold = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/queue", drm_name.dir.home);
    if ((drm_name.dir.queue = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/deferred", drm_name.dir.home);
    if ((drm_name.dir.deferred = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/manual", drm_name.dir.home);
    if ((drm_name.dir.manual = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/incoming", drm_name.dir.home);
    if ((drm_name.dir.incoming = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/problems", drm_name.dir.home);
    if ((drm_name.dir.problems = strdup(buffer)) == NULL) return -1;

/*  Static Files  */

    sprintf(buffer, "%s/.locklock", drm_name.dir.home);
    if ((drm_name.path.locklock = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/drmd.pid", drm_name.dir.home);
    if ((drm_name.path.pid = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/cnf", drm_name.dir.etc);
    if ((drm_name.path.cnf = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/remap", drm_name.dir.etc);
    if ((drm_name.path.remap = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/abusers", drm_name.dir.etc);
    if ((drm_name.path.abusers = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/auth", drm_name.dir.etc);
    if ((drm_name.path.auth = strdup(buffer)) == NULL) return -1;

/*  Canned messages  */

    sprintf(buffer, "%s/header", drm_name.dir.msgs);
    if ((drm_name.canned.header = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/delivered", drm_name.dir.msgs);
    if ((drm_name.canned.delivered = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/fail", drm_name.dir.msgs);
    if ((drm_name.canned.fail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/nodata", drm_name.dir.msgs);
    if ((drm_name.canned.nodata = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/noperm", drm_name.dir.msgs);
    if ((drm_name.canned.noperm = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/dataready", drm_name.dir.msgs);
    if ((drm_name.canned.dataready = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/forceftp", drm_name.dir.msgs);
    if ((drm_name.canned.forceftp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/rcpfail", drm_name.dir.msgs);
    if ((drm_name.canned.rcpfail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/ftpfail", drm_name.dir.msgs);
    if ((drm_name.canned.ftpfail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/nochan", drm_name.dir.msgs);
    if ((drm_name.canned.nochan = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/remoteftp", drm_name.dir.msgs);
    if ((drm_name.canned.remoteftp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/wait", drm_name.dir.msgs);
    if ((drm_name.canned.wait = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/early", drm_name.dir.msgs);
    if ((drm_name.canned.early = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/queued", drm_name.dir.msgs);
    if ((drm_name.canned.queued = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/badreq", drm_name.dir.msgs);
    if ((drm_name.canned.badreq = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/help", drm_name.dir.msgs);
    if ((drm_name.canned.help = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/slist", drm_name.dir.msgs);
    if ((drm_name.canned.slist = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/calib", drm_name.dir.msgs);
    if ((drm_name.canned.calib = strdup(buffer)) == NULL) return -1;

    all_set = 1;
    if ((cnf = drm_rdcnf(drm_name.path.cnf, buffer)) == NULL) return -1;
    if (reqname == NULL) return 0;

    drm_name.flag.patience = NULL;

/*  Request specific drm_names  */

    if ((drm_name.request = strdup(reqname)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.incoming, drm_name.request);
    if ((drm_name.path.incoming = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.queue, drm_name.request);
    if ((drm_name.path.queue = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.manual, drm_name.request);
    if ((drm_name.path.manual = strdup(buffer)) == NULL) return -1;

    if (finfo->type == DRM_IDADRM) {
        string = DRM_IDADRM_STRING;
    } else if (finfo->type == DRM_SPYDER) {
        string = DRM_SPYDER_STRING;
    } else if (finfo->type == DRM_AUTODRM) {
        string = DRM_AUTODRM_STRING;
    } else {
        util_log(1, "%s: fatal error: unknown type `%d'", fid, finfo->type);
        return -1;
    }

/*  Work directory is in anonymous ftp area  */

    sprintf(buffer, "%s/%s", cnf->anonftp, drm_mkname(finfo));
    if ((drm_name.dir.anonftp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s%s", cnf->ftphome, drm_name.dir.anonftp);
    if ((drm_name.dir.work = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/pickup", cnf->anonftp);
    if ((drm_name.dir.anonpickup = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s%s", cnf->ftphome, drm_name.dir.anonpickup);
    if ((drm_name.dir.pickup = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.pickup, drm_name.request);
    if ((drm_name.path.message = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/raw", drm_name.dir.work);
    if ((drm_name.path.raw = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.hold, drm_name.request);
    if ((drm_name.path.hold = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s", drm_name.dir.problems, drm_name.request);
    if ((drm_name.path.problems = strdup(buffer)) == NULL) return -1;

/*  Archive directory holds requests, logs, flags, ftp scripts and timestamp  */

    sprintf(buffer, "%s/archive/%s/%02d%03d/%05d",
        drm_name.dir.home, string, finfo->yr, finfo->day, finfo->seqno
    );
    if ((drm_name.dir.archive = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/request.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.path.archive = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/log.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.path.log = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/ftpscript.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.path.ftpscript = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/ftplog.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.path.ftplog = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/timestamp", drm_name.dir.archive);
    if ((drm_name.path.timestamp = strdup(buffer)) == NULL) return -1;

/*  Flags  */

    sprintf(buffer, "%s/patience.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.patience = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/queued.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.queued = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/deferred.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.deferred = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/interrupted.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.interrupted = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/ignored.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.ignored = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/failed.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.failed = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/done.%d", drm_name.dir.archive, finfo->set);
    if ((drm_name.flag.done = strdup(buffer)) == NULL) return -1;

/*  Request specific scripts and email files are kept in local tmp area  */

    sprintf(buffer, "%s/%s.slist", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.slist = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.tmp", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.tmp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.info", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.path.info = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.badreq", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.badreq = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.delivered", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.delivered = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.fail", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.fail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.nodata", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.nodata = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.noperm", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.noperm = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.dataready", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.dataready = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.forceftp", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.forceftp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.rcpfail", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.rcpfail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.ftpfail", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.ftpfail = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.nochan", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.nochan = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.remoteftp", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.remoteftp = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.wait", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.wait = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.early", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.early = strdup(buffer)) == NULL) return -1;

    sprintf(buffer, "%s/%s.queued", drm_name.dir.tmp, drm_name.request);
    if ((drm_name.msg.queued = strdup(buffer)) == NULL) return -1;

    return 0;
}

struct drm_names *drm_names()
{
static char *fid = "drm_names";

    if (all_set) return &drm_name;
    util_log(1, "%s: fatal error: names not initialized", fid);
    drm_exit(DRM_BAD);
}

#define MAXTOKENS   4   /* type yrday seq set */
#define DELIMITERS "-_."

struct drm_finfo *drm_fparse(fname, buffer)
char *fname;
char *buffer;
{
static struct drm_finfo finfo;
static char *token[MAXTOKENS];
int i, status, ntoken;
long ltmp;

    strcpy(buffer, fname);
    ntoken = util_parse(buffer, token, DELIMITERS, MAXTOKENS, 0);
    if (ntoken != MAXTOKENS) return NULL;

    if (strcmp(util_lcase(token[0]), DRM_IDADRM_STRING) == 0) {
        finfo.type = DRM_IDADRM;
    } else if (strcmp(token[0], DRM_SPYDER_STRING) == 0) {
        finfo.type = DRM_SPYDER;
    } else if (strcmp(token[0], DRM_AUTODRM_STRING) == 0) {
        finfo.type = DRM_AUTODRM;
    } else {
        return NULL;
    }

    for (i = 0; i < strlen(token[1]); i++) {
        if (!isdigit(*(token[1] + i))) return NULL;
    }
    ltmp = atol(token[1]);
    finfo.yr  = ltmp / 1000;
    finfo.day = ltmp - (finfo.yr * 1000);

    for (i = 0; i < strlen(token[2]); i++) {
        if (!isdigit(*(token[2] + i))) return NULL;
    }
    finfo.seqno = atol(token[2]);

    for (i = 0; i < strlen(token[3]); i++) {
        if (!isdigit(*(token[3] + i))) return NULL;
    }
    finfo.set = atoi(token[3]);

    return &finfo;
}

char *drm_mkname(finfo)
struct drm_finfo *finfo;
{
char *string;
struct drm_names *name;
static char *delimiter = DELIMITERS;
static char fname[DRM_MAXNAMLEN];

    name = drm_names();

    if (finfo->type == DRM_IDADRM) {
        string = DRM_IDADRM_STRING;
    } else if (finfo->type == DRM_SPYDER) {
        string = DRM_SPYDER_STRING;
    } else if (finfo->type == DRM_AUTODRM) {
        string = DRM_AUTODRM_STRING;
    } else {
        return NULL;
    }

    sprintf(fname, "%s%c%02d%03d%c%05ld%c%d",
        string, delimiter[0],
        finfo->yr, finfo->day, delimiter[1],
        finfo->seqno, delimiter[2],
        finfo->set
    );

    return fname;
}

#ifdef DEBUG_TEST

int main(int argc, char **argv)
{
char buffer[DRM_BUFLEN];
char *home = NULL;
char *reqname = "idadrm-15.1";
struct drm_names *name;

    util_logopen(NULL, 1, 9, 1, NULL, argv[0]);

    home = (argc > 1) ? argv[1] : NULL;

    drm_init(home, buffer, reqname);

    name = drm_names();

    printf("name->dir.home         = %s\n", name->dir.home);
    printf("name->dir.etc          = %s\n", name->dir.etc);
    printf("name->dir.msgs         = %s\n", name->dir.msgs);
    printf("name->dir.tmp          = %s\n", name->dir.tmp);
    printf("name->dir.hold         = %s\n", name->dir.hold);
    printf("name->dir.queue        = %s\n", name->dir.queue);
    printf("name->dir.incoming     = %s\n", name->dir.incoming);
    printf("name->dir.problems     = %s\n", name->dir.problems);
    printf("name->dir.work         = %s\n", name->dir.work);
    printf("name->dir.anonftp      = %s\n", name->dir.anonftp);
    printf("name->path.raw         = %s\n", name->path.raw);
    printf("name->path.locklock    = %s\n", name->path.locklock);
    printf("name->path.cnf         = %s\n", name->path.cnf);
    printf("name->path.incoming    = %s\n", name->path.incoming);
    printf("name->path.queue       = %s\n", name->path.queue);
    printf("name->path.pid         = %s\n", name->path.pid);
    printf("name->dir.archive      = %s\n", name->dir.archive);
    printf("name->path.archive     = %s\n", name->path.archive);
    printf("name->path.log         = %s\n", name->path.log);
    printf("name->path.timestamp   = %s\n", name->path.timestamp);
    printf("name->path.ftpscript   = %s\n", name->path.ftpscript);
    printf("name->path.ftplog      = %s\n", name->path.ftplog);
    printf("name->canned.help      = %s\n", name->canned.help);
    printf("name->canned.slist     = %s\n", name->canned.slist);
    printf("name->canned.calib     = %s\n", name->canned.calib);
    printf("name->canned.badreq    = %s\n", name->canned.badreq);
    printf("name->canned.delivered = %s\n", name->canned.delivered);
    printf("name->canned.fail      = %s\n", name->canned.fail);
    printf("name->canned.nodata    = %s\n", name->canned.nodata);
    printf("name->canned.dataready = %s\n", name->canned.dataready);
    printf("name->canned.ftpfail   = %s\n", name->canned.ftpfail);
    printf("name->canned.nochan    = %s\n", name->canned.nochan);
    printf("name->canned.remoteftp = %s\n", name->canned.remoteftp);
    printf("name->canned.wait      = %s\n", name->canned.wait);
    printf("name->canned.early     = %s\n", name->canned.early);
    printf("name->canned.queued    = %s\n", name->canned.queued);
    printf("name->msg.badreq       = %s\n", name->msg.badreq);
    printf("name->msg.delivered    = %s\n", name->msg.delivered);
    printf("name->msg.fail         = %s\n", name->msg.fail);
    printf("name->msg.nodata       = %s\n", name->msg.nodata);
    printf("name->msg.dataready    = %s\n", name->msg.dataready);
    printf("name->msg.ftpfail      = %s\n", name->msg.ftpfail);
    printf("name->msg.nochan       = %s\n", name->msg.nochan);
    printf("name->msg.remoteftp    = %s\n", name->msg.remoteftp);
    printf("name->msg.wait         = %s\n", name->msg.wait);
    printf("name->msg.early        = %s\n", name->msg.early);
    printf("name->msg.queued       = %s\n", name->msg.queued);

    exit(0);
}
    
#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: names.c,v $
 * Revision 1.2  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
