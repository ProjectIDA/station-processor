/* Filename: st_cleandate2.c
 * Purpose:  This is an upgrade to st_cleandate.  It handles stdtime2 structures,
 *           which are accurate to the tenth of a millisecond
 */

#include <dcc_std.h>
#include <dcc_time.h>
#include <dcc_time_proto2.h>

LONG _calyear(LONG dy,LONG epoch,ULONG timflgs);
#define _tdays(x) (_tleap(x)?366:365)

_SUB	STDTIME2 ST_CleanDate2(STDTIME2 indate, WORD epoch, ULONG timflgs) {

	LONG dy,dd,dh,dm,ds,dms;
	STDTIME2 outtime;

	dy = indate.year;
	dd = indate.day;
	dh = indate.hour;
	dm = indate.minute;
	ds = indate.second;
	dms = indate.tenth_msec;

	dy = _calyear(dy, epoch, timflgs);
	dd--;		/* Let jul date start at 0 for now */

/*-------------Normalize the date-------------*/

	timenorm2(&dy, &dd, &dh, &dm, &ds, &dms);

	outtime.year = dy;
	outtime.day = dd + 1;
	outtime.hour = dh;
	outtime.minute = dm;
	outtime.second = ds;
	outtime.tenth_msec = dms;

	return(outtime);
}
