/* Filename: st_deltatoms2.c
 * Purpose:  This is an upgrade to st_deltatoms.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

LONG ST_DeltaToMS2(DELTA_T2 indelta) {

  LONG totalms;
  BOOL minf = FALSE;

  if (indelta.nday > 2) return(TIM_LIM);
  if (indelta.nday < -2) return(-TIM_LIM);

  if (indelta.nday < 0) {
    minf = TRUE;
    indelta = ST_MinusDelta2(indelta);
  }

  /* GTM the last multiplier on all the following statements used to be 1000L */
  totalms = ((LONG) indelta.nday) * 86400L * 10000L;
  totalms += ((LONG) indelta.nhour) * 3600L * 10000L;
  totalms += ((LONG) indelta.nmin) * 60L * 10000L;
  totalms += ((LONG) indelta.nsec) * 10000L;
  totalms += ((LONG) indelta.ntenth_msecs);

  if (minf) totalms = -totalms;

  if (totalms > TIM_LIM) totalms = TIM_LIM;
  if (totalms < -TIM_LIM) totalms = - TIM_LIM;

  return(totalms);

}
