#pragma ident "$Id: exit.c,v 1.2 2003/11/21 20:20:54 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.  Use only in drm_service.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "drm.h"
#include "service.h"

static int abort_flag = 0;
static int delivered = 0;
static int exiting = 0;
static int orig_status;
static char *return_address = NULL;
static struct drm_datreq *datreq = NULL;

void catch_signal(sig)
int sig;
{
int pid;

    util_log(1, "%s", util_sigtoa(sig));

    if (sig == SIGHUP || sig == SIGTERM || sig == SIGINT) {
        drm_exit(DRM_INTERRUPTED);
    } else if (sig==SIGBUS || sig==SIGSEGV || sig==SIGFPE || sig==SIGILL) {
        abort_flag = 1;
        drm_exit(DRM_BAD);
    } else {
        util_log(1, "quitting on signal");
        drm_exit(DRM_BAD);
    }
}

int exitcode()
{

    if (signal(SIGHUP,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGTERM, catch_signal) == SIG_ERR) return -1;
    if (signal(SIGINT,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGBUS,  catch_signal) == SIG_ERR) return -1;
    if (signal(SIGSEGV, catch_signal) == SIG_ERR) return -1;
    if (signal(SIGPIPE, catch_signal) == SIG_ERR) return -1;

    abort_flag = 0;
    delivered = 0;

    return 0;
}

void set_delivered_flag()
{
    delivered = 1;
}

void set_datreq(ptr)
struct drm_datreq *ptr;
{
    datreq = ptr;
}

void set_return_address(address)
char *address;
{
    return_address = address;
}

