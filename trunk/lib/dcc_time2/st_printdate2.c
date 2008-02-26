/* Filename: st_printdate2.c
 * Purpose:  This is an upgrade to st_printdate.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>


UBYTE *ST_PrintDate2(STDTIME2 odate, BOOL fixfmt) {

  static UBYTE _odata[60];
  LONG	tenth_ms;
  UBYTE 	msprt[8];

  if (fixfmt) {
    sprintf(_odata,"%04d,%03d,%02d:%02d:%02d.%04d",
	    (int) odate.year,
	    (int) odate.day,
	    (int) odate.hour,
	    (int) odate.minute,
	    (int) odate.second,
	    (int) odate.tenth_msec);
    return(_odata);
  }

  tenth_ms = odate.tenth_msec;
	
  if (tenth_ms == 0) 
    msprt[0] = '\0';
  else 
    if (tenth_ms%1000 == 0) 
      sprintf(msprt,".%01d", (int)tenth_ms/1000);
    else 
      if (tenth_ms%100 == 0) 
        sprintf(msprt,".%02d", (int) tenth_ms/100);
      else 
        if (tenth_ms%10 == 0) 
          sprintf(msprt,".%03d", (int) tenth_ms/10);
        else 
          sprintf(msprt,".%04d", (int) tenth_ms);

  sprintf(_odata,"%d,%d,%d:%02d:%02d%s",
	  (int) odate.year,
	  (int) odate.day,
	  (int) odate.hour,
	  (int) odate.minute,
	  (int) odate.second,
	  msprt);

  return(_odata);

}

