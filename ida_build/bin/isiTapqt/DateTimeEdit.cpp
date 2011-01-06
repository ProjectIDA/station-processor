// DateTimeEdit.cpp: implementation of the DateTimeEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "DateTimeEdit.h"
#include "util.h"

DateTimeEdit::DateTimeEdit(QWidget * parent, const char * name ):QLineEdit(parent, name)
	{
	nTimeFormat=0;
	}

DateTimeEdit::~DateTimeEdit()
{

}

void DateTimeEdit::SetFormat(int nFormat)
	{
	nTimeFormat=nFormat;
	switch(nTimeFormat)
		{
		case 0:
			setInputMask("NN.NN.NNNN NN:NN:NN");
			setText("00.00.0000 00:00:00");
//			m_strMask = _T("00.00.0000 00:00:00");
//			m_strLiteral = _T("__.__.____ __:__:__");
			break;
		case 1:
			setInputMask("NNNN.NNN NN:NN:NN");
			setText("0000.000 00:00:00");
//			m_strMask = _T("0000.000 00:00:00");
//			m_strLiteral = _T("____.___ __:__:__");
			break;
		case 2:
			setInputMask("NN:NN:NN NN/NN/NNNN");
			setText("00:00:00 00/00/0000");
//			m_strMask = _T("00:00:00 00/00/0000");
//			m_strLiteral = _T("__:__:__ __/__/____");
			break;

		}

	}
void DateTimeEdit::GetDateTime(int &yr, int &mnth, int &day, int &hh, int &mm, int &ss)
	{
	QString str;
	static char *format_str0 = "%2d%*[., ]%2d%*[., ]%4d%*[:, ]%2d%*[:,// ]%2d%*[:,// ]%2d";
	static char *format_str1 = "%4d%*[., ]%3d%*[ ]%2d%*[: ]%2d%*[: ]%2d";
	static char *format_str2 = "%2d%*[:, ]%2d%*[,: ]%2d%*[:, ]%2d%*[:,// ]%2d%*[:,// ]%4d";

	str = text();

	switch(nTimeFormat)
		{
		case 0:
			sscanf(str.latin1(),format_str0, &mnth, &day, &yr, &hh, &mm, &ss);
			break;
		case 1:
			{
			int DayOfYear;
			sscanf(str.latin1(),format_str1, &yr, &DayOfYear, &hh, &mm, &ss);
			util_jdtomd(yr, DayOfYear, &mnth, &day);
			}
			break;
		case 2:
			sscanf(str.latin1(),format_str2, &hh, &mm, &ss, &day, &mnth, &yr);
			break;
		}
	
	}

bool DateTimeEdit::Validate()
	{
	static int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int yr, mnth, day, hh, mm, ss;
	GetDateTime(yr, mnth, day, hh, mm, ss);
	if(mnth>12) return false;
	if(hh>23)   return false;
	if(mm>59)   return false;
	if(ss>59)   return false;
	if(mnth>12) return false;
	if(mnth==2 && ISLEAP(yr))
		{
		if(day>mdays[mnth-1]+1)	return false;
		}
	else
		{
		if(day>mdays[mnth-1])	return false;
		}
	return true;
	}
void DateTimeEdit::SetDateTime(int yr, int mnth, int day, int hh, int mm, int ss)
	{
	QString strText;
	
	switch(nTimeFormat)
		{
		case 0:
			strText.sprintf("%02d.%02d.%04d %02d:%02d:%02d", mnth,day,yr,hh,mm,ss);
			break;
		case 1:
			{
			int DayOfYear=utilYmdtojd(yr, mnth, day);
			strText.sprintf("%04d.%03d %02d:%02d:%02d",yr,DayOfYear, hh,mm,ss);
			}
			break;
		case 2:
			strText.sprintf("%02d:%02d:%02d %02d/%02d/%04d",hh,mm,ss, day, mnth, yr);
			break;
		}
//	m_str = strText;
//	CEdit::SetWindowText(strText);
	setText(strText);
	}
