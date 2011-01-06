#pragma ident "$Id: ftp.c,v 1.1.1.1 2000/02/08 20:19:58 dec Exp $"
/*======================================================================
 *
 *  Use a coprocess to ftp all the files in the current directory.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "service.h"

#define MAXFILES 128
#define MAXLINLEN 128

#define NL 0x0a

#ifndef FTP
#if (defined(BSD) || defined(SUNOS) || defined (SOLARIS))
#define FTP "/usr/ucb/ftp"
#else
#define FTP "/usr/bin/ftp"
#endif /* (defined(BSD) || defined(SUNOS) || defined (SOLARIS) */
#endif /* ifdef FTP */

int do_ftp(datreq, file, nfiles)
struct drm_datreq *datreq;
char **file;
int nfiles;
{
FILE *fp;
pid_t pid;
int i, status;
static char **msg, *errmsg[] = {
    "ot connected",
    "o such file or directory",
    "ermission denied",
    "failed",
    "incorrect",
    "determine",
    "ot found",
    NULL
};
char line[MAXLINLEN], *nlptr;
struct drm_names *name;
static char *fid = "do_ftp";

    name = drm_names();

/* Generate ftp script */

    util_log(1, "generating ftp script");

    if ((fp = fopen(name->path.ftplog, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", 
            fid, name->path.ftplog, syserrmsg(errno)
        );
        return DRM_FTPFAIL;
    }
    fclose(fp);

    if ((fp = fopen(name->path.ftpscript, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", 
            fid, name->path.ftpscript, syserrmsg(errno)
        );
        return DRM_FTPFAIL;
    }

    fprintf(fp, "#!/bin/csh -f\n");
    fprintf(fp, "%s -i -n %s << EOF >>& %s\n", 
        FTP, datreq->remote.address, name->path.ftplog
    );
    fprintf(fp, "user %s %s\n", datreq->remote.user, datreq->remote.passwd);
    fprintf(fp, "binary\n");
    fprintf(fp, "mkdir %s\n", datreq->remote.dir);
    if (nfiles > 0) {
        fprintf(fp, "mkdir %s/%s\n", datreq->remote.dir, name->request);
        for (i = 0; i < nfiles; i++) {
            fprintf(fp, "put %s %s/%s/%s\n",
                file[i], datreq->remote.dir, name->request, file[i]
            );
            fprintf(fp, "ls %s/%s/%s\n",
                datreq->remote.dir, name->request, file[i]
            );
        }
    } else {
        fprintf(fp, "put %s %s/%s\n",
            name->path.message, datreq->remote.dir, name->request
        );
        fprintf(fp, "ls %s/%s\n",
            datreq->remote.dir, name->request
        );
    }
    fprintf(fp, "EOF\n");
    fprintf(fp, "exit $status\n");

    fclose(fp);

    if (chmod(name->path.ftpscript, 0700) != 0) {
        util_log(1, "%s: chmod: %s: %s", 
            fid, name->path.ftpscript, syserrmsg(errno)
        );
        return DRM_FTPFAIL;
    }

/* Execute ftp script */

    util_log(1, "executing ftp script");

    if ((pid = fork()) < 0) return DRM_FTPFAIL;

    if (pid > 0) { /* parent */

        if (waitpid(pid, &status, 0) != pid) {
            util_log(1, "%s: waitpid: %s", fid, syserrmsg(errno));
            return DRM_FTPFAIL;
        }

        if (WIFEXITED(status)) {

            if (WEXITSTATUS(status) == 0) {

            /* Examine log file for error messages  */

                util_log(1, "examining ftp log for error messages");

                if ((fp = fopen(name->path.ftplog, "r")) == NULL) {
                    util_log(1, "%s: fopen: %s: %s", 
                        fid, name->path.ftplog, syserrmsg(errno)
                    );
                    return DRM_FTPFAIL;
                }

                while (fgets(line, MAXLINLEN, fp) != NULL) {
                    if ((nlptr = strchr(line,NL)) != NULL) *nlptr = (char) NULL;
                    for (msg = errmsg; *msg != NULL; ++msg) {
                        if (strstr(line, *msg) != NULL) {
                            util_log(1, "ftp error detected: %s", line);
                            return DRM_REMOTEFTP;
                        }
                    }
                }

                util_log(1, "no ftp errors detected, removing local files");
                if (nfiles > 0) {
                    for (i = 0; i < nfiles; i++) {
                        if (unlink(file[i]) != 0) {
                            util_log(1, "%s: warning: can't unlink %s: %s",
                                fid, file[i], syserrmsg(errno)
                            );
                        }
                    }
                } else if (unlink(name->path.message) != 0) {
                    util_log(1, "%s: warning: can't unlink %s: %s",
                        fid, name->path.message, syserrmsg(errno)
                    );
                }
                return DRM_DELIVERED;

            } else {
                util_log(1, "script failed, status %d", WEXITSTATUS(status));
                return DRM_FTPFAIL;
            }
        } else {
            util_log(1, "script terminated via something other than exit");
            return DRM_FTPFAIL;
        }

    } else { /* child */
        execl(name->path.ftpscript, name->path.ftpscript, NULL);
        util_log(1, "%s(child): execl(%s, %s, NULL) failed",
            fid, name->path.ftpscript, name->path.ftpscript
        );
        exit(1);
    }
}

int drm_ftp(datreq, merged)
struct drm_datreq *datreq;
int merged;
{
int nfiles, status;
DIR *dirp;
struct dirent *dp;
struct stat sbuf;
char *file[MAXFILES];
struct drm_names *name;
static char *fid = "drm_ftp";

    if (merged) return do_ftp(datreq, file, 0);

    if ((dirp = opendir(".")) == NULL) {
        util_log(1, "%s: fatal error: can't opendir `.': %s",
            fid, syserrmsg(errno)
        );
        return DRM_FTPFAIL;
    }

    nfiles = 0;
    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (stat(dp->d_name, &sbuf) != 0) {
                util_log(1, "%s: error: can't stat `./%s: %s'",
                    fid, dp->d_name, syserrmsg(errno)
                );
                closedir(dirp);
                return DRM_FTPFAIL;
            }
            if (S_ISREG(sbuf.st_mode)) {
                if (nfiles == MAXFILES) {
                    util_log(1, "%s: too many files, increase MAXFILES", fid);
                    closedir(dirp);
                    return DRM_FTPFAIL;
                }
                if ((file[nfiles++] = strdup(dp->d_name)) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    closedir(dirp);
                    return DRM_FTPFAIL;
                }
            }
        }
    }
    closedir(dirp);

    if (nfiles < 1) {
        util_log(1, "%s: fatal error: no files found!", fid);
        return DRM_FTPFAIL;
    } else {
        return do_ftp(datreq, file, nfiles);
    }
}

/* Revision History
 *
 * $Log: ftp.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
