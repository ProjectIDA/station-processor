/* Filename: st_difftimes2.c
 * Purpose:  This is an upgrade to st_difftimes.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

LONG _julday2(LONG year, LONG mon, LONG day);

DELTA_T2 ST_DiffTimes2(STDTIME2 intime1, STDTIME2 intime2) {

  DELTA_T2 outtime;
  LONG jul1, jul2;
  LONG dd, dh, dm, ds, dtenth_ms;

  jul1 = _julday2(intime1.year, 1, intime1.day);
  jul2 = _julday2(intime2.year, 1, intime2.day);

  dd = jul1 - jul2;
  dh = intime1.hour - intime2.hour;
  dm = intime1.minute - intime2.minute;
  ds = intime1.second - intime2.second;
  dtenth_ms = intime1.tenth_msec - intime2.tenth_msec;

/* Normalize the date */
  timenormd2(&dd, &dh, &dm, &ds, &dtenth_ms);

  outtime.nday = dd;
  outtime.nhour = dh;
  outtime.nmin = dm;
  outtime.nsec = ds;
  outtime.ntenth_msecs = dtenth_ms;

  return(outtime);

}
