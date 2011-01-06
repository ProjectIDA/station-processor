#include "stdafx.h"
#include "RTDisplay.h"
#include "crtd.h"
#include "xfer.h"
#include "crtdglob.h"
#include "TimeFunc.h"

void AddChanToWavRec(struct xfer01_wavreq *wavreq, CMWindow *pWaveI, double dstime)
	{
	int nchn;
	int nsta;
    for (nsta = 0; nsta<wavreq->nsta; ++nsta) 
		{
		if(lstrcmp(wavreq->sta->name, (LPCSTR)pWaveI->StaName)==0)
			{
			int nchn=wavreq->sta[nsta].nchn++;
			wavreq->sta[nsta].chn[nchn].beg = dstime;//XFER_YNGEST;//XFER_YNGEST;
			wavreq->sta[nsta].chn[nchn].end = XFER_YNGEST;
			lstrcpy(wavreq->sta[nsta].chn[nchn].name,pWaveI->ChanName);
			return;
			}
		}
	nsta=wavreq->nsta++;
	nchn=0;
	wavreq->sta[nsta].nchn=1;
    lstrcpy(wavreq->sta[nsta].name, pWaveI->StaName);

	wavreq->sta[nsta].chn[nchn].beg = dstime;
	wavreq->sta[nsta].chn[nchn].end = XFER_YNGEST;
	lstrcpy(wavreq->sta[nsta].chn[nchn].name, pWaveI->ChanName);
	}

DWORD WINAPI GettingDataThread(LPVOID lpV)
	{
	CRTDisplay *pDisplay=(CRTDisplay *)lpV;
	SYSTEMTIME st;
	GetSystemTime(&st);
	double dstime;

	dstime=ydhmsmtod(st.wYear, ymdtojd(st.wYear, st.wMonth, st.wDay), st.wHour, st.wMinute, st.wSecond, 0);
	dstime-=nBufferedTime;


	int sd,status;

	struct xfer_cnf cnf;
	struct xfer_req req;
	struct xfer_wav wav;
	struct xfer01_wavreq *wavreq;
	struct xfer_cnfgen1 *cnfgen;
	struct xfer_wavgen1 *wavgen;
	struct xfer_packet packet;


    req.protocol = 1;  /* that's all there is for now */
    req.type     = XFER_WAVREQ;
    req.timeout  = 60;
    req.sndbuf   = 0;  /* ie, use default socket send buffer size */
    req.rcvbuf   = 0;  /* ie, use default socket recv buffer size */
    req.preamble.ver01.format = XFER_CNFGEN1;
    req.preamble.ver01.client_id = 11111111;
    wavreq = &req.request.wav.ver01; /* use ptr for typing convenience */
    wavreq->keepup = 1;
    wavreq->comp   = XFER_CMPIGPP;  /* ONLY supported compression for now */
    wavreq->nsta   = 0;
    wavreq->format = XFER_WAVGEN1;
	

    cnfgen = (struct xfer_cnfgen1 *) &cnf.type.gen1;
    wavgen = (struct xfer_wavgen1 *) &wav.type.gen1;


while(bContinue)
	{

	int nElements=pDisplay->WaveformInfo.GetSize();

    wavreq->nsta = 0;



    for (int i = 0; i<nElements; ++i) 
		{
		AddChanToWavRec(wavreq, pDisplay->WaveformInfo[i], dstime);
        }

    sd = Xfer_Connect2((LPSTR)(LPCSTR)server, (char *)NULL, nNrtsPort, "tcp", &req, &cnf, 1,30);

    while ( ((status = Xfer_RecvWav(sd, &cnf, &wav)) == XFER_OK) &&  bContinue) 
		{
//		char cBuff[32],cTmp[128];
		/* get indices to look up static information */

		xfer_Convert(&cnf, &wav, &packet);
		
		/* use static info from connect and waveform header to reformat */

		
/* print selected information */
/*		char *util_dttostr(double dtime, int code);

		sprintf(cBuff," %-5s",   packet.sname);
		lstrcpy(cTmp,cBuff);
		lstrcat(cTmp," ");
		sprintf(cBuff," %-4s",   packet.cname);
		lstrcat(cTmp,cBuff);
		lstrcat(cTmp," ");
	    sprintf(cBuff," %s",util_dttostr(packet.beg, 0));
		lstrcat(cTmp,cBuff);
		lstrcat(cTmp," ");
		sprintf(cBuff," %-6s",   packet.instype);
		lstrcat(cTmp,cBuff);
		lstrcat(cTmp," ");*/
	if(!bContinue) break;

	pDisplay->Critical.Lock();

    for (int i = 0; i<nElements; ++i) 
		{
		CString Sta, Chan;
		Sta=packet.sname;
		Chan=packet.cname;

		if( (pDisplay->WaveformInfo[i]->StaName.CompareNoCase(Sta)==0) &&
            (pDisplay->WaveformInfo[i]->ChanName.CompareNoCase(Chan)==0) )
			{
			CDataBlock *xfd=new CDataBlock;
			if(xfd!=NULL)
				{
				for(int j=0; j<packet.nsamp;++j) xfd->data[j]=packet.data[j];

				lstrcpy(xfd->sname,packet.sname);
				xfd->lat=packet.lat;
				xfd->lon=packet.lon;
				xfd->elev=packet.elev ;
				xfd->depth=packet.depth;

				lstrcpy(xfd->cname,packet.cname);
				xfd->sint=packet.sint;
				xfd->beg=packet.beg;
				xfd->tear=packet.tear;
				xfd->nsamp=packet.nsamp;
				xfd->end=packet.end;

				pDisplay->WaveformInfo[i]->Add(xfd);
				}
			}
		}
	pDisplay->Critical.Unlock();

	}
	closesocket(sd);
	}
	CWnd *pW=pDisplay->GetParent();
	pW->PostMessage(WM_COMMAND,MAKEWPARAM(IDC_THREADEXIT,0),(LPARAM)pDisplay->m_hWnd);
	return 0;
	}
