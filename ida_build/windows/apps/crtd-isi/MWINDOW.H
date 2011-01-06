// MWindow.h: interface for the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MWINDOW_H__F0C2AFB5_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_MWINDOW_H__F0C2AFB5_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DataBlock.h"
class CMWindow  
{
public:
	unsigned long nBlocksRec;
	int x1,x2,y1,y2;
	int xw1,xw2,yw1,yw2;
	CString StaName;
	CString ChanName;
	CString LCODE;
	double dSpS;
	long amin,amax;
	CArray<CDataBlock *, CDataBlock *> xferData;

public:
	CMWindow();
	virtual ~CMWindow();
	Add(CDataBlock *dblk);

};

#endif // !defined(AFX_MWINDOW_H__F0C2AFB5_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
