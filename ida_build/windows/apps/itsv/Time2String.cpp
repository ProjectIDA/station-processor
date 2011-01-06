#include "stdafx.h"
#include "TimeFunc.h"

CString &TimeToString(double t, int nTimeFormat)
	{
	static CString s;
	int yr,mnth,day,hh,mm,ss,ms,da;

	if(nTimeFormat==0)
		{
		util_tsplit(t, &yr, &da, &hh, &mm, &ss, &ms);
		util_jdtomd(yr, da, &mnth, &day);
		s.Format("%02d.%02d.%04d %02d:%02d:%02d",mnth,day,yr,hh,mm,ss);
		}
	else
		{
		util_tsplit(t, &yr, &da, &hh, &mm, &ss, &ms);
		s.Format("%04d.%03d-%02d:%02d:%02d",yr,da,hh,mm,ss);
		}
	return s;
	}
double StringToTime(CString s, int nTimeFormat)
	{
	int i;
	int yr,mnth,day,hh,mm,ss,da;
	double t;
	char cBuff[128];
    static char *format_str = "%2d%*[.:,// ]%2d%*[.:,// ]%4d%*[, ]%2d%*[:, ]%2d%*[:, ]%2d";


	lstrcpy(cBuff,(LPCSTR)s);

    for (i = 0; i < lstrlen(cBuff); ++i)
		{
		if (isalpha(cBuff[i])) return -1.;
		}
    
	
	
	for (i = 0; i < lstrlen(cBuff); ++i)
		{
		if (!isdigit(cBuff[i])) cBuff[i] = ' ';
		}


	if(nTimeFormat==0)
		{
		sscanf(cBuff,format_str,&mnth,&day,&yr,&hh,&mm,&ss);
		da=ymdtojd(yr, mnth, day);
		t=ydhmsmtod(yr,da,hh,mm,ss,0);
		}
	else
		{
		t=attodt(cBuff);
		}

	return t;
	}
