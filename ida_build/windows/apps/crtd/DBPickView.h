#if !defined(AFX_DBPICKVIEW_H__14683239_3808_11D4_8D75_00B0D0233922__INCLUDED_)
#define AFX_DBPICKVIEW_H__14683239_3808_11D4_8D75_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBPickView.h : header file
//
#include "RTDisplay.h"
#include "MWindowX.h"


/////////////////////////////////////////////////////////////////////////////
// CDBPickView frame

class CDBPickView : public CFrameWnd
{
	DECLARE_DYNCREATE(CDBPickView)

private:
static int TimeBarYsize;
static int WindowInfoXsize;

	CRTDisplay *pRTDisplay;

public:
	CDBPickView(CRTDisplay *pRTD);           // protected constructor used by dynamic creation
	CDBPickView::CDBPickView();

public:
	CBitmap Bitmap; // off screen bitmap
	CArray<CMWindowX *, CMWindowX *> WaveformInfo; // array of all windows 

	CCriticalSection ScreenBitmapCriticalSection;

	int MaxX, MaxY;

	double t1,t2;
	double tB1,tB2;
	int nTimerCount;
	int nActiveScreen;
// Operations
public:

	void AddWindow(CString Sta, CString Chan, double dSpS);
	void RemoveWindow(CString Sta, CString Chan);
	void Run();
	void Stop();
	void DrawWaveforms();
	void OnTimer();
	void RemoveOldData();
	void OnAutoScale();
	void OnExpandVert();
	void OnContractVert();
	void OnShiftUp();
	void OnShiftDown();


private:
	int nLastMarkRightPos; 

protected:
	void SetTime();
	void CalculateWindowsSize();
	void ClearWaveformInfo();
	void DrawFocused(CDC *pDC);
	void GetTextRectangle(LPSTR lpText, RECT &rc);
	void DrawTimePanel(CDC *pDC);
	void DrawWindow(int nWindow, CDC *pDC);
	void ClearDisplay();
	void ApplyAutoscale(int nWindow);
	void DrawGaps(int nWindow, CDC *pDC);
	void ApplyExpand(int nWindow);
	void ApplyContract(int nWindow);




// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBPickView)
	public:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDBPickView();

	// Generated message map functions
	//{{AFX_MSG(CDBPickView)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBPICKVIEW_H__14683239_3808_11D4_8D75_00B0D0233922__INCLUDED_)
