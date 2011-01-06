/****************************************************************************
** Form interface generated from reading ui file 'idd_results.ui'
**
** Created: Сб 9. апр 14:36:25 2005
**      by: The User Interface Compiler ($Id: isiTapResults.h,v 1.1 2005/05/17 23:55:19 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ISITAPRESULTS_H
#define ISITAPRESULTS_H

#include <qvariant.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qtable.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QIconView;
class QIconViewItem;
class QTextEdit;
class QProgressBar;

class isiTapResults : public QDialog
{
    Q_OBJECT

public:
    isiTapResults( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~isiTapResults();

    QPushButton* PushButtonOK;
    QPushButton* PushButtonCANCEL;
	QTable* IDC_CHAN;
    QLabel* outputLog;
    QProgressBar* progressBar1;
	QVBoxLayout* Layout1;
	QHBoxLayout* Layout2;

	bool bExitPossible;
	enum
		{
		STOP				=	QEvent::User+1,
		LOG					=	QEvent::User+2,
		PROGRESS            =	QEvent::User+3
		};

protected:
	virtual void customEvent(QCustomEvent *event); 
	void closeEvent ( QCloseEvent * e ); 
	void fillResultsTable();
	void adjustTableColumnWidth();


protected slots:
    virtual void languageChange();
	void PushButtonCANCEL_clicked();
	void PushButtonOK_clicked();


};

#endif // ISITAPRESULTS_H
