#if !defined(AFX_PREVIEW_H__D91E33C0_FF91_11D2_8666_EA57DB000000__INCLUDED_)
#define AFX_PREVIEW_H__D91E33C0_FF91_11D2_8666_EA57DB000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Preview.h : header file
//
#include "RTDisplay.h"

/////////////////////////////////////////////////////////////////////////////
// CPreview window

class CPreview : public CWnd
{
// Construction
public:
	CPreview(CRTDisplay *);
public:
	void DrawPreviewBitmap();
	void Stop();

private:
	void DrawWaveform(CDC &MemDC);
	CCriticalSection ScreenBitmapCriticalSection;


// Attributes
public:
//	CRTDisplay *pDisplay;
	CBitmap Bitmap;
	int MaxX, MaxY;
	CRTDisplay *pDisplay;
	int x1,x2,y1,y2,xw1, xw2, yw1, yw2;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreview)
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

#endif // !defined(AFX_PREVIEW_H__D91E33C0_FF91_11D2_8666_EA57DB000000__INCLUDED_)
