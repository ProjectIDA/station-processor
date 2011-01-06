#include <qapplication.h>
#include <qmessagebox.h>
#include "GettingDataThread.h"
#include "util.h"
#include "isi.h"
#include "cssio.h"
#include "isitap_globals.h"
#include "isitap_time.h"
#include "wfdisc.h"
#include "ReadFileCache.h"
#include "isiTapResults.h"
#include "nrtsinfo.h"
#include "isitapdlg.h"
#include "prcinfo.h"

int ISI_GENERIC_TS_WrtPacket(int nFileHandle,  ISI_GENERIC_TS *packet);
int ISI_GENERIC_TS_RdPacket(CReadFileCache &FC, ISI_GENERIC_TS *packet);

void ThreadExit(isiTapResults *p)
	{
	QCustomEvent *event = new QCustomEvent(p->LOG);
	prcInfo *pinfo = new prcInfo;
	pinfo->sInfo   = "Done";
	pinfo->percent = 100;
	
	event->setData(pinfo);
	QApplication::postEvent(p, event);


	event = new QCustomEvent(p->STOP);
	QApplication::postEvent(p, event);
	}


void TypeStatus(isiTapResults *p, ISI_GENERIC_TS *packet)
	{
	char cbuff[1024];
	QString cMsg;
//	int nPos;
//	nPos=(int)(100.*(packet->hdr.tols.value-pIsiTapDlg->tbeg)/(pIsiTapDlg->tend-pIsiTapDlg->tbeg));
//	pIsiTapDlg->m_progress.SetPos(nPos);
	utilDttostr(packet->hdr.tols.value,0,cbuff);
	cMsg.sprintf("%s:%s %s", packet->hdr.name.sta, packet->hdr.name.chn,cbuff);
//	pIsiTapDlg->m_infotext.SetWindowText(cMsg);
	QCustomEvent *event = new QCustomEvent(p->LOG);
	prcInfo *pinfo = new prcInfo;
	pinfo->sInfo   = cMsg;
	pinfo->percent = ((packet->hdr.tols.value-tbeg)/(tend-tbeg))*100.;
	
	event->setData(pinfo);
	QApplication::postEvent(p, event);
	}

void ThreadErrorExit(isiTapResults *p, const char * lpErrMsg)
	{

	QCustomEvent *event = new QCustomEvent(p->LOG);
	prcInfo *pinfo = new prcInfo;
	pinfo->sInfo   = lpErrMsg;
	pinfo->percent = -1;
	
	event->setData(pinfo);
	QApplication::postEvent(p, event);

	event = new QCustomEvent(p->STOP);
	QApplication::postEvent(p, event);

	}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



GettingDataThread::GettingDataThread()
	{

	}

GettingDataThread::~GettingDataThread()
	{

	}
void GettingDataThread::Init(isiTapResults *p)
	{
	pisiTapResults = p;
	}
