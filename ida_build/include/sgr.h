#pragma ident "$Id: sgr.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"
/*======================================================================
 *
 *  Include file for routines which manipulate Berkeley SGR format.
 *
 *  ONLY B1CZ AND B3CZYX FORMAT FILES ARE SUPPORTED!
 *
 *====================================================================*/
#ifndef sgr_h_included
#define sgr_h_included

#include <stdio.h>

#define SGR_HDRLEN 258

/* binary format header */

struct sgr_header {
    char  filtyp[6+1];     /* file type                               */
    char  dattyp[6+1];     /* type and size data elements             */
    char  network[30+1];   /* network or station name                 */
    char  sname[5+1];      /* station identifier                      */
    char  iname[6+1];      /* instrument identifier                   */
    char  motion[12+1];    /* type of recording                       */
    char  datestr[9+1];    /* date string ( DDMMMYYY )                */
    char  event_id[10+1];  /* event ident                             */
    short yangle;          /* angle of y comp, deg E of N             */
    char  tunit[10+1];     /* time units                              */
    float sint;            /* samples per 'tunit'                     */
    char  aunit[10+1];     /* amplitude units                         */
    float apc;             /* 'aunit's per digital count              */
    float vector;          /* max vector sum of time series data      */
    short year;            /* year of first sample                    */
    short jday;            /* jday "    "     "                       */
    short hour;            /* hour "    "     "                       */
    short min;             /* min  "    "     "                       */
    float sec;             /* sec  "    "     "                       */
    long  nsamp;           /* number of samples per component         */
    char  comment[56+1];   /* event or data comment                   */
    char  history[68+1];   /* processing history                      */
};

/*  sgrio library function prototypes  */

#ifdef __STDC__

int    rb_sgrhead(FILE *, struct sgr_header *);
double sgrttodt(struct sgr_header *);
int    wb_sgrhead(FILE *, struct sgr_header *);
void   strdates(struct sgr_header *);

#else

int    rb_sgrhead();
double sgrttodt();
int    wb_sgrhead();
void   strdates();

#endif /* ifdef __STDC__ */

#endif

/* Revision History
 *
 * $Log: sgr.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
