#include "stdafx.h"
#include "RTDisplay.h"
#include "crtd.h"
#include "crtdglob.h"

extern "C" {
#include "isi.h"
#include "util.h"
}


DWORD WINAPI IsiInterruptThread(LPVOID lpV)
	{
	ISI **pisi = (ISI **)lpV;

	while(bContinue)
		{
		Sleep(2000);
		}
	if(*pisi!=NULL)
		{
		isiSetFlag(*pisi, ISI_FLAG_BREAK);
		}
	return 0;
	}



DWORD WINAPI GettingDataThread(LPVOID lpV)
	{
	CRTDisplay *pDisplay=(CRTDisplay *)lpV;
	int rc;

	ISI *isi=NULL;
	
	int status;
	ISI_GENERIC_TS *ts;
	ISI_DATA_REQUEST *dreq;
	#define LOCALBUFLEN 1024
	double beg = ISI_NEWEST;
	double end = ISI_KEEPUP;
	int compress  = ISI_COMP_IDA;
//	int compress  = ISI_COMP_NONE;
	int format    = ISI_FORMAT_GENERIC;
	char *StreamSpec;
	CString streams;

	ISI_PARAM par;

	SYSTEMTIME st;
	GetSystemTime(&st);
	double dstime;
	DWORD dwId;


	HANDLE hThread=CreateThread(NULL,0,IsiInterruptThread, &isi,0, &dwId);
	SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST );



	dstime=utilYdhmsmtod(st.wYear, utilYmdtojd(st.wYear, st.wMonth, st.wDay), st.wHour, st.wMinute, st.wSecond, 0);
	beg=dstime - nBufferedTime;


    utilNetworkInit();
    isiInitDefaultPar(&par);
	par.port = nNrtsPort;


	int nElements=pDisplay->WaveformInfo.GetSize();
	for (int i = 0; i<nElements; ++i) 
		{
		CMWindow *pWaveI = pDisplay->WaveformInfo[i];
		CString channelName="+";
		channelName+=pWaveI->StaName+"."+pWaveI->ChanName+"."+pWaveI->LCODE;
		streams+=channelName;
		}

	StreamSpec = (char *)(LPCSTR)streams;



	while(bContinue)
		{

		if ((dreq = isiAllocSimpleDataRequest(beg, end, StreamSpec)) == NULL) 
			{
			continue;
			}
		isiSetDatreqFormat(dreq, format);
		isiSetDatreqCompress(dreq, compress);

		if(!bContinue) break;

		if ((isi = isiInitiateDataRequest((char *)(LPCSTR)server, &par, dreq)) == NULL) 
			{
			continue;
			}

		isiFreeDataRequest(dreq);

		if(!bContinue) break;

		while ( bContinue) 
			{
//			fprintf(stderr, "%s\n", isiGenericTsHdrString(&ts->hdr, (char *) buf));
//			if (datdir != NULL) WriteFrame(ts);

//			isiDecompressGenericTS(ts, &ts1);

			ts = isiReadGenericTS(isi, &status);

			if(status == ISI_ERROR) continue;
			if(status == ISI_BREAK) break;
			if(status == ISI_DONE) break;

			if( !(ts->hdr.desc.type==ISI_TYPE_INT16||ts->hdr.desc.type==ISI_TYPE_INT32)) continue;

			char *sta, *chan, *loc;
			double sint;

			sint = isiSrateToSint(&ts->hdr.srate);

			sta  = ts->hdr.name.sta;
			chan = ts->hdr.name.chn;
			loc  = ts->hdr.name.loc;

			int nsamp = ts->hdr.nsamp;
			long *data = new long[nsamp];
			short *shData = (short *)ts->data;
			long  *lData  = (long *)ts->data;

//			(long *)ts->data;

			for(i=0; i<nsamp; ++i)
				{
				if(ts->hdr.desc.type==ISI_TYPE_INT16)
					{
					data[i]=(long)shData[i];
					}
				if(ts->hdr.desc.type==ISI_TYPE_INT32)
					{
					data[i]=(long)lData[i];
					}

				}

//			utilSwapINT32((UINT32 *) ts->data, nsamp);

		pDisplay->Critical.Lock();

		int nElements = pDisplay->WaveformInfo.GetSize();
		for (i = 0; i<nElements; ++i) 
			{
			if( (pDisplay->WaveformInfo[i]->StaName.CompareNoCase(sta)==0) &&
				(pDisplay->WaveformInfo[i]->ChanName.CompareNoCase(chan)==0) &&
				(pDisplay->WaveformInfo[i]->LCODE.Compare(loc))==0)
				{

				CDataBlock *xfd=new CDataBlock;
				if(xfd!=NULL)
					{
					xfd->PutData(nsamp, data);

					lstrcpy(xfd->sname,sta);
					xfd->lat   = 0.;
					xfd->lon   = 0.;
					xfd->elev  = 0.;
					xfd->depth = 0.;

					lstrcpy(xfd->cname, chan);
					xfd->sint  = sint;
					xfd->beg   = ts->hdr.tofs.value;
					xfd->tear  = 0;
					xfd->nsamp = nsamp;
					xfd->end   = ts->hdr.tols.value;

					pDisplay->WaveformInfo[i]->Add(xfd);
					break;
					}
				}
			}
		pDisplay->Critical.Unlock();
		delete [] data;

		}
	}
//	isiDestroyGenericTS(ts);

	WaitForSingleObject(hThread,20000);
	Sleep(2000);

	isiClose(isi);

	CWnd *pW=pDisplay->GetParent();
	pW->PostMessage(WM_COMMAND,MAKEWPARAM(IDC_THREADEXIT,0),(LPARAM)pDisplay->m_hWnd);
	return 0;
	}
