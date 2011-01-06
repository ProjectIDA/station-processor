#if !defined(AFX_DRAGLIST_H__13E6FDC8_AA4F_4965_87D3_957D097D15D2__INCLUDED_)
#define AFX_DRAGLIST_H__13E6FDC8_AA4F_4965_87D3_957D097D15D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragList.h : header file
//
#include "DropFiles.h"

/////////////////////////////////////////////////////////////////////////////
// CDragList window

class CDragList : public CListCtrl
{
// Construction
public:
	CDragList();

// Attributes
public:

// Operations
public:
	void SetDataDirectory( const CString &);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragList)
	//}}AFX_VIRTUAL


protected:
	bool m_bDragging;
	CImageList* m_pDragImage;
	CString m_dataDirectory;


// Implementation

public:
	virtual ~CDragList();
protected:
	void CopyToClipboard();
	BOOL PrepareFileBuff(CDropFiles& DropFiles) const;


	// Generated message map functions
protected:
	//{{AFX_MSG(CDragList)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGLIST_H__13E6FDC8_AA4F_4965_87D3_957D097D15D2__INCLUDED_)
