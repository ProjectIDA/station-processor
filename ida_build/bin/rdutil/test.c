#pragma ident "$Id: test.c,v 1.2 2000/05/01 22:59:02 dec Exp $"
#include <stdio.h>
#include "rdio.h"
#include "util.h"

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

/* Revision History
 *
 * $Log: test.c,v $
 * Revision 1.2  2000/05/01 22:59:02  dec
 * import existing sources
 *
 */
