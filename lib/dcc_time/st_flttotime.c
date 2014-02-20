#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	STDTIME	ST_FLTToTime(FLTTIME intime)
{

	STDTIME outtime;

	LONG secpd,juldate;
	WORD t1;
	FLTTIME tmpacum;

	juldate = tmpacum/86400.0;
	tmpacum = fmod(intime, 86400.0);	/* Calculate Days */	

	ST_CnvJulToCal(juldate,&outtime.year,&t1,&t1,&outtime.day);

	secpd = tmpacum;
	tmpacum = ((tmpacum - ((FLTTIME) secpd)) * 1000.0) + .5;
	outtime.second = secpd % 60;
	secpd /= 60;
	outtime.minute = secpd % 60;
	secpd /= 60;
	outtime.hour = secpd;
	outtime.msec = tmpacum;

	return(outtime);

}
