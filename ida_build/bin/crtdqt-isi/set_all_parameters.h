/****************************************************************************
** Form interface generated from reading ui file 'set_all_parameters.ui'
**
** Created: Пт 4. мар 16:22:21 2005
**      by: The User Interface Compiler ($Id: set_all_parameters.h,v 1.1 2005/05/18 00:06:45 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SET_ALL_PARAMETERS_H
#define SET_ALL_PARAMETERS_H

#include "nrtsinfo.h"
#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QTabWidget;
class QWidget;
class QTable;
class QLabel;
class QLineEdit;
class QFrame;

class Set_All_Parameters : public QDialog
{
    Q_OBJECT

public:
    Set_All_Parameters( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Set_All_Parameters();
	void SaveParameters();
	void LoadStaList();
	void LoadChanList(QString &sSta);
	void SetChanDrawState(const QString &sSta, const QString &sChan, const QString &LCODE, BOOL bSelected);
	void SetStationCheck();


	CNRTSINFO NrtsInfoX;

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* tabWidget;
    QWidget* Channels;
    QTable* IDC_CHAN;
    QLabel* Port_number_2;
    QLabel* ISI_Server_2;
    QPushButton* IDC_LOADLIST;
    QTable* IDC_STA;
    QLineEdit* IDC_NRTSSERVER;
    QLineEdit* IDC_PORT;
    QWidget* Display_parameters;
    QFrame* frame3;
    QLabel* Displayed_window_size;
    QLineEdit* IDC_REFRESHTIME;
    QLineEdit* IDC_DELAY;
    QLabel* Buffered_window_size;
    QLineEdit* IDC_BUFFEREDTIME;
    QLineEdit* IDC_DISPLAYTIME;
    QLabel* Refresh_time;
    QLabel* Delay__sec;

private:
	BOOL bHasSelectedChan(QString sSta);
	BOOL bHasSelectedChan(CNRTSSTAINFO *si);

protected:
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;
    QGridLayout* layout14;
	QString ActiveSta;

protected slots:
    virtual void languageChange();
    virtual void OnOk();
	void OnLoadList();
	void OnClickStaListTableItem(int row, int col);
	void OnClickChanListTableItem(int row, int col);

};

#endif // SET_ALL_PARAMETERS_H
