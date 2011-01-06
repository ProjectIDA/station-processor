#if !defined(AFX_SELECTFILESONCDDLG_H__F62E9547_8C18_41A9_B2E9_2CB51E682392__INCLUDED_)
#define AFX_SELECTFILESONCDDLG_H__F62E9547_8C18_41A9_B2E9_2CB51E682392__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectFilesOnCDDlg.h : header file
//
#include "DialogWB.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectFilesOnCDDlg dialog

class CSelectFilesOnCDDlg : public CDialogWB
{
// Construction
public:
	CSelectFilesOnCDDlg(LPCSTR lpcCDDISK, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectFilesOnCDDlg();
	void LoadInfoFromDisk();
	void LoadFileNames();
	void SetListHeader();
	void SelUnselFiles(BOOL bSel);



	CString cDisk;
	CStringList FilesList;
	CString Station;
	int nRelease;

// Dialog Data
	//{{AFX_DATA(CSelectFilesOnCDDlg)
	enum { IDD = IDD_SELECTFILESONCD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectFilesOnCDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int MakeFilesList();

	// Generated message map functions
	//{{AFX_MSG(CSelectFilesOnCDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelall();
	afx_msg void OnUnselall();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTFILESONCDDLG_H__F62E9547_8C18_41A9_B2E9_2CB51E682392__INCLUDED_)
