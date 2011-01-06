#pragma ident "$Id: oldmtio.h,v 1.2 2007/01/04 23:31:20 dechavez Exp $"
/*======================================================================
 *
 *  Typedefs and function prototypes for the old mtio library
 *
 *====================================================================*/
#ifndef mtio_h_included
#define mtio_h_included

#include "platform.h"

#ifdef MSDOS

#include "asc88.h"
#include "scsi.h"

typedef struct scsi_handle {
    int target;
    int lun;
    int rew;
} SCSI;

typedef SCSI * TAPE;

TAPE mtopen(char *, char *);
void mtclose(TAPE);
int  mtweof(TAPE, int);
int  mteom(TAPE);
int  mtfsf(TAPE, int);
long mtfsr(TAPE, long);
int  mtrew(TAPE, int);
int  mtoffl(TAPE, int);
int  mtpos(TAPE);
int  mterase(TAPE, int);
void mterror(char *);

long mtread (TAPE, unsigned char far *, long);
long mtwrite(TAPE, unsigned char far *, long);

#define mttur(tp) asc88tstrdy(tp->target, tp->lun)

#endif /* MSDOS */

#ifdef unix

#ifdef SCO_SV
#include <sys/tape.h>
typedef struct tape_info mtget;
#endif

typedef int TAPE;

#define  mtread(fd, buf, count)  read(fd-1, buf, count)
#define mtwrite(fd, buf, count) write(fd-1, buf, count)
#define mtclose(fd) close(fd-1)
#define mterror perror
#define mttur(tp) 0

#ifdef __STDC__

#include <sys/types.h>

TAPE mtopen  ( char *, char * );
int  mtweof  ( int, daddr_t );
int  mtfsf   ( int, daddr_t );
int  mtbsf   ( int, daddr_t );
int  mtfsr   ( int, daddr_t );
int  mtbsr   ( int, daddr_t );
int  mtrew   ( int, int );
int  mtoffl  ( int, int );
int  mtnop   ( int );
int  mtreten ( int );
int  mterase ( int, int );
int  mteom   ( int );
int  mtnbsf  ( int, daddr_t );
struct mtget *mtstatus( int );

#else

TAPE mtopen();
int  mtweof();
int  mtfsf();
int  mtbsf();
int  mtfsr();
int  mtbsr();
int  mtrew();
int  mtoffl();
int  mtnop();
int  mtreten();
int  mterase();
int  mteom();
int  mtnbsf();
struct mtget *mtstatus();

#endif /* __STDC__  */

#endif /* unix */

#endif /* mtio_h_included */

/* Revision History
 *
 * $Log: oldmtio.h,v $
 * Revision 1.2  2007/01/04 23:31:20  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
