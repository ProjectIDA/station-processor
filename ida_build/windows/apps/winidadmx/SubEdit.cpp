// SubEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SubEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubEdit

CSubEdit::CSubEdit()
{
}

CSubEdit::~CSubEdit()
{
}


BEGIN_MESSAGE_MAP(CSubEdit, CEdit)
	//{{AFX_MSG_MAP(CSubEdit)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubEdit message handlers

void CSubEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
	{
	lpwndpos->x=m_x;
	lpwndpos->y=m_y;
	lpwndpos->cy=m_cy;


	if(lpwndpos->cx<30) lpwndpos->cx=30;
	if(lpwndpos->x<=0) lpwndpos->x=0;
	if(lpwndpos->y<=0) lpwndpos->y=17;

	CEdit::OnWindowPosChanging(lpwndpos);
	
	}
