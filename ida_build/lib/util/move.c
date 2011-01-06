#pragma ident "$Id: move.c,v 1.3 2001/07/25 05:42:52 dec Exp $"
/*======================================================================
 *
 *  Move the contents of one directory to another.  Both arguments must
 *  be directories, and both must already exist.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"

#ifdef SUNOS
char *getwd();
#endif

int util_move(char *src, char *dest)
{
DIR *dirp;
struct dirent *dp;
struct stat sbuf;
int src_is_dir, dest_is_dir;
char temp1[MAXPATHLEN+1];
char temp2[MAXPATHLEN+1];
char from[MAXPATHLEN+1];
char to[MAXPATHLEN+1];
char wdir[MAXPATHLEN+1];
static char *fid = "util_move";

#ifdef SUNOS
    if (getwd(wdir) == (char *) NULL) return -1;
#else
    getcwd(wdir, MAXPATHLEN+1);
#endif

    if (src[0] != '/') {
        sprintf(temp1, "%s/%s", wdir, src);
        src = temp1;
    }

    if (dest[0] != '/') {
        sprintf(temp2, "%s/%s", wdir, dest);
        dest = temp2;
    }

    if (stat(src, &sbuf) != 0) return -2;

    src_is_dir = S_ISDIR(sbuf.st_mode);

    if (stat(dest, &sbuf) != 0) {
        if (errno == ENOENT) {
            if (util_mkpath(dest, 0755) != 0) return -3;
            dest_is_dir = 1;
        } else {
            return -4;
        }
    } else {
        dest_is_dir = S_ISDIR(sbuf.st_mode);
    }

    if (!src_is_dir && !dest_is_dir) return rename(src, dest);

    if (!src_is_dir || !dest_is_dir) {
        errno = ENOTDIR;
        return -5;
    }

    if ((dirp = opendir(src)) == NULL) return -6;

    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            sprintf(from, "%s/%s", src,  dp->d_name);
            sprintf(to,   "%s/%s", dest, dp->d_name);
            if (stat(from, &sbuf) != 0) return -7;
            if (S_ISDIR(sbuf.st_mode)) {
                if (util_move(from, to) != 0) return -8;
            } else {
                if (rename(from, to) != 0) return -9;
            }
        }
    }
    closedir(dirp);

    return rmdir(src);
}

#ifdef DEBUG_TEST

main(argc, argv)
int argc;
char *argv[];
{
int status;
char *src;
char *dest;

    if (argc != 3) {
        fprintf(stderr, "usage: %s src dest\n", argv[0]);
        exit(1);
    }

    src =  argv[1];
    dest = argv[2];

    printf("moving contents of `%s' to `%s'\n", src, dest);
    status = util_move(src, dest);
    printf("util_move() returns %d\n", status);

    exit(-status);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: move.c,v $
 * Revision 1.3  2001/07/25 05:42:52  dec
 * comments around tokens after #endif
 *
 * Revision 1.2  2001/05/07 22:40:13  dec
 * ANSI function declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
