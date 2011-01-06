#include <qstring.h>
#include <ctype.h>
#include "util.h"


QString TimeToString(double t, int nTimeFormat)
	{
	QString s;
	int yr,mnth,day,hh,mm,ss,ms,da;

	switch(nTimeFormat)
		{
		case 0:
			util_tsplit(t, &yr, &da, &hh, &mm, &ss, &ms);
			util_jdtomd(yr, da, &mnth, &day);
			s.sprintf("%02d.%02d.%04d %02d:%02d:%02d",mnth,day,yr,hh,mm,ss);
			break;
		case 1:
			util_tsplit(t, &yr, &da, &hh, &mm, &ss, &ms);
			s.sprintf("%04d.%03d-%02d:%02d:%02d",yr,da,hh,mm,ss);
			break;
		case 2:
			util_tsplit(t, &yr, &da, &hh, &mm, &ss, &ms);
			util_jdtomd(yr, da, &mnth, &day);
			s.sprintf("%02d:%02d:%02d %02d.%02d.%04d",hh,mm,ss,day,mnth,yr);
			break;
		}
	return s;
	}
