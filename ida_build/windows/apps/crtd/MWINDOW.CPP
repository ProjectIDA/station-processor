// MWindow.cpp: implementation of the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crtd.h"
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
	nBlocksRec=0;
	}

CMWindow::Add(CDataBlock *dblk)
	{
/*	if(nBlocksRec%25!=0)*/	xferData.Add(dblk);
	++nBlocksRec;
	}
CMWindow::~CMWindow()
	{
	while(xferData.GetSize()>0)
		{
		CDataBlock *dblk=xferData[0];
		xferData.RemoveAt(0);
		delete dblk;
		}

	}
