#pragma ident "$Id: rdio.c,v 1.2 2003/12/10 06:13:17 dechavez Exp $"
/*======================================================================
 *
 *  Selected Solaris disk I/O operations.
 *
 *  Function       Description
 *  --------       -----------------------------------------------
 *  rdio_close     detach
 *  rdio_eject     eject the disk pack
 *  rdio_nrec      determine number of records currently written
 *  rdio_offline   take device offline
 *  rdio_open      attach to the device
 *  rdio_read      read
 *  rdio_state     determine drive state
 *  rdio_status    status report
 *  rdio_write     write
 *
 *====================================================================*/
#ifdef SOLARIS
#include "platform.h"
#include "rdio.h"

static RDISK *null = (RDISK *) NULL;

static int _rdio_putnumrec(rd)
RDISK *rd;
{
long ltmp, numrec;
static void *buf[DEV_BSIZE];
off_t pos;
static char *fid = "_rdio_putnumrec";

/* Save our current position */

    if ((pos = rdio_tell(rd)) == -1) return -1;
    numrec = pos / DEV_BSIZE;

/* Record counter is stored in network byte order */

    ltmp = htonl(numrec);

/* Store the record counter */

    if (lseek(rd->fd, RDIO_COFF, SEEK_SET) == -1) return -1;
    memcpy(buf, &ltmp, sizeof(unsigned long));
    if ((write(rd->fd, buf, DEV_BSIZE)) != DEV_BSIZE) return -1;

/* Return to our original position */

    if (rdio_seek(rd, pos, SEEK_SET) == -1) return -1;

    return 0;
}

static int _rdio_open(RDISK *rd)
{
long nblk;
struct dk_geom geom;
static char *fid = "_rdio_open";

    rd->fd    = open(rd->path, rd->mode);
    rd->state = (rd->fd == -1) ? RDIO_OFFLINE : RDIO_ONLINE;
if (rd->state == RDIO_OFFLINE) perror("open");
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

long rdio_nrec(rd)
RDISK *rd;
{
static void *buf[DEV_BSIZE];
long ltmp;
off_t pos;

    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) return 0;
    }

/* Save our current position */

    if ((pos = rdio_tell(rd)) == -1) return -1;

/* Read the record counter */

    if (lseek(rd->fd, RDIO_COFF, SEEK_SET) == -1) return -1;
    if ((read(rd->fd, buf, DEV_BSIZE)) != DEV_BSIZE) return -1;

/* Return to our original position */

    if (rdio_seek(rd, pos, SEEK_SET) == -1) return -1;

/* Record counter is stored in network byte order */

    memcpy(&ltmp, buf, sizeof(long));
    return ntohl(ltmp);
}

void rdio_offline(rd)
RDISK *rd;
{
    if (rd == null || rd->state == RDIO_OFFLINE) return;

    rd->state = RDIO_OFFLINE;
    if (rd->fd > 0) close(rd->fd);
    rd->fd = -1;
}

int rdio_eject(rd)
RDISK *rd;
{
    if (rd == null) {
        errno = EINVAL;
        return -1;
    }

    if (rd->state == RDIO_OFFLINE) {
        if (_rdio_open(rd) != 0) return -1;
        if (rd->state == RDIO_OFFLINE) return 0;
    }

    if (ioctl(rd->fd, DKIOCEJECT) == -1) return -1;

    rdio_offline(rd);

    return 0;
}

RDISK *rdio_open(path, perm)
char *path;
char *perm;
{
int fd, mode, truncate, seekeof;
struct stat buf;
RDISK *rd;

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
        seekeof  = 0;

    } else if (
        strcasecmp(perm, "w"  ) == 0 ||
        strcasecmp(perm, "wb" ) == 0 ||
        strcasecmp(perm, "w+" ) == 0 ||
        strcasecmp(perm, "w+b") == 0 ||
        strcasecmp(perm, "wb+") == 0
    ) {
        mode     = O_RDWR;
        truncate = 1;
        seekeof  = 0;

    } else if (
        strcasecmp(perm, "a"  ) == 0 ||
        strcasecmp(perm, "ab" ) == 0 ||
        strcasecmp(perm, "a+" ) == 0 ||
        strcasecmp(perm, "a+b") == 0 ||
        strcasecmp(perm, "ab+") == 0
    ) {
        mode     = O_RDWR;
        truncate = 0;
        seekeof  = 1;

    } else {
        errno = EINVAL;
        return null;
    }

    mode |= O_SYNC | O_DSYNC | O_RSYNC;

    if ((rd = (RDISK *) malloc(sizeof(RDISK))) == null) return null;
    if ((rd->path = (char *) strdup(path)) == (char *) NULL) return null;

    rd->bsize    = DEV_BSIZE;
    rd->mode     = mode;
    rd->truncate = truncate;
    rd->seekeof  = seekeof;

    return _rdio_open(rd) ? null : rd;
}

void rdio_close(rd)
RDISK *rd;
{
    if (rd == null) return;
    if (rd->path != (char *) NULL) free(rd->path);
    free(rd);
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
#ifdef i86pc
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

#ifdef i86pc
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
 * $Log: rdio.c,v $
 * Revision 1.2  2003/12/10 06:13:17  dechavez
 * casts and other such things to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:37  dec
 * import existing IDA/NRTS sources
 *
 */
