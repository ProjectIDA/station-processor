#if !defined(AFX_RTDISPLAY_H__79C61541_FD47_11D2_8660_FE895B000000__INCLUDED_)
#define AFX_RTDISPLAY_H__79C61541_FD47_11D2_8660_FE895B000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RTDisplay.h : header file
//
#include "MWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CRTDisplay window
const int WindowInfoXsize=100;

class CRTDisplay : public CWnd
{
// Construction
public:
	CRTDisplay();

// Attributes
public:
	CBitmap Bitmap; // off screen bitmap
	CArray<CMWindow *, CMWindow *> WaveformInfo; // array of all waveforms 
	CArray<int , int > tmmarks; // array time - marks 

	CCriticalSection Critical;
	CCriticalSection ScreenBitmapCriticalSection;

	int MaxX, MaxY;
	BOOL bRepaint;

	double t1,t2;
	double tB1,tB2;
	int nTimerCount;
	int nActiveScreen;
	int TimeBarYsize;

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
	void DrawTimeMark(int x, double tt, CDC *pDC);
	void DrawTimeBounds(CDC *pDC);
	int nWindowFromPoint(int x, int y);




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

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTDisplay)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRTDisplay();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRTDisplay)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTDISPLAY_H__79C61541_FD47_11D2_8660_FE895B000000__INCLUDED_)
