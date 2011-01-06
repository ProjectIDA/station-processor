#if !defined(AFX_VIEWRESULTS_H__3F01069D_59E1_11D4_8D85_00B0D0233922__INCLUDED_)
#define AFX_VIEWRESULTS_H__3F01069D_59E1_11D4_8D85_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewResults.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewResults dialog

class CViewResults : public CDialog
{
// Construction
public:
	CViewResults(LPCSTR s, CWnd* pParent = NULL);   // standard constructor
	CString CFileName;
	void ParserForOutputFileName();
	LPCSTR text;
// Dialog Data
	//{{AFX_DATA(CViewResults)
	enum { IDD = IDD_VIEWRESULTSDLG };
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewResults)
	virtual void OnOK();
	afx_msg void OnRunviewdata();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWRESULTS_H__3F01069D_59E1_11D4_8D85_00B0D0233922__INCLUDED_)
