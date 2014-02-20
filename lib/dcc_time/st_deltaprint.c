#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

static UBYTE _odata[60];

DELTA_T ST_MinusDelta(DELTA_T indelta);

_SUB	UBYTE *ST_DeltaPrint(DELTA_T delta,BOOL printplusflag,BOOL fixms)
{

	LONG ms;
	BOOL minus;
	DELTA_T tdelta;
	UBYTE	osign[3],
		msprt[8];

	minus=FALSE;

	if (delta.nday<0) {
		minus = TRUE;
		tdelta = ST_MinusDelta(delta);
	} else tdelta = delta;

	ms = tdelta.nmsecs;

	if (ms==0) msprt[0]='\0';
	else if (ms%100==0) sprintf(msprt,".%01d",ms/100);
	else if (ms%10==0) sprintf(msprt,".%02d",ms/10);
	else sprintf(msprt,".%03d",ms);
	if (fixms) sprintf(msprt,".%03d",ms);

	osign[0]='\0';
	osign[1]='\0';

	if (minus) osign[0]='-';
	else if (printplusflag) osign[0]='+';

	if (tdelta.nday>0) {
		sprintf(_odata,"%s%d,%d:%02d:%02d%s",
			osign,
			(int) tdelta.nday,
			(int) tdelta.nhour,
			(int) tdelta.nmin,
			(int) tdelta.nsec,
			msprt);
	} else if (tdelta.nhour>0) {
		sprintf(_odata,"%s%d:%02d:%02d%s",
			osign,
			(int) tdelta.nhour,
			(int) tdelta.nmin,
			(int) tdelta.nsec,
			msprt);
	} else if (tdelta.nmin>0) {
		sprintf(_odata,"%s%d:%02d%s",
			osign,
			(int) tdelta.nmin,
			(int) tdelta.nsec,
			msprt);
	} else {
		sprintf(_odata,"%s%d%s",
			osign,
			(int) tdelta.nsec,
			msprt);
	}

	return(_odata);
}
