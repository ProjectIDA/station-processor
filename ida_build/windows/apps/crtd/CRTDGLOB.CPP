#include "stdafx.h"
#include "stdlib.h"

BOOL bContinue=FALSE;
CString server="fsuhub.gsras.ru";
int nNrtsPort=14002;
int nDisplayedTime=900;
int nBufferedTime=7200;
int nRefreshTime=1;
int nDelay=0;
char *pIniFile=".\\CRTD.INI";

void LoadGlobalParamFromIni()
	{
	char cBuff[128];

	GetPrivateProfileString("-CONFIG-","SERVER","",cBuff,sizeof(cBuff),pIniFile);
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
	if(*cBuff) nDelay=atoi(cBuff);

	}
