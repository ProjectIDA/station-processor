#if !defined(AFX_SELECTCHAN_H__F0C2AFB2_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_SELECTCHAN_H__F0C2AFB2_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectChan.h : header file
//

#include "nrtsinfo.h"
#include "NRTSSTAINFO.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectChan dialog

class CSelectChan : public CPropertyPage
{

// Construction
public:
	CSelectChan(CNRTSINFO &NrtsInfo);
	~CSelectChan();
public:
	CNRTSINFO NrtsInfoX;
	CImageList StateImage;
	CString ActiveSta;

public:
	void SetStaHeader();
	void SetChanHeader();
	void LoadStaList();
	void LoadChanList(CString sSta);
	BOOL OnChanListNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void SetChanDrawState(const CString &sSta, const CString &sChan, const CString &LCODE, BOOL bDraw);
	BOOL bHasSelectedChan(CString sSta);
	void SetStationCheck();

// Dialog Data
	//{{AFX_DATA(CSelectChan)
	enum { IDD = IDD_SELECT };
	CListCtrl	m_sta;
	CListCtrl	m_chan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSelectChan)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSelectChan)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTCHAN_H__F0C2AFB2_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
