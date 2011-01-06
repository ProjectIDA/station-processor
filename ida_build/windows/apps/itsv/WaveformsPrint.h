// WaveformsPrint.h: interface for the CWaveformsPrint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEFORMSPRINT_H__8275CCA4_63EF_11D4_8D8D_00B0D0233922__INCLUDED_)
#define AFX_WAVEFORMSPRINT_H__8275CCA4_63EF_11D4_8D8D_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WaveformsDisplay.h"

class CWaveformsPrint  
	{
public:
	CWaveformsPrint(CWaveformsDisplay *pWaveformsDisplay);
	virtual ~CWaveformsPrint();
	void Print();

public:

private:
	float p_mm_x, p_mm_y;
	int MaxX, MaxY, nHeadHeight;
	int topMargin;
	int TimeBarYsize,WindowInfoXsize;
private:
	void PrintWaveforms(CDC *pDC,CString s);
	void PrintStatus(CDC *pDC);
	void PrintLabelStringX(CDC *pDC, int x, int y, LPSTR s);
	void GetTextRectangle(CDC *pdc, LPSTR lpText, RECT &rc, UINT nFormat=DT_SINGLELINE);
	void DrawTimeMarks(CDC *pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2);

private:
	CWaveformsDisplay *pWD;
	};

#endif // !defined(AFX_WAVEFORMSPRINT_H__8275CCA4_63EF_11D4_8D8D_00B0D0233922__INCLUDED_)
