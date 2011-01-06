// NRTSINFO.cpp: implementation of the CNRTSINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "isi.h"
#include "util.h"

//#include "CRTDglob.h"
#include <memory.h>
#include <qstring.h>
#include <qsettings.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qobject.h>
#include "isitap_globals.h"
#include "nrtsinfo.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSINFO::CNRTSINFO(CNRTSINFO &Info)
	{
	CopyFrom(Info);
	}


CNRTSINFO::CNRTSINFO()
	{
	}
void CNRTSINFO::CopyFrom(CNRTSINFO &Info)
	{
	
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	Clear();

	int nStations = Info.StaInfo.count();

	for(si=Info.StaInfo.first(); si; si=Info.StaInfo.next())
		{
		int nChannels = si->ChanInfo.count();
		for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
			{
			AddStaChan(si->Sta, ci->Chan, ci->LCODE, ci->dSpS, ci->bSelected);
			}	
		}

	}
CNRTSINFO & CNRTSINFO::operator =(CNRTSINFO & Info)
	{
	CopyFrom(Info);
	return *this;
	}

void CNRTSINFO::AddStaChan(QString &sSta, QString &sChan, QString &csLCODE,  double dSpS, BOOL bSel=FALSE)
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	for(si=StaInfo.first(); si; si=StaInfo.next())
		{
		if( sSta.compare(si->Sta)==0)
			{

			for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
				{
				if(sChan.compare(ci->Chan)==0 && csLCODE.compare(ci->LCODE)==0)
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
	StaInfo.append(si);
	}



void CNRTSINFO::LoadChannelListFromFile()

	{
	QString qsSta, qsChan, qsLcode;
	char cSta[10], cChan[10], cLcode[10],  cbuff[128];
	float dSpS;
	int bSelected;
	FILE *f;
	int i=0;
	QString qs;
	Clear();


	f=fopen(qParamFileName.latin1(),"r");
	if(f==NULL) return;



	while(fgets(cbuff, sizeof(cbuff)-2, f)!=NULL)
		{
		sscanf(cbuff,"%s %s %s %f %d", cSta, cChan, cLcode, &dSpS, &bSelected);
		qsSta=cSta;
		qsChan=cChan;
		qsLcode=cLcode;
		if(qsLcode.compare("__")==0) qsLcode="  ";
		AddStaChan(qsSta, qsChan, qsLcode, dSpS, bSelected);
		}
	fclose(f);
	}



/*
void CNRTSINFO::LoadConfigFromFile()
	{
	char cSta[10], cChan[10], cLcode[10];
	double dSpS;
	BOOL bSelected;
	QFile f;


	int i=0;
	QString qs;

	Clear();

	f.setName(qParamFileName);


	if(!f.open(IO_ReadOnly))
		{
        QMessageBox::critical(
                NULL,
				"Open failed",
                "Could not open file for writing: "+qParamFileName
                );
            return;
		}
	while(f.readLine(qs, 128)>0)
		{
		QString qsSta, qsChan, qsLcode;
		sscanf(qs.latin1(),"%s %s %s %f %d", cSta, cChan, cLcode, &dSpS, &bSelected);
		qsSta=cSta;
		qsChan=cChan;
		qsLcode=cLcode;
		if(qsLcode.compare("__")==0) qsLcode="  ";
		AddStaChan(qsSta, qsChan, qsLcode, dSpS, bSelected);
		}

	f.close();

	}*/
void CNRTSINFO::SaveChannelListToFile()
	{

	QFile f;
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	int i=0;
	QString qs;

	f.setName(qParamFileName);
	if(!f.open(IO_WriteOnly))
		{
        QMessageBox::critical(
                NULL,
				"Open failed",
                "Could not open file for writing: "+qParamFileName
                );
            return;
		}

	for(si=StaInfo.first(); si; si=StaInfo.next(),++i)
		{

		for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
			{
			QString lcode;
			if(ci->LCODE.compare("  ")==0)
				lcode="__";
			else
				lcode=ci->LCODE;
			qs.sprintf("%s %s %s %f %d\n", si->Sta.ascii(), ci->Chan.ascii(), lcode.latin1(), ci->dSpS, ci->bSelected);
			f.writeBlock(qs.ascii(), qs.length());
			}


		}
	f.close();
	}



int CNRTSINFO::LoadConfigFromServer()
	{
	int i;
/*	CWaitCursor cw;*/
	Clear();

	ISI_PARAM par;

    utilNetworkInit();
    isiInitDefaultPar(&par);

	

	ISI_CNF_REPORT *report;

	par.port = nNrtsPort;

//    if ((report = isiCnf((char *)(LPCSTR)server, &par)) == NULL) 
    if ((report = isiCnf((char*)server.latin1(), &par)) == NULL) 
		{
		return 0;
		}


    for (i = 0; i < report->nentry; i++)
		{
		float sps;
		QString sta, chan, clcode;
		double sint;

		sta    = report->entry[i].name.sta;
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
	while(1)
		{
		CNRTSSTAINFO *si=StaInfo.first();
		if(si==NULL) return;
		StaInfo.removeFirst();
		delete si;
		}
	}


CNRTSINFO::~CNRTSINFO()
	{
	Clear();
	}

int CNRTSINFO::GetSelectedChanNumber()
	{
	QString			sSta,sChan;
	CNRTSSTAINFO	*si;
	CNRTSCHANINFO	*ci;

	int nStations=0, nChannels=0, nSelected=0;

	for(si=StaInfo.first(); si; si=StaInfo.next())
		{
		sSta=si->Sta;
		for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
			{
			if(ci->bSelected)
				{
				++nSelected;
				}
			}
		}
	return nSelected;
	}
CNRTSSTAINFO	* CNRTSINFO::LookupStation(const QString s)
	{
	QString			sSta;
	CNRTSSTAINFO	*si;

	int nStations=0, nChannels=0, nSelected=0;

	for(si=StaInfo.first(); si; si=StaInfo.next())
		{
		sSta=si->Sta;
		if(s.compare(sSta)==0) return si;
		}
	return NULL;
	}
int CNRTSINFO::LoadTimeInfo()
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	int port = nNrtsPort; 


	int i;

	ISI_PARAM par;

    utilNetworkInit();
    isiInitDefaultPar(&par);
	

	ISI_SOH_REPORT *report;

    if ((report = isiSoh((char *)server.latin1(), &par)) == NULL) 
		{
		return 0;
		} 

	for (i = 0; i < report->nentry; i++)
		{
		QString sta, chan, chandotlcode, clcode;
		
		sta    = report->entry[i].name.sta;
		chan   = report->entry[i].name.chn;
		clcode = report->entry[i].name.loc;

		si = LookupStation(sta);
		
		if(si!=NULL)
			{
			ci = si->LookupChannel(chan, clcode);
			if(ci!=NULL)
				{
				ci->dTstart = report->entry[i].tofs.value;
				ci->dTend   = report->entry[i].tols.value;
//				ci->inst    = report->entry[i].inst.inst;
				}
			}


		}


	isiFreeSoh(report);

	return 1;
	}
