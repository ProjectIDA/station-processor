/*----------------------------------------------------------------------*
 *	Albuquerque Seismological Laboratory - USGS - US Dept of Inter	* 
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 *	dcc_std.h - Standards and constants definition for project	*
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*
 *	Modification and history 					*
 *									*
 *-----Edit---Date----Who-------Description of changes------------------*
 *	001 24-Apr-86 SH	Set up standards file			*
 *	002  9-Jul-86 SH	Add mfree to free for unix29		*
 *	003 22-Dec-86 SH	Tailor VMS definitions 			*
 *	004 10-Jul-89 SH	Add ssdef.h to files loaded		*
 *	005 22-Aug-89 SH	Add math.h to files loaded		*
 *	006 13-Jun-91 SH	Make more unix oriented 		*
 *      007 29-May-92 SH        POSIX project                           *
 *----------------------------------------------------------------------*/

/*-----------------------Determine System Dependancies------------------*/

#ifndef __DCC_STD_H_
#define __DCC_STD_H_

#ifdef VMS

#include stdio
#include ssdef
#include stdlib
#include math
#include ctype
#include rms

#define _BIT16 short		/* What is the 16 bit quantity */
#define _BIT32 int		/* What is the 32 bit quantity */
#define _UNSGND unsigned	/* Non-Ints can be specified as unsigned */
#define _SIGND                  /* Explicit signed value */
#define _VOIDER void		/* The compiler really has a void type */
#define QTICKS 100		/* times CPU times are 1/100th of a second */
#define _mag_type int           /* Raw QIO's for magtape */

#define	LITTLE_ENDIAN 		/* VAX swap order */

#define EXIT_NORMAL	SS$_NORMAL
#define EXIT_ABORT	SS$_ABORT
#define EXIT_NOMEM	SS$_INSFMEM
#define EXIT_BADPARAM	SS$_BADPARAM
#define EXIT_NODEVAVL	SS$_NODEVAVL
#define EXIT_NOSUCHDEV	SS$_NOSUCHDEV
#define EXIT_TIMEOUT	SS$_TIMEOUT
#define EXIT_INSFMEM	SS$_INSFMEM

#define DB_NORMAL   RMS$_NORMAL
#define DB_RNF      RMS$_RNF
#define DB_RLK	    RMS$_RLK

#else

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>

#define exit exit_dummy		/* stdlib has problems... fake it out */
#define abort abort_dummy

#include <stdlib.h>

#undef exit
#undef abort

#include <string.h>

#define _BIT16 short		/* What is the 16 bit quantity */
#define _BIT32 int		/* What is the 32 bit quantity */
#define _UNSGND unsigned	/* Non-Ints can be specified as unsigned */
#define _SIGND signed           /* Explicit signed integer */
#define _VOIDER void		/* The compiler really has a void type */
#define QTICKS 100		/* times CPU times are 1/100th of a second */
#define _mag_type int        	/* Magtape uses file-descriptor # */

#define EXIT_NORMAL	0
#define EXIT_ABORT	1
#define EXIT_NOMEM	ENOMEM
#define EXIT_INSFMEM	ENOMEM
#define EXIT_BADPARAM	EINVAL
#define EXIT_NODEVAVL	EACCES
#define EXIT_NOSUCHDEV	ENODEV
#define EXIT_TIMEOUT	ETIME

#define DB_NORMAL   0
#define DB_RNF      100
#define DB_RLK	    101

#endif

/*--------------------------Define Data types------------------------------*/

#define STDFLT double		/* Standard floating */

typedef _VOIDER VOID;     	/* For functions which return nothing */
typedef _UNSGND char BOOL;	/* Flag quantities */
typedef _UNSGND char TEXT;	/* For character strings */

	/*----------------------------------------------*
	 *			8-Bit	16-Bit	32-Bit	*
	 *	Numbers:	BYTE	WORD	LONG	*
	 *	Unsigned:	UBYTE	UWORD	ULONG	*
	 *----------------------------------------------*/

/*	8-Bit quantities	*/

typedef _UNSGND char UBYTE;	/* An 8-Bit definition */
typedef _SIGND char BYTE;		/* Numeric 8-bit definition */

/*	16-Bit quantities	*/

typedef _UNSGND _BIT16 UWORD;	/* 16 bit unsigned */
typedef _BIT16 WORD;		/* 16 bit numeric quantity */

/*	32-Bit quantities	*/

typedef _UNSGND _BIT32 ULONG;	/* Definition of a 32 bit mask */
#define LONG long		/* A 32 bit number */

/* 	Misc definitions */

typedef ULONG DSKREC;

typedef _mag_type MAG_ID;

#define LOCAL static
#define FAST register
#define PUBLIC
#define PRIVATE static 
#define _SUB			/* Key for subroutine definitions */

/*--------------Define some macros and contants we will use-----------------*/

#define FOREVER for(;;)		/* Infinite loop	*/


#ifndef NULL
#define NULL	(0)		/* Impossible pointer	*/
#endif
#define TRUE	1		/* if (TRUE)		*/
#define FALSE	0		/* if (!TRUE)		*/
#define EOS	'\0'		/* End of string	*/

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define streq(a,b) (strcmp(a,b)==0)

#define streq(a,b) (strcmp(a,b)==0)

#define IUBYTE(x) (((WORD) x)&0xFF)

#define bomb_memory(x) if ((x)==NULL) _bombout(EXIT_INSFMEM,	\
	"\nFile=%s (%d)\nLine=%d\n",__FILE__,__DATE__,__LINE__)

#define bombout _bombdesc(__FILE__,__DATE__,__LINE__), _bomb2

/* DCC Standard environment variables */

#define DCC_SETUP_ENV "DCC_SETUPS"
#define DCC_BIN_ENV "DCC_BIN"

/* DCC Standard pool clasess */

#define NEWSTAGE_POOL "NEWSTAGE_POOL"
#define NEWSTAGE_ARCHIVE "NEWSTAGE_ARCHIVE"
#define NEWNET_POOL "NEWNET_POOL"
#define NEWVOL_POOL "NEWVOL_POOL"
#define OLDNET_POOL "OLDNET_POOL"
#define NET_ARCHIVE "NET_ARCHIVE"
#define NEWFOLDER_POOL "NEWFOLDER_POOL"
#define SBK_METRUM1 "SBK_METRUM"
#define SBK_METRUM2 "SBK_METRUMTMP"
#define NET_METRUM1 "NET_METRUM1"
#define NET_METRUM2 "NET_METRUM2"

#endif // __DCC_STD_H_
