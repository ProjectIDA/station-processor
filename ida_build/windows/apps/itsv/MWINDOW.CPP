// MWindow.cpp: implementation of the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "MWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMWindow::CMWindow()
	{
	amin=-3000;
	amax=3000;
	dSpS=1.;
	}
CMWindow::CMWindow(CString sSta, CString sChan, double dSpS)
	{
	amin=-3000;
	amax=3000;
	this->dSpS=dSpS;
	StaName=sSta;
	ChanName=sChan;
	}

CMWindow::~CMWindow()
	{
	while(wfdrec.GetSize()>0)
		{
		CWfdiscRecord *dblk=wfdrec[0];
		wfdrec.RemoveAt(0);
		delete dblk;
		}

	}
void CMWindow::AddWfdRecord(wfdisc wfd)
	{
	CWfdiscRecord *wf=new CWfdiscRecord(wfd);
	wfdrec.Add(wf);
//	wf->LoadData();
	}
void CMWindow::AutoScale(double t1, double t2)
	{
	BOOL bfirstcount=TRUE;
	int nDataBlocks=wfdrec.GetSize();

	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=wfdrec[j]->wfd.time;
		double t2packet=wfdrec[j]->wfd.endtime;
		int nsamp=wfdrec[j]->wfd.nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=wfdrec[j]->GetCount(k);// lData[k];
			double tt=t1packet+(1./dSpS)*k;

			if( (tt<t1) && (t1!=-1.) ) continue;
			if( (tt>t2) && (t2!=-1.) ) break;
			if(!bfirstcount)
				{
				if(a>amax) amax=a;
				if(a<amin) amin=a;
				}
			else
				{
				bfirstcount=FALSE;
				amax=a;
				amin=a;
				}

			}
		}

	}

