#pragma ident "$Id: rdio.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"
/*======================================================================
 *
 * Include file in support of the rdio, removable disk I/O library.
 *
 *----------------------------------------------------------------------
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef rdio_h_included
#define rdio_h_included

#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/dkio.h>
#include <sys/vtoc.h>

/* Constants */

#define RDIO_DOFF 2048 /* offset to start of data  */
#define RDIO_COFF 1024 /* offset to record counter */
#define RDIO_MAXTRY 10 /* number of times to retry I/O operations */

#define RDIO_UNKNOWN -1
#define RDIO_OFFLINE  0
#define RDIO_ONLINE   1

/* Macros */

#ifdef sun4
#define rdio_lock(rd)   ioctl(rd->fd, DKIOCLOCK)
#define rdio_unlock(rd) ioctl(rd->fd, DKIOCUNLOCK)
#else
#define rdio_lock(rd)   
#define rdio_unlock(rd) 
#endif /* sun4 */

#define rdio_seek(rd, offset, whence) lseek(rd->fd,offset+RDIO_DOFF,whence)
#define rdio_tell(rd) (lseek(rd->fd, 0, SEEK_CUR) - RDIO_DOFF)
#define rdio_rew(rd) (lseek(rd->fd, RDIO_DOFF, SEEK_SET))
#define rdio_capacity(rd) (rd->capacity)
#define rdio_bsize(rd) (rd->bsize)

/* Structure templates */

struct rdio_info {
    struct dk_cinfo ctrl;
    struct dk_geom  geom;
#ifdef i86pc
    struct dk_geom pgeom;
    struct dk_geom vgeom;
#endif /* i86pc */
    enum dkio_state state;
};

struct rdio_disk {
    int fd;
    int mode;
    int truncate;
    int seekeof;
    int state;
    int dkstate;
    char *path;
    long capacity;
    long bsize;
};

typedef struct rdio_disk RDISK;

/* Function prototypes */

void rdio_close(
    RDISK *
);

int rdio_eject(
    RDISK *
);

int rdio_init(
    char *
);

RDISK *rdio_open(
    char *,
    char *
);

void rdio_offline(
    RDISK *
);

long rdio_nrec(
    RDISK *
);

size_t rdio_read(
    RDISK *,
    void *,
    size_t
);

int rdio_state(
    RDISK *
);

size_t rdio_write(
    RDISK *,
    void *,
    size_t
);

#endif /* rdio_h_included */

/* Revision History
 *
 * $Log: rdio.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
