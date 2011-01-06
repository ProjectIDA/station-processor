// DialogWB.cpp : implementation file
//

#include "stdafx.h"
#include "DialogWB.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogWB dialog


CDialogWB::CDialogWB(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
	{
	}
CDialogWB::~CDialogWB()
	{
    while(ButtonsList.GetSize()>0)
		{
        CShadeButtonST *pBt=ButtonsList[0];
        ButtonsList.RemoveAt(0);
        delete pBt;
        }
	}

CShadeButtonST * CDialogWB::AddFunctionHelpButton(int nId, CString st, bool bFlat)
        {
        CShadeButtonST *pBt=new CShadeButtonST;

        pBt->SubclassDlgItem(nId,this);
        pBt->SetTooltipText((LPCSTR)st,TRUE);
		COLORREF crStandard = ::GetSysColor(COLOR_BTNFACE);
//		pBt->SetActiveBgColor(crStandard + RGB(20,20,20));
//		pBt->SetActiveFgColor(RGB(255,0,0));
        ButtonsList.Add(pBt);
        pBt->SetBtnCursor(IDC_HAND);
//        pBt->SetInactiveFgColor(RGB(0,0,0));
		pBt->SetFlat(bFlat);
		pBt->SetShade(CShadeButtonST::SHS_SOFTBUMP);
		return pBt;
        }


