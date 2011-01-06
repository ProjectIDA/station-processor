#pragma ident "$Id: mtio.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"
/*======================================================================
 *
 *  Typedefs and function prototypes for mtio library routines.
 *
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Author: David Chavez (dec@essw.com)
 *         Engineering Services & Software
 *         San Diego, CA, USA
 *====================================================================*/
#ifndef mtio_h_included
#define mtio_h_included

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/mtio.h>

#define MTIO_UNKNOWN -1
#define MTIO_OFFLINE  0
#define MTIO_ONLINE   1

/* Structure templates */

struct mtio_tape {
    int fd;
    int mode;
    int truncate;
    int seekeom;
    int state;
    char *path;
    struct mtop  cmnd;
    struct mtget stat;
};

typedef struct mtio_tape TAPE;

/* Function prototypes */

int mtio_weof(
    TAPE *,
    int count
);

int mtio_fsf(
    TAPE *,
    int count
);

int mtio_bsf(
    TAPE *,
    int count
);

int mtio_fsr(
    TAPE *,
    int count
);

int mtio_bsr(
    TAPE *,
    int count
);

int mtio_rew(
    TAPE *
);

int mtio_nop(
    TAPE *
);

int mtio_reten(
    TAPE *
);

int mtio_erase(
    TAPE *
);

int mtio_eom(
    TAPE *
);

int mtio_nbsf(
    TAPE *,
    int count
);

void mtio_offline(
    TAPE *
);

int mtio_eject(
    TAPE *
);

TAPE *mtio_open(
    char *,
    char *
);

void mtio_close(
    TAPE *
);

size_t mtio_read(
    TAPE *,
    char *,
    size_t
);

size_t mtio_write(
    TAPE *,
    char *,
    size_t
);

int mtio_state(
    TAPE *
);

int mtio_status(
    TAPE *,
    struct mtget *
);

#endif /* mtio_h_included */

/* Revision History
 *
 * $Log: mtio.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
