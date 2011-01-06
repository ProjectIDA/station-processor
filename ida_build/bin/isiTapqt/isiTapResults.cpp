#include "isiTapResults.h"
#include <qvariant.h>
#include <qpushbutton.h>
#include <qiconview.h>
#include <qtextedit.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "prcinfo.h"
#include "isitap_globals.h"
#include "isitap_time.h"


void StartGettingData(isiTapResults *p);
void GetTextRectangle(const QWidget *pWid, const char *lpText, QRect &rc);
void ClearWfdList();

/*
 *  Constructs a isiTapResults as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
isiTapResults::isiTapResults( QWidget* parent, const char* name, bool modal, WFlags fl )
: QDialog( parent, name, modal, fl )
{
	QRect rc;
	QHeader *header;

	bExitPossible=false;
    if ( !name )
	setName( "isiTapResults" );

    PushButtonOK = new QPushButton( this, "PushButtonOK" );
    PushButtonOK->setGeometry( QRect( 183, 310, 75, 23 ) );
    PushButtonOK->setFocusPolicy( QPushButton::NoFocus );
    PushButtonOK->setDefault( TRUE );
//	PushButtonOK->setMaximumSize(75,23);

    PushButtonCANCEL = new QPushButton( this, "PushButtonCANCEL" );
    PushButtonCANCEL->setGeometry( QRect( 290, 310, 75, 23 ) );
    PushButtonCANCEL->setFocusPolicy( QPushButton::NoFocus );
	PushButtonCANCEL->hide();
	PushButtonOK->hide();
//	PushButtonCANCEL->setMaximumSize(75,23);

	connect( PushButtonOK, SIGNAL( clicked() ), this,  SLOT(PushButtonOK_clicked())  );
	connect( PushButtonCANCEL, SIGNAL( clicked() ), this,  SLOT(PushButtonCANCEL_clicked())  );

	Layout1 = new QVBoxLayout( this, 0, 6, "Layout1"); 


    IDC_CHAN = new QTable( this, "IDC_CHAN" );
    IDC_CHAN->setGeometry( QRect( 12, 12, 538, 112 ) );
    IDC_CHAN->setNumRows( 0 );
    IDC_CHAN->setNumCols( 5 );

    IDC_CHAN->setSelectionMode( QTable::SingleRow );
	IDC_CHAN->verticalHeader()->hide();
	IDC_CHAN->setLeftMargin(0);

    header = IDC_CHAN->horizontalHeader();
//	GetTextRectangle(header,"99:99:99 99/99/9999", rc);

	header->setLabel( 0, QObject::tr( "Station" ), 75 );
    header->setLabel( 1, QObject::tr( "Channel" ), 75 );
	header->setLabel( 2, QObject::tr( "SpS" ),     75 );
	header->setLabel( 3, QObject::tr( "Tbeg" ),    75 );
	header->setLabel( 4, QObject::tr( "Tend" ),    75 );

	header->adjustHeaderSize();
	adjustTableColumnWidth();




	Layout1->addWidget(IDC_CHAN);




    outputLog = new QLabel( this, "outputLog" );
    outputLog->setGeometry( QRect( 12, 130, 538, 112 ) );
	Layout1->addWidget(outputLog);

//    outputLog->setReadOnly( TRUE );

    progressBar1 = new QProgressBar( this, "progressBar1" );
    progressBar1->setGeometry( QRect( 10, 260, 540, 22 ) );
	Layout1->addWidget(progressBar1);
	Layout1->addWidget(outputLog);

	QGridLayout *gridLayout1 = new QGridLayout( Layout1, 1, 0, 5); 
//	Layout1->addItem(gridLayout1);

	gridLayout1->addWidget(PushButtonOK,0,3);
	gridLayout1->addWidget(PushButtonCANCEL, 0, 4);

    languageChange();
    resize( QSize(556, 354).expandedTo(minimumSizeHint()) );

//	outputLog->adjust();

    clearWState( WState_Polished );
	StartGettingData(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
isiTapResults::~isiTapResults()
	{
    // no need to delete child widgets, Qt does it all for us
	ClearWfdList();
	}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void isiTapResults::languageChange()
	{
    setCaption( tr( "Results" ) );
    PushButtonOK->setText( tr( "View Data" ) );
    PushButtonCANCEL->setText( tr( "Exit" ) );
	QString s;

/*	QRect rc;
	s = PushButtonOK->text();
	GetTextRectangle(PushButtonOK, s.latin1(), rc);
	
	PushButtonOK->setMaximumSize(rc.width()+20,rc.height()+20);*/
	PushButtonOK->adjustSize();
	PushButtonCANCEL->adjustSize();
	}

