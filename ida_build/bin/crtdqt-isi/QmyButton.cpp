// QmyButton.cpp: implementation of the QmyButton class.
//
//////////////////////////////////////////////////////////////////////

#include "CRTDglob.h"
#include "QmyButton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QmyButton::QmyButton(QWidget * parent , const char * name)
:QPushButton(parent, name)
	{
	}

QmyButton::~QmyButton()
	{
	}

void QmyButton::mousePressEvent(QMouseEvent * e)
	{
	if(e->state()&Qt::ShiftButton)
		{
		bShiftButtonPressed=TRUE;
		}
	else
		{
		bShiftButtonPressed=FALSE;
		}
	e->ignore();
	QPushButton::mousePressEvent(e);
	}

