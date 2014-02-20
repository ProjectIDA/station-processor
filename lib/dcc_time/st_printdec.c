#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

LOCAL UBYTE _ddata[60];

LOCAL UBYTE *ShortNames[12] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec" };

_SUB	UBYTE *ST_PrintDECDate(STDTIME odate,BOOL printtime)
{

	LONG jdate;
	WORD mon,day,jday,jyr;

	jdate = ST_GetJulian(odate);

	ST_CnvJulToCal(jdate,&jyr,&mon,&day,&jday);

	if (printtime) 
		sprintf(_ddata,"%02d-%s-%04d:%02d:%02d:%02d.%03d",
			day,ShortNames[mon-1],jyr,
			odate.hour,odate.minute,odate.second,odate.msec);
	else
		sprintf(_ddata,"%02d-%s-%04d",day,ShortNames[mon-1],jyr);

	return(_ddata);

}
