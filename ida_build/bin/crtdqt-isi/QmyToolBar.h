// QmyToolBar.h: interface for the QmyToolBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QMYTOOLBAR_H__04886BF5_5119_4557_85D1_F7422A3FB341__INCLUDED_)
#define AFX_QMYTOOLBAR_H__04886BF5_5119_4557_85D1_F7422A3FB341__INCLUDED_

#include <qwidget.h>
#include <qstring.h>
#include <qtoolbar.h>
#include <qpushbutton.h>
//#include "MainFrame.h"
#include "QmyButton.h"

class QmyToolBar : public QToolBar  
{
public:
	QmyToolBar(const QString & label, QMainWindow * mainWindow, QWidget * parent, bool newLine = FALSE, const char * name = 0, WFlags f = 0);
	virtual ~QmyToolBar();
	QPushButton * runButton;
    QPushButton * configButton;
    QPushButton * aboutButton;

	QmyButton * autoButton;
	QmyButton * autoButton1;
	QPushButton * stopButton;
	QmyButton * expandButton;
	QmyButton * contractButton;
	QmyButton * upButton;
	QmyButton * downButton;
	void SetToolbarState(int nState);
	void languageChange();


};

#endif // !defined(AFX_QMYTOOLBAR_H__04886BF5_5119_4557_85D1_F7422A3FB341__INCLUDED_)
