#pragma ident "$Id: mio.h,v 1.2 2005/02/10 18:52:56 dechavez Exp $"
/*======================================================================
 *
 *  Defines, typedefs, templates for the mio (My I/O) library.
 *
 *====================================================================*/
#ifndef mio_h_included
#define mio_h_included

#include <stdio.h>
#include "oldmtio.h"

#define MIO_DISK 0x01
#define MIO_TAPE 0x02

#define MIO_RD   0x01
#define MIO_WR   0x02

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef MSDOS
#define CHAR unsigned char far
#else
#define CHAR char
#endif

/*  Structure templates  */

#ifdef WIN32
/*  Structure templates  */
typedef struct scsi_handle {
    int adapter;
    int target_id;
    int lun;
    unsigned char buf[65536];
    int nRead;
    int nQty;
} SCSI;

typedef struct {
    FILE *fp;
    SCSI *tp;
    int  perm;
    int  type;
    int  eof_count;
} MIO;


#else

typedef struct {
    FILE *fp;
    TAPE tp;
    int  perm;
    int  type;
    int  eof_count;
} MIO;
#endif
/*  Function prototypes  */

#ifdef __STDC__

void mioclose(MIO *);
int mioeof(MIO *);
MIO *mioopen(char *, char *);
long mioread(MIO *, CHAR *, long);
long miowrite(MIO *, CHAR *, long);

#else

void mioclose();
int mioeof();
MIO *mioopen();
long mioread();
long miowrite();

#endif /* ifdef __STDC__ */
#endif /* ifdef mio_h_included */

/* Revision History
 *
 * $Log: mio.h,v $
 * Revision 1.2  2005/02/10 18:52:56  dechavez
 * aap win32 portability mods
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
