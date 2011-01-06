/****************************************************************************
** Form interface generated from reading ui file 'crtdqtui.ui'
**
** Created: Пт 25. фев 10:01:13 2005
**      by: The User Interface Compiler ($Id: MainFrame.h,v 1.1 2005/05/18 00:06:43 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <qvariant.h>
#include <qmainwindow.h>
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
#include "RTDisplay.h"
#include "Preview.h"
#include "QmyButton.h"
#include "QmyToolBar.h"


class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    MainFrame( QWidget * parent = 0, const char * name = 0, WFlags f = WType_TopLevel );
    virtual ~MainFrame();

    QmyToolBar *toolBar;
	CRTDisplay *pDisplay;
	CPreview   *pPreview;

	enum
		{
		STOP				=	QEvent::User+1,
		SETTEXT			=	QEvent::User+2,
		ExpandVert		=	QEvent::User+3,
		ContractVert	=	QEvent::User+4,
		ShiftUp			=	QEvent::User+5,
		ShiftDown		=	QEvent::User+6
		};	

	void  SetTextToStatusbar(const char *pText);
private:
	int nTimerCount;
	int nPreviewHeight;

public slots:
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void configButton_clicked();
    virtual void newSlot();
    virtual void runButton_clicked();
	virtual void autoButton_clicked();
	virtual void autoButton1_clicked();
	virtual void expandButton_clicked();
	virtual void contractButton_clicked();
	virtual void upButton_clicked();
	virtual void downButton_clicked();
	virtual void exitButton_clicked();
	virtual void stopButton_clicked();
	virtual void aboutButton_clicked();


protected:
	virtual void  resizeEvent( QResizeEvent * );
	void customEvent(QCustomEvent *event); 

protected slots:
    virtual void languageChange();

};

#endif // MAINFRAME_H
