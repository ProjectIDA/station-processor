// MWindow.cpp: implementation of the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "MWindow.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMWindow::CMWindow()
	{
	amin=-3000;
	amax=3000;
	nBlocksRec=0;
	}

void CMWindow::Add(CDataBlock *dblk)
	{
/*	if(nBlocksRec%25!=0)*/	xferData.append(dblk);
	++nBlocksRec;
	}
CMWindow::~CMWindow()
	{
	while(xferData.count()>0)
		{
		CDataBlock *dblk=xferData.first();
		xferData.removeFirst();
		delete dblk;
		}

	}
