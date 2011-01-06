#if !defined(AFX_MAINFRAME_H__4D1688B7_FD1A_11D2_865F_00612F000000__INCLUDED_)
#define AFX_MAINFRAME_H__4D1688B7_FD1A_11D2_865F_00612F000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MainFrame.h : header file
//
#include "RTDisplay.h"
#include "Preview.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame window

class CMainFrame : public CFrameWnd
{
// Construction
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual ~CMainFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// MainFrame frame

class MainFrame : public CFrameWnd
{
public:
	DECLARE_DYNCREATE(MainFrame)
	MainFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	CStatusBar  m_wndStatusBar;
	CToolBar    m_secondToolBar;
	CToolBar    m_wndToolBar;
	CRTDisplay	m_Display;
	CPreview	m_Preview;

private:
	int nTimerCount;

// Operations
public:
	void OnSetTextToStatusbar1(LPSTR lpText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MainFrame)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~MainFrame();

	// Generated message map functions
	//{{AFX_MSG(MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnConfig();
	afx_msg void OnRun();
	afx_msg void OnUpdateView();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStop();
	afx_msg void OnAutoscale();
	afx_msg void OnExpandVert();
	afx_msg void OnContractVert();
	afx_msg void OnShiftup();
	afx_msg void OnShiftdown();
	afx_msg void OnThreadExit();
	afx_msg void OnAutoscale1();
	afx_msg void OnAutoscaleonoff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__4D1688B7_FD1A_11D2_865F_00612F000000__INCLUDED_)
