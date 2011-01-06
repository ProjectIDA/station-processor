#if !defined(AFX_VIEWLSTDLG_H__2E3D4FB6_34BD_4DEF_83B9_C7CC18C8E660__INCLUDED_)
#define AFX_VIEWLSTDLG_H__2E3D4FB6_34BD_4DEF_83B9_C7CC18C8E660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewLstDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewLstDlg dialog

class CViewLstDlg : public CDialog
{
// Construction
public:
	CViewLstDlg(LPCSTR csText, CWnd* pParent = NULL);   // standard constructor
	LPCSTR text;
// Dialog Data
	//{{AFX_DATA(CViewLstDlg)
	enum { IDD = IDD_VIEWLSTDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewLstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewLstDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWLSTDLG_H__2E3D4FB6_34BD_4DEF_83B9_C7CC18C8E660__INCLUDED_)
