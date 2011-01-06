/****************************************************************************
** Form implementation generated from reading ui file 'set_all_parameters.ui'
**
** Created: Пт 4. мар 16:22:24 2005
**      by: The User Interface Compiler ($Id: set_all_parameters.cpp,v 1.1 2005/05/18 00:06:45 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "set_all_parameters.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qtable.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h> 
#include "CRTDglob.h"
/*
 *  Constructs a Set_All_Parameters as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Set_All_Parameters::Set_All_Parameters( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	QHeader *header;
    if ( !name )
	setName( "Set_All_Parameters" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( FALSE);

	NrtsInfoX=NrtsInfo;

    QWidget* privateLayoutWidget = new QWidget( this, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 10, 340, 530, 28 ) );
    Layout1 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( privateLayoutWidget, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setGeometry( QRect( 10, 10, 530, 320 ) );

    Channels = new QWidget( tabWidget, "Channels" );

	QBoxLayout  * Layout0 = new QVBoxLayout( Channels );
	QBoxLayout  * Layout1 = new QHBoxLayout( Layout0  );
	QBoxLayout  * Layout2 = new QHBoxLayout( Layout0  );
	QGridLayout * GridLayout0 = new QGridLayout(Channels, 2,2,5);

    IDC_CHAN = new QTable( Channels, "IDC_CHAN" );

    IDC_CHAN->setGeometry( QRect( 150, 10, 360, 180 ) );
    IDC_CHAN->setNumRows( 0 );
    IDC_CHAN->setNumCols( 3 );
    IDC_CHAN->setSelectionMode( QTable::Single );
	IDC_CHAN->verticalHeader()->hide();
	IDC_CHAN->setLeftMargin(0);

    header = IDC_CHAN->horizontalHeader();
    header->setLabel( 0, QObject::tr( "Channel" ), 120 );
	header->setLabel( 1, QObject::tr( "Lcode" ),   120 );
	header->setLabel( 2, QObject::tr( "SpS" ),     120 );

    IDC_STA = new QTable( Channels, "IDC_STA" );
    IDC_STA->setGeometry( QRect( 10, 10, 120, 180 ) );

	Layout1->addWidget(IDC_STA);
	Layout1->addWidget(IDC_CHAN);


    Port_number_2 = new QLabel( Channels, "Port_number_2" );
    Port_number_2->setGeometry( QRect( 17, 256, 57, 20 ) );

    ISI_Server_2 = new QLabel( Channels, "ISI_Server_2" );
    ISI_Server_2->setGeometry( QRect( 17, 230, 57, 20 ) );

    IDC_LOADLIST = new QPushButton( Channels, "IDC_LOADLIST" );
    IDC_LOADLIST->setGeometry( QRect( 280, 230, 230, 30 ) );

/*    QFont IDC_STA_font(  IDC_STA->font() );
    IDC_STA_font.setFamily( "Times New Roman" );
    IDC_STA->setFont( IDC_STA_font ); */
    IDC_STA->setNumRows( 3 );
    IDC_STA->setNumCols( 1 );
    IDC_STA->setShowGrid( TRUE );
    IDC_STA->setRowMovingEnabled( FALSE );
    IDC_STA->setSelectionMode( QTable::Single );
	IDC_STA->setColumnReadOnly(0,TRUE);
//	QHeader *hd = IDC_STA->horizontalHeader();
//	hd->

    header = IDC_STA->horizontalHeader();
    header->setLabel( 0, QObject::tr( "Station" ), 40 );
	IDC_STA->verticalHeader()->hide();
	IDC_STA->setLeftMargin(0);
	IDC_STA->setColumnWidth(0, 118);


    IDC_NRTSSERVER = new QLineEdit( Channels, "IDC_NRTSSERVER" );
    IDC_NRTSSERVER->setGeometry( QRect( 80, 230, 165, 20 ) );

    IDC_PORT = new QLineEdit( Channels, "IDC_PORT" );
    IDC_PORT->setGeometry( QRect( 80, 256, 165, 20 ) );

	Layout2->addLayout(GridLayout0);
	GridLayout0->addWidget(Port_number_2 , 0, 0);GridLayout0->addWidget(IDC_PORT      ,     0, 1);
	GridLayout0->addWidget(ISI_Server_2  , 1, 0);GridLayout0->addWidget(IDC_NRTSSERVER  , 1, 1);

	Layout2->addWidget(IDC_LOADLIST);

    tabWidget->insertTab( Channels, QString::fromLatin1("") );

    Display_parameters = new QWidget( tabWidget, "Display_parameters" );

