#if !defined(AFX_WORKINGDLG_H__3F01069F_59E1_11D4_8D85_00B0D0233922__INCLUDED_)
#define AFX_WORKINGDLG_H__3F01069F_59E1_11D4_8D85_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkingDlg dialog

class CWorkingDlg : public CDialog
{
// Construction
public:
	CWorkingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorkingDlg)
	enum { IDD = IDD_WORKINGDLG };
	CStatic	m_pic;
	//}}AFX_DATA
public:
	HANDLE hThread;
	CBitmap hB1,hB2;
	int nbpm;
	bool bExit;
	void Stop();


public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkingDlg)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWorkingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnPict();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CWorkingDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
//	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKINGDLG_H__3F01069F_59E1_11D4_8D85_00B0D0233922__INCLUDED_)
