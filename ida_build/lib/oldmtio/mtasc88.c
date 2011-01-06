#pragma ident "$Id: mtasc88.c,v 1.1.1.1 2000/02/08 20:20:36 dec Exp $"
/*======================================================================
 *
 *  MS-DOS/ASC-88 mag tape functions.
 *
 *  User level functions:
 *
 *  Function  Description
 *  --------  -----------------------------------------------
 *  mtopen    attach to device
 *  mtclose   detach from device
 *  mtweof    write file mark(s)
 *  mteom     space to end of data
 *  mtfsf     forward space file marks
 *  mtfsr     forward space records
 *  mtrew     rewind
 *  mtoffl    rewind and take the drive offline
 *  mtpos     report approximate tape position
 *  mterase   erase the entire tape and rewind
 *  mterror   error reporting, perror style
 *
 *  mtfsf(), and mtfsr() return the number of files or records skipped. 
 *  mtopen() returns NULL on error, otherwise a SCSI * (typedef'd to
 *  TAPE in mtio).  All other functions return the 0 on success, -1 on
 *  error.
 *
 *  Low level functions:
 *
 *  Function      Description
 *  --------      -----------------------------------------------
 *  asc88tstrdy   test unit ready
 *  asc88reqsen   request sense
 *  asc88modsen   mode sense
 *  asc88modsel   mode select
 *  asc88varread  variable block mode read
 *  asc88varwrite variable block mode write
 *  asc88wrteof   write filemarks
 *  asc88offl     unload tape
 *  asc88rew      rewind tape
 *  asc88space    space
 *  asc88erase    erase
 *
 *  NOTES: 
 *  1) Only buffered variable block mode is available at the user level.
 *  2) ASC88 specific return codes to flag DMA over/under runs
 *     are ignored and converted to SCSI_GOOD or SCSI_CHECK codes.
 *  3) The user must globally enable disconnect via the ASC_MODE
 *     utility prior to running any ASC88 SCSI-PRO code.
 *
 *====================================================================*/
#ifdef MSDOS /* MS-DOS specific code */
#include <dos.h>
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include "mtio.h"

#define   rew_dev "/dev/rst"
#define norew_dev "/dev/nrst"

#define NUMLUN 8

static SCSI rew_handle[NUMLUN] = {
    { 0, 0, 1 },
    { 1, 0, 1 },
    { 2, 0, 1 },
    { 3, 0, 1 },
    { 4, 0, 1 },
    { 5, 0, 1 },
    { 6, 0, 1 },
    { 7, 0, 1 }
};

static SCSI norew_handle[NUMLUN] = {
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 2, 0, 0 },
    { 3, 0, 0 },
    { 4, 0, 0 },
    { 5, 0, 0 },
    { 6, 0, 0 },
    { 7, 0, 0 }
};

extern int _doserrno;

/* Locally global variables */

#define MAX_LOCAL_BUFSIZ 22
#define SENSE_BUFSIZ     22
static unsigned char localbuffer[MAX_LOCAL_BUFSIZ];
static unsigned char far sensebuffer[SENSE_BUFSIZ];
static struct asc88_jcb cmd;
static char far *cmd_address;
static union REGS   inregs, outregs;
static struct SREGS sregs;
static int sense_key, sense_data, sense_set = 0;
static long residue = 0;
static long reqlen, phylen;
static int asc88errno = 0;

/* Function asc88tstrdy() - test unit ready */

int asc88tstrdy(target, lun)
int target;
int lun;
{

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_TSTRDY;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88tstrdy: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88reqsen() - request sense */

int asc88reqsen(target, lun)
int target;
int lun;
{

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0x13;
    cmd.data_addr    = sensebuffer;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_REQSENSE;
    cmd.cdb[4]       = 0x13;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88reqsen: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    sense_set  = 1;

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno = cmd.status) == SCSI_CHECK) {
        fprintf(stderr,"asc88/asc88reqsen: check condition\n");
        exit(1);
    }

    sense_key  = sensebuffer[2] & 0x0f;
    sense_data = (sensebuffer[12] << 8) | sensebuffer[13];

    return cmd.status;
}

