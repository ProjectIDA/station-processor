// MWindowX.h: interface for the CMWindowX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MWINDOWX_H__1468323A_3808_11D4_8D75_00B0D0233922__INCLUDED_)
#define AFX_MWINDOWX_H__1468323A_3808_11D4_8D75_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMWindowX  
{
public:
	int x1,x2,y1,y2;
	int xw1,xw2,yw1,yw2;
	CString StaName;
	CString ChanName;
	CString LCODE;
	double dSpS;
	long amin,amax;

	CMWindowX();
	virtual ~CMWindowX();

};

#endif // !defined(AFX_MWINDOWX_H__1468323A_3808_11D4_8D75_00B0D0233922__INCLUDED_)
