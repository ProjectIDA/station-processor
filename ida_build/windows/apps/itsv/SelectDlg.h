#if !defined(AFX_SELECTDLG_H__AA568B9C_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_SELECTDLG_H__AA568B9C_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog
#include "StaInfo.h"

class CSelectDlg : public CDialog
{
// Construction
public:
	CSelectDlg(CWnd* pParent=NULL);   // standard constructor

public:
	CStaInfo StaInfo;
	CImageList StateImage;
	CString ActiveSta;

public:
	void SetStaHeader();
	void SetChanHeader();
	void LoadStaList();
	void LoadChanList(CString sSta);
	BOOL OnChanListNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void SetChanDrawState(CString sSta, CString sChan, BOOL bDraw);



// Dialog Data
	//{{AFX_DATA(CSelectDlg)
	enum { IDD = IDD_SELECT };
	CListCtrl	m_sta;
	CListCtrl	m_chan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDLG_H__AA568B9C_08E2_11D3_B68A_941D97000000__INCLUDED_)
