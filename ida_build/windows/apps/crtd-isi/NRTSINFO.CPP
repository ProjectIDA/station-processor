// NRTSINFO.cpp: implementation of the CNRTSINFO class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "isi.h"
#include "util.h"

#include "NRTSINFO.h"
#include "crtdglob.h"
#include <memory.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSINFO::CNRTSINFO(const CNRTSINFO &Info)
	{
	CopyFrom(Info);
	}


CNRTSINFO::CNRTSINFO()
	{
	}
void CNRTSINFO::CopyFrom(const CNRTSINFO &Info)
	{
	
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	Clear();

	int nStations = Info.StaInfo.GetSize();

	for(int i=0; i<nStations; ++i)
		{
		si=Info.StaInfo[i];
		int nChannels = si->ChanInfo.GetSize();
		for(int j=0; j<nChannels; ++j)
			{
			ci=si->ChanInfo[j];

			AddStaChan(si->Sta, ci->Chan, ci->LCODE, ci->dSpS, ci->bSelected);
			}	
		}

	}
CNRTSINFO & CNRTSINFO::operator =(const CNRTSINFO &Info)
	{
	CopyFrom(Info);
	return *this;
	}

void CNRTSINFO::AddStaChan(CString &sSta, CString &sChan, CString &csLCODE,  double dSpS, BOOL bSel=FALSE)
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;
	int nStations = StaInfo.GetSize();

	for(int i=0; i<nStations; ++i)
		{
		si=StaInfo[i];
		if( sSta.CompareNoCase(si->Sta)==0)
			{
			int nChannels = si->ChanInfo.GetSize();

			for(int j=0; j<nChannels; ++j)
				{
				ci=si->ChanInfo[j];
				if(sChan.CompareNoCase(ci->Chan)==0 && csLCODE.CompareNoCase(ci->LCODE)==0)
					{
					return;
					}
				}
			si->AddChan(sChan, csLCODE, dSpS, bSel);
			return;
			}
		}
	si=new CNRTSSTAINFO(sSta);
	si->AddChan(sChan, csLCODE, dSpS, bSel);
	StaInfo.Add(si);
	}

void CNRTSINFO::LoadConfigFromIni()
	{
	char cSta[16],cChan[16];
	int nStations=GetPrivateProfileInt("-STATIONS-","N",0,pIniFile);
	CNRTSSTAINFO *si;

	Clear();

	for(int i=0; i<nStations; ++i)
		{
		char cStaIndex[64];
		sprintf(cStaIndex,"STATION%d",i);
		GetPrivateProfileString("-STATIONS-",cStaIndex,"",cSta,sizeof(cSta),pIniFile);

		if(*cSta==0) continue;
		int nChan=GetPrivateProfileInt(cSta,"N",0,pIniFile);
		
		si=new 	CNRTSSTAINFO(cSta);

		for(int j=0; j<nChan; ++j)
			{
			char cChanIndex[64],cSpS[16],csLC[16];
			double dSpS;
			sprintf(cChanIndex,"CH%d",j);
			GetPrivateProfileString(cSta,cChanIndex,"",cChan,sizeof(cChan),pIniFile);

			sprintf(cChanIndex,"CH%d.LCODE",j);
			GetPrivateProfileString(cSta,cChanIndex,"",csLC,sizeof(csLC),pIniFile);

			if(*csLC==0)
				{
				csLC[0]=csLC[1]=' ';
				csLC[2]=0;
				}

			sprintf(cChanIndex,"CH%d.SpS",j);
			GetPrivateProfileString(cSta,cChanIndex,"",cSpS,sizeof(cSpS),pIniFile);

			dSpS=atof(cSpS);
			sprintf(cChanIndex,"CH%d.Selected",j);


			int nsel=GetPrivateProfileInt(cSta,cChanIndex,0,pIniFile);
			BOOL bSelected;
			if(nsel==0)
				{
				bSelected=FALSE;
				}
			else
				{
				bSelected=TRUE;
				}

			si->AddChan(cChan, csLC, dSpS, bSelected);
			}
		StaInfo.Add(si);
		}
	}