void isiTapResults::customEvent(QCustomEvent *event) {
	prcInfo *pinfo;
    switch (event->type()) 
		{
		case STOP:
			fillResultsTable();
			bExitPossible = TRUE;
			PushButtonCANCEL->show();
			PushButtonOK->show();
			break;
		case LOG:
			pinfo = (prcInfo *)event->data();
			outputLog->setText(pinfo->sInfo);
			progressBar1->setProgress(pinfo->percent);
			break;
		default:
			qWarning("Unknown custom event type: %d", event->type());
		}
	}
void isiTapResults::PushButtonOK_clicked()
	{
	}
void isiTapResults::PushButtonCANCEL_clicked()
	{
	done(0);
	}
void isiTapResults::closeEvent ( QCloseEvent * e ) 
	{
	if(bExitPossible) QDialog::closeEvent(e);
	}
void isiTapResults::fillResultsTable()
	{
	CWfdisc *wfd;

	QString sTime, qSta, qChan, qSpS;
	int nRow = 0;

	IDC_CHAN->insertRows(0, wfdarr.count());

	for (wfd=wfdarr.first();wfd;wfd=wfdarr.next(),++nRow) 
		{
		qSta=wfd->wfd.sta;
		qChan=wfd->wfd.chan;
		if(wfd->wfd.smprate>0)
			{
			qSpS.sprintf("%.3f",wfd->wfd.smprate);
			}
		else
			{
			qSpS = "-";
			}
		IDC_CHAN->setText(nRow,   0, qSta);
		IDC_CHAN->setText(nRow,   1, qChan);
		IDC_CHAN->setText(nRow,   2, qSpS);
		
		if(wfd->wfd.time>0)
			{
			sTime= TimeToString(wfd->wfd.time, nTimeFormat);
			}
		else
			{
			sTime="";
			}
		IDC_CHAN->setText(nRow, 3, sTime);
		if(wfd->wfd.endtime>0)
			{
			sTime = TimeToString(wfd->wfd.endtime, nTimeFormat);
			}
		else
			{
			sTime="";
			}
		IDC_CHAN->setText(nRow, 4, sTime);

		}
	adjustTableColumnWidth();
	}
void isiTapResults::adjustTableColumnWidth()
	{
	QRect rc;
	QHeader *header;
	QString qText;
	int nColumn, nRow, nWidth, nWtd;
	int nRows = IDC_CHAN->numRows();
	for(nColumn=0; nColumn<5; ++nColumn)
		{
		header = IDC_CHAN->horizontalHeader();
		qText = IDC_CHAN->text(0, nColumn); 
		GetTextRectangle(IDC_CHAN,qText, rc);
		nWidth = header->sectionSize(nColumn);
		for(nRow=1; nRow<nRows; ++nRow)
			{
			qText = IDC_CHAN->text(0, nColumn); 
			GetTextRectangle(IDC_CHAN,qText, rc);
			nWtd = rc.width()+4;
			if(nWidth < nWtd) nWidth = nWtd;
			}
		header->resizeSection(nColumn, nWidth);
		}
	}