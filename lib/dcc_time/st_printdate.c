#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

static UBYTE _odata[60];

_SUB	UBYTE *ST_PrintDate(STDTIME odate,BOOL fixfmt)
{

	LONG	ms;
	UBYTE 	msprt[8];

	if (fixfmt) {
		sprintf(_odata,"%04d,%03d,%02d:%02d:%02d.%03d",
			(int) odate.year,
			(int) odate.day,
			(int) odate.hour,
			(int) odate.minute,
			(int) odate.second,
			(int) odate.msec);
		return(_odata);
	}

	ms = odate.msec;
	
	if (ms==0) msprt[0]='\0';
	else if (ms%100==0) sprintf(msprt,".%01d",ms/100);
	else if (ms%10==0) sprintf(msprt,".%02d",ms/10);
	else sprintf(msprt,".%03d",ms);

	sprintf(_odata,"%d,%d,%d:%02d:%02d%s",
		(int) odate.year,
		(int) odate.day,
		(int) odate.hour,
		(int) odate.minute,
		(int) odate.second,
		msprt);

	return(_odata);

}
