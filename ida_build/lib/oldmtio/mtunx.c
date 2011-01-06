#pragma ident "$Id: mtunx.c,v 1.3 2003/12/10 06:03:36 dechavez Exp $"
/*======================================================================
 *
 *  Unix mag tape functions.  Not all operations are valid for all tape
 *  devices.
 *
 *  Function  Description
 *  --------  -----------------------------------------------
 *  mtopen    attach to device
 *  mtweof    write an end-of-file record
 *  mtfsf     forward space over file mark
 *  mtbsf     backward space over file mark (1/2", 8 mm only)
 *  mtfsr     forward space to inter-record gap
 *  mtbsr     backward space to inter-record gap
 *  mtrew     rewind
 *  mtoffl    rewind and take the drive offline
 *  mtnop     no operation, sets status only
 *  mtreten   retension the tape (cartridge tape only)
 *  mterase   erase the entire tape and rewind
 *  mteom     position to EOM
 *  mtnbsf    backward space file to beginning of file
 *  mtstatus  get status (device dependent)
 *  mtread    read  (defined as macro in oldmtio.h)
 *  mtwrite   write (defined as macro in oldmtio.h)
 *  mtclose   close (defined as macro in oldmtio.h)
 *
 *  mtbsf(), mtbsr(), mtfsf(), and mtfsr() return the number of files
 *  or records skipped.  mtopen() returns the open() return value + 1,
 *  so that it can return NULL (0) on error and thus be compatible with
 *  the MS-DOS/ASC88 version of these functions.  All these functions
 *  decrement the file descriptor by one to account for this.
 *
 *  mtstatus() returns NULL on error, otherwise a pointer to a static
 *  struct mtget.  All other functions return the ioctl return value,
 *  where 0 indicates success and -1 indicates error.
 *
 *====================================================================*/
#ifndef SCO_SV
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/mtio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include "oldmtio.h"

static struct mtop  mt_cmnd;
static struct mtget mt_stat;
static int fdm1;

TAPE mtopen(name, mode)
char *name;
char *mode;
{
int fd;

    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0) {
        fd = open(name, O_RDONLY);
    } else if (strcmp(mode, "w") == 0 || strcmp(mode, "wb") == 0) {
        fd = open(name, O_WRONLY);
    } else if (strcmp(mode, "r+") == 0 || strcmp(mode, "rw") == 0) {
        fd = open(name, O_RDWR, 0644);
    } else if (strcmp(mode, "R") == 0 || strcmp(mode, "RB") == 0) {
        fd = open(name, O_RDONLY);
    } else if (strcmp(mode, "W") == 0 || strcmp(mode, "WB") == 0) {
        fd = open(name, O_WRONLY);
    } else if (strcmp(mode, "R+") == 0 || strcmp(mode, "RW") == 0) {
        fd = open(name, O_RDWR, 0644);
    } else {
        fprintf(stderr,"mtopen: illegal mode '%s'\n", mode);
        exit(1);
    }

    return (fd < 0) ? (TAPE) NULL : (TAPE) ++fd;
}

int mtweof(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTWEOF;
    mt_cmnd.mt_count = count;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}

