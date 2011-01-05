/* Filename: st_timecomp2.c
 * Purpose:  This is an upgrade to st_timecomp.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond.
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

/* This function returns:
         1 if 1st time > 2nd time
	-1 if 1st time < 2nd time
	 0 if 1st time = 2nd time
*/

int ST_TimeComp2(STDTIME2 time_arg1, STDTIME2 time_arg2) {

  if (time_arg1.year   > time_arg2.year)    
    return(1);
  if (time_arg1.year   < time_arg2.year)    
    return(-1);

  if (time_arg1.day    > time_arg2.day)     
    return(1);
  if (time_arg1.day    < time_arg2.day)     
    return(-1);

  if (time_arg1.hour   > time_arg2.hour)    
    return(1);
  if (time_arg1.hour   < time_arg2.hour)    
    return(-1);

  if (time_arg1.minute > time_arg2.minute) 
    return(1);
  if (time_arg1.minute < time_arg2.minute)  
    return(-1);

  if (time_arg1.second > time_arg2.second)  
    return(1);
  if (time_arg1.second < time_arg2.second)  
    return(-1);

  if (time_arg1.tenth_msec   > time_arg2.tenth_msec)    
    return(1);
  if (time_arg1.tenth_msec   < time_arg2.tenth_msec)    
    return(-1);

  return(0);

}
