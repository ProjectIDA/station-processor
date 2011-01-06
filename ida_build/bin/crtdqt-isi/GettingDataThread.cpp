// GettingDataThread.cpp: implementation of the GettingDataThread class.
//
//////////////////////////////////////////////////////////////////////
#include <qapplication.h>
#include "GettingDataThread.h"
#include "CRTDglob.h"
#include "isi.h"
#include "util.h"
#include "MainFrame.h"



GettingDataThread::GettingDataThread()
	{
	}

GettingDataThread::~GettingDataThread()
	{
	}

void GettingDataThread::Init(MainFrame *p)
	{
	pMF = p;
	}


void GettingDataThread::run()
	{
	MainFrame *pParent=pMF;
	CRTDisplay *pDisplay=pMF->pDisplay;

	ISI *isi=NULL;
	
	int status;
	ISI_GENERIC_TS *ts;
	ISI_DATA_REQUEST *dreq;
	double beg = ISI_NEWEST;
	double end = ISI_KEEPUP;
	int compress  = ISI_COMP_IDA;
	int format    = ISI_FORMAT_GENERIC;
	char *StreamSpec;
	QString streams;

	ISI_PARAM par;

	double dstime;

	CMWindow *pWaveI;


	IsiInterruptT.Init(&isi);
	DrawT.Init(pParent);
	DrawT.start(QThread::NormalPriority);
	IsiInterruptT.start(QThread::NormalPriority);

	dstime = time(NULL);
	

	beg=dstime - nBufferedTime;

	utilNetworkInit();
	isiInitDefaultPar(&par);
	par.port = nNrtsPort;

	for (pWaveI=pDisplay->WaveformInfo.first();pWaveI;pWaveI=pDisplay->WaveformInfo.next()) 
		{
		QString channelName="+";
		channelName+=pWaveI->StaName+"."+pWaveI->ChanName+"."+pWaveI->LCODE;
		streams+=channelName;
		}

	StreamSpec = (char *)streams.latin1();

	while(bContinue)
		{

		if ((dreq = isiAllocSimpleDataRequest(beg, end, StreamSpec)) == NULL) 
			{
			continue;
			}
		isiSetDatreqFormat(dreq, format);
		isiSetDatreqCompress(dreq, compress);

		if(!bContinue) break;

		if ((isi = isiInitiateDataRequest((char *)server.latin1(), &par, dreq)) == NULL) 
			{
			continue;
			}

		isiFreeDataRequest(dreq);

		if(!bContinue) break;

		while ( bContinue) 
			{

			ts = isiReadGenericTS(isi, &status);

			if(status == ISI_ERROR) continue;
			if(status == ISI_BREAK) break;
			if(status == ISI_DONE) break;

			char *sta, *chan, *loc;
			double sint;

			sint = isiSrateToSint(&ts->hdr.srate);

			sta  = ts->hdr.name.sta;
			chan = ts->hdr.name.chn;
			loc  = ts->hdr.name.loc;

			int nsamp = ts->hdr.nsamp;
			long *data = (long *)ts->data;

//			utilSwapINT32((UINT32 *) ts->data, nsamp);


		pDisplay->Critical.lock();

		for (pWaveI=pDisplay->WaveformInfo.first();pWaveI;pWaveI=pDisplay->WaveformInfo.next()) 
			{
			if( (pWaveI->StaName.compare(sta)==0) &&
				(pWaveI->ChanName.compare(chan)==0) &&
				(pWaveI->LCODE.compare(loc))==0)
				{

				CDataBlock *xfd=new CDataBlock;
				if(xfd!=NULL)
					{
					xfd->PutData(nsamp, data);

					strcpy(xfd->sname,sta);
					xfd->lat   = 0.;
					xfd->lon   = 0.;
					xfd->elev  = 0.;
					xfd->depth = 0.;

					strcpy(xfd->cname, chan);
					xfd->sint  = sint;
					xfd->beg   = ts->hdr.tofs.value;
					xfd->tear  = 0;
					xfd->nsamp = nsamp;
					xfd->end   = ts->hdr.tols.value;

					pWaveI->Add(xfd);
					break;
					}
				}
			}
		pDisplay->Critical.unlock();

		}
	}
//	isiDestroyGenericTS(ts);
	

	DrawT.wait();
	IsiInterruptT.wait();
	StopDatavisualization();

	}

void GettingDataThread::StopDatavisualization()
	{
	}