int mtfsf(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTFSF;
    mt_cmnd.mt_count = count;
    if (ioctl(fdm1, MTIOCTOP, &mt_cmnd) == -1) return -1;
    if (ioctl(fdm1, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtbsf(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTBSF;
    mt_cmnd.mt_count = count;
    if (ioctl(fdm1, MTIOCTOP, &mt_cmnd) == -1) return -1;
    if (ioctl(fdm1, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtfsr(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTFSR;
    mt_cmnd.mt_count = count;
    if (ioctl(fdm1, MTIOCTOP, &mt_cmnd) == -1) return -1;
    if (ioctl(fdm1, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtbsr(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTBSR;
    mt_cmnd.mt_count = count;
    if (ioctl(fdm1, MTIOCTOP, &mt_cmnd) == -1) return -1;
    if (ioctl(fdm1, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtrew(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTREW;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}

int mtoffl(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTOFFL;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}

int mtnop(fd)
int fd;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTNOP;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}

#ifdef MTRETEN
int mtreten(fd)
int fd;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTRETEN;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}
#endif /* MTRETEN */

#ifdef MTERASE
int mterase(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTERASE;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}
#endif /* MTERASE */

#ifdef MTEOM
int mteom(fd)
int fd;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTEOM;
    mt_cmnd.mt_count = 1;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}
#endif /* MTEOM */

#ifdef MTNBSF
int mtnbsf(fd, count)
int fd;
daddr_t count;
{
    fdm1 = fd - 1;
    mt_cmnd.mt_op    = MTNBSF;
    mt_cmnd.mt_count = count;
    return ioctl(fdm1, MTIOCTOP, &mt_cmnd);
}
#endif /* MTNBSF */

struct mtget *mtstatus(fd)
int fd;
{
    fdm1 = fd - 1;
    return 
    ioctl(fdm1, MTIOCGET, (char *)&mt_stat) == -1 ? NULL : &mt_stat;
}

#else /* ie, ifdef SCO_SV */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/tape.h>
#include <ctype.h>
#include "oldmtio.h"

static struct tape_info mt_stat;

TAPE mtopen(name, mode)
char *name;
char *mode;
{
int fd;

    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0) {
        fd = open(name, O_RDONLY);
    } else if (strcmp(mode, "w") == 0 || strcmp(mode, "wb") == 0) {
        fd = open(name, O_WRONLY);
    } else if (strcmp(mode, "r+") == 0 || strcmp(mode, "rw") == 0) {
        fd = open(name, O_RDWR, 0644);
    } else if (strcmp(mode, "R") == 0 || strcmp(mode, "RB") == 0) {
        fd = open(name, O_RDONLY);
    } else if (strcmp(mode, "W") == 0 || strcmp(mode, "WB") == 0) {
        fd = open(name, O_WRONLY);
    } else if (strcmp(mode, "R+") == 0 || strcmp(mode, "RW") == 0) {
        fd = open(name, O_RDWR, 0644);
    } else {
        fprintf(stderr,"mtopen: illegal mode '%s'\n", mode);
        exit(1);
    }

    return (fd < 0) ? NULL : ++fd;
}

int mtweof(fd, count)
int fd;
daddr_t count;
{
    return ioctl(fd - 1, MT_WFM);
}

int mtfsf(fd, count)
int fd;
daddr_t count;
{
int i, skipped = 0;

    for (i = 0; i < count; i++) {
        if (ioctl(fd - 1, MT_RFM) != 0) {
            return skipped;
        } else {
            ++skipped;
        }
    }
    return skipped;
}

int mtbsf(fd, count)
int fd;
daddr_t count;
{
    return 0;
}

int mtfsr(fd, count)
int fd;
daddr_t count;
{
    return 0;
}

int mtbsr(fd, count)
int fd;
daddr_t count;
{
    return 0;
}

int mtrew(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    return ioctl(fd - 1, MT_REWIND);
}

int mtoffl(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    return ioctl(fd - 1, MT_UNLOAD);
}

int mtnop(fd)
int fd;
{
    return 0;
}

int mtreten(fd)
int fd;
{
    return ioctl(fd - 1, MT_RETEN);
}

int mterase(fd, dummy)
int fd;
int dummy; /* immed flag in MS-DOS/ASC-88 version */
{
    return ioctl(fd - 1, MT_ERASE);
}

int mteom(fd)
int fd;
{
    return ioctl(fd - 1, MT_EOD);
}

struct mtget *mtstatus(fd)
int fd;
{
    ioctl(fd - 1, MT_STATUS, (char *)&mt_stat) == -1 ? NULL : &mt_stat;
}

#endif /* SCO_SV */

/* Revision History
 *
 * $Log: mtunx.c,v $
 * Revision 1.3  2003/12/10 06:03:36  dechavez
 * added various includes to calm solaris cc
 *
 * Revision 1.2  2001/07/25 05:41:06  dec
 * comment tokens after #endif's
 *
 * Revision 1.1.1.1  2000/02/08 20:20:36  dec
 * import existing IDA/NRTS sources
 *
 */
