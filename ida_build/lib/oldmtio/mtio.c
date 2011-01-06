#pragma ident "$Id: mtio.c,v 1.1.1.1 2000/02/08 20:20:36 dec Exp $"
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
 *  mtio_offl    rewind and take the drive offline
 *  mtio_nop     no operation, sets status only
 *  mtio_reten   retension the tape (cartridge tape only)
 *  mtio_erase   erase the entire tape and rewind
 *  mtio_eom     position to EOM
 *  mtio_nbsf    backward space file to beginning of file
 *  mtio_status  get status (device dependent)
 *  mtio_read    read  (defined as macro in mtio.h)
 *  mtio_write   write (defined as macro in mtio.h)
 *  mtio_close   close (defined as macro in mtio.h)
 *
 *  mtio_bsf(), mtio_bsr(), mtio_fsf(), and mtio_fsr() return the number of files
 *  or records skipped.  mtio_open() returns the open() return value + 1,
 *  so that it can return NULL (0) on error and thus be compatible with
 *  the MS-DOS/ASC88 version of these functions.  All these functions
 *  decrement the file descriptor by one to account for this.
 *
 *  mtio_status() returns NULL on error, otherwise a pointer to a static
 *  struct mtget.  All other functions return the ioctl return value,
 *  where 0 indicates success and -1 indicates error.
 *
 *====================================================================*/
#ifdef SOLARIS
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/mtio.h>
#include <ctype.h>
#include "mtio.h"

TAPE mtopen(name, mode)
char *name;
char *mode;
{
TAPE *tp;

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

int mtio_weof(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTWEOF;
    tp->mtop.mt_count = count;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}

int mtio_fsf(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTFSF;
    tp->mtop.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->mtop) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtio_bsf(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTBSF;
    tp->mtop.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->mtop) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtio_fsr(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTFSR;
    tp->mtop.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->mtop) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtio_bsr(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTBSR;
    tp->mtop.mt_count = count;
    if (ioctl(tp->fd, MTIOCTOP, &tp->mtop) == -1) return -1;
    if (ioctl(tp->fd, MTIOCGET, (char *)&mt_stat) == -1) return -1;
    return count - mt_stat.mt_resid;
}

int mtio_rew(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTREW;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}

int mtio_eject(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTOFFL;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}

int mtio_nop(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTNOP;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}

#ifdef MTRETEN
int mtio_reten(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTRETEN;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}
#endif /* MTRETEN */

#ifdef MTERASE
int mtio_erase(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTERASE;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}
#endif /* MTERASE */

#ifdef MTEOM
int mtio_eom(tp)
TAPE *tp;
{
    tp->mtop.mt_op    = MTEOM;
    tp->mtop.mt_count = 1;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}
#endif MTEOM

#ifdef MTNBSF
int mtio_nbsf(tp, count)
TAPE *tp;
daddr_t count;
{
    tp->mtop.mt_op    = MTNBSF;
    tp->mtop.mt_count = count;
    return ioctl(tp->fd, MTIOCTOP, &tp->mtop);
}
#endif MTNBSF

int mtio_status(tp)
TAPE *tp;
{
    return ioctl(tp->fd, MTIOCGET, (char *)&tp->status);
}

#endif /* SOLARIS */
########

int _mtio_open(tp)
TAPE *tp;
{
long nblk;
struct dk_geom geom;
static char *fid = "_rdio_open";

    tp->fd    = open(tp->path, tp->mode);
    tp->state = (rd->tp == -1) ? RDIO_OFFLINE : RDIO_ONLINE;
    if (rd->state == RDIO_OFFLINE) return 0;

    if (rd->seekeof) {
        if ((nblk = rdio_nrec(rd)) < 0) return -1;
        if (rdio_seek(rd, nblk*DEV_BSIZE, SEEK_SET) == -1) return -1;
    } else {
        if (rdio_seek(rd, 0, SEEK_SET) == -1) return -1;
    }

    if (rd->truncate && _rdio_putnumrec(rd) != 0) return -1;

    if (ioctl(rd->fd, DKIOCGGEOM, &geom) == -1) return -1;
    rd->capacity = geom.dkg_ncyl * geom.dkg_nhead * geom.dkg_nsect;

    return 0;
}

void mtio_offline(tp)
TAPE *tp;
{
    if (tp == null || tp->state == MTIO_OFFLINE) return;

    tp->state = MTIO_OFFLINE;
    if (tp->fd > 0) close(tp->fd);
    tp->fd = -1;
}

int mtio_eject(tp)
TAPE *tp;
{
    if (tp == null) {
        errno = EINVAL;
        return -1;
    }

    if (tp->state == MTIO_OFFLINE) {
        if (_mtio_open(tp) != 0) return -1;
        if (tp->state == MTIO_OFFLINE) return 0;
    }

    tp->mtop.mt_op    = MTOFFL;
    tp->mtop.mt_count = 1;
    if (ioctl(tp->fd, MTIOCTOP, &tp->mtop) == -1) return -1;

    mtio_offline(tp);

    return 0;
}

TAPE *mtio_open(path, perm)
char *path;
char *perm;
{
int fd, mode, truncate, seekeof;
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
    tp->seekeom  = seekeof;

    return _mtio_open(tp) ? null : tp;
}

void mtio_close(tp)
TAPE *tp;
{
    if (tp == null) return;
    if (tp->path != (char *) NULL) free(tp->path);
    close(tp->fd);
    free(tp);
}

size_t rdio_read(rd, buf, nbyte)
RDISK *rd;
void *buf;
size_t nbyte;
{
char *ptr;
size_t got, count;
long i, nblk, maxblk, reqblk, nerr;
static char *fid = "rdio_read";

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

/* Can only deal with I/O in multiples of the device block size */

    if (nbyte % DEV_BSIZE) {
        errno = EINVAL;
        return -1;
    }

/* If currently off line, try to open again */

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) {
            errno = EAGAIN;
            return -1;
        }
    }

