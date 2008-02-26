/* Filename: st_getcurrent2.c
 * Purpose:  This is an upgrade to st_getcurrent.c.  It uses STDTIME2 structures, 
 *           and now handles times accurate to the tenth of a millisecond.
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>


UBYTE _dmsize[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#if VMS

STDTIME2 ST_GetCurrentTime2() {

  ULONG sysret;
  UWORD numret[7];
  STDTIME2 rettime;
  WORD i, ct, mon, day;

  sysret = SYS$NUMTIM(numret, 0);	/* Get current time */

  if (sysret != EXIT_NORMAL) {
    printf("ST_GetCurrentTime2 SYS$NUMTIM failed\n");
  }

  rettime.year = numret[0];
  mon = numret[1];
  day = numret[2];
  rettime.hour = numret[3];
  rettime.minute = numret[4];
  rettime.second = numret[5];
  rettime.tenth_msec = numret[6] * 100;

  _dmsize[1] = _tleap(rettime.year)?29:28;

  ct = 0;
  for (i = 0; i < (mon-1); i++) 
    ct+=_dmsize[i];

  ct += day;

  rettime.day = ct;

  return(rettime);

}

#else

#include <time.h> 

STDTIME2 ST_GetCurrentTime2(void) {

  struct tm *intime;
  time_t tloc;

  STDTIME2 rettime;
  WORD i,ct,mon,day;

  time(&tloc);
  intime = gmtime(&tloc);

  rettime.year = intime->tm_year + 1900;
  mon = intime->tm_mon + 1;
  day = intime->tm_mday;
  rettime.hour = intime->tm_hour;
  rettime.minute = intime->tm_min;
  rettime.second = intime->tm_sec;
  rettime.tenth_msec = 0;

  _dmsize[1] = _tleap(rettime.year)?29:28;

  ct = 0;
  for (i = 0; i < (mon-1); i++) 
    ct+=_dmsize[i];

  ct += day;

  rettime.day = ct;

  return(rettime);

}

#endif