/* Function asc88modsen() - mode sense (page code 0x00 only) */

int asc88modsen(target, lun, buffer)
int target;
int lun;
unsigned char far *buffer;
{

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0x0c;
    cmd.data_addr    = (unsigned char far *) buffer;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_MODSENSE;
    cmd.cdb[4]       = 0x0c;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88modsen: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88modsel() - mode select (buffered mode, user size) */

int asc88modsel(target, lun, size, buffer)
int target;
int lun;
long size;
unsigned char far *buffer;
{
int status;

    sense_set = 0;

    if (size < 0) return asc88errno = ASC88_BADARG;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_OUT;
    cmd.data_len     = 0x0c;
    cmd.data_addr    = (unsigned char far *) buffer;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_MODSEL;
    cmd.cdb[4]       = 0x0c;

    memset(buffer, 0, cmd.data_len);
    buffer[ 2] = 0x10;
    buffer[ 3] = 0x08;
    buffer[ 9] = (size & 0x00ff0000) >> 16;
    buffer[10] = (size & 0x0000ff00) >>  8;
    buffer[11] = (size & 0x000000ff);

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es   = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88modsel: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88varread() - variable block read */

int asc88varread(target, lun, want, buffer)
int target;
int lun;
long want;
unsigned char far *buffer;
{

    if (want < 0) return asc88errno = ASC88_BADARG;

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len_msb = (want & 0x00ff0000) >> 16;
    cmd.data_len     = (want & 0x0000ffff);
    cmd.data_addr    = buffer;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_READ;
    cmd.cdb[1]       = 0x00;
    cmd.cdb[2]       = (want & 0x00ff0000) >> 16;
    cmd.cdb[3]       = (want & 0x0000ff00) >>  8;
    cmd.cdb[4]       = (want & 0x000000ff);

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88varread: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    residue = 0L;
    if ((asc88errno = cmd.status) == SCSI_CHECK) {
        asc88reqsen(target, lun);
        if (sensebuffer[2] & 0xc0 || sense_key == SCSI_BLANK) {
            residue = want;           /* to flag eof, eod, etc. */
        } else if (sensebuffer[2] & 0x20) { /* ili */
            asc88errno = ASC88_ILI;
            residue |= (sensebuffer[3] << 8) | sensebuffer[4];
            residue <<= 16;
            residue |= (sensebuffer[5] << 8) | sensebuffer[6];
        } else {
            asc88errno = cmd.status;
        }
    }

    return asc88errno;
}

/* Function asc88varwrite() - variable block write */

int asc88varwrite(target, lun, want, buffer)
int target;
int lun;
long want;
unsigned char far *buffer;
{

    if (want < 0) return asc88errno = ASC88_BADARG;

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_OUT;
    cmd.data_len_msb = (want & 0x00ff0000) >> 16;
    cmd.data_len     = (want & 0x0000ffff);
    cmd.data_addr    = buffer;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_WRITE;
    cmd.cdb[1]       = 0x00;
    cmd.cdb[2]       = (want & 0x00ff0000) >> 16;
    cmd.cdb[3]       = (want & 0x0000ff00) >>  8;
    cmd.cdb[4]       = (want & 0x000000ff);

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88varwrite: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target, lun);

    return cmd.status;
}

/* Function asc88wrteof() - write filemarks */

int asc88wrteof(target, lun, count)
int target;
int lun;
long count;
{

    if (count < 0) return asc88errno = ASC88_BADARG;

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_OUT;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_WRTEOF;
    cmd.cdb[1]       = 0x00;
    cmd.cdb[2]       = (count & 0x00ff0000) >> 16;
    cmd.cdb[3]       = (count & 0x0000ff00) >>  8;
    cmd.cdb[4]       = (count & 0x000000ff);

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88wrteof: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88offl() - unload tape */

int asc88offl(target, lun, immed)
int target;
int lun;
int immed;
{

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_LOAD;
    cmd.cdb[1]       = immed ? 0x01 : 0x00;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88offl: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88rew() - rewind tape */

int asc88rew(target, lun, immed)
int target;
int lun;
int immed;
{
int i;

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_REWIND;
    cmd.cdb[1]       = immed ? 0x01 : 0x00;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88rew: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88erase() - erase tape */

int asc88erase(target, lun, immed)
int target;
int lun;
int immed;
{
int i;

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_ERASE;
    cmd.cdb[1]       = immed ? 0x03 : 0x01;

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88erase: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    if ((asc88errno=cmd.status) == SCSI_CHECK) asc88reqsen(target,lun);

    return cmd.status;
}

/* Function asc88space() - space */

int asc88space(target, lun, code, count)
int target;
int lun;
unsigned char code;
long count;
{

    sense_set = 0;

    cmd.target_id    = target;
    cmd.lun_no       = lun;
    cmd.arb          = ASC88_NO;
    cmd.incl_init_id = ASC88_NO;
    cmd.attention    = ASC88_NO;
    cmd.disconnect   = ASC88_NO;
    cmd.data_dir     = ASC88_IN;
    cmd.data_len     = 0;
    cmd.cdb_len      = 6;
    memset(cmd.cdb, 0, 12);
    cmd.cdb[0]       = SCSI_SPACE;
    cmd.cdb[1]       = code;
    cmd.cdb[2]       = (count & 0x00ff0000) >> 16;
    cmd.cdb[3]       = (count & 0x0000ff00) >>  8;
    cmd.cdb[4]       = (count & 0x000000ff);

    cmd_address = (char far *) &cmd;
    inregs.x.bx = FP_OFF(cmd_address);
    sregs.es    = FP_SEG(cmd_address);

    inregs.x.ax = 0x9900;
    int86x(0x40, &inregs, &outregs, &sregs);

    if (outregs.x.cflag != 0 && _doserrno != 0) {
        fprintf(stderr, "asc88/asc88space: int86x failed: ");
        fprintf(stderr, "cflag = %04X ", outregs.x.cflag);
        fprintf(stderr, "_doserrno = %04X\n", _doserrno);
        exit(1);
    }

    if (cmd.status == ASC88_CHK_DMA_UR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_CHK_DMA_OR) {
        cmd.status = SCSI_CHECK;
    } else if (cmd.status == ASC88_NOR_DMA_UR) {
        cmd.status = SCSI_GOOD;
    } else if (cmd.status == ASC88_NOR_DMA_OR) {
        cmd.status = SCSI_GOOD;
    }

    residue = 0L;
    if ((asc88errno = cmd.status) == SCSI_CHECK) {
        asc88reqsen(target, lun);
        if (sense_key == SCSI_NOSENSE || sense_key == SCSI_BLANK) {
            residue |= (sensebuffer[3] << 8) | sensebuffer[4];
            residue <<= 16;
            residue |= (sensebuffer[5] << 8) | sensebuffer[6];
        }
    }

    return asc88errno;
}

/********************* BEGIN USER LEVEL FUNCTIONS *********************/

/*  Function mtopen() - attach to tape, and set variable block mode  */

TAPE mtopen(devspec, dummy)
char *devspec;
char *dummy;
{
int target, lun;
TAPE tp;
int badspec, rew_flag;

    if (strncmp(devspec, rew_dev, strlen(rew_dev)) == 0) {
        rew_flag = 1;
        if (strlen(devspec) != strlen(rew_dev) + 2) {
            badspec = 1;
        } else {
            sscanf(devspec+strlen(rew_dev), "%1d%1d", &lun, &target);
            if (lun != 0 || target < 0 || target >= NUMLUN) {
                badspec = 1;
            } else {
                tp = rew_handle + target;
                badspec = 0;
            }
        }
    } else if (strncmp(devspec, norew_dev, strlen(norew_dev)) == 0) {
        rew_flag = 0;
        if (strlen(devspec) != strlen(norew_dev) + 2) {
            badspec = 1;
        } else {
            sscanf(devspec+strlen(norew_dev), "%1d%1d", &lun, &target);
            if (lun != 0 || target < 0 || target >= NUMLUN) {
                badspec = 1;
            } else {
                tp = norew_handle + target;
                badspec = 0;
            }
        }
    } else {
        badspec = 1;
    }

    if (badspec) {
        fprintf(stderr,"%s: no such device\n", devspec);
        exit(1);
    }

    if (asc88tstrdy(tp->target, tp->lun) != SCSI_GOOD) return NULL;
    
    if (
        asc88modsel(tp->target, tp->lun, 0L, localbuffer)
        != SCSI_GOOD) {
        return NULL;
    }

    return tp;
}

/*  Function mtclose() - detach from tape  */

void mtclose(tp)
TAPE tp;
{
    if (tp != NULL) {
        
        asc88wrteof(tp->target, tp->lun, 0L); /* flush output */
        if (tp->rew) mtrew(tp, 0L);
    }
}

/* Function mtread() - read from tape */

long mtread(tp, buffer, want)
TAPE tp;
unsigned char far *buffer;
long want;
{
long retval;

    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return (long) SCSI_GENERR;
    }

    asc88varread(tp->target, tp->lun, want, buffer);

/*  If all OK, return the number of bytes read  */

    if (asc88errno == SCSI_GOOD) {

        retval = want - residue;

/*  Check for illegal length indicator  */

    } else if (asc88errno == ASC88_ILI) {

        reqlen = want;
        phylen = reqlen - residue;

    /*  It's OK to ask for more than the physical length  */

        if (residue >= 0) {

            retval = want - residue;

    /*  But it is an error to ask for less  */

        } else {
            sense_set = 0;
            retval = (long) (asc88errno = SCSI_SMLREQ);
        }

/*  Some other error must have occured  */

    } else {
        retval = (long) (asc88errno = SCSI_GENERR);
    }

/*  Return number of bytes read or error code  */

    return retval;
}

/* Function mtwrite() - write to tape */

long mtwrite(tp, buffer, count)
TAPE tp;
unsigned char far *buffer;
long count;
{
    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return 0;
    }

    asc88varwrite(tp->target, tp->lun, count, buffer);

    return (asc88errno == SCSI_GOOD) ? count : (long) SCSI_GENERR;
}

/* Function mtrew() - rewind tape */

int mtrew(tp, immed)
TAPE tp;
int immed;
{

    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return 0;
    }

    return asc88rew(tp->target, tp->lun, immed);
}

/* Function mterase() - erase tape */

int mterase(tp, immed)
TAPE tp;
int immed;
{

    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return 0;
    }

    return asc88erase(tp->target, tp->lun, immed);
}

