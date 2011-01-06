#pragma ident "$Id: merge.c,v 1.1.1.1 2000/02/08 20:19:59 dec Exp $"
/*======================================================================
 *
 *  Merge all files in the current directory into a single file.
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

int do_merge(datreq, file, nfiles)
struct drm_datreq *datreq;
char **file;
int nfiles;
{
int i;
FILE *fp;
struct drm_names *name;
static char *fid = "do_merge";

    name = drm_names();

/* Create output file and add optional GSE2.0 message preamble */

    if (util_mkpath(name->dir.pickup, 0755) != 0) {
        util_log(1, "%s: fatal error: util_mkpath: %s: %s",
            fid, name->dir.pickup, syserrmsg(errno)
        );
        drm_exit(DRM_BAD);
    }

    util_log(1, "create `%s'", name->path.message);

    if (datreq->type == DRM_AUTODRM) {
        util_log(1, "append %s", name->path.info);
        if ((util_cat(name->path.info, name->path.message, NULL)) != 0) {
            util_log(1, "%s: util_cat: %s, %s: %s", 
                fid, name->path.info, name->path.message, syserrmsg(errno)
            );
            return -10;
        }

        if ((fp = fopen(name->path.message, "a")) == NULL) {
            util_log(1, "%s: fopen: %s: %s:",
                fid, name->path.message, syserrmsg(errno)
            );
            return -20;
        }

        fprintf(fp, "DATA_TYPE WAVEFORM GSE2.0\n");
        fclose(fp);
    }

/* Concatenate the data files into the message file */

    for (i = 0; i < nfiles; i++) {
        util_log(1, "append %s", file[i]);
        if (util_cat(file[i], name->path.message, NULL) != 0) {
            util_log(1, "%s: util_cat: %s, %s: %s", 
                fid, file[i], name->path.message, syserrmsg(errno)
            );
            return -30;
        }
    }

/* Add the STOP line if this is a GSE2.0 message */

    if (datreq->type == DRM_AUTODRM) {
        if ((fp = fopen(name->path.message, "a")) == NULL) {
            util_log(1, "%s: fopen: %s: %s", 
                fid, name->path.message, syserrmsg(errno)
            );
            return -40;
        }
        fprintf(fp, "STOP\n");
        if (ferror(fp)) return -50;
        fclose(fp);
    }

/*  Delete the individual data files */

    util_log(1, "deleting individual data files");
    for (i = 0; i < nfiles; i++) {
        if (unlink(file[i]) != 0) {
            util_log(1, "%s: unlink: %s: %s",
                fid, file[i], syserrmsg(errno)
            );
            return -60;
        }
    }

    set_merged_flag();

    return 0;
}

int merge(datreq)
struct drm_datreq *datreq;
{
int nfiles;
DIR *dirp;
struct dirent *dp;
struct stat sbuf;
char *file[MAXFILES];
static char *fid = "merge";

    util_log(1, "merging files into a single message");

/* Get list of all files in current directory */

    if ((dirp = opendir(".")) == NULL) {
        util_log(1, "%s: fatal error: can't opendir `.': %s",
            fid, syserrmsg(errno)
        );
        return -1;
    }

    nfiles = 0;
    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (stat(dp->d_name, &sbuf) != 0) {
                util_log(1, "%s: error: can't stat `./%s: %s'",
                    fid, dp->d_name, syserrmsg(errno)
                );
                closedir(dirp);
                return -2;
            }
            if (S_ISREG(sbuf.st_mode)) {
                if (nfiles == MAXFILES) {
                    util_log(1, "%s: too many files, increase MAXFILES", fid);
                    closedir(dirp);
                    return -3;
                }
                if (
                    strcmp(dp->d_name, "raw") != 0 &&
                   (file[nfiles++] = strdup(dp->d_name)) == NULL
                ) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    closedir(dirp);
                    return -4;
                }
            }
        }
    }
    closedir(dirp);

    if (nfiles < 1) {
        util_log(1, "%s: fatal error: no files found!", fid);
        return -5;
    } else {
        return do_merge(datreq, file, nfiles);
    }
}

/* Revision History
 *
 * $Log: merge.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
