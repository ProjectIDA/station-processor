#if !defined(AFX_OPENFILEDLG_H__D6E45482_10E6_11D3_B6A4_32ED2A000000__INCLUDED_)
#define AFX_OPENFILEDLG_H__D6E45482_10E6_11D3_B6A4_32ED2A000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OpenFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OpenFileDlg dialog

class OpenFileDlg : public CFileDialog
{
	DECLARE_DYNAMIC(OpenFileDlg)

public:
	OpenFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

		void OnFileNameChange();
		void OnInitDone();
		void SetListHeader();
		void LoadWfdiscRecords(LPSTR lpFileName);

public:

protected:
	//{{AFX_MSG(OpenFileDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENFILEDLG_H__D6E45482_10E6_11D3_B6A4_32ED2A000000__INCLUDED_)
