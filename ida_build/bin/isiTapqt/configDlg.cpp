#include "configDlg.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "isitap_globals.h"

/*
 *  Constructs a configDlg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
configDlg::configDlg( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
	{
    if ( !name )
	setName( "configDlg" );

	QVBoxLayout *VLayout0 = new QVBoxLayout( this, 0, 1, "VLayout0"); 
	QHBoxLayout *HLayout1 = new QHBoxLayout( VLayout0, 0,  "HLayout1"); 
	QHBoxLayout *HLayout2 = new QHBoxLayout( VLayout0, 0,  "HLayout1"); 
//	QHBoxLayout *HLayout3 = new QHBoxLayout( VLayout0, 0,  "HLayout1"); 
	QGridLayout *gridLayout3 = new QGridLayout( VLayout0, 1, 5, 5); 

    GroupBox0 = new QGroupBox( this, "GroupBox0" );
    GroupBox0->setGeometry( QRect( 10, 10, 321, 54 ) );

    portNumberLabel = new QLabel( GroupBox0, "portNumberLabel" );
    portNumberLabel->setGeometry( QRect( 20, 20, 135, 20 ) );
    portNumberLabel->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

    portNumberLineEdit = new QLineEdit( GroupBox0, "portNumberLineEdit" );
    portNumberLineEdit->setGeometry( QRect( 180, 20, 130, 23 ) );
    portNumberLineEdit->setAlignment( int( QLineEdit::AlignLeft ) );

    GroupBox0->setColumnLayout(0, Qt::Vertical );
    GroupBox0->layout()->setSpacing( 6 );
    GroupBox0->layout()->setMargin( 11 );

	QHBoxLayout *HLayoutPortnumber = new QHBoxLayout( GroupBox0->layout(), 5,  "HLayout1"); 

	HLayoutPortnumber->addWidget(portNumberLabel);
	HLayoutPortnumber->addWidget(portNumberLineEdit);

	HLayout1->addWidget(GroupBox0);


    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setGeometry( QRect( 10, 70, 130, 90 ) );

    buttonGroup1->setColumnLayout(0, Qt::Vertical );
//    buttonGroup1->layout()->setSpacing( 6 );
//    buttonGroup1->layout()->setMargin( 11 );


    INTELBYTEORDER = new QRadioButton( buttonGroup1, "INTELBYTEORDER" );
    INTELBYTEORDER->setGeometry( QRect( 10, 30, 105, 16 ) );
//    INTELBYTEORDER->setFocusPolicy( QRadioButton::NoFocus );

    SPARCBYTEORDER = new QRadioButton( buttonGroup1, "SPARCBYTEORDER" );
    SPARCBYTEORDER->setGeometry( QRect( 10, 60, 108, 16 ) );
    SPARCBYTEORDER->setFocusPolicy( QRadioButton::NoFocus );

	buttonGroup1->layout()->add(INTELBYTEORDER);
	buttonGroup1->layout()->add(SPARCBYTEORDER);

	HLayout2->addWidget(buttonGroup1);


    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setGeometry( QRect( 140, 70, 190, 92 ) );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );

//    buttonGroup2->setFocusPolicy( QGroupBox::NoFocus );

    TIMEFORMAT1 = new QRadioButton( buttonGroup2, "TIMEFORMAT1" );
    TIMEFORMAT1->setGeometry( QRect( 10, 20, 169, 16 ) );
//    TIMEFORMAT1->setFocusPolicy( QRadioButton::NoFocus );

    TIMEFORMAT2 = new QRadioButton( buttonGroup2, "TIMEFORMAT2" );
    TIMEFORMAT2->setGeometry( QRect( 10, 40, 142, 16 ) );
//    TIMEFORMAT2->setFocusPolicy( QRadioButton::NoFocus );

    TIMEFORMAT3 = new QRadioButton( buttonGroup2, "TIMEFORMAT3" );
    TIMEFORMAT3->setGeometry( QRect( 10, 60, 169, 16 ) );
//    TIMEFORMAT3->setFocusPolicy( QRadioButton::NoFocus );
	buttonGroup2->layout()->add(TIMEFORMAT1);
	buttonGroup2->layout()->add(TIMEFORMAT2);
	buttonGroup2->layout()->add(TIMEFORMAT3);

	HLayout2->addWidget(buttonGroup2);



    ButtonOK = new QPushButton( this, "ButtonOK" );
    ButtonOK->setGeometry( QRect( 69, 173, 75, 23 ) );
//    ButtonOK->setFocusPolicy( QPushButton::NoFocus );
    ButtonOK->setDefault( TRUE );

    ButtonCANCEL = new QPushButton( this, "ButtonCANCEL" );
    ButtonCANCEL->setGeometry( QRect( 204, 173, 75, 23 ) );
//    ButtonCANCEL->setFocusPolicy( QPushButton::NoFocus );

//	HLayout3->addWidget(ButtonOK);
//	HLayout3->addWidget(ButtonCANCEL);
	gridLayout3->addWidget(ButtonOK,0,3);
	gridLayout3->addWidget(ButtonCANCEL, 0, 4);


    languageChange();


	QString s;

	s.sprintf("%d", nNrtsPort);
	portNumberLineEdit->setText(s);

	switch(nTimeFormat)
		{
		case 0:
			TIMEFORMAT1->setChecked(true);
			break;
		case 1:
			TIMEFORMAT2->setChecked(true);
			break;
		case 2:
			TIMEFORMAT3->setChecked(true);
			break;
		}


	switch(nOutputByteOrder)
		{
		case 0:
			INTELBYTEORDER->setChecked(true);
			break;
		case 1:
			SPARCBYTEORDER->setChecked(true);
			break;
		}



	connect( ButtonOK, SIGNAL( clicked() ), this,  SLOT(ButtonOK_clicked())  );
	connect( ButtonCANCEL, SIGNAL( clicked() ), this,  SLOT(reject())  );


//    resize( QSize(337, 210).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
	adjustSize();
	}

/*
 *  Destroys the object and frees any allocated resources
 */
