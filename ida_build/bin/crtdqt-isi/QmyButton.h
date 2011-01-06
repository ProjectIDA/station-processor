// QmyButton.h: interface for the QmyButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QMYBUTTON_H__E24E8D82_13D6_4EE6_80D3_C20B38BA9E8C__INCLUDED_)
#define AFX_QMYBUTTON_H__E24E8D82_13D6_4EE6_80D3_C20B38BA9E8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <qpushbutton.h>
#include <qevent.h>

class QmyButton : public QPushButton  
{
public:
	QmyButton(QWidget * parent , const char * name) ;
	virtual ~QmyButton();
	virtual void mousePressEvent(QMouseEvent * e);

};

#endif // !defined(AFX_QMYBUTTON_H__E24E8D82_13D6_4EE6_80D3_C20B38BA9E8C__INCLUDED_)