void drm_exit(status)
int status;
{
FILE *fp;
char *dest;
int loglevel;
struct stat buf;
char subject[128];
char pname[MAXPATHLEN+1];
DIR *dirp;
struct dirent *dp;
struct drm_cnf *cnf;
struct drm_names *name;
static char *fid = "drm_exit";

    if (exiting) {
        util_log(1, "could not set either name or cnf");
        sprintf(subject, "IDADRM process %d failed", getpid());
        util_email("root", subject, NULL);
        util_log(1, "exit %d", orig_status);
        exit(orig_status);
    }

    exiting = 1;
    orig_status = (status == DRM_OK) ? 0 : 1;

    name = drm_names();
    cnf  = drm_cnf();

    if (status == DRM_OK) {
        dest = name->path.archive;
        loglevel = 2;
    } else if (status == DRM_IGNORE) {
        dest = name->path.archive;
        loglevel = 1;
        if ((fp = fopen(name->flag.ignored, "w")) == NULL) {
            util_log(1, "%s: ERROR: can't create flag `%s': %s",
                fid, name->flag.ignored, syserrmsg(errno)
            );
        } else {
            fclose(fp);
            util_log(2, "set ignored flag `%s'", name->flag.ignored);
        }
    } else if (status == DRM_TRYLATER) {
        util_log(1, "putting request on hold");
        dest = name->path.hold;
        loglevel = 2;
    } else if (status == DRM_DEFER || status == DRM_WATCHDOG) {
        util_log(1, "deferring request");
        if (datreq == NULL) {
            dest = name->path.manual;
        } else {
            sprintf(pname, "%s/%s", name->dir.deferred, datreq->sname);
            if (util_mkpath(pname, 0755) != 0) {
                util_log(1, "%s: ERROR: util_mkpath: %s: %s",
                    fid, pname, syserrmsg(errno)
                );
                exiting = 0;
                drm_exit(DRM_BAD);
            }
            sprintf(pname+strlen(pname), "/%s", name->request);
            dest = pname;
        }
        loglevel = 1;
        if ((fp = fopen(name->flag.deferred, "w")) == NULL) {
            util_log(1, "%s: ERROR: can't create flag `%s': %s",
                fid, name->flag.deferred, syserrmsg(errno)
            );
        } else {
            fclose(fp);
            util_log(2, "set deferred flag `%s'", name->flag.deferred);
        }
    } else if (status == DRM_INTERRUPTED) {
        util_log(1, "request processing interrupted");
        if (datreq == NULL) {
            dest = name->path.manual;
        } else {
            sprintf(pname, "%s/%s", name->dir.deferred, datreq->sname);
            if (util_mkpath(pname, 0755) != 0) {
                util_log(1, "%s: ERROR: util_mkpath: %s: %s",
                    fid, pname, syserrmsg(errno)
                );
                exiting = 0;
                drm_exit(DRM_BAD);
            }
            sprintf(pname+strlen(pname), "/%s", name->request);
            dest = pname;
        }
        loglevel = 1;
        if ((fp = fopen(name->flag.interrupted, "w")) == NULL) {
            util_log(1, "%s: ERROR: can't create flag `%s': %s",
                fid, name->flag.interrupted, syserrmsg(errno)
            );
        } else {
            fclose(fp);
            util_log(2, "set interrupted flag `%s'", name->flag.interrupted);
        }
    } else {
        dest = name->path.problems;
        loglevel = 1;
        if ((fp = fopen(name->flag.failed, "w")) == NULL) {
            util_log(1, "%s: ERROR: can't create flag `%s': %s",
                fid, name->flag.failed, syserrmsg(errno)
            );
        } else {
            fclose(fp);
            util_log(2, "set failed flag `%s'", name->flag.failed);
        }
    }

    util_log(loglevel, "moving %s to %s", name->request, dest);
    if (rename(name->path.queue, dest) != 0) {
        util_log(1, "%s: ERROR: rename (%s, %s): %s",
            fid, name->path.queue, dest, syserrmsg(errno)
        );
    }
    chmod(dest, DRM_UNLOCKED);

    if (status == DRM_OK || status == DRM_IGNORE) {
        if (stat(name->path.timestamp, &buf) != 0) {
            util_log(1, "%s: warning: can't stat timestamp %s: %s",
                fid, name->path.timestamp, syserrmsg(errno)
            );
        } else {
            util_log(1, "elapsed time: %s",
                util_lttostr(time(NULL) - buf.st_mtime, 2)
            );
        }
    }

/* We no longer destroy the working directory on error exits */

    if (status == DRM_BAD && chdir(name->dir.work) == 0) {
        util_log(1, "working directory %s NOT removed", name->dir.work);
    }
        
/* Notify administrator and requestor of all error exits */

    if (status == DRM_BAD) {
        sprintf(subject, "FAILED %s request", name->request);
        util_email(cnf->admin, subject, name->path.log);
        if (datreq != NULL && datreq->type != DRM_SPYDER) {
            if (!delivered && return_address != NULL) {
                drm_ack(return_address, DRM_FAIL);
            } else {
                util_log(1, "WARNING: requestor has not been notified");
            }
        }
    } else if (status == DRM_WATCHDOG) {
        if (datreq != NULL && datreq->type != DRM_SPYDER) {
            if (stat(name->flag.queued, &buf) == 0) {
                util_log(2, "%s exists... no message sent", name->flag.queued);
            } else {
                if ((fp = fopen(name->flag.queued, "w")) == NULL) {
                    util_log(1, "%s: fopen: %s: %s",
                        fid, name->flag.queued, syserrmsg(errno)
                    );
                }
                drm_ack(datreq->email, DRM_TRYLATER);
            }
        }
        sprintf(subject, "WATCHDOG T/O %s", name->request);
        util_email(cnf->admin, subject, name->path.log);
    }

/* Remove all temporary message files */

    util_log(2, "removing %s temp files", name->dir.work);
    if (name->msg.tmp        != NULL) unlink(name->msg.tmp);
    if (name->msg.header     != NULL) unlink(name->msg.badreq);
    if (name->msg.badreq     != NULL) unlink(name->msg.badreq);
    if (name->msg.delivered  != NULL) unlink(name->msg.delivered);
    if (name->msg.fail       != NULL) unlink(name->msg.fail);
    if (name->msg.nodata     != NULL) unlink(name->msg.nodata);
    if (name->msg.dataready  != NULL) unlink(name->msg.dataready);
    if (name->msg.forceftp   != NULL) unlink(name->msg.forceftp);
    if (name->msg.ftpfail    != NULL) unlink(name->msg.ftpfail);
    if (name->msg.rcpfail    != NULL) unlink(name->msg.rcpfail);
    if (name->msg.nochan     != NULL) unlink(name->msg.nochan);
    if (name->msg.remoteftp  != NULL) unlink(name->msg.remoteftp);
    if (name->msg.wait       != NULL) unlink(name->msg.wait);
    if (name->msg.early      != NULL) unlink(name->msg.early);
    if (name->msg.queued     != NULL) unlink(name->msg.queued);
    if (name->msg.noperm     != NULL) unlink(name->msg.noperm);
    if (name->path.info      != NULL) unlink(name->path.info);

    if (status == DRM_OK) {
        if (name->flag.patience  != NULL) unlink(name->flag.patience);
        if (name->flag.queued    != NULL) unlink(name->flag.queued);
        if (name->flag.deferred  != NULL) unlink(name->flag.deferred);
        if (name->flag.failed    != NULL) unlink(name->flag.failed);
        if (name->flag.ignored   != NULL) unlink(name->flag.ignored);
    }

/* Exit in the appropriate manner */

    if (abort_flag) {
        util_log(1, "abort - core dumped (pwd = %s)", getwd(pname));
        abort();
    } else if (
        status == DRM_OK ||
        status == DRM_IGNORE ||
        status == DRM_TRYLATER ||
        status == DRM_WATCHDOG ||
        status == DRM_DEFER
    ) {
        util_log(1, "exit %d", (status = 0));
    } else {
        util_log(1, "exit %d", (status = 1));
    }
    exit(status);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.2  2003/11/21 20:20:54  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
