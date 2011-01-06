#if !defined(AFX_PRINTDLG_H__E4E57F32_63DC_11D4_8D8D_00B0D0233922__INCLUDED_)
#define AFX_PRINTDLG_H__E4E57F32_63DC_11D4_8D8D_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog

class CPrintDlg : public CPrintDialog
{
	DECLARE_DYNAMIC(CPrintDlg)

public:
	CPrintDlg(BOOL bPrintSetupOnly,
		// TRUE for Print Setup, FALSE for Print Dialog
		DWORD dwFlags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS
			| PD_HIDEPRINTTOFILE | PD_NOSELECTION ,
		CWnd* pParentWnd = NULL);

	CString	GetHeaderText();


	CString m_headertext;
protected:
virtual void OnOK( );
protected:
	//{{AFX_MSG(CPrintDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLG_H__E4E57F32_63DC_11D4_8D8D_00B0D0233922__INCLUDED_)
