// QmyToolBar.cpp: implementation of the QmyToolBar class.
//
//////////////////////////////////////////////////////////////////////

#include <qtooltip.h>
#include "QmyToolBar.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QmyToolBar::QmyToolBar(const QString & label, QMainWindow * mainWindow, QWidget * parent, bool newLine , const char * name , WFlags f  ):QToolBar(label, mainWindow, parent, newLine , name,f )
	{
    runButton = new QPushButton( this, "runButton" );
    runButton->setPixmap( QPixmap::fromMimeSource( "run.bmp" ) );

    configButton = new QPushButton( this, "configButton" );
    configButton->setPixmap( QPixmap::fromMimeSource( "config.bmp" ) );

    aboutButton = new QPushButton( this, "aboutButton" );
    aboutButton->setPixmap( QPixmap::fromMimeSource( "about.bmp" ) );

	stopButton = new QPushButton( this, "stopButton" );
    stopButton->setPixmap( QPixmap::fromMimeSource( "stop.bmp" ) );

	autoButton = new QmyButton( this, "autoButton" );
    autoButton->setPixmap( QPixmap::fromMimeSource( "autoscale.bmp" ) );

	autoButton1 = new QmyButton( this, "autoButton1" );
    autoButton1->setPixmap( QPixmap::fromMimeSource( "autoscale1.bmp" ) );


	expandButton = new QmyButton( this, "expandButton" );
    expandButton->setPixmap( QPixmap::fromMimeSource( "expandvert.bmp" ) );
	
	contractButton = new QmyButton( this, "" );
    contractButton->setPixmap( QPixmap::fromMimeSource( "contractvert.bmp" ) );


	upButton = new QmyButton( this, "upButton" );
    upButton->setPixmap( QPixmap::fromMimeSource( "shiftup.bmp" ) );
	
	downButton = new QmyButton( this, "downButton" );
    downButton->setPixmap( QPixmap::fromMimeSource( "shiftdown.bmp" ) );


    connect( configButton, SIGNAL( clicked() ), parent, SLOT( configButton_clicked() ) );
    connect( runButton, SIGNAL( clicked() ), parent, SLOT( runButton_clicked() ) );
	connect( autoButton, SIGNAL( clicked() ), parent, SLOT( autoButton_clicked() ) );
	connect( autoButton1, SIGNAL( clicked() ), parent, SLOT( autoButton1_clicked() ) );
	connect( expandButton, SIGNAL( clicked() ), parent, SLOT( expandButton_clicked() ) );
	connect( contractButton, SIGNAL( clicked() ), parent, SLOT( contractButton_clicked() ) );
	connect( upButton, SIGNAL( clicked() ), parent, SLOT( upButton_clicked() ) );
	connect( downButton, SIGNAL( clicked() ), parent, SLOT( downButton_clicked() ) );
	connect( stopButton, SIGNAL( clicked() ), parent, SLOT( stopButton_clicked() ) );
	connect( aboutButton, SIGNAL( clicked() ), parent, SLOT( aboutButton_clicked() ) );

	}

QmyToolBar::~QmyToolBar()
	{

	}
void QmyToolBar::SetToolbarState(int nState)
	{
	if(nState==0)
		{
		runButton->show();
		configButton->show();
		aboutButton->show();

		autoButton->hide();
		autoButton1->hide();
		stopButton->hide();
		expandButton->hide();
		contractButton->hide();
		upButton->hide();
		downButton->hide();
		adjustSize ();
		return;
		}

	if(nState==1)
		{
		runButton->hide();
		configButton->hide();
		aboutButton->hide();
		autoButton->show();
		autoButton1->show();
		stopButton->show();
		expandButton->show();
		contractButton->show();
		upButton->show();
		downButton->show();
		adjustSize();
		return;
		}

	if(nState==2)
		{
		runButton->hide();
		configButton->hide();
		aboutButton->hide();
		autoButton->hide();
		autoButton1->hide();
		stopButton->hide();
		expandButton->hide();
		contractButton->hide();
		upButton->hide();
		downButton->hide();
		adjustSize();
		return;
		}

	}
void QmyToolBar::languageChange()
	{
    setLabel( tr( "Tools" ) );
    runButton->setText( QString::null );
    QToolTip::add( runButton, tr( "Run", "Run" ) );
    configButton->setText( QString::null );
    QToolTip::add( configButton, tr( "Config", "Config" ) );
    aboutButton->setText( QString::null );
	QToolTip::add( autoButton, tr( "Fit displayed signal data to window", "Fit displayed signal data to window" ) );
    QToolTip::add( aboutButton, tr( "About", "About" ) );
	QToolTip::add( autoButton1, tr("Fit buffered signal data to window","Fit buffered signal data to window"));
	QToolTip::add( expandButton, tr( "Expand vertical", "Expand vertical" ) );
	QToolTip::add( contractButton, tr( "Contract vertical", "Contract vertical" ) );
	QToolTip::add( contractButton, tr( "Contract vertical", "Contract vertical" ) );
	QToolTip::add( downButton, tr( "Shift graph down", "Shift graph down" ) );
	QToolTip::add( upButton, tr( "Shift graph up", "Shift graph up" ) );
	QToolTip::add( stopButton, tr( "Stop", "Stop" ) );
	
	
	}