/* Function mteom() - space to end of data */

int mteom(tp)
TAPE tp;
{
int retval;

    if (tp == NULL) return asc88errno = ASC88_BADARG;

    asc88space(tp->target, tp->lun, SCSI_SPACE_EOD, 1L);

/*  Return the number of items spaced or error flag  */

    if (asc88errno == SCSI_GOOD) {
        retval = 1;
    } else {
        retval = (asc88errno = SCSI_GENERR);
    }

    return retval;
}

/* Function mtfsf() - forward space file marks */

int mtfsf(tp, count)
TAPE tp;
int count;
{
int  retval;
long want;

    want = (long) count;

    if (tp == NULL || want < 0) return asc88errno = ASC88_BADARG;

    asc88space(tp->target, tp->lun, SCSI_SPACE_EOF, want);

/*  Return the number of items spaced or error flag  */

    if (asc88errno == SCSI_GOOD) {
        retval = (int) (want - residue);
    } else {
        retval = asc88errno = SCSI_GENERR;
    }

    return retval;
}

/* Function mtfsr() - forward space records */

long mtfsr(tp, count)
TAPE tp;
long count;
{
long retval;
long want;

    want = (long) count;

    if (tp == NULL || want < 0) return asc88errno = ASC88_BADARG;

    asc88space(tp->target, tp->lun, SCSI_SPACE_BLOCKS, want);

/*  Return the number of items spaced or error flag  */

    if (asc88errno == SCSI_GOOD) {
        retval = (long) (want - residue);
    } else {
        retval = (long) (asc88errno = SCSI_GENERR);
    }

    return retval;
}