void CNRTSINFO::SaveConfigToIni()
	{
	char cSta[16],cChan[16],cBuff[128];

	int nStations=StaInfo.GetSize();

	sprintf(cBuff,"%d",nStations);
	WritePrivateProfileString("-STATIONS-","N",cBuff,pIniFile);

	for(int i=0; i<nStations; ++i)
		{
		char cStaIndex[64];
		sprintf(cStaIndex,"STATION%d",i);
		lstrcpy(cSta,StaInfo[i]->Sta);
		WritePrivateProfileString("-STATIONS-",cStaIndex,cSta,pIniFile);

		int nChan=StaInfo[i]->ChanInfo.GetSize();
		sprintf(cBuff,"%d",nChan);
		WritePrivateProfileString(cSta,"N",cBuff,pIniFile);
		
		for(int j=0; j<nChan; ++j)
			{
			char cChanIndex[64],cSpS[16],*pSelected;
			sprintf(cChanIndex,"CH%d",j);
			lstrcpy(cChan,StaInfo[i]->ChanInfo[j]->Chan);
			WritePrivateProfileString(cSta,cChanIndex,cChan,pIniFile);

			sprintf(cChanIndex,"CH%d.SpS",j);
			sprintf(cSpS,"%.4f",StaInfo[i]->ChanInfo[j]->dSpS);
			WritePrivateProfileString(cSta,cChanIndex,cSpS,pIniFile);


			sprintf(cChanIndex,"CH%d.LCODE",j);
			WritePrivateProfileString(cSta,cChanIndex,StaInfo[i]->ChanInfo[j]->LCODE,pIniFile);


			if(StaInfo[i]->ChanInfo[j]->bSelected)
				{
				pSelected="1";
				}
			else
				{
				pSelected="0";
				}
			sprintf(cChanIndex,"CH%d.Selected",j);
			WritePrivateProfileString(cSta,cChanIndex,pSelected,pIniFile);

			}


		}

	}
int CNRTSINFO::LoadConfigFromServer()
	{
	int i;
	CWaitCursor cw;
	Clear();

	ISI_PARAM par;

    utilNetworkInit();
    isiInitDefaultPar(&par);

	

	ISI_CNF_REPORT *report;

	par.port = nNrtsPort;

    if ((report = isiCnf((char *)(LPCSTR)server, &par)) == NULL) 
		{
		return 0;
		}


    for (i = 0; i < report->nentry; i++)
		{
		float sps;
		CString sta, chan, clcode;
		double sint;
		char *p;


		p = report->entry[i].name.sta;
		sta    = p;
		chan   = report->entry[i].name.chn;
		clcode = report->entry[i].name.loc;

		sint = isiSrateToSint(&report->entry[i].srate);


		if(report->entry[i].srate.factor<0)
			{
			sps  = (double)report->entry[i].srate.multiplier;
			}
		else
			{
			sps  = (double)report->entry[i].srate.multiplier*(double)report->entry[i].srate.factor;
			}
		AddStaChan(sta, chan, clcode, sps);
		}
	isiFreeCnf(report);


/*	for (int ich = 0; ich < (int) channelList.length; ich++)
		{
		char *sta = 0; 
		char *chan = 0;
		sta = strdup(channelList.channel[ich].name);
		if ( (chan = strchr(sta, '.')) == NULL ) 
			{
			free(sta);
			continue;
			}
		*chan++ = '\0';
		AddStaChan(sta, chan, -1.);
		free(sta);
		}
	closesocket (NSocket);
	bContinue = FALSE;*/
	return 1;
	}
void CNRTSINFO::Clear()
	{
	while(StaInfo.GetSize()>0)
		{
		CNRTSSTAINFO *si=StaInfo[0];
		StaInfo.RemoveAt(0);
		delete si;
		}
	}


CNRTSINFO::~CNRTSINFO()
	{
	while(StaInfo.GetSize()>0)
		{
		CNRTSSTAINFO *si=StaInfo[0];
		StaInfo.RemoveAt(0);
		delete si;
		}
	}

int CNRTSINFO::GetSelectedChanNumber()
	{
	CString			sSta,sChan;
	CNRTSSTAINFO	*si;
	CNRTSCHANINFO	*ci;

	int nStations=0, nChannels=0, nSelected=0;

	nStations=StaInfo.GetSize();
	for(int i=0; i<nStations; ++i)
		{
		si=StaInfo[i];
		sSta=si->Sta;
		nChannels=si->ChanInfo.GetSize();
		for(int j=0; j<nChannels; ++j)
			{
			ci=si->ChanInfo[j];
			if(ci->bSelected)
				{
				++nSelected;
				}
			}
		}
	return nSelected;
	}