void GettingDataThread::run()
	{
	CWfdisc *wfd;
	char *pDataType;
	const char *sta, *chan, *loc;
	int status;
	ISI *isi=NULL;
	ISI_GENERIC_TS *ts;
	ISI_GENERIC_TS packet;
	ISI_DATA_REQUEST *dreq;
	int compress  = ISI_COMP_IDA;
	int format    = ISI_FORMAT_GENERIC;
	char *StreamSpec;
	QString streams;
	QString key,keych;
	char cCN[64];
	ISI_PARAM par;
	CReadFileCache RCache;
	int nFileHanle;
	char cTempFile[L_tmpnam];
	isiTapDlg *pISITapDlg = (isiTapDlg *)pisiTapResults->parentWidget();


	tmpnam(cTempFile);
	nFileHanle = open(cTempFile, O_RDWR|O_CREAT/*|O_TEMPORARY*/|O_BINARY, 0666 );

	if ( nFileHanle < 0)
		{
		ThreadErrorExit(pisiTapResults, "Temporary file creation error");
		unlink(cTempFile);
		return ;
		}
  


    utilNetworkInit();
    isiInitDefaultPar(&par);
	par.port = nNrtsPort;


	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;
   
//***********************************************************

   for(si=pISITapDlg->NrtsInfo.StaInfo.first(); si; si=pISITapDlg->NrtsInfo.StaInfo.next())
		{
		for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
			{

			if(ci->bSelected)
				{
				CWfdisc *wfd=new CWfdisc();
				
				wfd->SetWorkingDir(WfdiscDir);

				wfd->wfd.smprate = (float)ci->dSpS;
				strcpy(wfd->wfd.chan, ci->Chan);
				strcat(wfd->wfd.chan, ci->LCODE);
				strcpy(wfd->wfd.sta, si->Sta);

				strcpy(wfd->wfd.dfile, si->Sta);
				strcat(wfd->wfd.dfile,".");
				strcat(wfd->wfd.dfile, ci->Chan);
				strcat(wfd->wfd.dfile,".");

				if(ci->LCODE.compare("  ")==0)
					{
					strcat(wfd->wfd.dfile, "__");
					}
				else
					{
					strcat(wfd->wfd.dfile, ci->LCODE);
					}

				strcat(wfd->wfd.dfile,".w");
				strcpy(wfd->wfd.dir,".");
				wfd->wfd.foff=0;

				wfdarr.append(wfd);
				QString channelName="+";
				channelName+=si->Sta+"."+ci->Chan+"."+ci->LCODE;
				streams+=channelName;
				}

			}

		}


	StreamSpec = (char *)streams.latin1();


	if ((dreq = isiAllocSimpleDataRequest(tbeg, tend, StreamSpec)) == NULL) 
		{
		ThreadErrorExit(pisiTapResults, "Alloc Simple Data Request error");
		close(nFileHanle);
		unlink(cTempFile);
		return ;
		}
	isiSetDatreqFormat(dreq, format);
	isiSetDatreqCompress(dreq, compress);


	if ((isi = isiInitiateDataRequest((char *)server.latin1(), &par, dreq)) == NULL) 
		{
		ThreadErrorExit(pisiTapResults, "Initiate Data Request error");
		close(nFileHanle);
		unlink(cTempFile);
		return ;
		}

	isiFreeDataRequest(dreq);


	while (TRUE) 
		{
		ts = isiReadGenericTS(isi, &status);
		if(status==ISI_DONE||status==ISI_BREAK)
			{
			break;
			}
		if(status==ISI_ERROR)
			{
			char cMsg[256];
			sprintf(cMsg,"NRTS server i/o error\n Press: \n'Ignore' to convert received packets\n'Retry' to request rest of data\n'Abort' to abort data transmission and drop received data");
			int nRes =QMessageBox::question(pISITapDlg,"", cMsg,QMessageBox::Retry, QMessageBox::Abort, QMessageBox::Ignore );
			
			if(nRes==QMessageBox::Abort)
				{
				close(nFileHanle);
				isiClose(isi);
				unlink(cTempFile);
				return ;
				}
			else
				{
				if(nRes==QMessageBox::Retry)
					{
					continue;
					}
				else
					{
					if(nRes==QMessageBox::Ignore)
						{
						break;
						}
					}


				}
			}


		double sint;

		sint = isiSrateToSint(&ts->hdr.srate);

		sta  = ts->hdr.name.sta;
		chan = ts->hdr.name.chn;
		loc  = ts->hdr.name.loc;

		int nsamp = ts->hdr.nsamp;
		long *data = (long *)ts->data;

		int nRes = ISI_GENERIC_TS_WrtPacket(nFileHanle, ts);

		if(nRes<0)
			{
			ThreadErrorExit(pisiTapResults, "Temporary file write error");
			isiClose(isi);
			break;
			}
		TypeStatus(pisiTapResults, ts);

		}
	isiClose(isi);

	lseek(nFileHanle,0,SEEK_SET);

	RCache.SetFileHandle(nFileHanle);

	int nElem=wfdarr.count();
	unlink(wfdFilePath);

//	pISITapDlg->m_infotext.SetWindowText("Decoding data");
	while(ISI_GENERIC_TS_RdPacket(RCache, &packet)>0)
		{
		sta  = packet.hdr.name.sta;
		chan = packet.hdr.name.chn;
		loc  = packet.hdr.name.loc;
		strcpy(cCN, chan);
		strcat(cCN, loc);
		for (wfd=wfdarr.first();wfd;wfd=wfdarr.next()) 
			{
			char *pS=wfd->wfd.sta;
			char *pC=wfd->wfd.chan;
			
			if(qstricmp(pS,sta)!=0) continue;
			if(qstricmp(pC,cCN)!=0) continue;

			pDataType=cssio_datatype(packet.hdr.desc.size, ((nOutputByteOrder==0)?LTL_ENDIAN_ORDER:BIG_ENDIAN_ORDER), 1);
			strcpy(wfd->wfd.datatype,pDataType);
			double dt   = fabs(packet.hdr.tofs.value-wfd->wfd.endtime);
			double sint = isiSrateToSint(&packet.hdr.srate);
			if(dt>1.1*sint || dt<0.9*sint)
				{
				if(wfd->wfd.nsamp>0)
					{
					if(wfd->WriteWfdisk(wfdFilePath)<0)
						{
						ThreadErrorExit(pisiTapResults, "Wfdisc write error");
						isiDestroyGenericTS(&packet);
						close(nFileHanle);
						unlink(cTempFile);
						return ;
						}
					}
				wfd->wfd.time    = packet.hdr.tofs.value;
				wfd->wfd.nsamp   = 0;
				wfd->wfd.endtime = 0;
				wfd->wfd.time = packet.hdr.tofs.value;
				if(wfd->WriteData(&packet, pDataType)<0)
					{
					ThreadErrorExit(pisiTapResults, "Data file write error");
					isiDestroyGenericTS(&packet);
					close(nFileHanle);
					unlink(cTempFile);
					return ;
					}

				}
			else
				{
				if(wfd->WriteData(&packet, pDataType)<0)
					{
					ThreadErrorExit(pisiTapResults, "Data file write error");
					isiDestroyGenericTS(&packet);
					close(nFileHanle);
					unlink(cTempFile);
					return ;
					}
				}
			isiDestroyGenericTS(&packet);
			break;
			}
		}
//	pISITapDlg->m_infotext.SetWindowText("Writing wfdisc");
	int nWfdRec=0;
	for (wfd=wfdarr.first();wfd;wfd=wfdarr.next()) 
		{
		wfd->CloseFileHandle();
		if(wfd->wfd.smprate<0) continue;
		if(wfd->wfd.nsamp<0)   continue;
		if(wfd->WriteWfdisk(wfdFilePath)<0)
			{
			close(nFileHanle);
			ThreadErrorExit(pisiTapResults, "Wfdisc write error");
			unlink(cTempFile);
			return ;
			}
		++nWfdRec;
		}
	if(nWfdRec==0)
		{
		ThreadErrorExit(pisiTapResults, "No data available");
		close(nFileHanle);
		unlink(cTempFile);
		return ;
		}
/*	MessageBeep(0);*/
	ThreadExit(pisiTapResults);
	unlink(cTempFile);
	return ;

	close(nFileHanle);
	unlink(cTempFile);
	return ;
	}




void ClearStatus(isiTapDlg *pIsiTapDlg)
	{
/*	pIsiTapDlg->m_infotext.SetWindowText("");
	pIsiTapDlg->m_progress.SetPos(0);
	pIsiTapDlg->m_infotext.ShowWindow(FALSE);
	pIsiTapDlg->m_progress.ShowWindow(FALSE);*/
	}

void ActivateStatus(isiTapDlg *pIsiTapDlg)
	{
/*	pIsiTapDlg->m_infotext.ShowWindow(TRUE);
	pIsiTapDlg->m_progress.ShowWindow(TRUE);
	pIsiTapDlg->m_infotext.EnableWindow(TRUE);
	pIsiTapDlg->m_progress.EnableWindow(TRUE);*/
	}