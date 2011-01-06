// Screen.h: interface for the CScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREEN_H__A13B7115_3AFD_11D4_8D76_00B0D0233922__INCLUDED_)
#define AFX_SCREEN_H__A13B7115_3AFD_11D4_8D76_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScreenElem.h"

struct SREANELEM
	{
	int nPos;
	int nWfdRec;
	};


class CScreen  
{
public:
	CScreen();
	virtual ~CScreen();
	AddElement(int nRec);
	int GetMaxWindow();
	int SelectUnselectElement(int nWindow);
	int GetWaveformN(int n);
	void Clear();
	bool IsSelected(int n);
	void SelectTraces();
	void DeleteTraces();

	void UnSelectAll();
	void SelectAll();
	int GetSelectionNumber(int n);

private:	

	CArray<CScreenElem  *, CScreenElem  *> Screen;
	int nMaxSel;
};

#endif // !defined(AFX_SCREEN_H__A13B7115_3AFD_11D4_8D76_00B0D0233922__INCLUDED_)
