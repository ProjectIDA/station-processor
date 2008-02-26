/* Filename: st_timenorm2.c
 * Purpose:  This is an upgrade to st_timenorm.c.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

extern BOOL _tleap(short int year);

#define _tdays(x) (_tleap(x)?366:365)


VOID timenorm2(LONG *year, LONG *day, LONG *hour, LONG *minute, LONG *second, LONG *tenth_msec)
{

  LONG tl;

  /* adjust tenth_msecs */
  if (abs(*tenth_msec) > 9999) {
    tl = *tenth_msec / 10000;
    *second += tl;
    *tenth_msec %= 10000;
  }		
  while (*tenth_msec < 0) { 
    *tenth_msec += 10000;  
    (*second)--;  
  }
  while (*tenth_msec > 9999) { 
    *tenth_msec -= 10000;  
    (*second)++;  
  }

  /* adjust seconds */
  if (abs(*second)>59) {
    tl = *second / 60;
    *minute += tl;
    *second %= 60;
  }
  while (*second < 0) {
    *second += 60;  
    (*minute)--; 
  }
  while (*second > 59) { 
    *second -= 60;  
    (*minute)++; 
  }

  /* adjust minutes */
  if (abs(*minute) > 59) {
    tl = *minute / 60;
    *hour += tl;
    *minute %= 60;
  }
  while (*minute < 0) {
    *minute += 60;  
    (*hour)--; 
  }
  while (*minute > 59) {
    *minute -= 60;  
    (*hour)++; 
  }

  /* adjust hours */
  if (abs(*hour) > 23) {
    tl = *hour / 24;
    *day += tl;
    *hour %= 24;
  }
  while (*hour < 0) {
    *hour += 24;
    (*day)--; 
  }
  while (*hour > 23) {
    *hour -= 24;
    (*day)++; 
  }

  /* adjust days */
  while (*day >= _tdays(*year)) {
    *day -= _tdays(*year);
    (*year)++; 
  }
  while (*day < 0) {
    (*year)--; 
    *day += _tdays(*year); 
  }

  if (*year >= 4000) {
    *year = 0;
    *day  = 1;
    *hour = 0;
    *minute = 0;
    *second = 0;
    *tenth_msec = 0;
  }

  return;
}


VOID timenormd2(LONG *day, LONG *hour, LONG *minute, LONG *second, LONG *tenth_msec)
{

  LONG tl;
  /* adjust tenth_msecs */
  if (abs(*tenth_msec) > 9999) {
    tl = *tenth_msec / 10000;
    *second += tl;
    *tenth_msec %= 10000;
  }		
  while (*tenth_msec < 0) {
    *tenth_msec += 10000;
    (*second)--; 
  }
  while (*tenth_msec > 9999) {
    *tenth_msec -= 10000;
    (*second)++; 
  }

  /* adjust seconds */
  if (abs(*second) > 59) {
    tl = *second / 60;
    *minute += tl;
    *second %= 60;
  }
  while (*second < 0) {
    *second += 60;  
    (*minute)--; 
  }
  while (*second > 59) {
    *second -= 60;  
    (*minute)++; 
  }

  /* adjust minutes */
  if (abs(*minute) > 59) {
    tl = *minute / 60;
    *hour += tl;
    *minute %= 60;
  }
  while (*minute < 0) {
    *minute += 60;  
    (*hour)--; 
  }
  while (*minute > 59) {
    *minute -= 60;  
    (*hour)++; 
  }

  /* adjust hours */
  if (abs(*hour) > 23) {
    tl = *hour / 24;
    *day += tl;
    *hour %= 24;
  }
  while (*hour < 0) { 
    *hour += 24;
    (*day)--; 
  }
  while (*hour > 23) {
    *hour -= 24;
    (*day)++; 
  }

  return;
}
