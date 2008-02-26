/* Filename: st_addtotime22.c
 * Purpose:  This is an upgrade to st_addtotime.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

STDTIME2 ST_AddToTime2(STDTIME2 intime, LONG inday, LONG inhour,	
                       LONG inminute, LONG insecond, LONG intenthmsec) {

  STDTIME2 outtime;
  LONG dy;

  dy = intime.year;
  inday = intime.day + inday - 1;	/* Let jul date start at 0 for now */
  inhour = intime.hour + inhour;
  inminute = intime.minute + inminute;
  insecond = intime.second + insecond;
  intenthmsec = intime.tenth_msec + intenthmsec;	

/* Normalize the date */
  timenorm2(&dy, &inday, &inhour, &inminute, &insecond, &intenthmsec);

  outtime.year = dy;
  outtime.day = inday + 1;
  outtime.hour = inhour;
  outtime.minute = inminute;
  outtime.second = insecond;
  outtime.tenth_msec = intenthmsec;

  return(outtime);	

}
