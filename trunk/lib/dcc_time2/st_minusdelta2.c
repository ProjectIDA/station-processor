/* Filename: st_minusdelta2.c
 * Purpose:  This is an upgrade to st_minusdelta.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

DELTA_T2 ST_MinusDelta2(DELTA_T2 indelta) {

  DELTA_T2 outtime;
  LONG dd, dh, dm, ds, dtenth_ms;

  dd = - indelta.nday;
  dh = - indelta.nhour;
  dm = - indelta.nmin;
  ds = - indelta.nsec;
  dtenth_ms = - indelta.ntenth_msecs;	

/* Normalize the date */
  timenormd2(&dd, &dh, &dm, &ds, &dtenth_ms);

  outtime.nday = dd;
  outtime.nhour = dh;
  outtime.nmin = dm;
  outtime.nsec = ds;
  outtime.ntenth_msecs = dtenth_ms;

  return(outtime);

}