/* Determine location of end of data */

    if ((nblk = rdio_nrec(rd)) < 0) return -1;
    maxblk = nblk - (rdio_tell(rd) / DEV_BSIZE);

/* Read requested bytes */

    reqblk = nbyte / DEV_BSIZE;
    if (reqblk > maxblk) reqblk = maxblk;

    ptr   = buf;
    count = 0;
    for (i = 0; i < reqblk; i++) {
        nerr = 0;
        while (read(rd->fd, ptr, DEV_BSIZE) != DEV_BSIZE) {
            if (errno == EAGAIN) {
                rdio_offline(rd);
                return 0;
            } else if (errno != EINTR) {
                if (++nerr > RDIO_MAXTRY) return -1;
            }
        }
        count += DEV_BSIZE;
        ptr   += DEV_BSIZE;
    }

    return count;
}

size_t rdio_write(rd, buf, nbyte)
RDISK *rd;
void *buf;
size_t nbyte;
{
int nerr;
char *ptr;
off_t  pos;
size_t put, count;
static char *fid = "rdio_write";

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

/* Can only deal with I/O in multiples of the device block size */

    if (nbyte % DEV_BSIZE) {
        errno = EINVAL;
        return -1;
    }

/* If currently off line, try to open again */

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) {
            errno = EAGAIN;
            return 0;
        }
    }

    ptr = buf;
    count = 0;
    while (count != nbyte) {
        nerr = 0;
        while (write(rd->fd, ptr, DEV_BSIZE) != DEV_BSIZE) {
            if (errno == EAGAIN) {
                rdio_offline(rd);
                return 0;
            } else if (errno != EINTR) {
                if (++nerr > RDIO_MAXTRY) return -1;
            }
        }
        ptr   += DEV_BSIZE;
        count += DEV_BSIZE;
    }
    if (_rdio_putnumrec(rd) != 0) return -1;

    return count;
}

int rdio_status(rd, info)
RDISK *rd;
struct rdio_info *info;
{

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

/* If currently off line, try to open again */

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) {
            errno = EAGAIN;
            return 0;
        }
    }

    if (ioctl(rd->fd, DKIOCINFO,       &info->ctrl)  == -1) return -1;
    if (ioctl(rd->fd, DKIOCGGEOM,      &info->geom)  == -1) return -2;
#if (MACHINE == i86pc)
    if (ioctl(rd->fd, DKIOCG_PHYGEOM,  &info->pgeom) == -1) return -3;
    if (ioctl(rd->fd, DKIOCG_VIRTGEOM, &info->vgeom) == -1) return -4;
#endif /* i86pc */

    return 0;
}

int rdio_state(rd)
RDISK *rd;
{
struct rdio_info info;

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

    if (rdio_nrec(rd) < 0) rdio_offline(rd);

    return rd->state;
}

int rdio_wait(rd)
RDISK *rd;
{

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

/* If currently off line, try to open again */

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) {
            errno = ENXIO;
            return -1;
        }
    }

    if (ioctl(rd->fd, DKIOCSTATE, &rd->dkstate) == -1) return -1;

    return 0;
}

#ifdef DEBUG_TEST

#include <stdio.h>

void print_ctrl(fp, ctrl)
FILE *fp;
struct dk_cinfo *ctrl;
{
    fprintf(fp, "drive      name   = %s\n", ctrl->dki_dname);

