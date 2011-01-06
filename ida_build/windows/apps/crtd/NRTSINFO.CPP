// NRTSINFO.cpp: implementation of the CNRTSINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NRTSINFO.h"
#include "xfer.h"
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

CNRTSINFO::CNRTSINFO()
	{
	nSta=0;
	LoadConfigFromIni();
	}
void CNRTSINFO::AddStaChan(CString sSta, CString sChan, double dSpS)
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	for(int i=0; i<StaInfo.GetSize(); ++i)
		{
		si=StaInfo[i];
		if( sSta.CompareNoCase(si->Sta)==0)
			{
			for(int j=0; j<si->ChanInfo.GetSize(); ++j)
				{
				ci=si->ChanInfo[j];
				if(sChan.CompareNoCase(ci->Chan)==0)
					{
					return;
					}
				}
			si->AddChan(sChan, dSpS, FALSE);
			return;
			}
		}
	si=new CNRTSSTAINFO(sSta);
	si->AddChan(sChan,dSpS,FALSE);
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
			char cChanIndex[64],cSpS[16];
			double dSpS;
			sprintf(cChanIndex,"CH%d",j);
			GetPrivateProfileString(cSta,cChanIndex,"",cChan,sizeof(cChan),pIniFile);

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

			si->AddChan(cChan,dSpS,bSelected);
			}
		StaInfo.Add(si);
		++nSta;
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
int CNRTSINFO::LoadConfigFromNRTSServer()
	{
	CWaitCursor cw;
	int port = nNrtsPort; 
	int  sd ;
	struct xfer_cnf cnf;
	struct xfer_req req;
	struct xfer_cnfgen1 *cnfgen;

	memset(&cnf,0,sizeof(cnf));
	memset(&req,0,sizeof(req));


    req.protocol = 1;  
    req.type	 = XFER_CNFREQ;
    req.timeout  = 60;
    req.sndbuf   = 0;  
    req.rcvbuf   = 0;  
    req.preamble.ver01.format = XFER_CNFGEN1;
    req.preamble.ver01.client_id = 11111111;

    cnfgen = (struct xfer_cnfgen1 *) &cnf.type.gen1;

	bContinue=TRUE;
    sd = Xfer_Connect2((LPSTR)(LPCSTR)server, (char *)NULL, port, "tcp", &req, &cnf, 1,30);

    if (sd < 0) return -1;
	bContinue=FALSE;
	closesocket(sd);

	Clear();

	int nStations=cnf.type.gen1.nsta;
	char cSta[16],cChan[16];
	for(int i=0; i<nStations; ++i)
		{
		lstrcpy(cSta,cnf.type.gen1.sta[i].name);
		CNRTSSTAINFO *si=new 	CNRTSSTAINFO(cSta);
		int nChan=cnf.type.gen1.sta[i].nchn;
		for(int j=0; j<nChan; ++j)
			{
			lstrcpy(cChan,cnf.type.gen1.sta[i].chn[j].name);
			double dSpS=1./cnf.type.gen1.sta[i].chn[j].sint;
			si->AddChan(cChan,dSpS,FALSE);
			}
		StaInfo.Add(si);
		++nSta;

		}

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
	nSta=0;
	}


CNRTSINFO::~CNRTSINFO()
	{
	while(StaInfo.GetSize()>0)
		{
		CNRTSSTAINFO *si=StaInfo[0];
		StaInfo.RemoveAt(0);
		delete si;
		}
	nSta=0;
	}
