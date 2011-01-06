// MainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "RTDisplay.h"
#include "crtd.h"
#include "MainFrame.h"
#include "SelectChan.h"
#include "NetworkPage.h"
#include "DisplayParamDlg.h"
#include "crtdglob.h"
#include "Splash.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_STATUSTXT,
};


/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNCREATE(MainFrame, CFrameWnd)

MainFrame::MainFrame():m_Preview(&m_Display)
{
}

MainFrame::~MainFrame()
{
}


BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_CONFIG, OnConfig)
	ON_COMMAND(ID_RUN, OnRun)
	ON_COMMAND(IDC_UPDATEVIEW, OnUpdateView)
	ON_WM_TIMER()
	ON_COMMAND(IDC_STOP, OnStop)
	ON_COMMAND(IDC_AUTOSCALE, OnAutoscale)
	ON_COMMAND(IDC_EXPANDVERT, OnExpandVert)
	ON_COMMAND(IDC_CONTRACTVERT, OnContractVert)
	ON_COMMAND(IDC_SHIFTUP, OnShiftup)
	ON_COMMAND(IDC_SHIFTDOWN, OnShiftdown)
	ON_COMMAND(IDC_THREADEXIT,OnThreadExit)
	ON_COMMAND(IDC_AUTOSCALE1, OnAutoscale1)
	ON_COMMAND(IDC_AUTOSCALEONOFF, OnAutoscaleonoff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers


int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	if (!m_wndStatusBar.Create(this,WS_CLIPCHILDREN|WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
		{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
		}
	
	if (!m_wndStatusBar.SetIndicators(indicators,2))
		{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
		}
	if (!m_wndToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP|CBRS_TOOLTIPS|TBSTYLE_WRAPABLE) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
		}

	{
	UINT nID, nStyle;
	int nWidth;

	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, 250);
	m_wndStatusBar.SetPaneText(1,"",TRUE);
	}


	HBRUSH hBrush=(HBRUSH)GetStockObject(LTGRAY_BRUSH);
	HCURSOR hCur=(HCURSOR)::LoadCursor(NULL,IDC_ARROW);
	HICON hIco=NULL;
	LPCTSTR lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);

	RECT rcM = { 0,0,100,100 };
	GetClientRect(&rcM);
	rcM.bottom-=10;
	if(!m_Display.Create(lpClass,"", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,rcM,this,901))	return -1;
	lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);
	if(!m_Preview.Create(lpClass,"",WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,rcM,this,902)) return -1;

//	CSplashWnd::ShowSplashScreen(this);
	return 0;
	}

void MainFrame::OnSize(UINT nType, int cx, int cy) 
	{
	CFrameWnd::OnSize(nType, cx, cy);

	RECT rcM,rc1,rc,rc2;
	int sbH,tbH;

	GetClientRect(&rcM);
	m_wndStatusBar.GetWindowRect(&rc1);
	m_wndToolBar.GetWindowRect(&rc2);

	sbH=(rc1.bottom-rc1.top);
	tbH=(rc2.bottom-rc2.top);

// Set Display window position
	rc=rcM;
	rc.bottom=rcM.bottom-sbH-35;
	rc.top=tbH+1;
	m_Display.MoveWindow(&rc);

// Set Scroll bar position
	rc=rcM;
	rc.top=rcM.bottom-sbH-35;
	rc.bottom=rcM.bottom-sbH;
	m_Preview.MoveWindow(&rc);
	
	
	}


void MainFrame::OnConfig() 
	{
	// TODO: Add your command handler code here
	CPropertySheet SetALLParam("Set All Parameters",this,0);
	CNetworkPage netpage;
	CSelectChan selectchanpage(NrtsInfo);
	CDisplayParamDlg displayparam;

	SetALLParam.AddPage(&selectchanpage);
	SetALLParam.AddPage(&netpage);
	SetALLParam.AddPage(&displayparam);

	if(SetALLParam.DoModal()==IDOK)
		{
		}
	}

void MainFrame::OnRun() 
	{
	// TODO: Add your command handler code here

	if(NrtsInfo.GetSelectedChanNumber()==0)
		{
		AfxMessageBox("No channels selected",MB_ICONSTOP);
		return;
		}



	m_wndToolBar.LoadToolBar(IDR_TOOLBARRUN);


	TBBUTTON lptbutton;
	int idx = 0;
	CToolBarCtrl& Ctrl = m_wndToolBar.GetToolBarCtrl();
	idx = Ctrl.CommandToIndex(IDC_AUTOSCALEONOFF);
	m_wndToolBar.SetButtonStyle(idx, TBSTYLE_CHECK);
	BOOL bGet = Ctrl.GetButton(idx,&lptbutton);
	Ctrl.CheckButton(lptbutton.idCommand,true);
	m_Display.SetAutoScale(true);



	m_Display.Run();
	nTimerCount=0;
	SetTimer(1,1000,NULL);

	}
void MainFrame::OnUpdateView() 
	{
	m_Preview.DrawPreviewBitmap();
	}
void MainFrame::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default

	if(!bContinue) return;


	if((nTimerCount--)==0)
		{
		nTimerCount=nRefreshTime;
		m_Display.OnTimer();
		m_Preview.DrawPreviewBitmap();
		}

	CFrameWnd::OnTimer(nIDEvent);
	}

BOOL MainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFrameWnd::OnCommand(wParam, lParam);
	}

void MainFrame::OnStop() 
	{
	// TODO: Add your command handler code here
	KillTimer(1);
	bContinue=FALSE;
	}

void MainFrame::OnAutoscale() 
	{
	// TODO: Add your command handler code here
	m_Display.OnAutoScale(0);	
	}

void MainFrame::OnExpandVert() 
	{
	// TODO: Add your command handler code here
	m_Display.OnExpandVert();
	}

void MainFrame::OnContractVert() 
	{
	m_Display.OnContractVert();
	}

void MainFrame::OnShiftup() 
	{
	m_Display.OnShiftUp();
	}

void MainFrame::OnShiftdown() 
	{
	m_Display.OnShiftDown();
	}
void MainFrame::OnThreadExit()
	{
	CWaitCursor cw;
	m_Display.Stop();
	m_Preview.Stop();
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
	}
void MainFrame::OnSetTextToStatusbar1(LPSTR lpText)
	{
	m_wndStatusBar.SetPaneText(1,lpText,TRUE);
	}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CG: The following line was added by the Splash Screen component.
//	CSplashWnd::ShowSplashScreen(this);

	return 0;
}



void MainFrame::OnAutoscale1() 
	{
	m_Display.OnAutoScale(1);
	}

void MainFrame::OnAutoscaleonoff() 
	{
	TBBUTTON tbb;
	UINT state;
	CToolBarCtrl& Ctrl = m_wndToolBar.GetToolBarCtrl();
	int idx = Ctrl.CommandToIndex(IDC_AUTOSCALEONOFF);
	state = Ctrl.GetState(idx);
	Ctrl.GetButton(idx, &tbb);
	if(tbb.fsState&TBSTATE_CHECKED)
		{
		m_Display.SetAutoScale(true);
		}
	else
		{
		m_Display.SetAutoScale(false);
		}
	
	}