/*    frame3 = new QFrame( Display_parameters, "frame3" );
    frame3->setGeometry( QRect( 8, 6, 361, 171 ) );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );*/

//    QWidget* privateLayoutWidget_2 = new QWidget( Display_parameters, "layout14" );
	


//    privateLayoutWidget_2->setGeometry( QRect( 2, 26, 232, 200 ) );
//	privateLayoutWidget_2->setGeometry( QRect( 2, 26, 400, 200 ) );
//    privateLayoutWidget_2->setGeometry( tabWidget->rect() );

    layout14 = new QGridLayout( Display_parameters/*privateLayoutWidget_2*/, 1, 1, 11, 6, "layout14"); 

    Displayed_window_size = new QLabel( Display_parameters/*privateLayoutWidget_2*/, "Displayed_window_size" );
    Displayed_window_size->setFrameShape( QLabel::NoFrame );

    layout14->addWidget( Displayed_window_size, 0, 0 );

    IDC_REFRESHTIME = new QLineEdit( Display_parameters/*privateLayoutWidget_2*/, "IDC_REFRESHTIME" );

    layout14->addWidget( IDC_REFRESHTIME, 2, 1 );

    IDC_DELAY = new QLineEdit( Display_parameters/*privateLayoutWidget_2*/, "IDC_DELAY" );

    layout14->addWidget( IDC_DELAY, 3, 1 );

    Buffered_window_size = new QLabel( Display_parameters/*privateLayoutWidget_2*/, "Buffered_window_size" );
    Buffered_window_size->setFrameShape( QLabel::NoFrame );

    layout14->addWidget( Buffered_window_size, 1, 0 );

    IDC_BUFFEREDTIME = new QLineEdit( Display_parameters/*privateLayoutWidget_2*/, "IDC_BUFFEREDTIME" );

    layout14->addWidget( IDC_BUFFEREDTIME, 1, 1 );

    IDC_DISPLAYTIME = new QLineEdit( Display_parameters/*privateLayoutWidget_2*/, "IDC_DISPLAYTIME" );
    IDC_DISPLAYTIME->setFrameShape( QLineEdit::LineEditPanel );
    IDC_DISPLAYTIME->setFrameShadow( QLineEdit::Sunken );

    layout14->addWidget( IDC_DISPLAYTIME, 0, 1 );

    Refresh_time = new QLabel( Display_parameters/*privateLayoutWidget_2*/, "Refresh_time" );

    layout14->addWidget( Refresh_time, 2, 0 );

    Delay__sec = new QLabel( Display_parameters/*privateLayoutWidget_2*/, "Delay__sec" );

    layout14->addWidget( Delay__sec, 3, 0 );

	Display_parameters->adjustSize();

    tabWidget->insertTab( Display_parameters, QString::fromLatin1("") );
    languageChange();
    resize( QSize(558, 403).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this,  SLOT(OnOk())  );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( IDC_LOADLIST, SIGNAL( clicked() ), this,  SLOT(OnLoadList())  );
	connect( IDC_STA, SIGNAL(  currentChanged( int , int) ), this,  SLOT(OnClickStaListTableItem( int , int))  );
	connect( IDC_CHAN, SIGNAL(  valueChanged ( int , int ) ), this,  SLOT(OnClickChanListTableItem(int, int))  );
	

    // tab order
    setTabOrder( buttonHelp, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, tabWidget );
    setTabOrder( tabWidget, IDC_DISPLAYTIME );
    setTabOrder( IDC_DISPLAYTIME, IDC_BUFFEREDTIME );
    setTabOrder( IDC_BUFFEREDTIME, IDC_REFRESHTIME );
    setTabOrder( IDC_REFRESHTIME, IDC_DELAY );
    setTabOrder( IDC_DELAY, IDC_CHAN );
    setTabOrder( IDC_CHAN, IDC_PORT );
    setTabOrder( IDC_PORT, IDC_NRTSSERVER );
    setTabOrder( IDC_NRTSSERVER, IDC_LOADLIST );
    setTabOrder( IDC_LOADLIST, IDC_STA );

    // buddies
    Displayed_window_size->setBuddy( IDC_DISPLAYTIME );
    Buffered_window_size->setBuddy( IDC_BUFFEREDTIME );
    Refresh_time->setBuddy( IDC_REFRESHTIME );
    Delay__sec->setBuddy( IDC_DELAY );

	QString text;
	text.sprintf("%d",nRefreshTime);
	IDC_REFRESHTIME->setText(text);
	text.sprintf("%d",nDelay);
	IDC_DELAY->setText(text);
	IDC_NRTSSERVER->setText(server);
	text.sprintf("%d",nNrtsPort);
	IDC_PORT->setText(text);
	text.sprintf("%d",nDisplayedTime);
	IDC_DISPLAYTIME->setText(text);
	text.sprintf("%d",nBufferedTime);
	IDC_BUFFEREDTIME->setText(text);
	layout14->sizeHint();
	LoadStaList();
	ActiveSta = "";




}

