/* Filename: st_getjulian2.c
 * Purpose:  This is an upgrade to st_getjulian.c.  It uses STDTIME2 structures, 
 *           and now handles times accurate to the tenth of a  millisecond.
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

LONG _julday2(LONG, LONG, LONG);

LONG ST_GetJulian2(STDTIME2 intime) {

  LONG jul1;

  jul1 = _julday2(intime.year, 1, intime.day);

  return(jul1);

}
