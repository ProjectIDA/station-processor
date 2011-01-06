#if !defined(AFX_BROWSESUBDIRDLG_H__F66F880D_3ED8_4631_89EF_F19FE699686D__INCLUDED_)
#define AFX_BROWSESUBDIRDLG_H__F66F880D_3ED8_4631_89EF_F19FE699686D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrowseSubdirDlg.h : header file
//
#include "SubEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CBrowseSubdirDlg dialog

#include "DialogWB.h"

class CBrowseSubdirDlg : public CDialogWB
{
// Construction
public:
	void LoadDirectoryList();
	CBrowseSubdirDlg(LPCSTR lpDir, CWnd* pParent = NULL);   // standard constructor
	void SetListHeader();
	void InitTreeImage();
	void SetFileListHeader();
	void LoadFileList(LPCSTR lpDir);
	void ClearFileList();
	void ClearPreview();
	void SetPreviewListHeader();
	void LoadPreview(LPCSTR);
	CString FindFirstWfdisc(LPCSTR sSub);
	BOOL DeleteDirectory(LPCSTR lpDir);
	void LoadDirectoryInfo(int nItem);
	void MoveSelectedDirTo(LPCSTR lpPath);



	CString sDirectory;
	CString m_subdir;
	CImageList m_stateImage;
	CSubEdit m_editWnd;
	int m_editedItem;

// Dialog Data
	//{{AFX_DATA(CBrowseSubdirDlg)
	enum { IDD = IDD_BROWSESUBDIR };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseSubdirDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowseSubdirDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedSubdirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeldir();
	afx_msg void OnMovedir();
	afx_msg void OnBeginlabeleditSubdirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditSubdirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickSubdirlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRenamedir();
	afx_msg void OnDblclkSubdirlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSESUBDIRDLG_H__F66F880D_3ED8_4631_89EF_F19FE699686D__INCLUDED_)
