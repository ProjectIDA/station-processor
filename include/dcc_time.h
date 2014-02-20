/* Filename: dcc_time.h
 * Modifications:
 * Jan. 31, 2003 - GTM - added the __sjtim2 and __deltatime2 struct's and their
 *   corresponding typedef's (STDTIME2 and DELTA_T2).  These are used to
 *   obtain tenth of a millisecond precision in the time functions located
 *   in the DCC_TIME2 library.
 * Mar. 10, 2003 - GTM - added the __deltatime3 struct to accomodate the
 *   change in the data type of chan_index duration field (in the 
 *   Network_Inventory_Record's Span_List (netinv3.h)). The duration was formerly
 *   a LONG and is now an unsigned long.  The use of the DELTA_T3 structure is
 *   needed only for code that is using the Span_List of a Network_Inventory_Record.
 */

#ifndef TMLB_DEFINED
#define TMLB_DEFINED

/* GTM - changed msec to tenth_msec */
struct	__sjtim2	{		/* Day of year pseudo-julian date form */
	WORD	year;
	WORD	day;
	BYTE	hour;
	BYTE	minute;
	BYTE	second;
	WORD	tenth_msec;   /* units are now tenths of milliseconds */
};
typedef struct __sjtim2 STDTIME2;



/* GTM - changed nmsecs from UWORD to LONG and renamed nmsecs to ntenth_msecs */
struct	__deltatime2	{	/* Difference of a time amount */
	LONG	nday;
	UBYTE	nhour;
	UBYTE	nmin;
	UBYTE	nsec;
        LONG	ntenth_msecs;  /* units are now tenths of milliseconds */
};
typedef struct __deltatime2 DELTA_T2;



/* GTM - changed datatype of ntenth_msecs from LONG to unsigned long */
struct	__deltatime3	{	/* Difference of a time amount */
	LONG	nday;
	UBYTE	nhour;
	UBYTE	nmin;
	UBYTE	nsec;
        unsigned long	ntenth_msecs;  /* units are now tenths of milliseconds */
};
typedef struct __deltatime3 DELTA_T3;



struct	__sjtim	{		/* Day of year pseudo-julian date form */
	WORD	year;
	WORD	day;
	BYTE	hour;
	BYTE	minute;
	BYTE	second;
	WORD	msec;
};

typedef struct __sjtim STDTIME;

struct	__deltatime	{	/* Difference of a time amount */
	LONG	nday;
	UBYTE	nhour;
	UBYTE	nmin;
	UBYTE	nsec;
	UWORD	nmsecs;
};
typedef struct __deltatime DELTA_T;

/* these utime fields were added by michael 2001-01-5 */
struct	__sjutim	{	/* Day of year pseudo-julian date form */
	WORD	year;
	WORD	day;
	BYTE	hour;
	BYTE	minute;
	BYTE	second;
	WORD	msec;
	WORD	usec;
};

typedef struct __sjutim STDUTIME;

struct	__deltautime	{	/* Difference of a utime amount */
	LONG	nday;
	UBYTE	nhour;
	UBYTE	nmin;
	UBYTE	nsec;
	UWORD	nmsecs;
	UWORD	nusecs;
};

typedef struct __deltautime DELTA_UT;

typedef long JULIAN;

#define zerotime(x) (x.year == 0)

#define tpr(x) tfpr(stdout,x)
LONG	timsb();
BOOL	tleap();
STDTIME	*timall();

typedef double FLTTIME;    	/* Number of seconds since julian instant */

#define TM_MILEN 1000
#define TM_CENT 100
#define TM_DECADE 10

#define TIM_LIM (20L*86400L*1000L)	/* Time limit (fits in 31 bits) */
					/* 20 Days */

#define ST_SPAN_NIL	0	/* No intersection */
#define ST_SPAN_ACB	1	/* A contains B */
#define ST_SPAN_BCA	2	/* B contains A */
#define ST_SPAN_ALB	3	/* A less than B */
#define ST_SPAN_BLA	4	/* B less than A */

#include "dcc_time_proto.h"

#endif

