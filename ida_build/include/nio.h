#pragma ident "$Id: nio.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"
/*======================================================================
 *
 *  Include file of defines, type definitions and function declarations
 *  for the NIO library C routines.
 *
 *====================================================================*/
#ifndef nio_h_included
#define nio_h_included

/*  Includes  */

#include <stdio.h>

/*  Constants  */

enum {NIO_READ, NIO_WRITE, NIO_CREATE, NIO_SHORT, NIO_LONG, NIO_FLOAT, 
      NIO_SAC,  NIO_B1CZ};

/*  Function prototypes  */

#ifdef __STDC__

long  cintio ( long *, long, long, long, long, long);
long  crealio(float *, long, long, long, long, long);
long  cshrtio(short *, long, long, long, long, long);
long  clongis(long);
void  nioclose(long);
FILE *niofp(long, int);
char *nioname(long);
int   niotype(long);

#else

long  cintio();
long  crealio();
long  cshrtio();
long  clongis();
void  nioclose();
FILE *niofp();
char *nioname();
int   niotype();

#endif  /* ifdef __STDC__ */

#endif

/* Revision History
 *
 * $Log: nio.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
