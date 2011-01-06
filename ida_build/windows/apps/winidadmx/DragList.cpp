// DragList.cpp : implementation file
//

#include "stdafx.h"
#include "winidadmx.h"
#include "DragList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragList

CDragList::CDragList()
	{
	m_bDragging=false;
	}

CDragList::~CDragList()
{
}


BEGIN_MESSAGE_MAP(CDragList, CListCtrl)
	//{{AFX_MSG_MAP(CDragList)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragList message handlers

/*void CDragList::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_bDragging=true;
	SetCapture();

	int m_nDragIndex = pNMListView->iItem;

	POINT pt;
    int nOffset = 10; //offset in pixels for drag image (up and 
                          //to the left)
    pt.x = nOffset;
    pt.y = nOffset;

    m_pDragImage = CreateDragImage(m_nDragIndex, &pt);
    ASSERT(m_pDragImage); //make sure it was created
    m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset));
    m_pDragImage->DragEnter(GetDesktopWindow(), pNMListView->ptAction);	



	
	*pResult = 0;
	}*/


void CDragList::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CDropFiles DropFiles;
	
	if(!PrepareFileBuff(DropFiles)){
		ASSERT(0);
	}

	COleDropSource DropSource;
	COleDataSource DropData;

	HGLOBAL hMem = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, DropFiles.GetBuffSize());		
	memcpy( (char*)::GlobalLock(hMem), DropFiles.GetBuffer(), DropFiles.GetBuffSize() );
	::GlobalUnlock(hMem);

	DropData.CacheGlobalData( CF_HDROP, hMem );
	DROPEFFECT de = DropData.DoDragDrop(DROPEFFECT_COPY,NULL);

	if(de == DROPEFFECT_COPY){
		// Copy files if CTRL btn is hold;
	}
	else{
		// Move files, as default;
	}

	
	*pResult = 0;
	}




void CDragList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	if(nChar==VK_INSERT)
		{
		SHORT shState=GetAsyncKeyState(VK_CONTROL);
		if(shState!=0)
			{
			CopyToClipboard();
			MessageBeep(0);
			}
		}
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}


void CDragList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
	}


void CDragList::CopyToClipboard()
	{
	CDropFiles DropFiles;

	if(!PrepareFileBuff(DropFiles)){
		return;
	}
	
	if (OpenClipboard())
		{
		BeginWaitCursor();
		::EmptyClipboard();
		
		HGLOBAL hMem = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, DropFiles.GetBuffSize());		
		memcpy( (char*)::GlobalLock(hMem), DropFiles.GetBuffer(), DropFiles.GetBuffSize() );
		::SetClipboardData (CF_HDROP, hMem  );

		::CloseClipboard();
		EndWaitCursor();
		}


	}

void CDragList::SetDataDirectory( const CString &sDatDir)
	{
	m_dataDirectory = sDatDir;
	}
BOOL CDragList::PrepareFileBuff(CDropFiles& DropFiles) const
	{
	CString s,sfile,sDir = m_dataDirectory;
	sDir.TrimRight();
	sDir.TrimLeft();
	if(sDir.Right(1)!="\\") sDir+="\\";

	POSITION pos = GetFirstSelectedItemPosition();

	if( pos == NULL){
		return FALSE;
	}

	while (pos) 
		{		
		int index = GetNextSelectedItem(pos);
		s = GetItemText(index,0);
		sfile = sDir + s;

		DropFiles.AddFile( sfile );
		}

	DropFiles.CreateBuffer();

	return TRUE;
}