    fprintf(fp, "controller name   = %s\n", ctrl->dki_cname);
    fprintf(fp, "controller type   = %d ", ctrl->dki_ctype);
    switch (ctrl->dki_ctype) {
      case DKC_UNKNOWN:    fprintf(fp, "DKC_UNKNOWN\n");    break;
      case DKC_CDROM:      fprintf(fp, "DKC_CDROM\n");      break;
      case DKC_WDC2880:    fprintf(fp, "DKC_WDC2880\n");    break;
      case DKC_XXX_0:      fprintf(fp, "DKC_XXX_0\n");      break;
      case DKC_XXX_1:      fprintf(fp, "DKC_XXX_1\n");      break;
      case DKC_DSD5215:    fprintf(fp, "DKC_DSD5215\n");    break;
      case DKC_XY450:      fprintf(fp, "DKC_XY450\n");      break;
      case DKC_ACB4000:    fprintf(fp, "DKC_ACB4000\n");    break;
      case DKC_MD21:       fprintf(fp, "DKC_MD21\n");       break;
      case DKC_XXX_2:      fprintf(fp, "DKC_XXX_2\n");      break;
      case DKC_NCRFLOPPY:  fprintf(fp, "DKC_NCRFLOPPY\n");  break;
      case DKC_XD7053:     fprintf(fp, "DKC_XD7053\n");     break;
      case DKC_SMSFLOPPY:  fprintf(fp, "DKC_SMSFLOPPY\n");  break;
      case DKC_SCSI_CCS:   fprintf(fp, "DKC_SCSI_CCS\n");   break;
      case DKC_INTEL82072: fprintf(fp, "DKC_INTEL82072\n"); break;
      case DKC_PANTHER:    fprintf(fp, "DKC_PANTHER\n");    break;
      case DKC_MD:         fprintf(fp, "DKC_MD\n");         break;
      case DKC_CDC_9057:   fprintf(fp, "DKC_CDC_9057\n");   break;
      case DKC_FJ_M1060:   fprintf(fp, "DKC_FJ_M1060\n");   break;
      case DKC_INTEL82077: fprintf(fp, "DKC_INTEL82077\n"); break;
      case DKC_DIRECT:     fprintf(fp, "DKC_DIRECT\n");     break;
      case DKC_PCMCIA_MEM: fprintf(fp, "DKC_PCMCIA_MEM\n"); break;
      case DKC_PCMCIA_ATA: fprintf(fp, "DKC_PCMCIA_ATA\n"); break;
      default: fprintf(fp, "<unknown>\n");
    }

    fprintf(fp, "flags             = x%02x", ctrl->dki_flags);
    if (ctrl->dki_flags) {
        if (ctrl->dki_flags & DKI_BAD144)     fprintf(fp, " BAD144");
        if (ctrl->dki_flags & DKI_MAPTRK)     fprintf(fp, " MAPTRK");
        if (ctrl->dki_flags & DKI_FMTTRK)     fprintf(fp, " FMTTRK");
        if (ctrl->dki_flags & DKI_FMTVOL)     fprintf(fp, " FMTVOL");
        if (ctrl->dki_flags & DKI_FMTCYL)     fprintf(fp, " FMTCYL");
        if (ctrl->dki_flags & DKI_HEXUNIT)    fprintf(fp, " HEXUNIT");
        if (ctrl->dki_flags & DKI_PCMCIA_PFD) fprintf(fp, " PCMCIA_PFD");
        fprintf(fp, "\n");
    }

    fprintf(fp, "controller num    = %d\n", ctrl->dki_cnum);
    fprintf(fp, "controller addr   = 0x%x\n", ctrl->dki_addr);
    fprintf(fp, "controller bus    = 0x%x\n", ctrl->dki_space);
    fprintf(fp, "interrupt priorty = %d\n", ctrl->dki_prio);
    fprintf(fp, "interrupt vector  = %d\n", ctrl->dki_vec);
    fprintf(fp, "unit number       = %d\n", ctrl->dki_unit);
    fprintf(fp, "slave number      = %d\n", ctrl->dki_slave);
    fprintf(fp, "partition number  = %d\n", ctrl->dki_partition);
    fprintf(fp, "max transfer size = %d blocks\n", ctrl->dki_maxtransfer);
    fprintf(fp, "block size        = %d bytes\n", DEV_BSIZE);
}

void print_geom(fp, geom)
FILE *fp;
struct dk_geom *geom;
{
    fprintf(fp,
        "ncyl = %d : acyl = %d : bcyl = %d : nhead = %d : nsect = %d\n",
        geom->dkg_ncyl, geom->dkg_acyl, geom->dkg_bcyl, geom->dkg_nhead,
        geom->dkg_nsect
    );

