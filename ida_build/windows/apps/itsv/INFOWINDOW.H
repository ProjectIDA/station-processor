#if !defined(AFX_INFOWINDOW_H__21ABF790_0F56_11D3_B69F_DCF7A5000000__INCLUDED_)
#define AFX_INFOWINDOW_H__21ABF790_0F56_11D3_B69F_DCF7A5000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InfoWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoWindow window

class CInfoWindow : public CWnd
{
// Construction
public:
	CInfoWindow();
	void SetText(CString cs);

	CString sText;
	CRect rcc;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInfoWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoWindow)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOWINDOW_H__21ABF790_0F56_11D3_B69F_DCF7A5000000__INCLUDED_)
