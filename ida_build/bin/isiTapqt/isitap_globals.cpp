#include <qstring.h>
#include <qsettings.h>
#include <qwidget.h>
#include <qrect.h>
#include "stdlib.h"
#include "platform.h"
#include "wfdisc.h"

BOOL bContinue=false;
QString server="fsuhub";
int nNrtsPort=39136;
int nLocalPort = 0;
/*char *pIniFile=".\\isiTap.ini";*/
int nOutputByteOrder=0;
int nTimeFormat=0;
QString wfdFilePath;
QString WfdiscDir;
int nGlobalError=0;
QString qParamFileName = "isiTap.ch";
QString PARAMPATH("nrtsutil/isitap/config/");

QPtrList <CWfdisc> wfdarr;
double tbeg=0, tend=1;

void LoadGlobalParam()
	{
	QSettings settings;
	server		= settings.readEntry( PARAMPATH + "SERVER", "idahub.ucsd.edu");
	nNrtsPort	= settings.readNumEntry(PARAMPATH + "PORT",39136);
	nOutputByteOrder = settings.readNumEntry(PARAMPATH + "OUTPUTBYTEORDER", 0);
	nTimeFormat	= settings.readNumEntry(PARAMPATH + "TIMEFORMAT",0);

	WfdiscDir		= settings.readEntry( PARAMPATH + "WFDDIR", "./");
	
	}
/*
CString GetDirFromPath(CString FileName)
	{
	CString s="";
	int n=FileName.ReverseFind('\\');
	if(n>0)
		{
		s=FileName.Left(n);
		}
	return s;
	}*/
void SaveGlobalParam()
	{
	QSettings settings;
	settings.writeEntry(PARAMPATH + "SERVER", server);
	settings.writeEntry(PARAMPATH + "PORT", nNrtsPort);
	settings.writeEntry(PARAMPATH + "OUTPUTBYTEORDER", nOutputByteOrder);
	settings.writeEntry(PARAMPATH + "TIMEFORMAT", nTimeFormat);
	settings.writeEntry(PARAMPATH + "WFDDIR", WfdiscDir);
	}

void GetTextRectangle(const QWidget *pWid, const char *lpText, QRect &rc)
	{

	QFont f=pWid->font();

	QFontMetrics qfm(f);
	rc.setRect(0,0,0,0);
	rc = qfm.boundingRect(lpText, -1);

	}
void ClearWfdList()
	{
	CWfdisc *wfd;
	while(1)
		{
		wfd=wfdarr.first();
		if(wfd==NULL) return;
		wfdarr.removeFirst();
		delete wfd;
		}
	}