configDlg::~configDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void configDlg::languageChange()
	{
    setCaption( tr( "ISI Tap Configuration" ) );
    GroupBox0->setTitle( tr( "Network" ) );
    portNumberLabel->setText( tr( "NRTS Server port number" ) );
    buttonGroup1->setTitle( tr( "Output byte order" ) );
    INTELBYTEORDER->setText( tr( "little-endian (intel)" ) );
    SPARCBYTEORDER->setText( tr( "big-endian (sparc)" ) );
    buttonGroup2->setTitle( tr( "Time format" ) );
    TIMEFORMAT1->setText( tr( "MM.DD.YYYY hour:min:sec" ) );
    TIMEFORMAT2->setText( tr( "YYYY.DDD hour:min:sec" ) );
    TIMEFORMAT3->setText( tr( " hour:min:sec DD/MM/YYYY" ) );
    ButtonOK->setText( tr( "OK" ) );
    ButtonCANCEL->setText( tr( "Cancel" ) );
	}

void configDlg::ButtonOK_clicked()
	{
	QString s;
	bool ok;
	s = portNumberLineEdit->text();
	int n = s.toInt( &ok, 10);

	if(!ok)
		{
		portNumberLineEdit->setFocus();
		return;
		}
	if(n<=0 || n>65535)
		{
		portNumberLineEdit->setFocus();
		return;
		}
	if(INTELBYTEORDER->isChecked())
		{
		nOutputByteOrder=0;
		}
	if(SPARCBYTEORDER->isChecked())
		{
		nOutputByteOrder=1;
		}

	if(TIMEFORMAT1->isChecked())
		{
		nTimeFormat = 0;
		}
	if(TIMEFORMAT2->isChecked())
		{
		nTimeFormat = 1;
		}
	if(TIMEFORMAT3->isChecked())
		{
		nTimeFormat = 2;
		}
	accept();
	}