/*
 *  Destroys the object and frees any allocated resources
 */
Set_All_Parameters::~Set_All_Parameters()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Set_All_Parameters::languageChange()
{
    setCaption( tr( "Set All Parameters" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    QToolTip::add( tabWidget, tr( "Display parameters" ) );
    Port_number_2->setText( tr( "Port number" ) );
    ISI_Server_2->setText( tr( "ISI Server" ) );
    IDC_LOADLIST->setText( tr( "Load config from ISI server" ) );
    tabWidget->changeTab( Channels, tr( "Channels" ) );
    Displayed_window_size->setText( tr( "Displayed window size (sec.)" ) );
    Buffered_window_size->setText( tr( "Buffered window size (sec.)" ) );
    Refresh_time->setText( tr( "Refresh time(sec.)" ) );
    Delay__sec->setText( tr( "Delay (sec)" ) );
    tabWidget->changeTab( Display_parameters, tr( "Display parameters" ) );
}

// SIGNAL OnOk
void Set_All_Parameters::OnOk()
	{
	long nTmp;
	bool bOk;
 	QString text;


	text = IDC_REFRESHTIME->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nRefreshTime = nTmp;
		}
	else
		{
		IDC_REFRESHTIME->setFocus();
		return;
		}


	text = IDC_DELAY->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nDelay = nTmp;
		}
	else
		{
		IDC_DELAY->setFocus();
		return;
		}


	text = IDC_PORT->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nNrtsPort = nTmp;
		}
	else
		{
		IDC_PORT->setFocus();
		return;
		}

	server = IDC_NRTSSERVER->text();

	IDC_NRTSSERVER->setText(server);


	text = IDC_DISPLAYTIME->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nDisplayedTime = nTmp;
		}
	else
		{
		IDC_DISPLAYTIME->setFocus();
		return;
		}

	text = IDC_BUFFEREDTIME->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nBufferedTime = nTmp;
		}
	else
		{
		IDC_BUFFEREDTIME->setFocus();
		return;
		}
	SaveParameters();
	accept();   
	}
void Set_All_Parameters::SaveParameters()
	{
	SaveGlobalParam();
//	NrtsInfoX.SaveConfigToIni();
	NrtsInfoX.SaveConfigToFile();
	::NrtsInfo = NrtsInfoX;
	}
void Set_All_Parameters::LoadStaList()
	{
	char cSta[16];

/*	m_sta.DeleteAllItems();*/


	while(IDC_STA->numRows()>0)
		{
		IDC_STA->removeRow(0);
		}

	CNRTSSTAINFO *si;
	int nRows = NrtsInfoX.StaInfo.count();
	IDC_STA->setNumRows(nRows);
	int nRow = 0;
	for(si=NrtsInfoX.StaInfo.first(); si; si=NrtsInfoX.StaInfo.next())
		{

		strcpy(cSta,si->Sta.ascii());

		IDC_STA->setText(nRow,0, cSta);
		if(bHasSelectedChan(si))
			{
			IDC_STA->setPixmap(nRow,0, QPixmap::fromMimeSource( "flag.bmp" ) );
			}
		else
			{
			IDC_STA->setPixmap(nRow,0, QPixmap::fromMimeSource( "blank.bmp" ));
			}
		nRow++;
/*
		m_sta.InsertItem(&lvi);
		if(bHasSelectedChan(cSta))
			{
			m_sta.SetItemState(i,0x2000,LVIS_STATEIMAGEMASK);
			}
		else
			{
			m_sta.SetItemState(i,0x1000,LVIS_STATEIMAGEMASK);
			}
*/
		}
	}
void Set_All_Parameters::OnLoadList()
	{

	long nTmp;
	bool bOk;
 	QString text;

	text = IDC_PORT->text();
	nTmp=text.toInt(&bOk);
	if(bOk)
		{
		nNrtsPort = nTmp;
		}
	else
		{
		IDC_PORT->setFocus();
		return;
		}

	server = IDC_NRTSSERVER->text();



	if(NrtsInfoX.LoadConfigFromServer()==1)
		{
		LoadStaList();
		}
	else
		{
		QMessageBox mb("ERROR","Load Config from server error",QMessageBox::Critical, QMessageBox::Ok, NULL, NULL);
		mb.exec();
		}
	}
