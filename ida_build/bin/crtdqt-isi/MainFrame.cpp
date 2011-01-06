/****************************************************************************
** Form implementation generated from reading ui file 'crtdqtui.ui'
**
** Created: Пт 25. фев 10:01:13 2005
**      by: The User Interface Compiler ($Id: MainFrame.cpp,v 1.1 2005/05/18 00:06:42 dechavez Exp $)
**
****************************************************************************/
#include <qapplication.h> 
#include <qvariant.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstatusbar.h> 
#include <qmessagebox.h>
#include <qfont.h>
#include "set_all_parameters.h"
#include "CRTDglob.h"
#include "MainFrame.h"


void StartAcquisition(MainFrame *p);


/*
 *  Constructs a MainFrame as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainFrame::MainFrame( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
	{
    (void)statusBar();
    if ( !name )
	setName( "MainFrame" );


	nPreviewHeight = 35;


    // toolbars
    toolBar = new QmyToolBar( QString(""), this, this ); 

    toolBar->setMovingEnabled( FALSE );
    toolBar->addSeparator();

	pDisplay = new CRTDisplay(this, "CRTDisplay",WStyle_NormalBorder);
	pDisplay->show();

	pPreview = new CPreview(pDisplay, this,"CPreview",WStyle_NormalBorder);
	pPreview->show();


	toolBar->SetToolbarState(0);
    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
//    clearWState( WState_Polished );

    // signals and slots connections
/*    connect( configButton, SIGNAL( clicked() ), this, SLOT( configButton_clicked() ) );
    connect( runButton, SIGNAL( clicked() ), this, SLOT( runButton_clicked() ) );
	connect( autoButton, SIGNAL( clicked() ), this, SLOT( autoButton_clicked() ) );
	connect( expandButton, SIGNAL( clicked() ), this, SLOT( expandButton_clicked() ) );
	connect( contractButton, SIGNAL( clicked() ), this, SLOT( contractButton_clicked() ) );
	connect( upButton, SIGNAL( clicked() ), this, SLOT( upButton_clicked() ) );
	connect( downButton, SIGNAL( clicked() ), this, SLOT( downButton_clicked() ) );*/

	}

/*
 *  Destroys the object and frees any allocated resources
 */
MainFrame::~MainFrame()
	{
	}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainFrame::languageChange()
	{
    setCaption( tr( "CRTD QT(ISI)" ) );
    toolBar->setLabel( tr( "Tools" ) );
	toolBar->languageChange();
	}

void MainFrame::helpIndex()
	{

	}


void MainFrame::helpContents()
	{

	}


void MainFrame::helpAbout()
	{
	}


void MainFrame::configButton_clicked()
	{
	Set_All_Parameters SAL(this, 0, TRUE, 0);
	SAL.exec();
	}


void MainFrame::newSlot()
	{
	}


void MainFrame::runButton_clicked()
	{
	if(NrtsInfo.GetSelectedChanNumber()==0)
		{
		QMessageBox mb("ERROR","No channels selected",QMessageBox::Critical, QMessageBox::Ok, NULL, NULL);
		mb.exec();
		return;
		}
	toolBar->SetToolbarState(1);
	pDisplay->Run();
	StartAcquisition(this);
	nTimerCount=0;
	}

void MainFrame::autoButton_clicked()
	{
	QCustomEvent *event = new QCustomEvent(pDisplay->AutoScale0);
	QApplication::postEvent(pDisplay, event);
	}
void MainFrame::autoButton1_clicked()
	{
	QCustomEvent *event = new QCustomEvent(pDisplay->AutoScale1);
	QApplication::postEvent(pDisplay, event);
	}

void MainFrame::expandButton_clicked()
	{
	QCustomEvent *event = new QCustomEvent(CRTDisplay::ExpandVert);
	QApplication::postEvent(pDisplay, event);
	}

void MainFrame::contractButton_clicked()
	{
	QCustomEvent *event = new QCustomEvent(CRTDisplay::ContractVert);
	QApplication::postEvent(pDisplay, event);
	}
void MainFrame::upButton_clicked()
	{
	QCustomEvent *event = new QCustomEvent(CRTDisplay::ShiftUp);
	QApplication::postEvent(pDisplay, event);
	}
void MainFrame::downButton_clicked()
	{
	QCustomEvent *event = new QCustomEvent(CRTDisplay::ShiftDown);
	QApplication::postEvent(pDisplay, event);
	}
void MainFrame::aboutButton_clicked()
	{
	QMessageBox::about(this,"About CRTDQT(ISI)","CRTD Version 0.0.0\nCopyright (C) will be here");
	}

void  MainFrame::resizeEvent( QResizeEvent *q )
	{
	QStatusBar *pStatusB;
	QRect qzt, qzs, qz, rc;
	int sbH, tbH;

	rc = rect();

	qzt=toolBar->frameGeometry();
	pStatusB = statusBar();

	qzs=pStatusB->frameGeometry();


	sbH=qzs.height();
	tbH=qzt.height();



	qz = rc;
	qz.setTop(tbH+1);
	qz.setBottom(rc.bottom()-sbH-nPreviewHeight);
	pDisplay->setGeometry(qz);

	qz.setTop(rc.bottom()-sbH-nPreviewHeight);
	qz.setBottom(rc.bottom()-sbH);
	pPreview->setGeometry(qz);

	}
void  MainFrame::SetTextToStatusbar(const char *pText)
	{
	QStatusBar *pStatusB;
	pStatusB = statusBar();
	pStatusB->message(pText);
	}
void MainFrame::exitButton_clicked()
	{
	QApplication::exit();
	}

void StopAcquisition(MainFrame *p);

void MainFrame::stopButton_clicked()
	{
	StopAcquisition(this);
	}
void MainFrame::customEvent(QCustomEvent *event) {
    switch (event->type()) 
		{
		case STOP:
			pPreview->Stop();
			pDisplay->Stop();
			toolBar->SetToolbarState(0);
			break;
		case SETTEXT:
			SetTextToStatusbar( ((QString *) event->data())->latin1());
			break;

		default:
			{
			qWarning("Unknown custom event type: %d", event->type());
			}
		}
	}
