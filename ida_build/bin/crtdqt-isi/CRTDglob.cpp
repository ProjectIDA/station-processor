#include <qstring.h> 
#include <stdlib.h>
#include <qsettings.h> 
#include "platform.h"
#include "nrtsinfo.h"

BOOL bContinue=FALSE;
QString server="fsuhub";
int nNrtsPort=39136;
int nDisplayedTime=60;
int nBufferedTime=3600;
int nRefreshTime=1;
int nDelay=12;
CNRTSINFO		NrtsInfo;

QSettings settings;
const QString WINDOWS_REGISTRY = "/nrtsutil/CRTD";
const QString APP_KEY_C = "/config/";
bool bShiftButtonPressed=FALSE;


QString qParamFileName="crtdqt.ch";

int nFontSize = 10;


void LoadGlobalParam()
	{
#ifdef WIN32
	settings.insertSearchPath( QSettings::Windows, WINDOWS_REGISTRY);
#else
//	settings.insertSearchPath( QSettings::Unix, "./" );
#endif

	server		= settings.readEntry( APP_KEY_C + "SERVER", "idahub.ucsd.edu");
	nNrtsPort	= settings.readNumEntry(APP_KEY_C + "PORT",39136);
	nDisplayedTime	= settings.readNumEntry(APP_KEY_C + "DISPLAYEDTIME",60);
	nBufferedTime	= settings.readNumEntry(APP_KEY_C + "BUFFEREDTIME",3600);
	nRefreshTime	= settings.readNumEntry(APP_KEY_C + "REFRESHTIME",1);
	nDelay	= settings.readNumEntry(APP_KEY_C + "DELAY",12);

/*	GetPrivateProfileString("-CONFIG-","SERVER","",cBuff,sizeof(cBuff),pIniFile);
	if(*cBuff) server=cBuff;
	GetPrivateProfileString("-CONFIG-","PORT","",cBuff,sizeof(cBuff), pIniFile);
	if(*cBuff) nNrtsPort=atoi(cBuff);

	GetPrivateProfileString("-CONFIG-","DISPLAYEDTIME","",cBuff,sizeof(cBuff), pIniFile);
	if(*cBuff) nDisplayedTime=atoi(cBuff);

	GetPrivateProfileString("-CONFIG-","BUFFEREDTIME","",cBuff,sizeof(cBuff), pIniFile);
	if(*cBuff) nBufferedTime=atoi(cBuff);

	GetPrivateProfileString("-CONFIG-","REFRESHTIME","",cBuff,sizeof(cBuff), pIniFile);
	if(*cBuff) nRefreshTime=atoi(cBuff);

	GetPrivateProfileString("-CONFIG-","DELAY","",cBuff,sizeof(cBuff), pIniFile);
	if(*cBuff) nDelay=atoi(cBuff);*/

	}
void SaveGlobalParam()
	{
	settings.writeEntry(APP_KEY_C + "SERVER", server);
	settings.writeEntry(APP_KEY_C + "PORT",nNrtsPort);
	settings.writeEntry(APP_KEY_C + "DISPLAYEDTIME",nDisplayedTime);
	settings.writeEntry(APP_KEY_C + "BUFFEREDTIME",nBufferedTime);
	settings.writeEntry(APP_KEY_C + "REFRESHTIME",nRefreshTime);
	settings.writeEntry(APP_KEY_C + "DELAY",nDelay);
	}