void Set_All_Parameters::OnClickStaListTableItem(int row, int col)
	{
//	int row=0;
//	int col=0;
	row = IDC_STA->currentRow();
	QCheckTableItem * item = (QCheckTableItem *)IDC_STA->item(row, col);
	QString Sta=IDC_STA->text(row, 0);
	LoadChanList(Sta);
	ActiveSta=Sta;
	}
void Set_All_Parameters::OnClickChanListTableItem(int row, int col)
	{
	QCheckTableItem * item = (QCheckTableItem *)IDC_CHAN->item(row, 0);
	QString Chan  = IDC_CHAN->text(row, 0);
	QString LCODE = IDC_CHAN->text(row, 1);
	if(item->isChecked())
		{
		SetChanDrawState(ActiveSta, Chan, LCODE, TRUE);
		}
	else
		{
		SetChanDrawState(ActiveSta, Chan, LCODE, FALSE);
		}
//	ActiveSta=Sta;
	}
void Set_All_Parameters::SetChanDrawState(const QString &sSta, const QString &sChan, const QString &LCODE, BOOL bSelected)
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	for(si=NrtsInfoX.StaInfo.first(); si; si=NrtsInfoX.StaInfo.next())
		{
		if(sSta.compare(si->Sta)==0)
			{
			for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
				{
				if(sChan.compare(ci->Chan)==0)
					{
					if(LCODE.isEmpty()&&ci->LCODE.isEmpty())
						{
						ci->bSelected=bSelected;
						SetStationCheck();
						return;
						}
					else
						{
						if( (LCODE.compare(ci->LCODE)==0) &&
							(!ci->LCODE.isEmpty()))
							{
							ci->bSelected=bSelected;
							SetStationCheck();
							return;
							}
						}
					}
				}
			return;
			}
		}
	}

void Set_All_Parameters::LoadChanList(QString &sSta)
	{
	int nRow = 0;
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	QCheckTableItem * item;
	char cChan[16],cSpS[16], cLoc[16];

	while(IDC_CHAN->numRows()>0)
		{
		IDC_CHAN->removeRow(0);
		}
	
	for(si=NrtsInfoX.StaInfo.first(); si; si=NrtsInfoX.StaInfo.next())
		{
		if(sSta.compare(si->Sta)==0)
			{
			int nChan=si->ChanInfo.count();
			IDC_CHAN->setNumRows(nChan);
			for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
				{
				strcpy(cChan, ci->Chan);
				if(ci->dSpS>0)
					{
					sprintf(cSpS,"%.2f",ci->dSpS);
					}
				else
					{
					sprintf(cSpS,"-");
					}
				strcpy(cLoc, ci->LCODE);
				
		item = new QCheckTableItem( IDC_CHAN, cChan);
		if(ci->bSelected) 	item->setChecked(TRUE);
		IDC_CHAN->setItem(nRow,   0, item);
		IDC_CHAN->setText(nRow,   1, cLoc);
		IDC_CHAN->setText(nRow++, 2, cSpS);


/*				if(NrtsInfoX.StaInfo[i]->ChanInfo[j]->bSelected)
					{
					m_chan.SetItemState(j,0x2000,LVIS_STATEIMAGEMASK);
					}
				else
					{
					m_chan.SetItemState(j,0x1000,LVIS_STATEIMAGEMASK);
					}*/

				}
			return;
			}
		}

	}

void Set_All_Parameters::SetStationCheck()
	{
	int row = IDC_STA->currentRow();
	QString Sta=IDC_STA->text(row, 0);

	if(bHasSelectedChan(ActiveSta))
		{
		IDC_STA->setPixmap(row,0, QPixmap::fromMimeSource( "flag.bmp" ) );
		}
	else
		{
		IDC_STA->setPixmap(row,0, QPixmap::fromMimeSource( "blank.bmp" ));
		}
	}
BOOL Set_All_Parameters::bHasSelectedChan(QString sSta)
	{
	CNRTSSTAINFO *si;
	CNRTSCHANINFO *ci;

	for(si=NrtsInfoX.StaInfo.first(); si; si=NrtsInfoX.StaInfo.next())
		{
		if(sSta.compare(si->Sta)==0)
			{
			for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
				{
				if(ci->bSelected) return TRUE;
				}
			}
		}
	return FALSE;
	}
BOOL Set_All_Parameters::bHasSelectedChan(CNRTSSTAINFO *si)
	{
	CNRTSCHANINFO *ci;
	for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
		{
		if(ci->bSelected) return TRUE;
		}
	return FALSE;
	}
