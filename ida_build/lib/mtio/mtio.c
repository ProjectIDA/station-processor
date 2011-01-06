#pragma ident "$Id: mtio.c,v 1.1.1.1 2000/02/08 20:20:28 dec Exp $"
/*======================================================================
 *
 *  Unix mag tape functions.  Not all operations are valid for all tape
 *  devices.
 *
 *  Function  Description
 *  --------  -----------------------------------------------
 *  mtio_open    attach to device
 *  mtio_weof    write an end-of-file record
 *  mtio_fsf     forward space over file mark
 *  mtio_bsf     backward space over file mark (1/2", 8 mm only)
 *  mtio_fsr     forward space to inter-record gap
 *  mtio_bsr     backward space to inter-record gap
 *  mtio_rew     rewind
 *  mtio_offline take device off line
 *  mtio_nop     no operation, sets status only
 *  mtio_reten   retension the tape (cartridge tape only)
 *  mtio_erase   erase the entire tape and rewind
 *  mtio_eom     position to EOM
 *  mtio_nbsf    backward space file to beginning of file
 *  mtio_status  get status (device dependent)
 *  mtio_read    read
 *  mtio_write   write
 *  mtio_close   detach from a device
 *
 *  mtio_bsf(), mtio_bsr(), mtio_fsf(), and mtio_fsr() return the number
 *  of files or records skipped.  mtio_open() returns a pointer to a
 *  TAPE object that contains all the stuff needed internally.
 *
 *  All other functions return the ioctl return value, where 0
 *  indicates success and -1 indicates error.
 *
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Author: David Chavez (dec@essw.com)
 *         Engineering Services & Software
 *         San Diego, CA, USA
 *====================================================================*/
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mtio.h"

static TAPE *null = (TAPE *) NULL;

int mtio_weof(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTWEOF
    tp->cmnd.mt_op    = MTWEOF;
    tp->cmnd.mt_count = count;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_fsf(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTFSF
    tp->cmnd.mt_op    = MTFSF;
    tp->cmnd.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&tp->stat) == -1) return -1;
    return count - tp->stat.mt_resid;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_bsf(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTBSF
    tp->cmnd.mt_op    = MTBSF;
    tp->cmnd.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&tp->stat) == -1) return -1;
    return count - tp->stat.mt_resid;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_fsr(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTFSR
    tp->cmnd.mt_op    = MTFSR;
    tp->cmnd.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&tp->stat) == -1) return -1;
    return count - tp->stat.mt_resid;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_bsr(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTBSR
    tp->cmnd.mt_op    = MTBSR;
    tp->cmnd.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&tp->stat) == -1) return -1;
    return count - tp->stat.mt_resid;
#else
    errno = ENOSYSl
    return -1;
#endif
}

int mtio_rew(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTREW
    tp->cmnd.mt_op    = MTREW;
    tp->cmnd.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_nop(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTNOP
    tp->cmnd.mt_op    = MTNOP;
    tp->cmnd.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    return 0;
#endif
}

int mtio_reten(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTRETEN
    tp->cmnd.mt_op    = MTRETEN;
    tp->cmnd.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_erase(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTERASE
    tp->cmnd.mt_op    = MTERASE;
    tp->cmnd.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_eom(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTEOM
    tp->cmnd.mt_op    = MTEOM;
    tp->cmnd.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mtio_nbsf(TAPE *tp, int count)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

#ifdef MTNBSF
    tp->cmnd.mt_op    = MTNBSF;
    tp->cmnd.mt_count = count;
    return ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
#else
    errno = ENOSYS;
    return -1;
#endif
}

static int _mtio_open(TAPE *tp)
{
long nblk;
static char *fid = "_mtio_open";

    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    tp->fd    = open(tp->path, tp->mode);
    tp->state = (tp->fd == -1) ? MTIO_OFFLINE : MTIO_ONLINE;
    if (tp->state == MTIO_OFFLINE) return 0;

    if (tp->seekeom) return mtio_eom(tp);

    return 0;
}

void mtio_offline(TAPE *tp)
{
    if (tp == null || tp->state == MTIO_OFFLINE) return;

    tp->state = MTIO_OFFLINE;
    if (tp->fd > 0) close(tp->fd);
    tp->fd = -1;
}

int mtio_status(TAPE *tp, struct mtget *info)
{
int status;
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) return -1;
    }

    tp->cmnd.mt_op    = MTNOP;
    tp->cmnd.mt_count = 1;
/*
status = ioctl(tp->fd, MTIOCTOP, &tp->cmnd);
memset((char *) info, 0, sizeof(struct mtget));
status = ioctl(tp->fd, MTIOCGET, (char *) info);
*/
    return (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) != -1) ?
            ioctl(tp->fd, MTIOCGET, (char *) info) : -1;
}

int mtio_eject(TAPE *tp)
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) return 0;
    }

    tp->cmnd.mt_op    = MTOFFL;
    tp->cmnd.mt_count = 1;
    if (ioctl(tp->fd, MTIOCTOP, &tp->cmnd) == -1) return -1;

    mtio_offline(tp);

    return 0;
}

