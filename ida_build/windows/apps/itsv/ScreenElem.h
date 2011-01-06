// ScreenElem.h: interface for the CScreenElem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENELEM_H__A13B7119_3AFD_11D4_8D76_00B0D0233922__INCLUDED_)
#define AFX_SCREENELEM_H__A13B7119_3AFD_11D4_8D76_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScreenElem  
{
public:
	CScreenElem(int);
	virtual ~CScreenElem();

public:
	int state;
	int nWfdRec;
	int nSel;

};

#endif // !defined(AFX_SCREENELEM_H__A13B7119_3AFD_11D4_8D76_00B0D0233922__INCLUDED_)
