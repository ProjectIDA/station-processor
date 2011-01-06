/****************************************************************************
** Form interface generated from reading ui file 'isitapdlg.ui'
**
** Created: Пн 4. апр 09:21:12 2005
**      by: The User Interface Compiler ($Id: isitapdlg.h,v 1.1 2005/05/17 23:55:20 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ISITAPDLG_H
#define ISITAPDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include "DateTimeEdit.h"
#include "nrtsinfo.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class QGroupBox;
class QTable;
class QPushButton;
class QLineEdit;
class QProgressBar;

class isiTapDlg : public QDialog
{
    Q_OBJECT

public:
    isiTapDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~isiTapDlg();

	void LoadServersList();
	void RestoreParam();
	void SaveParam();
	void AddStringToServerList(const QString &cNew);
	void SaveServersList();
	void LoadStaList();
	void LoadChanList(QString &sSta);
	void SetChanDrawState(const QString &sSta, const QString &sChan, const QString &LCODE, BOOL bSelected);
	void SetStationCheck(CNRTSSTAINFO *si);
	BOOL bHasSelectedChan(const QString &sSta);
	BOOL bHasSelectedChan(CNRTSSTAINFO *si);
	void RemoveChanList(const QString &sSta);
	void SetStartTime(double t);
	void SetEndTime(double t);
	void adjustTableColumnWidth();




    QLabel* textLabel1;
    QComboBox* comboBox;
    QGroupBox* groupBox1;
    QTable* IDC_STA;
    QPushButton* UnSelectAllStaButton;
    QPushButton* SelectAllStaButton;
    QGroupBox* groupBox2;
    QPushButton* pushButton4;
    QTable* IDC_CHAN;
    QPushButton* resetButton;
    QGroupBox* groupBox3;
    QLabel* timeFormattextLabel;
    QLabel* endTimeLabel;
    DateTimeEdit* endTime;
    DateTimeEdit* startTime;
    QLabel* startTimeLabel;
    QGroupBox* groupBox4;
    QProgressBar* progressBar;
    QPushButton* getDataButton;
    QPushButton* configButton;
    QPushButton* exitButton;
    QPushButton* queryButton;
    QPushButton* queryTimeButton;

	CNRTSINFO NrtsInfo;

protected:
	QStringList servers_list;
	void UpdateTimeInChanList();
	double GetStartTime();
	double GetEndTime();
	void ClearChanList();
	int NumOfSelectedChan();
	void SetTime();


protected slots:
    virtual void languageChange();
	virtual void exitButton_clicked();
	virtual void queryButton_clicked();
	virtual void comboBox_selchange(const QString & string);
	virtual void OnClickStaListTableItem(int row, int col, int button, const QPoint & mousePos);
	virtual void OnClickChanListTableItem(int row, int col);
	virtual void queryTimeButton_clicked();
	virtual void resetButton_clicked();
	virtual void getDataButton_clicked();
	virtual void configButton_clicked();
	virtual void SelectAllStaButton_clicked();
	virtual void UnSelectAllStaButton_clicked();
	virtual void OnDblClickChanListTableItem(int row, int col, int button, const QPoint & mousePos);


private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;
    QPixmap image7;
    QPixmap image8;
    QPixmap image9;

};

#endif // ISITAPDLG_H
