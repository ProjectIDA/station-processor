/* Filename: st_julian2.c
 * Purpose:  This is an upgrade to st_julian.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>


#define CAL_CONS 1720982L	/* Nov, 1, 1BC */
	
extern UBYTE _dmsize[12];

BOOL _tleap(year)               /* Gregorian leap rules */
WORD year;
{

        if (year%400==0) return(TRUE);  /* Yes on 400s */
        if (year%100==0) return(FALSE); /* No on 100s */
        if (year%4==0) return(TRUE);    /* Yes on 4s */
        return(FALSE);                  /* Otherwise no */

}

/* This algorithm is only accurate from 1-Mar-1900 to 28-Feb-2100 */
LONG _julday2(LONG year, LONG mon, LONG day) {

  LONG julian, yearprime, monprime, dayprime;

  yearprime = year;
  monprime = mon + 1;
  dayprime = day;

  if (mon == 1 || mon == 2) {
    yearprime = year - 1;
    monprime = mon + 13;
  }

  julian = dayprime + CAL_CONS;
  julian += (36525L * yearprime) / 100L;
  julian += (306001L * monprime) / 10000L;

  return(julian);

}


LONG ST_Julian2(LONG year, LONG mon, LONG day) {
  return(_julday2(year, mon, day));
}

/* This algorithm is only accurate from 1-Jan-1901 to 28-Feb-2100 */
VOID ST_CnvJulToCal2(LONG injul, WORD *outyr, WORD *outmon, WORD *outday, WORD *outjday) {

  int y1, m1, d, m, y, j, t;
  float a, b, jdu;

  jdu = injul - CAL_CONS;
  y1 = ( (jdu - 122.1) / 365.25 );
  a = (int)( 365.25  *( (float)y1 )); 
  m1 = (jdu - a) / 30.6001;
  b = (int)(30.6001 * ( (float)m1 ));
  d = jdu - a - b;

  if (m1 == 14 || m1 == 15)
    m = m1 - 13;
  else 
    m = m1 - 1;

  if (m > 2) 
    y = y1;
  else 
    y = y1 + 1;

  if (_tleap(y)) 
    _dmsize[1] = 29;
  else 
    _dmsize[1] = 28;
	
  j = d;
  for (t = 0; t < (m-1); t++) 
    j+=_dmsize[t];

  *outyr = y;
  *outmon = m;
  *outday = d;
  *outjday = j;

  return;	

}

STDTIME2 ST_CnvJulToSTD2(JULIAN injul) {

  STDTIME2 rettim = {0, 0, 0, 0, 0, 0};

  WORD outyr, outmon, outday, outjday;

  ST_CnvJulToCal2(injul, &outyr, &outmon, &outday, &outjday);

  rettim.year = outyr;
  rettim.day = outjday;

  return(rettim);
}

_SUB	LONG _calyear(LONG dy,LONG epoch,ULONG timflgs)
{
/*  This routine cannot handle real time data that only transmits the year
    up to the decade or less in the station record header.  The problem
    occurs on a year that changes across the melinia for a time zone 
    ahead of the data processing center time zone.  Fortunately, all 
    stations that telemeter data should be transmitting the full year.     */

	STDTIME2 curtime;
	LONG tyear,tfact;

	tyear=dy;

	if (tyear>1900) return(tyear);		/* Don't labor the point */
				/* Not much digital seismic data before 1900 */

	if (epoch<1900) {
		curtime = ST_GetCurrentTime2();
		epoch = curtime.year;
	}

	switch(timflgs) {

	case TM_CENT:	tfact=1000;
			break;
	case TM_DECADE:	tfact=100;
			break;
	default:	tfact=10;
			break;
	}

/* Calculate the year based on flags which show how many digits of
   the date are really present.  */

	tyear+=(epoch/tfact)*tfact;
	if (tyear>epoch+1) tyear-=tfact; /* +1 fudge factor needed only
   for stations that telemeter data and transmit the decade of the year
   or less in its station record headers, in which case the year of the
   data could end up greater than the year at the data processing center. */

	return(tyear);

}

