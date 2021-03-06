#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

static UBYTE _odata[60];

_SUB	UBYTE *ST_SpanPrint(STDTIME ftime,STDTIME etime,BOOL fixfmt)
{

	LONG	ms;
	UBYTE 	msprt[8];
	UBYTE	outfp[50];
	UBYTE	yrprt[8],
		dayprt[8];

	ms = ftime.msec;
	
	if (fixfmt) {
sprintf(_odata,"%d,%03d,%02d:%02d:%02d.%03d->%d,%03d,%02d:%02d:%02d.%03d",
			(int) ftime.year,
			(int) ftime.day,
			(int) ftime.hour,
			(int) ftime.minute,
			(int) ftime.second,
			(int) ftime.msec,
			(int) etime.year,
			(int) etime.day,
			(int) etime.hour,
			(int) etime.minute,
			(int) etime.second,
			(int) etime.msec);
		return(_odata);
	}

	if (ms==0) msprt[0]='\0';
	else if (ms%100==0) sprintf(msprt,".%01d",ms/100);
	else if (ms%10==0) sprintf(msprt,".%02d",ms/10);
	else sprintf(msprt,".%03d",ms);

	sprintf(outfp,"%d,%d,%d:%02d:%02d%s",
		(int) ftime.year,
		(int) ftime.day,
		(int) ftime.hour,
		(int) ftime.minute,
		(int) ftime.second,
		msprt);

	yrprt[0]='\0';
	dayprt[0]='\0';

	if (ftime.year!=etime.year) {
		sprintf(yrprt,"%d,",etime.year%10);
		sprintf(dayprt,"%d,",etime.day);
	} else if (ftime.day!=etime.day) {
		sprintf(dayprt,"%d,",etime.day);
	}

	ms = ftime.msec;
	
	if (ms==0) msprt[0]='\0';
	else if (ms%100==0) sprintf(msprt,".%01d",ms/100);
	else if (ms%10==0) sprintf(msprt,".%02d",ms/10);
	else sprintf(msprt,".%03d",ms);

	sprintf(_odata,"%s->%s%s%d:%02d:%02d%s",	
		outfp,
		yrprt,
		dayprt,
		(int) etime.hour,
		(int) etime.minute,
		(int) etime.second,
		msprt);

	return(_odata);
}
