/* Filename: st_parsetime2.c
 *  This is a version of st_parsetime.c that has been cleaned up
 *  and upgraded to use STDTIME2 structures.  All routines herein
 *  now end with a 2.
 */
#include <stdio.h>
#include <dcc_std.h>
#include <dcc_time.h>

static STDTIME2 setuptime;
static BOOL dateset,timeset;
extern UBYTE _dmsize[];

void timeload2(int hour, int minute, int second, LONG tenth_msec) {
  timeset = TRUE;
  setuptime.hour = hour;
  setuptime.minute = minute;
  setuptime.second = second;
  setuptime.tenth_msec = tenth_msec;
}


void _setyear2(void) {
  int y, f;

  y = setuptime.year;

  if (y > 1900) 
    return;		/* Its probably ok */

  f = 0;

  if (y > 99)
    f = TM_CENT;
  else 
    if (y > 9) 
      f = TM_DECADE;

  setuptime.year = _calyear(y, 0, f);
}


void juldateset2(int year, int day) {
  dateset = TRUE;
  setuptime.year = year;
  _setyear2();
  setuptime.day = day;
}


void nordate2(int month, int day, int year) {
  int ct, i;
  dateset = TRUE;
  setuptime.year = year;
  _setyear2();
  _dmsize[1] = _tleap(setuptime.year)?29:28;
  ct = 0;
  for (i = 0; i < month-1; i++) {
    ct += _dmsize[i];
  }
  ct += day;
  setuptime.day = ct;
}

STDTIME2 ST_CleanDate2(STDTIME2 indate, WORD epoch, ULONG timflgs);


STDTIME2 ST_ParseTime2(UBYTE *inbuff) {
  int getbuf,i;
  int year, doy, hour, minute, second, tmsec;
  int iArg;

  UBYTE trsbuf[120];

  for (i = 0; i < strlen(inbuff); i++) {
    if (isupper(inbuff[i])) 
      trsbuf[i] = tolower(inbuff[i]);
    else 
      trsbuf[i] = inbuff[i];
  }

  trsbuf[i++] = '\0';

  year = 0;
  doy = 0;
  hour = 0;
  minute = 0;
  second = 0;
  tmsec = 0;

  iArg = sscanf(trsbuf, "%d,%d,%d:%d:%d.%d",
      &year, &doy, &hour, &minute, &second, &tmsec);

  setuptime.year = year;
  setuptime.day = doy;
  setuptime.hour = hour;
  setuptime.minute = minute;
  setuptime.second = second;
  setuptime.tenth_msec = tmsec;

  dateset = timeset = FALSE;

  return(ST_CleanDate2(setuptime, 0, TM_MILEN));

}