    fprintf(fp,
        "intrlv = %d : apc = %d : pcyl = %d : wri = %d : rri = %d : rpm = %d\n",
        geom->dkg_intrlv, geom->dkg_apc, geom->dkg_pcyl,
        geom->dkg_write_reinstruct, geom->dkg_read_reinstruct, geom->dkg_rpm
    );
}

#define STATUS 0
#define EJECT  1
#define LOCK   2
#define UNLOCK 3
#define DETECT 4

main(argc, argv)
int argc;
char *argv[];
{
int fd, status, command, save_errno;
struct rdio_info info;
RDISK *rd;

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s raw-disk [command]\n", argv[0]);
        exit(1);
    }

    util_logopen(NULL, 1, 9, 1, NULL, argv[0]);

    if (argc == 3) {
        if (strcasecmp(argv[2], "status") == 0) {
            command = STATUS;
        } else if (strcasecmp(argv[2], "eject") == 0) {
            command = EJECT;
        } else if (strcasecmp(argv[2], "lock") == 0) {
            command = LOCK;
        } else if (strcasecmp(argv[2], "unlock") == 0) {
            command = UNLOCK;
        } else if (strcasecmp(argv[2], "detect") == 0) {
            command = DETECT;
        } else {
            fprintf(stderr, "%s: unknown command `%s'\n", argv[0], argv[2]);
            exit(1);
        }
    } else {
        command = STATUS;
    }

    if ((rd = rdio_open(argv[1], "r")) == (RDISK *) NULL) {
        fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
        perror("rdio_open");
        exit(1);
    }

    if (rd->state == RDIO_OFFLINE) {
        fprintf(stderr, "%s: device off-line\n", argv[1]);
        exit(0);
    }

    if (command == STATUS) {
        if ((status = rdio_status(rd, &info)) != 0) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("rdio_status");
            fprintf(stderr, "rdio_status failed with status %d\n", status);
            exit(1);
        }

        printf("Device:   %s\n", argv[1]);
        printf("State:    ");
        if (info.state == DKIO_EJECTED) {
            printf("ejected\n");
        } else if (info.state == DKIO_INSERTED) {
            printf("inserted\n");
        } else if (info.state == DKIO_NONE) {
            printf("<none>\n");
        } else {
            printf("unknown (%d)\n", info.state);
        }
        printf("Nrec:     %ld\n", rdio_nrec(rd));
        printf("Capacity: %ld\n", rdio_capacity(rd));

        printf("\n");
        printf("Controller information\n");
        printf("----------------------\n");
        print_ctrl(stdout, &info.ctrl);

        printf("\n");
        printf("Controller geomentry\n");
        printf("--------------------\n");
        print_geom(stdout, &info.geom);

#if (MACHINE == i86pc)
        printf("\n");
        printf("Physical geometry\n");
        printf("-----------------\n");
        print_geom(stdout, &info.pgeom);

        printf("\n");
        printf("Virtual geometry\n");
        printf("----------------\n");
        print_geom(stdout, &info.vgeom);
#endif /* i86pc */

    } else if (command == EJECT) {

        if ((status = rdio_eject(rd)) == -1) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("rdio_eject");
            fprintf(stderr, "rdio_eject failed with status %d\n", status);
            exit(1);
        }

    } else if (command == LOCK) {

        if ((status = rdio_lock(rd)) == -1) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("rdio_lock");
            fprintf(stderr, "rdio_lock failed with status %d\n", status);
            exit(1);
        }

    } else if (command == UNLOCK) {

        if ((status = rdio_unlock(rd)) == -1) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("rdio_unlock");
            fprintf(stderr, "rdio_unlock failed with status %d\n", status);
            exit(1);
        }

    } else if (command == DETECT) {

        if ((status = rdio_status(rd, &info)) != 0) {
            fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
            perror("rdio_status");
            fprintf(stderr, "rdio_status failed with status %d\n", status);
            exit(1);
        }

        printf("Device: %s\n", argv[0]);
        while (1) {
            printf("State is ");
            if (info.state == DKIO_EJECTED) {
                printf("ejected\n");
            } else if (info.state == DKIO_INSERTED) {
                printf("inserted\n");
            } else if (info.state == DKIO_NONE) {
                printf("<none>\n");
            } else {
                printf("unknown (%d)\n", info.state);
            }

            printf("waiting for state change..."); fflush(stdout);
            if (rdio_wait(rd) == -1) {
                fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
                perror("rdio_wait");
                exit(1);
            }
            printf("\n");
        }

    } else {

        fprintf(stderr, "%s: program error\n", argv[0]);
        exit(1);

    }

    close(fd);
    exit(0);
}
#endif /* DEBUG_TEST */
#endif /* SOLARIS */

/* Revision History
 *
 * $Log: mtio.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:36  dec
 * import existing IDA/NRTS sources
 *
 */
