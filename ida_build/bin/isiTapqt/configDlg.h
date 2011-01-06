/****************************************************************************
** Form interface generated from reading ui file 'idd_configdlg.ui'
**
** Created: Пн 11. апр 10:59:42 2005
**      by: The User Interface Compiler ($Id: configDlg.h,v 1.1 2005/05/17 23:55:19 dechavez Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLabel;
class QLineEdit;
class QButtonGroup;
class QRadioButton;
class QPushButton;

class configDlg : public QDialog
{
    Q_OBJECT

public:
    configDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~configDlg();
	QGroupBox *GroupBox0;
    QLabel* portNumberLabel;
    QLineEdit* portNumberLineEdit;
    QButtonGroup* buttonGroup1;
    QRadioButton* INTELBYTEORDER;
    QRadioButton* SPARCBYTEORDER;
    QButtonGroup* buttonGroup2;
    QRadioButton* TIMEFORMAT1;
    QRadioButton* TIMEFORMAT2;
    QRadioButton* TIMEFORMAT3;
    QPushButton* ButtonOK;
    QPushButton* ButtonCANCEL;

protected:

protected slots:
    virtual void languageChange();
	virtual void ButtonOK_clicked();


};

#endif // CONFIGDLG_H