/* Function mtweof() - write file marks */

int mtweof(tp, count)
TAPE tp;
int count;
{
    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return 0;
    }

    return asc88wrteof(tp->target, tp->lun, (long) count);
}

/* Function mtoffl() - unload tape */

int mtoffl(tp, immed)
TAPE tp;
int immed;
{

    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return 0;
    }

    return asc88offl(tp->target, tp->lun, immed);
}

/* Function mtpos() - report approximate tape position */

int mtpos(tp)
TAPE tp;
{

    if (tp == NULL) {
        asc88errno = ASC88_BADARG;
        return SCSI_GENERR;
    }

    asc88reqsen(tp->target, tp->lun);

    if (sense_key == SCSI_NOSENSE) {
        if (sense_data == 0x0001) {
            return SCSI_AT_EOF;
        } else if (sense_data == 0x0002) {
            return SCSI_AT_EOM;
        } else if (sense_data == 0x0003) {
            return SCSI_AT_SET;
        } else if (sense_data == 0x0004) {
            return SCSI_AT_BOM;
        } else if (sense_data == 0x0005) {
            return SCSI_AT_EOD;
        } else {
            return SCSI_AT_MID;
        }
    }
}

/* Function mterror() - perror() sytle error reporting */

void mterror(string)
char *string;
{
int i;
int check_more = 0;
int dump_sense = 1;

    if (string[0] != 0) fprintf(stderr, "%s: ", string);

/*  If haven't done a request sense, error is SCSI status code  */

    if (!sense_set) {
        switch (asc88errno) {
          case SCSI_GOOD:
            fprintf(stderr,"good\n"); 
            return;
          case SCSI_CHECK:
            fprintf(stderr,"check condition\n"); 
            return;
          case SCSI_MET:
            fprintf(stderr,"condition met/good\n"); 
            return;
          case SCSI_BUSY:
            fprintf(stderr,"busy\n"); 
            return;
          case SCSI_INTGOOD:
            fprintf(stderr,"intermediate/good\n"); 
            return;
          case SCSI_INTMET:
            fprintf(stderr,"intermediate/condition met/good\n"); 
            return;
          case SCSI_CONFLICT:
            fprintf(stderr,"reservation conflict\n"); 
            return;
          case SCSI_SMLREQ:
            fprintf(stderr,"requested buffer smaller than physical ");
            fprintf(stderr,"record length\n");
            fprintf(stderr,"Requested length = %ld bytes\n", reqlen);
            fprintf(stderr,"Physical  length = %ld bytes\n", phylen);
            return;
          case ASC88_BUSY:
            fprintf(stderr,"warning - busy error\n"); 
            return;
          case ASC88_DISCON:
            fprintf(stderr,"target disconnected\n"); 
            return;
          case ASC88_IDERR:
            fprintf(stderr,"target id same as host\n"); 
            return;
          case ASC88_RECONERR:
            fprintf(stderr,"reconnect not followed by message-in\n");
            return;
          case ASC88_NOR_DMA_OR:
            fprintf(stderr,"DMA over run\n");
            return;
          case ASC88_NOR_DMA_UR:
            fprintf(stderr,"DMA under run\n");
            return;
          case ASC88_CHK_DMA_OR:
            fprintf(stderr,"DMA over run (check condition)\n");
            return;
          case ASC88_CHK_DMA_UR:
            fprintf(stderr," DMA under run (check condition)\n");
            return;
          case ASC88_ARBTO:
            fprintf(stderr,"arbitration timeout\n"); 
            return;
          case ASC88_SELTO:
            fprintf(stderr,"select timeout\n"); 
            return;
          case ASC88_BADARG:
            fprintf(stderr,"illegal argument\n"); 
            return;
          case ASC88_UNKNOWN:
            fprintf(stderr,"unknown error\n"); 
            return;
          default:
            fprintf(stderr,"error 0x%02X\n", asc88errno); return;
        }
    }

/* Interpret request sense data */

    switch (sense_key) {

      case SCSI_NOSENSE:
        dump_sense = 0;
        fprintf(stderr, "no sense: ");
        switch (sense_data) {
          case 0x0000:
            fprintf(stderr,"successful command completion\n"); break;
          case 0x0001:
            fprintf(stderr,"filemark detected\n"); break;
          case 0x0002:
            fprintf(stderr,"end of medium detected\n"); break;
          case 0x0003:
            fprintf(stderr,"save-set mark detected\n"); break;
          case 0x0004:
            fprintf(stderr,"beginning of medium detected\n"); break;
          case 0x0005:
            fprintf(stderr,"end-of-data detected\n"); break;
          case 0x0a00:
            fprintf(stderr,"error rate warning\n"); break;
          case 0x8100:
            fprintf(stderr,"humidity warning\n"); break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;
        
      case SCSI_RECOVERED:
        dump_sense = 1;
        fprintf(stderr, "recovered error: ");
        switch (sense_data) {
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;
   
      case SCSI_NOTRDY:
        dump_sense = 0;
        fprintf(stderr, "drive not ready: ");
        switch (sense_data) {
          case 0x0400:
            fprintf(stderr,"off-line (tape present)\n"); break;
          case 0x0401:
            fprintf(stderr,"tape being loaded\n"); break;
          case 0x3a00:
            fprintf(stderr,"no tape present\n"); break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;
    
      case SCSI_MEDERR:
        dump_sense = 1;
        fprintf(stderr, "medium error: ");
        switch (sense_data) {
          case 0x0002:
            fprintf(stderr,"physical end-of-medium encountered\n");
            break;
          case 0x0c00:
          case 0x1100:
            fprintf(stderr,"flawed media\n");
            break;
          case 0x3000:
            fprintf(stderr,"incompatable medium installed\n");
            break;
          case 0x3001:
            fprintf(stderr,"unknown format (audio) encountered\n");
            break;
          case 0x3b00:
            fprintf(stderr,"positioning error\n");
            break;
          case 0x5000:
            fprintf(stderr,"append error\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;
    
      case SCSI_HARDERR:
        dump_sense = 1;
        fprintf(stderr, "hardware error: ");
        switch (sense_data) {
          case 0x0300:
            fprintf(stderr,"failure during write\n");
            break;
          case 0x0900:
            fprintf(stderr,"track following error\n");
            break;
          case 0x4400:
            fprintf(stderr,"unexpected internal error condition\n");
            break;
          case 0x5300:
            fprintf(stderr,"media load/eject failed\n");
            break;
          case 0x8280:
            fprintf(stderr,"moisture detected\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        fprintf(stderr,"FRU code = 0x%02X\n", sensebuffer[14]);
        break;
    
      case SCSI_ILLREQ:
        dump_sense = 1;
        fprintf(stderr, "illegal request: ");
        switch (sense_data) {
          case 0x1a00:
            fprintf(stderr,"parameter length error\n");
            break;
          case 0x2000:
            fprintf(stderr,"invalid command operation code\n");
            break;
          case 0x2400:
            fprintf(stderr,"invalid command descriptor block\n");
            check_more = 1;
            break;
          case 0x2500:
            fprintf(stderr,"logical unit not supported\n");
            break;
          case 0x2600:
            fprintf(stderr,"invalid test number, header, or ");
            fprintf(stderr,"field in parameter list\n");
            break;
          case 0x2601:
            fprintf(stderr,"parameter page not supported\n");
            check_more = 1;
            break;
          case 0x3d00:
            fprintf(stderr,"invalid bits in IDENTIFY message\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        if (check_more) {
            if (sensebuffer[15] & 0x80) {
                fprintf(stderr,"Illegal parameter in ");
                if (sensebuffer[15] & 0x40) {
                    fprintf(stderr,"command descriptor block.\n");
                } else {
                    fprintf(stderr,"parameter list.\n");
                }
                fprintf(stderr,"byte pointer: ");
                fprintf(stderr,"0x%02X", sensebuffer[16]);
                fprintf(stderr,"%02X\n",  sensebuffer[17]);
                if (sensebuffer[15] & 0x08) {
                    fprintf(stderr,"bit pointer:  ");
                    fprintf(stderr,"0x%x\n", sensebuffer[15] & 0x07);
                }
            }
        }
        break;
  
      case SCSI_ATTENTION:
        dump_sense = 0;
        fprintf(stderr, "unit attention: ");
        switch (sense_data) {
          case 0x2800:
            fprintf(stderr,"tape may have been changed\n");
            break;
          case 0x2900:
            fprintf(stderr,"drive reset\n");
            break;
          case 0x2980:
            fprintf(stderr,"self-test failed\n");
            break;
          case 0x2a01:
            fprintf(stderr,"mode parameters may have been changed\n");
            break;
          case 0x2a02:
            fprintf(stderr,"log parameters may have been changed\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;
    
      case SCSI_WRTPROT:
        dump_sense = 0;
        fprintf(stderr, "write protected");
        switch (sense_data) {
          case 0x2700:
            fprintf(stderr," media\n");
            break;
          default: 
            fprintf(stderr,": 0x%04X\n", sense_data);
        }
        break;
 
      case SCSI_BLANK:
        dump_sense = 0;
        fprintf(stderr, "blank check: ");
        switch (sense_data) {
          case 0x0000:
            fprintf(stderr,"blank tape encountered at BOT\n");
            break;
          case 0x0005:
            fprintf(stderr,"EOD encountered on read\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      case SCSI_CPYABORT:
        dump_sense = 0;
        fprintf(stderr, "copy aborted: ");
        switch (sense_data) {
          case 0x0000:
            fprintf(stderr,"error at source or dest device\n");
            break;
          case 0x2b00:
            fprintf(stderr,"host cannot disconnect\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      case SCSI_CMDABORT:
        dump_sense = 0;
        fprintf(stderr, "aborted command: ");
        switch (sense_data) {
          case 0x0000:
            fprintf(stderr,"host sent abort message\n");
            break;
          case 0x2c00:
            fprintf(stderr,"unexpected phase sequence\n");
            break;
          case 0x4300:
            fprintf(stderr,"unexpected message phase\n");
            break;
          case 0x4500:
            fprintf(stderr,"SCSI selection/reselection error\n");
            break;
          case 0x4700:
            fprintf(stderr,"SCSI parity error detected\n");
            break;
          case 0x4800:
            fprintf(stderr,"initiator detected error\n");
            break;
          case 0x4900:
            fprintf(stderr,"illegal message received\n");
            break;
          case 0x4a00:
            fprintf(stderr,"unexpected command phase\n");
            break;
          case 0x4b00:
            fprintf(stderr,"unexpected data phase (DMA error?)\n");
            break;
          case 0x4e00:
            fprintf(stderr,"new command received during ");
            fprintf(stderr,"execution of current command\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      case SCSI_EQUAL:
        dump_sense = 1;
        fprintf(stderr, "equal: ");
        switch (sense_data) {
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      case SCSI_OVRFLO:
        dump_sense = 0;
        fprintf(stderr, "volume overflow: ");
        switch (sense_data) {
          case 0x0002:
            fprintf(stderr,"physical end-of-medium detected ");
            fprintf(stderr,"while writing filemarks\n");
            break;
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      case SCSI_MISCMP:
        dump_sense = 1;
        fprintf(stderr, "mis-compare: ");
        switch (sense_data) {
          default: 
            fprintf(stderr,"0x%04X\n", sense_data);
        }
        break;

      default:
        dump_sense = 1;
        fprintf(stderr, "unrecognized sense key: 0x%02X: ", sense_key);
        fprintf(stderr,"0x%04X\n", sense_data);
        break;
    }

    if (dump_sense) {
        fprintf(stderr,"\n");
        fprintf(stderr,"            ");
        for (i = 0; i < 19; i++) fprintf(stderr,"%2d ", i);
        fprintf(stderr,"\n");
        fprintf(stderr,"Sense data: ");
        for (i = 0; i < 19; i++) fprintf(stderr,"%02X ",sensebuffer[i]);
        fprintf(stderr,"\n");
    }
}

#else

int mtasc88_dummy() {}

#endif /* ifdef MSDOS */

/* Revision History
 *
 * $Log: mtasc88.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:36  dec
 * import existing IDA/NRTS sources
 *
 */
