#if !defined(AFX_DIALOGWB_H__B8AB6ADE_D4FD_45FC_99EF_320A8945F7F0__INCLUDED_)
#define AFX_DIALOGWB_H__B8AB6ADE_D4FD_45FC_99EF_320A8945F7F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogWB.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogWB dialog
#include "ShadeButtonST.h"


class CDialogWB : public CDialog
{
// Construction
public:
	CDialogWB(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogWB();
	CShadeButtonST *AddFunctionHelpButton(int nId, CString st, bool bFlat=FALSE);

private:
		CArray <CShadeButtonST *, CShadeButtonST *> ButtonsList;

};


#endif // !defined(AFX_DIALOGWB_H__B8AB6ADE_D4FD_45FC_99EF_320A8945F7F0__INCLUDED_)
