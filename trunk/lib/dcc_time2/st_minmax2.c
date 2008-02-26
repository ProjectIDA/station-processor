/* Filename: st_minmax2.c
 * Purpose:  This is an upgrade to st_minmax.c.  It uses STDTIME2 structures, 
 *           and now handles times accurate to the tenth of a millisecond.
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

STDTIME2 ST_TimeMin2(STDTIME2 time_arg1, STDTIME2 time_arg2) {

  if (ST_TimeComp2(time_arg1, time_arg2) <= 0) 
    return(time_arg1);
  else 
    return(time_arg2);

}


STDTIME2 ST_TimeMax2(STDTIME2 time_arg1, STDTIME2 time_arg2) {

  if (ST_TimeComp2(time_arg1, time_arg2) >= 0)
    return(time_arg1);
  else 
    return(time_arg2);

}
