// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AA568B88_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_MAINFRM_H__AA568B88_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WaveformsDisplay.h"
#include "WaveformPreview.h"
 
class CMainFrame : public CFrameWnd
{
public: 
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
	CWaveformsDisplay	m_Display;
	CWaveformPreview	m_Preview;
	CToolTipCtrl tooltip;
	CMenu MaiFrameMenu;


public:
	void OnSetTextToStatusbar1(LPSTR lpText);



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nDatePaneNo;
	int m_nTimePaneNo;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	afx_msg void OnUpdateView();

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRepaintPreview();
	afx_msg void OnFileOpen();
	afx_msg void OnStart();
	afx_msg void OnSelectChanSta();
	afx_msg void OnFitToWindow();
	afx_msg void OnExpandHorizontal();
	afx_msg void OnContractHorizontal();
	afx_msg void OnContractVertical();
	afx_msg void OnExpandVertical();
	afx_msg void OnShiftLeft();
	afx_msg void OnShiftRight();
	afx_msg void OnDown();
	afx_msg void OnUp();
	afx_msg void OnAlignVertical();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelecttraces();
	afx_msg void OnOriginal();
	afx_msg void OnSelectall();
	afx_msg void OnDeleteall();
	afx_msg void OnAutoamp();
	afx_msg void OnAmpfixed();
	afx_msg void OnCliponoff();
	afx_msg void OnAutoa();
	afx_msg void OnAuto0();
	afx_msg void OnAuto1();
	afx_msg void OnInvert();
	afx_msg void OnSetgain1();
	afx_msg void OnChangeGain( UINT nID );
	afx_msg void OnSetFilter(UINT nID );
	afx_msg void OnSettime();
	afx_msg void OnExit();
	afx_msg void OnXfit();
	afx_msg void OnDeletetraces();
	afx_msg void OnFilternone();
	afx_msg void OnPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bMenuSelect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AA568B88_08E2_11D3_B68A_941D97000000__INCLUDED_)
