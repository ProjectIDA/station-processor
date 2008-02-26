/* Filename: st_deltaprint2.c
 * Purpose:  This is an upgrade to st_deltaprint.c.  It uses STDTIME2 and
 *           DELTA_T2 structures, and now handles times accurate to the tenth of a 
 *           millisecond.
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

static UBYTE _odata[60];

UBYTE *ST_DeltaPrint2(DELTA_T2 delta, BOOL printplusflag, BOOL fixms) {

  LONG tenth_ms;
  BOOL minus;
  DELTA_T2 tdelta;
  UBYTE osign[3], msprt[8];

  minus = FALSE;

  if (delta.nday < 0) {
    minus = TRUE;
    tdelta = ST_MinusDelta2(delta);
  } else {
    tdelta = delta;
  }

  tenth_ms = tdelta.ntenth_msecs;

  if (tenth_ms == 0)
    msprt[0] = '\0';
  else if (tenth_ms % 1000 == 0) 
    sprintf(msprt, ".%01d", (int)tenth_ms/1000);
  else if (tenth_ms % 100 == 0) 
    sprintf(msprt, ".%02d", (int)tenth_ms/100);
  else if (tenth_ms % 10 == 0) 
    sprintf(msprt, ".%03d", (int)tenth_ms/10);
  else 
    sprintf(msprt, ".%04d", (int)tenth_ms);

  if (fixms) 
    sprintf(msprt, ".%04d", (int)tenth_ms);

  osign[0] = '\0';
  osign[1] = '\0';

  if (minus) 
    osign[0] = '-';
  else if (printplusflag) 
    osign[0] = '+';

  if (tdelta.nday > 0) {
    sprintf(_odata, "%s%d,%d:%02d:%02d%s", osign, (int)tdelta.nday, (int)tdelta.nhour, (int)tdelta.nmin, (int) delta.nsec, msprt);
  } else if (tdelta.nhour > 0) {
    sprintf(_odata, "%s%d:%02d:%02d%s", osign, (int)tdelta.nhour, (int)tdelta.nmin, (int)tdelta.nsec, msprt);
  } else if (tdelta.nmin > 0) {
    sprintf(_odata, "%s%d:%02d%s", osign, (int)tdelta.nmin, (int)tdelta.nsec, msprt);
  } else {
    sprintf(_odata, "%s%d%s", osign, (int)tdelta.nsec, msprt);
  }

  return(_odata);

}