TAPE *mtio_open(char *path, char *perm)
{
int fd, mode, truncate, seekeom;
struct stat buf;
TAPE *tp;

    if (stat(path, &buf) != 0) return null;

    if (
        strcasecmp(perm, "r"  ) == 0 ||
        strcasecmp(perm, "rb" ) == 0 ||
        strcasecmp(perm, "r+" ) == 0 ||
        strcasecmp(perm, "r+b") == 0 ||
        strcasecmp(perm, "rb+") == 0
    ) {
        mode     = O_RDONLY;
        truncate = 0;
        seekeom  = 0;

    } else if (
        strcasecmp(perm, "w"  ) == 0 ||
        strcasecmp(perm, "wb" ) == 0 ||
        strcasecmp(perm, "w+" ) == 0 ||
        strcasecmp(perm, "w+b") == 0 ||
        strcasecmp(perm, "wb+") == 0
    ) {
        mode     = O_RDWR;
        truncate = 1;
        seekeom  = 0;

    } else if (
        strcasecmp(perm, "a"  ) == 0 ||
        strcasecmp(perm, "ab" ) == 0 ||
        strcasecmp(perm, "a+" ) == 0 ||
        strcasecmp(perm, "a+b") == 0 ||
        strcasecmp(perm, "ab+") == 0
    ) {
        mode     = O_RDWR;
        truncate = 0;
        seekeom  = 1;

    } else {
        errno = EINVAL;
        return null;
    }

    if ((tp = (TAPE *) malloc(sizeof(TAPE))) == null) return null;
    if ((tp->path = (char *) strdup(path)) == (char *) NULL) return null;

    tp->mode     = mode;
    tp->truncate = truncate;
    tp->seekeom  = seekeom;

    return _mtio_open(tp) ? null : tp;
}

void mtio_close(TAPE *tp)
{
    if (tp == null) return;
    if (tp->path != (char *) NULL) free(tp->path);
    close(tp->fd);
    free(tp);
}

size_t mtio_read(TAPE *tp, char *buf, size_t nbyte)
{
size_t retval;

    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) {
            errno = EAGAIN;
            return -1;
        }
    }

    retval = read(tp->fd, buf, nbyte);
    return retval;
}

size_t mtio_write(TAPE *tp, char *buf, size_t nbyte)
{
size_t retval;
static char *fid = "mtio_write";

    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) {
            errno = EAGAIN;
            return -1;
        }
    }

    retval = write(tp->fd, buf, nbyte);

    return retval;
}

int mtio_state(TAPE *tp)
{
struct mtget status;

    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) {
            errno = EAGAIN;
            return -1;
        }
    }

    tp->state = mtio_status(tp, &status) != -1 ? MTIO_ONLINE : MTIO_OFFLINE;

    return tp->state;
}

#ifdef DEBUG_TEST

#include <stdio.h>

#define STATUS 0
#define EJECT  1

main(argc, argv)
int argc;
char *argv[];
{
int fd, status, command, save_errno;
struct mtget info;
TAPE *tp;

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s tape_device [command]\n", argv[0]);
        exit(1);
    }

    util_logopen(NULL, 1, 9, 1, NULL, argv[0]);

    if (argc == 3) {
        if (strcasecmp(argv[2], "status") == 0) {
            command = STATUS;
        } else if (strcasecmp(argv[2], "eject") == 0) {
            command = EJECT;
        } else {
            fprintf(stderr, "%s: unknown command `%s'\n", argv[0], argv[2]);
            exit(1);
        }
    } else {
        command = STATUS;
    }

    if ((tp = mtio_open(argv[1], "r")) == (TAPE *) NULL) {
        fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
        perror("mtio_open");
        exit(1);
    }

    if (tp->state == MTIO_OFFLINE) {
        fprintf(stderr, "%s: device off-line\n", argv[1]);
        exit(0);
    }

    if (command == STATUS) {
        if ((status = mtio_status(tp, &info)) != 0) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("mtio_status");
            fprintf(stderr, "mtio_status failed with status %d\n", status);
            exit(1);
        }

        printf("Device:   %s\n", argv[1]);
        printf("type   = %d\n", info.mt_type);
        printf("dsreg  = %d\n", info.mt_dsreg);
        printf("erreg  = %d\n", info.mt_erreg);
        printf("resid  = %d\n", info.mt_resid);
        printf("fileno = %d\n", info.mt_fileno);
        printf("blkno  = %d\n", info.mt_blkno);
        printf("flags  = %d\n", info.mt_flags);
        printf("bf     = %d\n", info.mt_bf);

    } else if (command == EJECT) {

        if ((status = mtio_eject(tp)) == -1) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("mtio_eject");
            fprintf(stderr, "mtio_eject failed with status %d\n", status);
            exit(1);
        }

    } else {

        fprintf(stderr, "%s: program error\n", argv[0]);
        exit(1);

    }

    close(fd);
    exit(0);
}
#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: mtio.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:28  dec
 * import existing IDA/NRTS sources
 *
 */
