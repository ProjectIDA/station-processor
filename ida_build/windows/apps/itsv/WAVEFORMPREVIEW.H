#if !defined(AFX_WAVEFORMPREVIEW_H__AA568B95_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_WAVEFORMPREVIEW_H__AA568B95_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaveformPreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaveformPreview window

#include "WaveformsDisplay.h"


class CWaveformPreview : public CWnd
{
// Construction
public:
	CWaveformPreview(CWaveformsDisplay *pDisplay);

// Attributes
public:

// Operations
public:
	void DrawPreviewBitmap();
	void DrawWaveform(CDC &MemDC);


// Attributes
public:
	CBitmap Bitmap;
	int MaxX, MaxY;
	CWaveformsDisplay *pDisplay;
	int x1,x2,y1,y2,xw1, xw2, yw1, yw2;
	long amax,amin;
private:
	CCriticalSection ScreenBitmapCriticalSection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveformPreview)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWaveformPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWaveformPreview)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEFORMPREVIEW_H__AA568B95_08E2_11D3_B68A_941D97000000__INCLUDED_)
