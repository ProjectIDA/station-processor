#include <qstring.h>
#include <qptrlist.h>
#include <qrect.h>
#include "wfdisc.h"
extern BOOL bContinue;
extern QString server;
extern int nNrtsPort;
extern char *pIniFile;
extern int nOutputByteOrder;
extern int nTimeFormat;
extern QString wfdFilePath;
extern QString WfdiscDir;
extern int nGlobalError;
extern QString qParamFileName;
extern QString PARAMPATH;
extern QPtrList <CWfdisc> wfdarr;
extern double tbeg, tend;


void LoadGlobalParam();
void SaveGlobalParam();
void ClearWfdList();
void GetTextRectangle(const QWidget *pWid, const char *lpText, QRect &rc);
